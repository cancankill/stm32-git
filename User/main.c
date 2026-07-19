#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "Motor.h"
#include "key.h"
#include "Serial.h"
#include "HC_SR04.h"
#include "Tracking.h"
#include <string.h>

// 电机速度
#define SPEED_FULL 90
#define SPEED_MID  50
#define SPEED_STOP 0

// 运行模式
#define MODE_STOP     0
#define MODE_MANUAL   1
#define MODE_PATROL   2

// 蓝牙串口接收
extern char     Serial_RxPacket[];
extern uint8_t  Serial_RxFlag;

extern const uint8_t YueXinMao[22][8][128];
uint8_t frame = 0;
uint8_t RunState = MODE_STOP;  // 0=停止, 1=手动前进, 2=循迹巡逻

void Run_Full(void) // 全速前进
{
   Motor_SetSpeed_BL(SPEED_FULL);
   Motor_SetSpeed_BR(SPEED_FULL);
   Motor_SetSpeed_FL(SPEED_FULL);
   Motor_SetSpeed_FR(SPEED_FULL);
}

void Run_Mid(void) // 中速前进
{
   Motor_SetSpeed_BL(SPEED_MID);
   Motor_SetSpeed_BR(SPEED_MID);
   Motor_SetSpeed_FL(SPEED_MID);
   Motor_SetSpeed_FR(SPEED_MID);
}

void Turn_left(void)  // 前进左转
{
   Motor_SetSpeed_BL(SPEED_MID);
   Motor_SetSpeed_BR(SPEED_FULL);
   Motor_SetSpeed_FL(SPEED_MID);
   Motor_SetSpeed_FR(SPEED_FULL);
}

void Turn_right(void)  // 前进右转
{
   Motor_SetSpeed_BL(SPEED_FULL);
   Motor_SetSpeed_BR(SPEED_MID);
   Motor_SetSpeed_FL(SPEED_FULL);
   Motor_SetSpeed_FR(SPEED_MID);
}

void Turn_left_stop(void)  // 原地左转
{
   Motor_SetSpeed_BL(-SPEED_MID);
   Motor_SetSpeed_BR(SPEED_MID);
   Motor_SetSpeed_FL(-SPEED_MID);
   Motor_SetSpeed_FR(SPEED_MID);
}

void Turn_right_stop(void)  // 原地右转
{
   Motor_SetSpeed_BL(SPEED_MID);
   Motor_SetSpeed_BR(-SPEED_MID);
   Motor_SetSpeed_FL(SPEED_MID);
   Motor_SetSpeed_FR(-SPEED_MID);
}

void Stop(void)
{
   Motor_SetSpeed_BL(SPEED_STOP);
   Motor_SetSpeed_BR(SPEED_STOP);
   Motor_SetSpeed_FL(SPEED_STOP);
   Motor_SetSpeed_FR(SPEED_STOP);
}

int main(void)
{
    OLED_Init();
    key_Init();
    Motor_Init();
    Serial_Init();        // 蓝牙串口
    HC_SR04_Init();       // 超声波避障
    Tracking_GPIO_Init(); // 四路循迹模块

    while(1)
    {
        // === 蓝牙串口控制 ===
        if (Serial_RxFlag)
        {
            Serial_RxFlag = 0;   // 清除标志

            if (strcmp(Serial_RxPacket, "QJ") == 0)
            {
                RunState = MODE_MANUAL;
                Run_Mid();
            }
            else if (strcmp(Serial_RxPacket, "XJ") == 0)
            {
                RunState = MODE_PATROL;    // 蓝牙发送 "XJ" 进入循迹模式
            }
            else if (strcmp(Serial_RxPacket, "STOP") == 0)
            {
                RunState = MODE_STOP;
                Stop();
            }
        }

        // === 按键控制 ===
        uint8_t key = Key_Get();
        if (key == 1)  // PB0 按下 → 模式切换: 停止→手动→巡逻→停止
        {
            RunState = (RunState + 1) % 3;
            if (RunState == MODE_STOP)
                Stop();
            else if (RunState == MODE_MANUAL)
                Run_Mid();
        }
        if (key == 2)  // PB11 按下 → 直接停车
        {
            RunState = MODE_STOP;
            Stop();
        }

        // === 模式执行 ===
        if (RunState == MODE_PATROL)
        {
            Tracking_Patrol(SPEED_MID);   // PD 循迹，基础速度 50
        }

        // === 超声波避障 ===
        if (RunState == MODE_MANUAL || RunState == MODE_PATROL)
        {
            float dist = HC_SR04_GetDistance();
            if (dist > 0 && dist < 3.0f)
            {
                RunState = MODE_STOP;
                Stop();
            }
        }

        // === OLED 显示 ===
        OLED_Clear();

        if (RunState == MODE_PATROL)
        {
            // 循迹模式：显示传感器状态
            int L1, L2, R1, R2;
            Tracking_GetSensors(&L1, &L2, &R1, &R2);

            OLED_ShowString(0, 0, "TRACK:", OLED_6X8);

            // 四个传感器状态 (# = 检测到黑线, - = 白底)
            OLED_ShowChar(42, 0, (L1 == 0) ? '#' : '-', OLED_6X8);
            OLED_ShowChar(52, 0, (L2 == 0) ? '#' : '-', OLED_6X8);
            OLED_ShowChar(62, 0, (R1 == 0) ? '#' : '-', OLED_6X8);
            OLED_ShowChar(72, 0, (R2 == 0) ? '#' : '-', OLED_6X8);

            // 计算误差用于位置判断
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

            // 模式标识
            OLED_ShowString(0, 32, "MODE: PATROL", OLED_6X8);
        }
        else
        {
            // 停止/手动模式：显示动画
            OLED_ShowImage(0, 0, 128, 64, &YueXinMao[frame][0][0]);
            frame++;
            if (frame >= 22)
            {
                frame = 0;
            }
        }

        OLED_Update();
        Delay_ms(20);
    }
}
