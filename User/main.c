/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The TFT DISPLAY application function
*********************************************************************************************************/
/* ------------------------------------------包含的头文件-----------------------------------------------*/
#include"main.h"


extern uint16_t Phy_X,Phy_Y;             //触笔对于LCD的物理位置
RCC_ClocksTypeDef RCC_Clocks;
BYTE   textFileBuffer[]={"黄永全 河南科技大学机器人实验室电子信息工程学院0123456"};
WM_HMEM hProBar;

//
/*************************************************************************************
  * 函数名称：main()
  * 参数    ：void
  * 返回值  ：void
  * 描述    ：程序主入口main函数
  *************************************************************************************/
int main(void)
{   
    u16 i,j;   
	u16 t=0;
	
 /*****************************初始化****************************************/   		                                   //初始化系统时钟，设置时钟为168Mhz
//  	LED_Init();
  	SysTick_Init(168); 
    TIM4_Init();
//    TIM3_init();//pwm	
	TIM5_PWM_Init();
	TIM5_PWM_OUTPUT(40,20,10,50);
    COM6_Init(115200);//串口6初始化
	
	bsp_InitDAC();//这样初始化adc和dac不会出问题
	bsp_InitADC();
	bsp_InitDAC();
  	TFTLCD_Init();
    Touch_Init();

	//EXTI_Configuration();
	GUI_Init(); //GUI 初始化 
	GUI_Clear();
	Timer2_Init(20);	//初始化LED的GPIO配置
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
void TIM2_IRQHandler(void)   //TIM2中断
{   static u16 i=0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
			//PE_OUT(1);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
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

