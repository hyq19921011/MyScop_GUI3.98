#include "delay.h"
__IO uint32_t TimingDelay;
static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
/***************************************************************************
*函数名：SysTick_Init()
*参数：void
*返回值:void
*功能：系统节拍时钟初始化函数
*****************************************************************************/
void SysTick_Init(u8 SYSCLK)
{
	 /****************************************
	 *SystemFrequency/1000      1ms中断一次  *
	 *SystemFrequency/100000    10us中断一次 *
	 *SystemFrequency/1000000   1us中断一次  *
	 *****************************************/
    while(SysTick_Config(SystemCoreClock/1000)!=0);    //设置系统节拍时钟为1us中断一次
	/*附加笔记：
	SysTick_Config(uint32_t ticks)函数为core_cm4.h中定义的设置节拍时钟的函数，当设置成功时，返回0，失败返回1
	*/
}

/***************************************************************************
*函数名：delay_nus()
*参数：u32 n  时钟延时的微秒数
*返回值:void
*功能：使用系统节拍时钟精准延时n个微秒函数
*****************************************************************************/
void SYS_delay_nus(u32 nus)
{
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us-1; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
}

/***************************************************************************
*函数名：delay_nms()
*参数：u32 n  时钟延时的毫秒数
*返回值:void
*功能：系统节拍时钟初始化函数
*****************************************************************************/
void SYS_delay_nms(u32 nms)
{
  u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms-1;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
}
/***************************************************************************
*函数名：delay_nms()
*参数：u32 n  时钟延时的毫秒数
*返回值:void
*功能：系统节拍时钟初始化函数
*****************************************************************************/
void delay_nms(u32 n)
{
	 u16 i = 0;
   for(n;n > 0; n --)
			for(i = 30000;i > 0;i --);
}
void delay_nus(u32 n)
{
	 u16 i = 0;
   for(n;n > 0; n --)
			for(i = 24;i > 0;i --);
}
void Delay_nop(u32 n)
{
	while(n--)
	{
		__NOP();
	}
}
void Delay(u32 n)
{
	while(n--);

}