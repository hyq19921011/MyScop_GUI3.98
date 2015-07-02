#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "stm32f4xx_conf.h" 


//与触摸屏芯片连接引脚	   
#define CLR_TDIN()		GPIO_ResetBits(GPIOD, GPIO_Pin_3)
#define SET_TDIN()		GPIO_SetBits(GPIOD, GPIO_Pin_3)
#define CLR_TCLK()		GPIO_ResetBits(GPIOD, GPIO_Pin_6)
#define SET_TCLK()		GPIO_SetBits(GPIOD, GPIO_Pin_6)
#define CLR_TCS()		GPIO_ResetBits(GPIOD, GPIO_Pin_12)
#define SET_TCS()		GPIO_SetBits(GPIOD, GPIO_Pin_12)

#define TDOUT_IN()  	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)   //GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define PEN_State()		GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13)   //检测触摸屏响应信号

#define SAMP_CNT      8                           //采用次数
#define SAMP_CNT_DIV2 4                           //采用取值个数
#define ADJUST_DE     100                          //校验的阀门值，越小越精准但越难校验成功
#define CMD_RDY       0X90  
#define CMD_RDX	      0XD0    											 	    

/******************************************************************************************
*函数名：Touch_Init()
* 参数：void
* 返回值：void
* 功能：触摸芯片初始化
*********************************************************************************************/
void Touch_Init(void);
/******************************************************************************************
*函数名：DrawTouchPoint()
* 参数：uint16_t x,uint16_t y xy坐标
* 返回值：void
* 功能：绘制触摸笔笔尖的点
*********************************************************************************************/
void DrawTouchPoint(uint16_t x,uint16_t y);
/******************************************************************************************
*函数名：DrawAdjustPoint()
* 参数：uint16_t x,uint16_t y xy坐标
* 返回值：void
* 功能：绘制触摸校准时使用的准星
*********************************************************************************************/
void DrawAdjustPoint(uint16_t x,uint16_t y,uint16_t text_color);
/******************************************************************************************
*函数名：AdjustTouchPanel()
* 参数：void
* 返回值：void
* 功能：触摸屏校准函数
*********************************************************************************************/
void AdjustTouchPanel(void);
/******************************************************************************************
*函数名：ADS_Read()
* 参数：u8 CMD 指令，读x或读y
* 返回值：void
* 功能：读取ADS的ADC值
*********************************************************************************************/
uint16_t ADS_Read_AD(u8 CMD)	;
/******************************************************************************************
*函数名：Touch_GetPhyXY()
* 参数：void
* 返回值：uint8_t 返回1表示成功，返回0表示失败
* 功能：读取XY物理位置
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void);
void Touch_Painting(void);//画画
#endif


