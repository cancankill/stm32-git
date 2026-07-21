/**
 * LED 驱动 (HAL 版本)
 */

#include "stm32f1xx_hal.h"
#include "led.h"

void LED_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pin   = GPIO_PIN_1 | GPIO_PIN_2;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);
}

void led1_on(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void led1_off(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

void LED1_turn(void)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
}

void led2_on(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
}

void led2_off(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
}

void LED2_turn(void)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
}
