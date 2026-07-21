/**
 * 串口驱动 — USART1 蓝牙通信 (HAL + FreeRTOS 版本)
 *
 * 协议: @ + command + # + *
 *   例: @QJ#*  → 前进
 *       @XJ#*  → 循迹
 *       @STOP#* → 停止
 *
 * 接收在中断中逐字节完成，状态机解析。
 */

#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include <stdio.h>

/* 串口接收缓冲区 */
extern char    Serial_RxPacket[100];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

/* printf 重定向到串口 */
int fputc(int ch, FILE *f);

#endif
