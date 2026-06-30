#include "stm32f10x.h"

/**
  * @brief  寰�绉�绾у欢���
  * @param  xus 寤舵�舵�堕�匡�������达��0~233015
  * @retval ���
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//璁剧疆瀹���跺�ㄩ��瑁����
	SysTick->VAL = 0x00;					//娓�绌哄�����璁℃�板��
	SysTick->CTRL = 0x00000005;				//璁剧疆��堕��婧�涓�HCLK锛������ㄥ����跺��
	while(!(SysTick->CTRL & 0x00010000));	//绛�寰�璁℃�板��0
	SysTick->CTRL = 0x00000004;				//��抽��瀹���跺��
}

/**
  * @brief  姣�绉�绾у欢���
  * @param  xms 寤舵�舵�堕�匡�������达��0~4294967295
  * @retval ���
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  绉�绾у欢���
  * @param  xs 寤舵�舵�堕�匡�������达��0~4294967295
  * @retval ���
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 
