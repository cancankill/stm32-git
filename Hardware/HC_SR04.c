/**
 * HC-SR04 超声波测距 (HAL + FreeRTOS 版本)
 *
 * 使用 DWT 做微秒延时，不依赖 SysTick。
 * 测量在控制任务中调用，阻塞约 0.1~38ms。
 */

#include "stm32f1xx_hal.h"
#include "Delay.h"

/* 引脚定义 */
#define TRIG_PORT   GPIOB
#define TRIG_PIN    GPIO_PIN_6
#define ECHO_PORT   GPIOB
#define ECHO_PIN    GPIO_PIN_7

/* 声速 340m/s = 0.034 cm/μs */
#define SOUND_SPEED 0.034f

void HC_SR04_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};

    /* Trig → 推挽输出 */
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pin   = TRIG_PIN;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TRIG_PORT, &gpio);

    /* Echo → 下拉输入 */
    gpio.Mode  = GPIO_MODE_INPUT;
    gpio.Pull  = GPIO_PULLDOWN;
    gpio.Pin   = ECHO_PIN;
    HAL_GPIO_Init(ECHO_PORT, &gpio);

    /* 初始 Trig 低 */
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

/**
 * @brief  发送 20μs 触发脉冲
 */
static void HC_SR04_Trig(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    Delay_us(20);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

/**
 * @brief  获取距离 (cm)，-1 表示超时或无回波
 */
float HC_SR04_GetDistance(void)
{
    uint32_t timeout;
    uint32_t time_us = 0;

    HC_SR04_Trig();

    /* 等待 Echo 上升沿（超时约 7ms） */
    timeout = 0;
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_RESET)
    {
        timeout++;
        if (timeout > 100000) return -1.0f;
    }

    /* 测量高电平持续时间（微秒级） */
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET)
    {
        Delay_us(1);
        time_us++;
        if (time_us > 60000) return -1.0f;  /* 超出 10m 范围 */
    }

    return (float)time_us * SOUND_SPEED / 2.0f;
}
