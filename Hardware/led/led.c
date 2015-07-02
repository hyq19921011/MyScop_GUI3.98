#include "led.h"

/******************************************************************************************
*��������LED_GPIO_Conf()
* ������void
* ����ֵ��void
* ���ܣ�LED�Ĺܽ�����
*********************************************************************************************/
void LED_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;		                                              //����GPIO��ʼ���ṹ��
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);			  					  //ʹ��GPIOB��AHP1ʱ��
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										  //���ó�ʼ��ģʽΪ���ģʽ
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;										  //�����������Ϊ�������
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;									  //��������ٶ�Ϊ100Mhz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  
   GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_12|GPIO_Pin_13);								  //�����ʼ���Ĺܽ�ΪPin14	Pin15
   GPIO_Init(GPIOD, &GPIO_InitStructure);
	
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);			  					  //ʹ��GPIOB��AHP1ʱ��
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;										  //���ó�ʼ��ģʽΪ���ģʽ
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;										  //�����������Ϊ�������
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;									  //��������ٶ�Ϊ100Mhz
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  
   GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_0);								  //�����ʼ���Ĺܽ�ΪPin14	Pin15
   GPIO_Init(GPIOE, &GPIO_InitStructure);	//��ʼ��GPIOB��Pin_14�ܽ�

}
void LEDOn(Led_Def Led)//LED��
{
  	LED_PORT[Led]->BSRRL=LED_PIN[Led];
}

void LEDOff(Led_Def Led)//LED��
{
  	LED_PORT[Led]->BSRRH=LED_PIN[Led];  
}

void LEDTog(Led_Def Led)//LED״̬��ת
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
	
