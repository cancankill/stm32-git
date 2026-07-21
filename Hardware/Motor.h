/**
 * Motor driver — 四轮小车电机控制
 *
 * 方向引脚（Speed >= 0 正转, Speed < 0 反转）:
 *   左前 FL: PA4(+) PA5(-)  PWM: TIM2_CH1 (PA0)
 *   右前 FR: PB12(+) PB13(-) PWM: TIM2_CH2 (PA1)
 *   左后 BL: PA8(+) PA15(-)  PWM: TIM2_CH3 (PA2)
 *   右后 BR: PB14(+) PB15(-) PWM: TIM2_CH4 (PA3)
 */

#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

void Motor_Init(void);
void Motor_SetSpeed_FL(int8_t Speed);
void Motor_SetSpeed_FR(int8_t Speed);
void Motor_SetSpeed_BL(int8_t Speed);
void Motor_SetSpeed_BR(int8_t Speed);

#endif
