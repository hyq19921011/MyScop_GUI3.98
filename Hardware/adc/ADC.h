#ifndef __ADC_H
#define __ADC_H
#include "stm32f4xx_conf.h"

//extern u16 AD_New_Buf[1024];
extern void ADC3_CH13_DMA_Config(void);
extern void TIM1_Config(void);
extern u32 ADC3_getvol(void);
/////////////////////////OSC////////////////////
extern void ADC_DMA_Config(void);
extern unsigned int ADC2_data(void);
extern unsigned int ADC1_data(void);

extern void ADC2_CH13_DMA_Config(unsigned char speed);

#endif

