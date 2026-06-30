#include "stm32f10x.h"

 void LED_Init(void)
 {
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 
	 GPIO_InitTypeDef GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP ;
	 GPIO_InitStructure.GPIO_Pin= GPIO_Pin_1 | GPIO_Pin_2;
	 GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA,&GPIO_InitStructure);
	 	 
 }
 
 
 void led1_on(void)
 {
     GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	 
 
 }
 
 void led1_off(void)
 {
	 
    GPIO_SetBits(GPIOA, GPIO_Pin_1);	 
	 
 }
 
 void  LED1_turn(void)
 {
	 if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1)==0)
	 {
		 
            GPIO_SetBits(GPIOA,GPIO_Pin_1);		 
		 
	 }
	 else
	 {
	   GPIO_ResetBits(GPIOA,GPIO_Pin_1 );	 
          }
	 
 }
                                                          
 void led2_on(void)
 {
     GPIO_ResetBits(GPIOA, GPIO_Pin_2);
 
 }
 
 void led2_off(void)
 {
	 
    GPIO_SetBits(GPIOA, GPIO_Pin_2);	 
	 
 }
 
 void  LED2_turn(void)
 {
	 if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2)==0)
	 {
		 
            GPIO_SetBits(GPIOA,GPIO_Pin_2);		 
		 
	 }
	 else
	 {
	   GPIO_ResetBits(GPIOA,GPIO_Pin_2 );	 
          }
	 
 }
