/**
 * Motor driver — 四路电机方向 + PWM 调速（HAL 版本）
 */

#include "stm32f1xx_hal.h"
#include "Motor.h"
#include "PWM.h"

/* 方向引脚定义 */
#define FL_PLUS_PIN     GPIO_PIN_4
#define FL_MINUS_PIN    GPIO_PIN_5
#define FL_PORT         GPIOA

#define FR_PLUS_PIN     GPIO_PIN_12
#define FR_MINUS_PIN    GPIO_PIN_13
#define FR_PORT         GPIOB

#define BL_PLUS_PIN     GPIO_PIN_8
#define BL_MINUS_PIN    GPIO_PIN_15
#define BL_PORT         GPIOA

#define BR_PLUS_PIN     GPIO_PIN_14
#define BR_MINUS_PIN    GPIO_PIN_15
#define BR_PORT         GPIOB

void Motor_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    /* PA4, PA5, PA8, PA15 — FL 和 BL 方向 */
    gpio.Pin = FL_PLUS_PIN | FL_MINUS_PIN | BL_PLUS_PIN | BL_MINUS_PIN;
    HAL_GPIO_Init(FL_PORT, &gpio);

    /* PB12, PB13, PB14, PB15 — FR 和 BR 方向 */
    gpio.Pin = FR_PLUS_PIN | FR_MINUS_PIN | BR_PLUS_PIN | BR_MINUS_PIN;
    HAL_GPIO_Init(FR_PORT, &gpio);

    /* 初始全低 */
    HAL_GPIO_WritePin(FL_PORT, FL_PLUS_PIN | FL_MINUS_PIN | BL_PLUS_PIN | BL_MINUS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FR_PORT, FR_PLUS_PIN | FR_MINUS_PIN | BR_PLUS_PIN | BR_MINUS_PIN, GPIO_PIN_RESET);

    PWM_Init();
}

void Motor_SetSpeed_FL(int8_t Speed)
{
    if (Speed >= 0)
    {
        HAL_GPIO_WritePin(FL_PORT, FL_PLUS_PIN,  GPIO_PIN_SET);
        HAL_GPIO_WritePin(FL_PORT, FL_MINUS_PIN, GPIO_PIN_RESET);
        PWM_SetCompare1(Speed);
    }
    else
    {
        HAL_GPIO_WritePin(FL_PORT, FL_PLUS_PIN,  GPIO_PIN_RESET);
        HAL_GPIO_WritePin(FL_PORT, FL_MINUS_PIN, GPIO_PIN_SET);
        PWM_SetCompare1(-Speed);
    }
}

void Motor_SetSpeed_FR(int8_t Speed)
{
    if (Speed >= 0)
    {
        HAL_GPIO_WritePin(FR_PORT, FR_PLUS_PIN,  GPIO_PIN_SET);
        HAL_GPIO_WritePin(FR_PORT, FR_MINUS_PIN, GPIO_PIN_RESET);
        PWM_SetCompare2(Speed);
    }
    else
    {
        HAL_GPIO_WritePin(FR_PORT, FR_PLUS_PIN,  GPIO_PIN_RESET);
        HAL_GPIO_WritePin(FR_PORT, FR_MINUS_PIN, GPIO_PIN_SET);
        PWM_SetCompare2(-Speed);
    }
}

void Motor_SetSpeed_BL(int8_t Speed)
{
    if (Speed >= 0)
    {
        HAL_GPIO_WritePin(BL_PORT, BL_PLUS_PIN,  GPIO_PIN_SET);
        HAL_GPIO_WritePin(BL_PORT, BL_MINUS_PIN, GPIO_PIN_RESET);
        PWM_SetCompare3(Speed);
    }
    else
    {
        HAL_GPIO_WritePin(BL_PORT, BL_PLUS_PIN,  GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BL_PORT, BL_MINUS_PIN, GPIO_PIN_SET);
        PWM_SetCompare3(-Speed);
    }
}

void Motor_SetSpeed_BR(int8_t Speed)
{
    if (Speed >= 0)
    {
        HAL_GPIO_WritePin(BR_PORT, BR_PLUS_PIN,  GPIO_PIN_SET);
        HAL_GPIO_WritePin(BR_PORT, BR_MINUS_PIN, GPIO_PIN_RESET);
        PWM_SetCompare4(Speed);
    }
    else
    {
        HAL_GPIO_WritePin(BR_PORT, BR_PLUS_PIN,  GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BR_PORT, BR_MINUS_PIN, GPIO_PIN_SET);
        PWM_SetCompare4(-Speed);
    }
}
