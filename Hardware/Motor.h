#ifndef __MOTOR__H
#define __MOTOR__H

void Motor_Init(void);

void Motor_SetSpeed_FL(int8_t Speed) ;    //左前轮速度 
void Motor_SetSpeed_FR(int8_t Speed) ;   //右前轮速度  
void Motor_SetSpeed_BL(int8_t Speed) ;   //左后轮速度
void Motor_SetSpeed_BR(int8_t Speed) ;   //右后轮速度

#endif