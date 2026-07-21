/**
 * Main program header for 循迹小车 (Tracking Car)
 *
 * HAL + FreeRTOS 移植
 * STM32F103C8  @ 72 MHz
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*──────────────────────────────────────────────────────────
 * Standard includes
 *──────────────────────────────────────────────────────────*/
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*──────────────────────────────────────────────────────────
 * FreeRTOS includes
 *──────────────────────────────────────────────────────────*/
#include "FreeRTOS.h"
#include "task.h"

/*──────────────────────────────────────────────────────────
 * Hardware drivers
 *──────────────────────────────────────────────────────────*/
#include "delay.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "Motor.h"
#include "key.h"
#include "Serial.h"
#include "HC_SR04.h"
#include "Tracking.h"

/*──────────────────────────────────────────────────────────
 * Motor speed defines
 *──────────────────────────────────────────────────────────*/
#define SPEED_FULL  90
#define SPEED_MID   50
#define SPEED_STOP  0

/*──────────────────────────────────────────────────────────
 * Run mode
 *──────────────────────────────────────────────────────────*/
#define MODE_STOP    0
#define MODE_MANUAL  1
#define MODE_PATROL  2

/*──────────────────────────────────────────────────────────
 * External globals (defined in Serial.c)
 *──────────────────────────────────────────────────────────*/
extern char     Serial_RxPacket[100];
extern uint8_t  Serial_RxFlag;

/*──────────────────────────────────────────────────────────
 * OLED animation data
 *──────────────────────────────────────────────────────────*/
extern const uint8_t YueXinMao[22][8][128];

/*──────────────────────────────────────────────────────────
 * UART handle (defined in Serial.c)
 *──────────────────────────────────────────────────────────*/
extern UART_HandleTypeDef huart1;

/*──────────────────────────────────────────────────────────
 * TIM handle (defined in PWM.c)
 *──────────────────────────────────────────────────────────*/
extern TIM_HandleTypeDef  htim2;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
