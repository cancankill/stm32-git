/**
 * PWM driver — STM32 HAL TIM2 四路 PWM
 *
 * TIM2_CH1: PA0  (左前轮)
 * TIM2_CH2: PA1  (右前轮)
 * TIM2_CH3: PA2  (左后轮)
 * TIM2_CH4: PA3  (右后轮)
 *
 * 频率: 72MHz / 72 / 100 = 10kHz
 * 占空比: 0~100 对应 0%~100%
 */

#ifndef __PWM_H
#define __PWM_H

#include <stdint.h>

void PWM_Init(void);
void PWM_SetCompare1(uint8_t Compare);
void PWM_SetCompare2(uint8_t Compare);
void PWM_SetCompare3(uint8_t Compare);
void PWM_SetCompare4(uint8_t Compare);

#endif
