/**
 * 按键驱动 (HAL + FreeRTOS 版本)
 *
 * 消抖使用 vTaskDelay（让出 CPU），仅在调度器运行后有效。
 * 调度器启动前（初始化阶段）不会调用 Key_Get，因此安全。
 */

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Key.h"

static uint8_t Key_State[2] = {0, 0};  /* 0=松开, 1=按下 */

void key_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Pin  = GPIO_PIN_0 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &gpio);
}

/**
 * @brief  读取按键（带 20ms 消抖）
 * @return 0=无按键, 1=PB0按下, 2=PB11按下
 */
uint8_t Key_Get(void)
{
    uint8_t result = 0;

    /* PB0 */
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        if (Key_State[0] == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20));  /* 消抖，让出 CPU */
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
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

    /* PB11 */
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
        if (Key_State[1] == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20));  /* 消抖 */
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET)
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
