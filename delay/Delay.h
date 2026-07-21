/**
 * Delay functions — HAL + FreeRTOS compatible
 *
 * Delay_us:  DWT cycle counter (no OS dependency)
 * Delay_ms:  FreeRTOS vTaskDelay (yields CPU)
 * Delay_s:   FreeRTOS vTaskDelay
 */

#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>

void Delay_Init(void);       /* Enable DWT for us delay */
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);

#endif
