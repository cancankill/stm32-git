#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// 引脚定义 — 使用 PB6(TRIG) PB7(ECHO)，避免与按键 PB11 冲突
#define TRIG_PORT   GPIOB
#define TRIG_PIN    GPIO_Pin_6
#define ECHO_PORT   GPIOB
#define ECHO_PIN    GPIO_Pin_7

// 声速 340m/s = 0.034cm/us
#define SOUND_SPEED 0.034f

/**
  * @brief  初始化 HC-SR04
  */
void HC_SR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // Trig 引脚设为推挽输出
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = TRIG_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

    // Echo 引脚设为下拉输入
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin = ECHO_PIN;
    GPIO_Init(ECHO_PORT, &GPIO_InitStruct);

    // 初始状态
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);
}

/**
  * @brief  发送触发脉冲
  */
static void HC_SR04_Trig(void)
{
    GPIO_SetBits(TRIG_PORT, TRIG_PIN);
    Delay_us(20);                      // 至少 10us 高电平
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);
}

/**
  * @brief  获取距离值 (cm)
  * @return 距离值，-1 表示超时
  */
float 																																																																							HC_SR04_GetDistance(void)
{
    uint32_t timeout;
    uint32_t time = 0;

    // 发送触发脉冲
    HC_SR04_Trig();

    // 等待 Echo 变为高电平（超时保护）
    timeout = 0;
    while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == 0)
    {
        timeout++;
        if (timeout > 100000) return -1;   // 超时返回 -1
    }

    // 测量高电平持续时间
    while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == 1)
    {
        Delay_us(1);
        time++;
        if (time > 60000) return -1;       // 超出测量范围
    }

    // 距离 = 时间(us) * 声速(cm/us) / 2
    return (float)time * SOUND_SPEED / 2.0f;
}
