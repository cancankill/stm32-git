#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "Motor.h"
#include "key.h"

// 电机速度
#define SPEED_FULL 90
#define SPEED_MID  70
#define SPEED_STOP 0

extern const uint8_t YueXinMao[22][8][128];
uint8_t frame = 0;
uint8_t RunState = 0;  // 0=停止, 1=中速直行

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

    while(1)
    {
        // 非阻塞按键检测
        uint8_t key = Key_Get();
        if (key == 1)  // PB0 按下，切换运动状态
        {
            RunState = !RunState;
            if (RunState)
                Run_Mid();
            else
                Stop();
        }
        if (key == 2)  // PB11 按下，停车
        {
            RunState = 0;
            Stop();
        }

        // OLED 动画
        OLED_Clear();
        Delay_ms(20);
        OLED_ShowImage(0, 0, 128, 64, &YueXinMao[frame][0][0]);
        frame++;
        if (frame >= 22)
        {
            frame = 0;
        }
        OLED_Update();
        Delay_ms(20);
    }
}
