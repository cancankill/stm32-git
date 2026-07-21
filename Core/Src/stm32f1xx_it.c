/**
 * Interrupt Service Routines — HAL + FreeRTOS
 *
 * SysTick: shared by HAL (HAL_IncTick) and FreeRTOS (xPortSysTickHandler)
 * USART1: delegates to HAL UART IRQ handler
 */

#include "stm32f1xx_it.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

/* External UART handle */
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*                 Cortex-M3 Processor Exceptions                             */
/******************************************************************************/

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1) {}
}

void MemManage_Handler(void)
{
    while (1) {}
}

void BusFault_Handler(void)
{
    while (1) {}
}

void UsageFault_Handler(void)
{
    while (1) {}
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

/**
 * @brief  SysTick ISR — shared between HAL and FreeRTOS
 *
 * HAL uses it for HAL_Delay / HAL_GetTick.
 * FreeRTOS uses it for the kernel tick.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}

/******************************************************************************/
/*                 STM32F1xx Peripheral Interrupt Handlers                    */
/******************************************************************************/

/**
 * @brief  USART1 interrupt — Bluetooth serial
 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
