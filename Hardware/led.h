/**
 * LED 驱动 (HAL 版本)
 *
 * LED1: PA1, LED2: PA2
 * 注意: PA1/PA2 与 PWM TIM2_CH2/CH3 共用引脚
 *       LED 未在 main 中使用，保留此驱动备用
 */

#ifndef __LED_H
#define __LED_H

void LED_Init(void);
void led1_on(void);
void led1_off(void);
void led2_on(void);
void led2_off(void);
void LED1_turn(void);
void LED2_turn(void);

#endif
