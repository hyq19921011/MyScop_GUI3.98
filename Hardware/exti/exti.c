#include "exti.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "stm32f4xx_syscfg.h"
#include "touch.h"
#include "TFTlcd.h"
#include "led.h"
#include "pwm.h"
#include "GUI.H"
void EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

//		/* GPIOX Periph clock enable */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//		GPIO_Init(GPIOD, &GPIO_InitStructure);
//	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//注意要打开SYSCFG时钟
        //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);    //407使用的配置函数
        EXTI_InitStructure.EXTI_Line = EXTI_Line4;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_InitStructure.EXTI_LineCmd = DISABLE;
        EXTI_Init(&EXTI_InitStructure); 		
		//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource13);    //407使用的配置函数
        EXTI_InitStructure.EXTI_Line = EXTI_Line13;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);

		/* Enable the Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}
void EXTI15_10_IRQHandler(void)//触摸中断
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		 
        GUI_TOUCH_Exec();
        EXTI_ClearITPendingBit(EXTI_Line13);               
	}
}

