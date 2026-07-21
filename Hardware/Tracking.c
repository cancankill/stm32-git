/**
 * 循迹模块 — 四路红外传感器 + 增量式 PID 控制器 (HAL 版本)
 */

#include "stm32f1xx_hal.h"
#include "Tracking.h"
#include "Motor.h"

/* ── 静态变量：增量式 PID + 丢线记忆 ────────── */
static float Track_Last_Error      = 0;   /* 丢线记忆              */
static float Track_Prev_Error      = 0;   /* e(k-1)                */
static float Track_Prev_Prev_Error = 0;   /* e(k-2)                */
static float Track_Turn            = 0;   /* 增量式 PID 输出累加值  */

/**
 * @brief  循迹传感器 GPIO 初始化
 *         每个传感器独立 #ifdef 控制，可灵活增减
 *         引脚配置为上拉输入 (Pull-up)，模块输出低电平有效
 */
void Tracking_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 解除 JTAG 复用，释放 PB3 */
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

#ifdef USE_TRACK_L1
    TRACK_L1_RCC();
    GPIO_InitStruct.Pin  = TRACK_L1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TRACK_L1_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_TRACK_L2
    TRACK_L2_RCC();
    GPIO_InitStruct.Pin  = TRACK_L2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TRACK_L2_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_TRACK_R1
    TRACK_R1_RCC();
    GPIO_InitStruct.Pin  = TRACK_R1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TRACK_R1_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_TRACK_R2
    TRACK_R2_RCC();
    GPIO_InitStruct.Pin  = TRACK_R2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TRACK_R2_PORT, &GPIO_InitStruct);
#endif
}

/**
 * @brief  读取四路传感器原始值
 * @param  pL1, pL2, pR1, pR2 — 返回各传感器状态
 *         BLACK_IS_LOW=1 → 0=黑线, 1=白底
 */
void Tracking_GetSensors(int *pL1, int *pL2, int *pR1, int *pR2)
{
#ifdef USE_TRACK_L1
    *pL1 = (HAL_GPIO_ReadPin(TRACK_L1_PORT, TRACK_L1_PIN) == GPIO_PIN_SET) ? 1 : 0;
#else
    *pL1 = 1;
#endif

#ifdef USE_TRACK_L2
    *pL2 = (HAL_GPIO_ReadPin(TRACK_L2_PORT, TRACK_L2_PIN) == GPIO_PIN_SET) ? 1 : 0;
#else
    *pL2 = 1;
#endif

#ifdef USE_TRACK_R1
    *pR1 = (HAL_GPIO_ReadPin(TRACK_R1_PORT, TRACK_R1_PIN) == GPIO_PIN_SET) ? 1 : 0;
#else
    *pR1 = 1;
#endif

#ifdef USE_TRACK_R2
    *pR2 = (HAL_GPIO_ReadPin(TRACK_R2_PORT, TRACK_R2_PIN) == GPIO_PIN_SET) ? 1 : 0;
#else
    *pR2 = 1;
#endif
}

/**
 * @brief  循迹巡逻 — 增量式 PID + 丢线恢复 + 差速执行
 * @param  baseSpeed — 基础速度 0~100
 *
 *  误差权重: L1=-3, L2=-1, R1=+1, R2=+3
 *  丢线时根据 Last_Error 方向继续偏转找回。
 */
void Tracking_Patrol(int baseSpeed)
{
    int L1, L2, R1, R2;
    Tracking_GetSensors(&L1, &L2, &R1, &R2);

    float error = 0;
    int   count = 0;

    /* ── 1. 加权偏差计算 ────────────────────── */
    /*     传感器    L1    L2    R1    R2         */
    /*     权重      -3    -1    +1    +3         */
    /*     BLACK_IS_LOW=1: 0=黑线 1=白底          */
#if BLACK_IS_LOW
    if (L1 == 0) { error += -3.0f; count++; }
    if (L2 == 0) { error += -1.0f; count++; }
    if (R1 == 0) { error +=  1.0f; count++; }
    if (R2 == 0) { error +=  3.0f; count++; }
#else
    if (L1 != 0) { error += -3.0f; count++; }
    if (L2 != 0) { error += -1.0f; count++; }
    if (R1 != 0) { error +=  1.0f; count++; }
    if (R2 != 0) { error +=  3.0f; count++; }
#endif

    /* ── 2. 偏差处理 ────────────────────────── */
    if (count > 0)
    {
        error = error / (float)count;           /* 平均偏差 */
        Track_Last_Error = error;               /* 记忆有效偏差 */
    }
    else
    {
        /* 丢线处理：根据最后偏差方向继续偏转 */
        if      (Track_Last_Error < -1.0f) error = -4.0f;  /* 偏左严重，右转找回 */
        else if (Track_Last_Error >  1.0f) error =  4.0f;  /* 偏右严重，左转找回 */
        else
        {
            /* 完全丢失 → 停车等待 */
            Motor_SetSpeed_FL(0);
            Motor_SetSpeed_FR(0);
            Motor_SetSpeed_BL(0);
            Motor_SetSpeed_BR(0);
            return;
        }
    }

    /* ── 3. 增量式 PID 控制器 ──────────────── */
    /*   Δu = Kp·[e(k)-e(k-1)] + Ki·e(k) + Kd·[e(k)-2e(k-1)+e(k-2)] */
    float delta = TRACK_KP * (error - Track_Prev_Error)
                + TRACK_KI * error
                + TRACK_KD * (error - 2.0f * Track_Prev_Error + Track_Prev_Prev_Error);

    /* 误差历史推移 */
    Track_Prev_Prev_Error = Track_Prev_Error;
    Track_Prev_Error      = error;

    /* 增量累加 + 输出限幅（防积分饱和） */
    Track_Turn += delta;
    if      (Track_Turn >  100.0f) Track_Turn =  100.0f;
    else if (Track_Turn < -100.0f) Track_Turn = -100.0f;

    float Turn = Track_Turn;

    /* ── 4. 差速分配 ────────────────────────── */
    int left_speed  = baseSpeed + (int)Turn;
    int right_speed = baseSpeed - (int)Turn;

    /* ── 5. 限幅 ────────────────────────────── */
    #define MOTOR_MAX  100
    #define MOTOR_MIN -100

    if (left_speed  >  MOTOR_MAX) left_speed  =  MOTOR_MAX;
    if (left_speed  < -MOTOR_MAX) left_speed  = -MOTOR_MAX;
    if (right_speed >  MOTOR_MAX) right_speed =  MOTOR_MAX;
    if (right_speed < -MOTOR_MAX) right_speed = -MOTOR_MAX;

    /* ── 6. 执行 ────────────────────────────── */
    Motor_SetSpeed_FL((int8_t)left_speed);
    Motor_SetSpeed_BL((int8_t)left_speed);
    Motor_SetSpeed_FR((int8_t)right_speed);
    Motor_SetSpeed_BR((int8_t)right_speed);
}
