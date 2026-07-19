#ifndef __HC_SR04_H
#define __HC_SR04_H

void HC_SR04_Init(void);
float HC_SR04_GetDistance(void);   // 返回距离(cm)，-1表示超时

#endif
