/**
 * Delay functions — HAL + FreeRTOS compatible
 *
 * Delay_us:  使用 DWT (Data Watchpoint and Trace) 周期计数器
 *            不依赖 SysTick，不影响操作系统
 * Delay_ms/s: 使用 FreeRTOS vTaskDelay，让出 CPU
 */

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Delay.h"

static uint32_t dwt_enabled = 0;

/**
 * @brief  初始化 DWT 周期计数器（用于微秒级延时）
 */
void Delay_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    dwt_enabled = 1;
}

/**
 * @brief  微秒延时（阻塞，精度高）
 * @param  us — 微秒数，最大约 59,000 (72MHz 下 32bit 计数)
 */
void Delay_us(uint32_t us)
{
    if (!dwt_enabled) return;

    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) {}
}

/**
 * @brief  毫秒延时（使用 FreeRTOS，让出 CPU）
 * @note   仅在调度器启动后使用；此前用 HAL_Delay
 */
void Delay_ms(uint32_t ms)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
    else
    {
        HAL_Delay(ms);  /* 调度器启动前回退 */
    }
}

/**
 * @brief  秒级延时
 */
void Delay_s(uint32_t s)
{
    while (s--)
    {
        Delay_ms(1000);
    }
}
