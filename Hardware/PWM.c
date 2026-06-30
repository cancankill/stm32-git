#include "stm32f10x.h"                  // Device header


void PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //选择内部时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
	
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//端口重映射
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //解除jtag的复用
	
	 
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP ;
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    TIM_InternalClockConfig(TIM2); //初始化时基单元
      
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitstructure;
    TIM_TimeBaseInitstructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitstructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitstructure.TIM_Period=100-1;  //ARR
    TIM_TimeBaseInitstructure.TIM_Prescaler=36-1; //PSC
    TIM_TimeBaseInitstructure.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitstructure)	;
	
	
     TIM_OCInitTypeDef TIM_OCInitStructure;
     TIM_OCStructInit(&TIM_OCInitStructure);  //给结构体赋初始值
     
     TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
     TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
     TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; 
     TIM_OCInitStructure.TIM_Pulse=0;  //CCR
     TIM_OC1Init(TIM2,&TIM_OCInitStructure);  //初始化oc比较器
     
     TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
     TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
     TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; 
     TIM_OCInitStructure.TIM_Pulse=0;  //CCR
     TIM_OC2Init(TIM2,&TIM_OCInitStructure);  //初始化oc比较器
     
     TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
     TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
     TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; 
     TIM_OCInitStructure.TIM_Pulse=0;  //CCR
     TIM_OC3Init(TIM2,&TIM_OCInitStructure);  //初始化oc比较器
     
     TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
     TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
     TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; 
     TIM_OCInitStructure.TIM_Pulse=0;  //CCR
     TIM_OC4Init(TIM2,&TIM_OCInitStructure);  //初始化oc比较器

     TIM_Cmd(TIM2,ENABLE);
	
  
}

void PWM_SetCompare1(uint8_t Compare)
{
   TIM_SetCompare1(TIM2,Compare);
}

void PWM_SetCompare2(uint8_t Compare)
{
   TIM_SetCompare2(TIM2,Compare);
}

void PWM_SetCompare3(uint8_t Compare)
{
   TIM_SetCompare3(TIM2,Compare);
}

void PWM_SetCompare4(uint8_t Compare)
{
   TIM_SetCompare4(TIM2,Compare);
}

