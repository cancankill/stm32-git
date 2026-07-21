/**
 * 串口驱动 — USART1 蓝牙通信 (HAL + FreeRTOS 版本)
 *
 * 接收协议: @ + 命令 + # + *
 * HAL 逐字节中断接收 + 状态机解析
 */

#include "stm32f1xx_hal.h"
#include "Serial.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* ── 全局变量 ───────────────────────────────── */
char    Serial_RxPacket[100];
uint8_t Serial_RxFlag;

/* ── USART1 句柄 ────────────────────────────── */
UART_HandleTypeDef huart1;

/* ── 逐字节接收缓冲 ─────────────────────────── */
static uint8_t uart_rx_byte;

/* ── 接收状态机 ─────────────────────────────── */
static uint8_t  rx_state  = 0;
static uint8_t  rx_index  = 0;

/**
 * @brief  HAL USART MSP 初始化（时钟、GPIO、NVIC）
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA9 = TX, 复用推挽 */
        GPIO_InitTypeDef gpio = {0};
        gpio.Mode  = GPIO_MODE_AF_PP;
        gpio.Pin   = GPIO_PIN_9;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &gpio);

        /* PA10 = RX, 上拉输入 */
        gpio.Mode  = GPIO_MODE_INPUT;
        gpio.Pull  = GPIO_PULLUP;
        gpio.Pin   = GPIO_PIN_10;
        HAL_GPIO_Init(GPIOA, &gpio);

        /* NVIC */
        HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

/**
 * @brief  初始化 USART1 — 9600-8-N-1
 */
void Serial_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 9600;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart1);

    /* 启动逐字节中断接收 */
    HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
}

/**
 * @brief  HAL 接收完成回调 — 状态机解析协议
 *
 * 协议格式: @ + 命令字符串 + # + *
 *   |→ rx_state=0: 等待 '@'
 *   |→ rx_state=1: 接收命令直到 '#'
 *   |→ rx_state=2: 等待 '*' 结束
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART1) return;

    switch (rx_state)
    {
        case 0:  /* 等待起始符 '@' */
            if (uart_rx_byte == '@' && Serial_RxFlag == 0)
            {
                rx_state = 1;
                rx_index = 0;
            }
            break;

        case 1:  /* 接收命令字符串直到 '#' */
            if (uart_rx_byte == '#')
            {
                rx_state = 2;
            }
            else
            {
                Serial_RxPacket[rx_index++] = uart_rx_byte;
            }
            break;

        case 2:  /* 等待结束符 '*' */
            if (uart_rx_byte == '*')
            {
                Serial_RxPacket[rx_index] = '\0';
                Serial_RxFlag = 1;
                rx_state = 0;
            }
            break;

        default:
            rx_state = 0;
            break;
    }

    /* 重新启动中断接收下一字节 */
    HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
}

/* ── 发送函数 ───────────────────────────────── */

void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart1, &Byte, 1, 10);
}

void Serial_SendArray(uint8_t *Array, uint16_t length)
{
    HAL_UART_Transmit(&huart1, Array, length, 0xFFFF);
}

void Serial_SendString(char *String)
{
    uint16_t len = 0;
    while (String[len] != '\0') len++;
    HAL_UART_Transmit(&huart1, (uint8_t *)String, len, 0xFFFF);
}

static uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--) Result *= X;
    return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

void Serial_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(String);
}

/**
 * @brief  printf 重定向到 USART1
 */
int fputc(int ch, FILE *f)
{
    Serial_SendByte((uint8_t)ch);
    return ch;
}
