#ifndef __pwm_H
#define __pwm_H
#include "stm32f4xx.h"
#define Period_25KHZ 1000
#define Period_10KHZ 400
#define Period_1KHZ  40
extern void TIM3_init();
//extern void PWM_Set_Duty_Period(u8 Duty);

#endif