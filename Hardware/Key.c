#include "stm32f10x.h"
#include "Delay.h"

static uint8_t Key_State[2] = {0, 0};  // 0=松开, 1=按下

void key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStucture;
    GPIO_InitStucture.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStucture.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11;
    GPIO_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStucture);
}

// 返回: 0=无按键, 1=PB0按下, 2=PB11按下 (带消抖)
uint8_t Key_Get(void)
{
    uint8_t result = 0;

    // PB0
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
    {
        if (Key_State[0] == 0)
        {
            Delay_ms(20);  // 消抖
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
            {
                Key_State[0] = 1;
                result = 1;
            }
        }
    }
    else
    {
        Key_State[0] = 0;
    }

    // PB11
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
    {
        if (Key_State[1] == 0)
        {
            Delay_ms(20);  // 消抖
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
            {
                Key_State[1] = 1;
                result = 2;
            }
        }
    }
    else
    {
        Key_State[1] = 0;
    }

    return result;
}
