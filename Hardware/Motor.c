#include "stm32f10x.h"                  // Device header
#include "PWM.h"

int8_t speed_fl;
int8_t speed_fr;
int8_t speed_bl;
int8_t speed_br;


void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_8 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 | GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	
  
	
    PWM_Init();
}

void Motor_SetSpeed_FL(int8_t Speed)   //左前轮 PA4,PA5
{
    
    if(Speed>=0)
    {
      GPIO_SetBits(GPIOA,GPIO_Pin_4); 
      GPIO_ResetBits(GPIOA,GPIO_Pin_5);
      PWM_SetCompare1(Speed);
    }
    else
    { 
      
      GPIO_ResetBits(GPIOA,GPIO_Pin_4);
      GPIO_SetBits(GPIOA,GPIO_Pin_5);
      PWM_SetCompare1(-Speed);
    }
    speed_fl=Speed;
    
}


void Motor_SetSpeed_FR(int8_t Speed)  //右前轮 PB12,PB13
{
    if(Speed>=0)
    {
      GPIO_SetBits(GPIOB,GPIO_Pin_12); 
      GPIO_ResetBits(GPIOB,GPIO_Pin_13);
      PWM_SetCompare2(Speed);
    }
    else
    { 
      
      GPIO_ResetBits(GPIOB,GPIO_Pin_12);
      GPIO_SetBits(GPIOB,GPIO_Pin_13);
      PWM_SetCompare2(-Speed);
    }
     speed_fr=Speed;
}

void Motor_SetSpeed_BL(int8_t Speed)   //左后轮 PA8,PA15
{
    if(Speed>=0)
    {
      GPIO_SetBits(GPIOA,GPIO_Pin_8); 
      GPIO_ResetBits(GPIOA,GPIO_Pin_15);
      PWM_SetCompare3(Speed);
    }
    else
    { 
      
      GPIO_ResetBits(GPIOA,GPIO_Pin_8);
      GPIO_SetBits(GPIOA,GPIO_Pin_15);
      PWM_SetCompare3(-Speed);
    }
    speed_bl=Speed;
}

void Motor_SetSpeed_BR(int8_t Speed)  //右后轮 PB14,PB15
{
    if(Speed>=0)
    {
      GPIO_SetBits(GPIOB,GPIO_Pin_14); 
      GPIO_ResetBits(GPIOB,GPIO_Pin_15);
      PWM_SetCompare4(Speed);
    }
    else
    { 
      
      GPIO_ResetBits(GPIOB,GPIO_Pin_14);
      GPIO_SetBits(GPIOB,GPIO_Pin_15);
      PWM_SetCompare4(-Speed);
    }
    speed_br=Speed;
}

