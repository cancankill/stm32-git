/**
 * HC-SR04 超声波模块 (HAL + FreeRTOS 版本)
 *
 * Trig: PB6 (输出)
 * Echo: PB7 (输入)
 *
 * 声速 340m/s = 0.034 cm/μs
 * 距离 = (高电平时间 × 0.034) / 2
 */

#ifndef __HC_SR04_H
#define __HC_SR04_H

void HC_SR04_Init(void);
float HC_SR04_GetDistance(void);   /* 返回距离(cm)，-1表示超时 */

#endif
