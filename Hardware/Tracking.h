/**
 * 循迹模块 — 四路红外传感器 (HAL 版本)
 *
 * 引脚:
 *   L1: PB3, L2: PB4, R1: PB5, R2: PB10
 *   全部上拉输入，模块输出低电平有效（黑线=0，白底=1）
 *
 * 增量式 PID 控制器，带丢线记忆恢复
 */

#ifndef __TRACKING_H
#define __TRACKING_H

#include <stdint.h>

/*──────────────────────────────────────────────────────────
 * 循迹传感器引脚宏配置
 *──────────────────────────────────────────────────────────*/

/* 左1（最左侧） */
#define USE_TRACK_L1
#define TRACK_L1_PORT    GPIOB
#define TRACK_L1_PIN     GPIO_PIN_3
#define TRACK_L1_RCC     __HAL_RCC_GPIOB_CLK_ENABLE

/* 左2 */
#define USE_TRACK_L2
#define TRACK_L2_PORT    GPIOB
#define TRACK_L2_PIN     GPIO_PIN_4
#define TRACK_L2_RCC     __HAL_RCC_GPIOB_CLK_ENABLE

/* 右1 */
#define USE_TRACK_R1
#define TRACK_R1_PORT    GPIOB
#define TRACK_R1_PIN     GPIO_PIN_5
#define TRACK_R1_RCC     __HAL_RCC_GPIOB_CLK_ENABLE

/* 右2（最右侧） */
#define USE_TRACK_R2
#define TRACK_R2_PORT    GPIOB
#define TRACK_R2_PIN     GPIO_PIN_10
#define TRACK_R2_RCC     __HAL_RCC_GPIOB_CLK_ENABLE

/*──────────────────────────────────────────────────────────
 * 黑线检测电平: 0 = 低电平表示黑线
 *──────────────────────────────────────────────────────────*/
#define BLACK_IS_LOW    1

/*──────────────────────────────────────────────────────────
 * 增量式 PID 参数
 *──────────────────────────────────────────────────────────*/
#define TRACK_KP           200.0f
#define TRACK_KI           10.0f
#define TRACK_KD           30.0f

/*──────────────────────────────────────────────────────────
 * API
 *──────────────────────────────────────────────────────────*/

void Tracking_GPIO_Init(void);
void Tracking_GetSensors(int *pL1, int *pL2, int *pR1, int *pR2);
void Tracking_Patrol(int baseSpeed);

#endif
