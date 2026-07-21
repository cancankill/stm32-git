/**
 * 按键驱动 (HAL + FreeRTOS 版本)
 *
 * Key1: PB0 — 模式切换 (停止→手动→巡逻)
 * Key2: PB11 — 急停
 */

#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

void    key_Init(void);
uint8_t Key_Get(void);   /* 返回: 0=无, 1=PB0按下, 2=PB11按下 (带 20ms 消抖) */

#endif
