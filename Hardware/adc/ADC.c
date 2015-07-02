#include "ADC.h"
#include "scope.h"
#include "delay.h"
#include <stdio.h>

#define ADC3_DR_ADDRESS     &ADC3->DR 
//u16 AD_New_Buf[1024] = {0}; 
//__IO uint16_t ADC3ConvertedValue = 0;
//__IO uint32_t ADC3ConvertedVoltage = 0;
 
void ADC3_CH13_DMA_Config(void) //ADC3设置为定时器触发
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
 
  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  /* DMA2 Stream0 channel0 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;//ADC地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AD_New_Buf;//内存地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设位数据源
  DMA_InitStructure.DMA_BufferSize = 1024;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址 固定
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//内存地址固定
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环传输
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);
  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);//使能DMA _ CH1中断
  /* Configure ADC3 Channel12 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//42M
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC3, &ADC_InitStructure);

  /* ADC3 regular channel12 configuration *************************************/
  ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_3Cycles);

// /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
  //ADC_ExternalTrigConvCmd(ADC3, ENABLE);
  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC3, ENABLE);

  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);		
//  ADC_SoftwareStartConv(ADC3);
}		
void TIM1_Config(void)
{
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 /* Time Base configuration */
 TIM_TimeBaseStructure.TIM_Prescaler = 8-1;
 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 TIM_TimeBaseStructure.TIM_Period = 30;
 TIM_TimeBaseStructure.TIM_ClockDivision = 0;
 TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
 TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
 
 /* Channel 1, 2,3 and 4 Configuration in PWM mode */
 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
 TIM_OCInitStructure.TIM_Pulse = 1;
 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
 TIM_OC1Init(TIM1, &TIM_OCInitStructure);
 TIM_ARRPreloadConfig(TIM1, ENABLE);
 TIM_OC1PreloadConfig (TIM1, TIM_OCPreload_Enable);
 TIM_CtrlPWMOutputs(TIM1, ENABLE);
 /* TIM1 counter enable */
 TIM_Cmd(TIM1, ENABLE);
}
u32 ADC3_getvol(void)
{ 
//	return ADC3ConvertedValue;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////OSC////////////////////////////////////////////////////////////////////
////////////////////////////////////OSC////////////////////////////////////////////////////////////////////
unsigned int ADCTripleConvertedValue_1 = 0, ADCTripleConvertedValue_2 = 0;

void ADC_DMA_Config(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  
	ADC_DeInit();
	DMA_DeInit(DMA2_Stream0);
  
  Delay(0xfff);


  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC1, ENABLE);

  /* DMA2 Stream0 channel0 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;    //
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC2->DR;  //DMA外设基地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCTripleConvertedValue_2;  //DMA内存基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;    //数据从外设到内存
  DMA_InitStructure.DMA_BufferSize = 1;          //DMA通道的缓存大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;     //外设地址寄存器递增与否
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;    //内存地址寄存器递增与否
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据位宽度，BYTE-8，HalfWord-16，Word-32
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //同上
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //工作模式 Circular-循环模式  Normal-正常模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;   //通道X的优先级  VeryHigh-最高，High-其次, Medium-正常， Low-低
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream2, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream2, ENABLE);
//////ADC1  
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;    //
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  //DMA外设基地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCTripleConvertedValue_1;  //DMA内存基地址 
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;   //通道X的优先级  VeryHigh-最高，High-其次, Medium-正常， Low-低    
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);
 
 /* Configure ADC1 Channel10 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC12 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC2, &ADC_InitStructure);
//////ADC1
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC3 regular channel12 configuration *************************************/
  ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, /*ADC_SampleTime_3Cycles*/ 0);
//////ADC1
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, /*ADC_SampleTime_3Cycles*/ 0);	
 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC2, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC3 */
  ADC_Cmd(ADC2, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  
  ADC_SoftwareStartConv(ADC2);
  ADC_SoftwareStartConv(ADC1);
}

///////////////////////////////////////////////////////////////////
//ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
unsigned int ADC2_data(void)
{
//	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
	return (ADCTripleConvertedValue_2 & AD_max);
}

unsigned int ADC1_data(void)
{
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	return (ADCTripleConvertedValue_1& AD_max);
}








