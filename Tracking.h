#ifndef __TRACKING_H
#define __TRACKING_H

#include <stdint.h>

/* ── 循迹传感器引脚宏配置 ────────────────────── */

/* 左1 (最左侧传感器) */
#define USE_TRACK_L1
#define TRACK_L1_PORT    GPIOB
#define TRACK_L1_PIN     GPIO_Pin_3
#define TRACK_L1_RCC     RCC_APB2Periph_GPIOB

/* 左2 */
#define USE_TRACK_L2
#define TRACK_L2_PORT    GPIOB
#define TRACK_L2_PIN     GPIO_Pin_4
#define TRACK_L2_RCC     RCC_APB2Periph_GPIOB

/* 右1 */
#define USE_TRACK_R1
#define TRACK_R1_PORT    GPIOB
#define TRACK_R1_PIN     GPIO_Pin_5
#define TRACK_R1_RCC     RCC_APB2Periph_GPIOB

/* 右2 (最右侧传感器) */
#define USE_TRACK_R2
#define TRACK_R2_PORT    GPIOB
#define TRACK_R2_PIN     GPIO_Pin_10
#define TRACK_R2_RCC     RCC_APB2Periph_GPIOB

/* ── 黑线检测电平 ────────────────────────────── */
/* 0 = 低电平表示黑线 (模块输出低电平有效)         */
/* 1 = 高电平表示黑线 (模块输出高电平有效)         */
#define BLACK_IS_LOW    1

/* ── PID 参数 ────────────────────────────────── */
#define TRACK_KP           200.0f
#define TRACK_KD           30.0f
#define TRACK_BASE_SPEED   50

/* ── API ─────────────────────────────────────── */

void Tracking_GPIO_Init(void);

/**
 * @brief  获取四路传感器原始状态
 * @param  pL1, pL2, pR1, pR2 — 四路传感器值指针
 *         BLACK_IS_LOW=1 时: 0=黑线, 1=白底
 */
void Tracking_GetSensors(int *pL1, int *pL2, int *pR1, int *pR2);

/**
 * @brief  循迹巡逻（PD 控制器）
 * @param  baseSpeed — 基础速度 0~100
 *
 * 根据四路传感器反馈实时修正左右轮差速。
 * 丢线时利用 Last_Error 记忆找回。
 */
void Tracking_Patrol(int baseSpeed);

#endif
