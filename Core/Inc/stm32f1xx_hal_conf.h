/**
 * STM32F1xx HAL Configuration File
 *
 * Only enable peripherals actually used by this project.
 * STM32F103C8 = medium density device.
 */

#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/*──────────────────────────────────────────────────────────
 * Module enable: only what the project uses
 *──────────────────────────────────────────────────────────*/
#define HAL_MODULE_ENABLED          /* Core / Cortex */
#define HAL_GPIO_MODULE_ENABLED     /* All GPIO */
#define HAL_RCC_MODULE_ENABLED      /* Reset & Clock Control */
#define HAL_TIM_MODULE_ENABLED      /* Timer (PWM) */
#define HAL_UART_MODULE_ENABLED     /* UART (Bluetooth serial) */
#define HAL_DMA_MODULE_ENABLED      /* DMA (not used yet, but often needed by UART) */
#define HAL_CORTEX_MODULE_ENABLED   /* NVIC / SysTick / system */

/*──────────────────────────────────────────────────────────
 * Oscillator values in Hz
 *──────────────────────────────────────────────────────────*/
#if !defined(HSE_VALUE)
  #define HSE_VALUE     8000000U   /* 8 MHz external crystal */
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT   100U
#endif

#if !defined(HSI_VALUE)
  #define HSI_VALUE     8000000U   /* 8 MHz internal RC */
#endif

#if !defined(LSI_VALUE)
  #define LSI_VALUE      40000U
#endif

#if !defined(LSE_VALUE)
  #define LSE_VALUE      32768U
#endif

/*──────────────────────────────────────────────────────────
 * HAL assert
 *──────────────────────────────────────────────────────────*/
#ifdef USE_FULL_ASSERT
  #define assert_param(expr)  ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr)  ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
