/**
 * 循迹小车 — HAL + FreeRTOS 主程序
 *
 * STM32F103C8 @ 72MHz
 *
 * 任务:
 *   vTaskControl  (prio 3, 10ms): 按键 → 串口 → 模式 → 超声 → 巡线PID → 电机
 *   vTaskDisplay  (prio 1, 50ms): OLED 显示更新
 *
 * 串口协议: @ + 命令 + # + *  (ISR 中解析)
 */

#include "main.h"

/* ── 全局状态 ───────────────────────────────── */
uint8_t RunState = MODE_STOP;
uint8_t frame    = 0;           /* OLED 动画帧 */

/* ── 外部: 串口接收（Serial.c） ────────────────── */
extern char     Serial_RxPacket[];
extern uint8_t  Serial_RxFlag;

/* ── 外部: 动画数据（OLED_Data.c） ────────────── */
extern const uint8_t YueXinMao[22][8][128];

/*──────────────────────────────────────────────────
 * 运动控制函数（与原版完全一致）
 *──────────────────────────────────────────────────*/

static void Run_Full(void)
{
    Motor_SetSpeed_BL(SPEED_FULL);
    Motor_SetSpeed_BR(SPEED_FULL);
    Motor_SetSpeed_FL(SPEED_FULL);
    Motor_SetSpeed_FR(SPEED_FULL);
}

static void Run_Mid(void)
{
    Motor_SetSpeed_BL(SPEED_MID);
    Motor_SetSpeed_BR(SPEED_MID);
    Motor_SetSpeed_FL(SPEED_MID);
    Motor_SetSpeed_FR(SPEED_MID);
}

static void Turn_left(void)
{
    Motor_SetSpeed_BL(SPEED_MID);
    Motor_SetSpeed_BR(SPEED_FULL);
    Motor_SetSpeed_FL(SPEED_MID);
    Motor_SetSpeed_FR(SPEED_FULL);
}

static void Turn_right(void)
{
    Motor_SetSpeed_BL(SPEED_FULL);
    Motor_SetSpeed_BR(SPEED_MID);
    Motor_SetSpeed_FL(SPEED_FULL);
    Motor_SetSpeed_FR(SPEED_MID);
}

static void Turn_left_stop(void)
{
    Motor_SetSpeed_BL(-SPEED_MID);
    Motor_SetSpeed_BR(SPEED_MID);
    Motor_SetSpeed_FL(-SPEED_MID);
    Motor_SetSpeed_FR(SPEED_MID);
}

static void Turn_right_stop(void)
{
    Motor_SetSpeed_BL(SPEED_MID);
    Motor_SetSpeed_BR(-SPEED_MID);
    Motor_SetSpeed_FL(SPEED_MID);
    Motor_SetSpeed_FR(-SPEED_MID);
}

static void Stop(void)
{
    Motor_SetSpeed_BL(SPEED_STOP);
    Motor_SetSpeed_BR(SPEED_STOP);
    Motor_SetSpeed_FL(SPEED_STOP);
    Motor_SetSpeed_FR(SPEED_STOP);
}

/*──────────────────────────────────────────────────
 * vTaskControl — 核心控制任务 (10ms 周期)
 *──────────────────────────────────────────────────*/

static void vTaskControl(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));

        /* ── 1. 蓝牙串口命令 ─────────────────── */
        if (Serial_RxFlag)
        {
            Serial_RxFlag = 0;

            if (strcmp(Serial_RxPacket, "QJ") == 0)
            {
                RunState = MODE_MANUAL;
                Run_Mid();
            }
            else if (strcmp(Serial_RxPacket, "XJ") == 0)
            {
                RunState = MODE_PATROL;
                Stop();  /* 切换时先停车，等下一周期巡线接管 */
            }
            else if (strcmp(Serial_RxPacket, "STOP") == 0)
            {
                RunState = MODE_STOP;
                Stop();
            }
        }

        /* ── 2. 按键控制 ─────────────────────── */
        uint8_t key = Key_Get();
        if (key == 1)  /* PB0 → 模式切换 */
        {
            RunState = (RunState + 1) % 3;
            if (RunState == MODE_STOP)
                Stop();
            else if (RunState == MODE_MANUAL)
                Run_Mid();
        }
        if (key == 2)  /* PB11 → 急停 */
        {
            RunState = MODE_STOP;
            Stop();
        }

        /* ── 3. 巡线模式 ─────────────────────── */
        if (RunState == MODE_PATROL)
        {
            Tracking_Patrol(SPEED_MID);
        }

        /* ── 4. 超声波避障 ───────────────────── */
        if (RunState == MODE_MANUAL || RunState == MODE_PATROL)
        {
            float dist = HC_SR04_GetDistance();
            if (dist > 0 && dist < 3.0f)
            {
                RunState = MODE_STOP;
                Stop();
            }
        }
    }
}

