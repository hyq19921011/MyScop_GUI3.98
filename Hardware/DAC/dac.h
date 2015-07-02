#ifndef __DAC_H
#define __DAC_H	 
#include "stm32f4xx.h"	   
#include "stm32f4xx_dac.h"
extern void Dac1_Init(void);//回环模式初始化		 	 
extern void Dac1_Set_Vol(u16 vol);
#endif
