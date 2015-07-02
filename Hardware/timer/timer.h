#ifndef __TIMER_H
#define __TIMER_H
#include <stm32f4xx.h>

#define Start_Tim2() TIM_Cmd(TIM2, ENABLE)
#define Stop_Tim2() TIM_Cmd(TIM2, DISABLE)
#define Start_Tim4IT() TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE)
#define Stop_Tim4IT() TIM_ITConfig(TIM4, TIM_IT_CC2, DISABLE)
void Timer2_Init(u16 arr); 
void TIM4_Init(void);


#endif