/*──────────────────────────────────────────────────
 * vTaskDisplay — OLED 显示任务 (50ms 周期)
 *──────────────────────────────────────────────────*/

static void vTaskDisplay(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));

        OLED_Clear();

        if (RunState == MODE_PATROL)
        {
            /* 巡线模式：显示传感器状态 */
            int L1, L2, R1, R2;
            Tracking_GetSensors(&L1, &L2, &R1, &R2);

            OLED_ShowString(0, 0, "TRACK:", OLED_6X8);

            OLED_ShowChar(42, 0, (L1 == 0) ? '#' : '-', OLED_6X8);
            OLED_ShowChar(52, 0, (L2 == 0) ? '#' : '-', OLED_6X8);
            OLED_ShowChar(62, 0, (R1 == 0) ? '#' : '-', OLED_6X8);
            OLED_ShowChar(72, 0, (R2 == 0) ? '#' : '-', OLED_6X8);

            /* 计算误差 → 位置文本 */
            float error = 0;
            int count = 0;
            if (L1 == 0) { error += -3.0f; count++; }
            if (L2 == 0) { error += -1.0f; count++; }
            if (R1 == 0) { error +=  1.0f; count++; }
            if (R2 == 0) { error +=  3.0f; count++; }

            OLED_ShowString(0, 16, "POS:", OLED_6X8);
            char posStr[16];
            if (count == 0)      strcpy(posStr, "OFF-LINE");
            else if (error < -2) strcpy(posStr, "L-FAR");
            else if (error <  0) strcpy(posStr, "L-NEAR");
            else if (error == 0) strcpy(posStr, "CENTER");
            else if (error <  2) strcpy(posStr, "R-NEAR");
            else                 strcpy(posStr, "R-FAR");
            OLED_ShowString(42, 16, posStr, OLED_6X8);

            OLED_ShowString(0, 32, "MODE: PATROL", OLED_6X8);
        }
        else
        {
            /* 停止 / 手动模式：显示动画 */
            OLED_ShowImage(0, 0, 128, 64, &YueXinMao[frame][0][0]);
            frame++;
            if (frame >= 22) frame = 0;
        }

        OLED_Update();
    }
}

/*──────────────────────────────────────────────────
 * Hook 函数 — FreeRTOS 错误处理
 *──────────────────────────────────────────────────*/

void vApplicationMallocFailedHook(void)
{
    /* 内存分配失败 → 停电机，进入死循环 */
    Stop();
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    /* 栈溢出 → 停电机，进入死循环 */
    Stop();
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

/*──────────────────────────────────────────────────
 * SystemClock_Config — HAL 时钟配置 (HSE 8MHz → PLL 72MHz)
 *──────────────────────────────────────────────────*/

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 使能 HSE + PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;  /* 8MHz × 9 = 72MHz */
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Flash 2 wait states (72MHz), HCLK=72, PCLK2=72, PCLK1=36 */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK
                                     | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1
                                     | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;   /* HCLK = 72MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;     /* PCLK1 = 36MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;     /* PCLK2 = 72MHz */
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

/*──────────────────────────────────────────────────
 * main — 硬件初始化 → 创建任务 → 启动调度器
 *──────────────────────────────────────────────────*/

int main(void)
{
    /* HAL 初始化 */
    HAL_Init();

    /* 系统时钟: HSE 8MHz → PLL 72MHz */
    SystemClock_Config();

    /* DWT 周期计数器（供 Delay_us 使用） */
    Delay_Init();

    /* 硬件外设初始化 */
    OLED_Init();
    key_Init();
    Motor_Init();           /* 含 PWM 初始化 */
    Serial_Init();          /* 蓝牙串口 (USART1) */
    HC_SR04_Init();         /* 超声波 */
    Tracking_GPIO_Init();   /* 四路循迹 */

    /* 创建 FreeRTOS 任务 */
    xTaskCreate(vTaskControl, "Control", 256, NULL, 3, NULL);
    xTaskCreate(vTaskDisplay, "Display", 256, NULL, 1, NULL);

    /* 启动调度器（永不返回） */
    vTaskStartScheduler();

    /* 不应该到达这里 */
    for (;;) {}
}
