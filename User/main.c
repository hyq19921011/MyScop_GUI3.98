/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The TFT DISPLAY application function
*********************************************************************************************************/
/* ------------------------------------------������ͷ�ļ�-----------------------------------------------*/
#include"main.h"


extern uint16_t Phy_X,Phy_Y;             //���ʶ���LCD������λ��
RCC_ClocksTypeDef RCC_Clocks;
BYTE   textFileBuffer[]={"����ȫ ���ϿƼ���ѧ������ʵ���ҵ�����Ϣ����ѧԺ0123456"};
WM_HMEM hProBar;

//
/*************************************************************************************
  * �������ƣ�main()
  * ����    ��void
  * ����ֵ  ��void
  * ����    �����������main����
  *************************************************************************************/
int main(void)
{   
    u16 i,j;   
	u16 t=0;
	
 /*****************************��ʼ��****************************************/   		                                   //��ʼ��ϵͳʱ�ӣ�����ʱ��Ϊ168Mhz
//  	LED_Init();
  	SysTick_Init(168); 
    TIM4_Init();
//    TIM3_init();//pwm	
	TIM5_PWM_Init();
	TIM5_PWM_OUTPUT(40,20,10,50);
    COM6_Init(115200);//����6��ʼ��
	
	bsp_InitDAC();//������ʼ��adc��dac���������
	bsp_InitADC();
	bsp_InitDAC();
  	TFTLCD_Init();
    Touch_Init();

	//EXTI_Configuration();
	GUI_Init(); //GUI ��ʼ�� 
	GUI_Clear();
	Timer2_Init(20);	//��ʼ��LED��GPIO����
	WaveTaskCreat();
	while(1)
	{  
		if(Wave_updat == 1)
		{
			Get_Signal();
			//Clculate_Signal();
			
		}
		Updata_signal();

	
		GUI_Exec();
	}
}
void TIM2_IRQHandler(void)   //TIM2�ж�
{   static u16 i=0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
			//PE_OUT(1);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
			GUI_TOUCH_Exec();
//			GUI_Exec();
//			Sin_Data[i]=100*sin((double)(i*3.14/40))+200;if (i == 1024) i = 0;
			//PE_OUT(0);
			
//			DAC_SetChannel1Data(DAC_Align_12b_R,1000+2000*sin((double)(i*3.14/40)));
			DAC_SetChannel1Data(DAC_Align_12b_R,10);
			i++;
			if (i == 80) i = 0;
		}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

