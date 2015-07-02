#include "led.h"

/******************************************************************************************
*函数名：LED_GPIO_Conf()
* 参数：void
* 返回值：void
* 功能：LED的管脚配置
*********************************************************************************************/
void LED_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;		                                              //定义GPIO初始化结构体
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);			  					  //使能GPIOB的AHP1时钟
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										  //设置初始化模式为输出模式
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;										  //设置输出类型为推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;									  //设置输出速度为100Mhz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  
   GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_12|GPIO_Pin_13);								  //定义初始化的管脚为Pin14	Pin15
   GPIO_Init(GPIOD, &GPIO_InitStructure);
	
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);			  					  //使能GPIOB的AHP1时钟
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										  //设置初始化模式为输出模式
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;										  //设置输出类型为推挽输出
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;									  //设置输出速度为100Mhz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  
   GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_0);								  //定义初始化的管脚为Pin14	Pin15
   GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化GPIOB的Pin_14管脚

}
void LEDOn(Led_Def Led)//LED亮
{
  	LED_PORT[Led]->BSRRL=LED_PIN[Led];
}

void LEDOff(Led_Def Led)//LED灭
{
  	LED_PORT[Led]->BSRRH=LED_PIN[Led];  
}

void LEDTog(Led_Def Led)//LED状态翻转
{
  	LED_PORT[Led]->ODR^=LED_PIN[Led];
}
void PE_OUT(u8 i)
{
	i?(GPIOE->BSRRL = GPIO_Pin_0):(GPIOE->BSRRH = GPIO_Pin_0);
}
void PE_Tog(uint16_t GPIO_Pin)
{
	GPIOE->ODR ^= GPIO_Pin;
}
	
