/**
 * PWM driver — HAL TIM2 四路 PWM 输出
 *
 * TIM2 挂在 APB1 (36MHz), 经过倍频后时钟 = 72MHz
 * PSC = 72-1 → 计数频率 1MHz
 * ARR = 100-1 → PWM 频率 10kHz, 占空比分辨率 0~100
 */

#include "stm32f1xx_hal.h"
#include "PWM.h"

TIM_HandleTypeDef htim2;

/**
 * @brief  HAL MSP 回调 — 初始化 TIM2 GPIO 和时钟
 *         由 HAL_TIM_PWM_Init() 内部调用
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();

        /* PA0, PA1, PA2, PA3 → TIM2_CH1~CH4 复用推挽输出 */
        GPIO_InitTypeDef gpio = {0};
        gpio.Mode  = GPIO_MODE_AF_PP;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
        HAL_GPIO_Init(GPIOA, &gpio);

        /* 解除 JTAG 复用，释放 PA15 / PB3 / PB4 （PA15 用于电机 BL 方向） */
        __HAL_AFIO_REMAP_SWJ_NOJTAG();
    }
}

/**
 * @brief  初始化 TIM2 四路 PWM
 */
void PWM_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance               = TIM2;
    htim2.Init.Prescaler         = 72 - 1;       /* 72MHz / 72 = 1MHz */
    htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim2.Init.Period            = 100 - 1;      /* 1MHz / 100 = 10kHz */
    htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_PWM_Init(&htim2);

    /* 配置四个 PWM 通道 */
    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse      = 0;                   /* 初始占空比 0 */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);

    /* 启动所有通道 */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void PWM_SetCompare1(uint8_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Compare);
}

void PWM_SetCompare2(uint8_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Compare);
}

void PWM_SetCompare3(uint8_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, Compare);
}

void PWM_SetCompare4(uint8_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, Compare);
}
