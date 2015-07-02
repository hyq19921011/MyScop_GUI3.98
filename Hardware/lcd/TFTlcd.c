#include "TFTlcd.h"
#include "usart.h"
#include "GBK2312.h"
#include <stdio.h>
#include "pwm.h"
#include <math.h>
/*******************液晶屏硬件连接**********************

PD14 -----FSMC_D0  ----D0
PD15 -----FSMC_D1  ----D1
PD0   -----FSMC_D2  ----D2
PD1   -----FSMC_D3  ----D3
PE7    -----FSMC_D4  ---D4
PE8    -----FSMC_D5  ---D5
PE9    -----FSMC_D6  ---D6
PE10  -----FSMC_D7   ----D7
PE11  -----FSMC_D8   ----D8
PE12  -----FSMC_D9   ----D9
PE13  -----FSMC_D10   ----D10
PE14  -----FSMC_D11   ----D11
PE15  -----FSMC_D12   ----D12
PD8   -----FSMC_D13   ----D13
PD9   -----FSMC_D14   ----D14
PD10 -----FSMC_D15   ----D15
PD4   -----FSMC_NOE -----RD
PD5   -----FSMC_NWE ----WR
PD7    -----FSMC_NE1  ----CS
PD11 -----FSMC_A16   ----RS

PB5-------------------LCD_LED
PB4-------------------LCD_RST
***************************************************************/
/* 选择BANK1-NORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
 * FSMC_A16 接LCD的DC(寄存器/数据选择)脚
 * 寄存器基地址 = 0X60000000
 * RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * 这里主要用于控制RS进行写寄存器写数据操作，因为板子的液晶RS接在A16管脚，所以要使地址线A16为高电平，则写地址到2的16次方处，并且因为液晶是16bit的
 *所以再乘上2，得出的地址加上BANK所在的地址即可。如RS接A0处，则RAM基地址为bank所在地址加上2的0次方的2倍，如是8bit屏则不需要乘以2
 */

/**************************************************************************************************************
 * 函数名：GPIO_AF_FSMC_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD连接的GPIO复用连接到FSMC
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void GPIO_AF_FSMC_Config(void)
{
  /*配置GPIOD相应的管脚复用连接到FSMC*/
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);	    //配置GPIOD的Pin0复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);			//配置GPIOD的Pin1复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);			//配置GPIOD的Pin4复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);			//配置GPIOD的Pin5复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);			//配置GPIOD的Pin7复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);			//配置GPIOD的Pin8复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);			//配置GPIOD的Pin9复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);		//配置GPIOD的Pin10复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);		//配置GPIOD的Pin11复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);		//配置GPIOD的Pin14复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);		//配置GPIOD的Pin15复用连接到FSMC
   /*配置GPIOE相应的管脚复用连接到FSMC*/
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);			//配置GPIOE的Pin7复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);			//配置GPIOE的Pin8复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);			//配置GPIOE的Pin9复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);		//配置GPIOE的Pin10复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);		//配置GPIOE的Pin11复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);		//配置GPIOE的Pin12复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	    //配置GPIOE的Pin13复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);		//配置GPIOE的Pin14复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);		//配置GPIOE的Pin15复用连接到FSMC
}
/**************************************************************************************************************
 * 函数名：LCD_GPIO_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的GPIO管脚与FSMC连接
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;		                                              //定义GPIO初始化结构体
     
    /* 使能FSMC的AHB3时钟，这里注意，不同芯片总线会不同 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    
    /* 使能GPIOB，GPIOD，GPIOE的AHB1时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE , ENABLE);
   	 /*配置液晶屏的背光控制管脚*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;											  //设置管脚模式为输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //设置翻转速度为100Mhz，M4就是快啊
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											  //设置输出类型为推挽输出
    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4);									//设置管脚为Pin_5,根据自己液晶接法设置
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  //设置上/下拉电阻为无上拉下拉电阻		
    GPIO_Init(GPIOB, &GPIO_InitStructure);													  //初始化GPIOB的Pin_5
    		   
    /* 配置基于FSMC的液晶屏的数据线
	 * 液晶屏数据线：FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //设置翻转速度为100Mhz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;											  //设置管脚模式为复用模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8  | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                  
    GPIO_Init(GPIOD, &GPIO_InitStructure);													  //初始化GPIOB的Pin_0，Pin_，Pin_2，
	                                                                    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* 配置基于FSMC的液晶屏控制线
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    GPIO_AF_FSMC_Config();				             //在F407中必须加上这个复用连接配置，否则不能连接到FSMC
    GPIO_SetBits(GPIOB, GPIO_Pin_4);			 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 
    	
}

/**************************************************************************************************************
 * 函数名：LCD_FSMC_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的FSMC
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_FSMC_Config(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  p;
	  LCD_GPIO_Config();                                                     //LCD管脚配置
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 1;                                           //地址建立时间
  	p.FSMC_AddressHoldTime = 0;                                            //地址保持时间
  	p.FSMC_DataSetupTime = 6;                                              //数据建立时间
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
  	p.FSMC_AccessMode = FSMC_AccessMode_A;                                  // 用模式A控制LCD
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   
  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 
}
//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(uint8_t regval)
{ 
	LCD_REG=regval;//写入要写的寄存器序号	 
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(uint16_t data)
{										    	   
	LCD_RAM=data;		 
}

/**************************************************************************************************************
 * 函数名：LCD_WriteReg()
 * 输入  ：uint8_t LCD_Reg 寄存器地址, uint16_t LCD_RegValue 寄存器的值
 * 输出  ：void
 * 描述  ：写LCD寄存器函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /*写寄存器地址*/
  LCD_REG = LCD_Reg;
  /*写入寄存器值*/
  LCD_RAM = LCD_RegValue;
}
/**************************************************************************************************************
 * 函数名：LCD_ReadReg()
 * 输入  ：uint8_t LCD_Reg 需要读取的寄存器地址
 * 输出  ：uint16_t 寄存器的值
 * 描述  ：读取LCD寄存器的值
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /*写寄存器地址*/
  LCD_REG = LCD_Reg;
  delay_nus(1);
  /*读出寄存器值并返回*/
  return (LCD_RAM);
}


/**************************************************************************************************************
 * 函数名：LCD_WriteRAM()
 * 输入  ：uint16_t RGB_data  16bit颜色数据
 * 输出  ：void
 * 描述  ：向LCD的RAM写数据
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_WriteRAM(uint16_t RGB_data)
{
  /*向LCD的RAM写数据*/
  LCD_RAM = RGB_data;
}

/**************************************************************************************************************
 * 函数名：LCD_ReadRAM()
 * 输入  ：void
 * 输出  ：uint16_t  16bit颜色值
 * 描述  ：读取LCD的RAM数据
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t LCD_ReadRAM(void)
{ 
  LCD_REG = 0x2E; 
  /*读取16bit RAM值*/
  return LCD_RAM;
}

/**************************************************************************************************************
 * 函数名：LCD_Init()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD初始化函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void TFTLCD_Init(void)
{       
        //LCD_GPIO_Config();
        //GPIO_AF_FSMC_Config();
        LCD_FSMC_Config();
            
        delay_nms(50); 
        LCD_RST(On);
        delay_nms(100);
        LCD_RST(Off);
        delay_nms(50);
        LCD_RST(On);
//        PWM_Set_Duty_Period(80);
        LCD_WR_REG(0x0011); //推出睡眠模式使能个功能模块 
        delay_nms(50); // delay 50 ms 		
		LCD_WR_REG(0x0013);  //进入正常模式
        delay_nms(50);
        
		LCD_WR_REG(0xD0); //设置电源参数
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x45);
		LCD_WR_DATA(0x18);

		LCD_WR_REG(0xD1); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x07);
		LCD_WR_DATA(0x10);

		LCD_WR_REG(0xD2); 
		LCD_WR_DATA(0x01); 
		LCD_WR_DATA(0x11);

		LCD_WR_REG(0xC0); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x3B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x02);
		LCD_WR_DATA(0x11);

		LCD_WR_REG(0xC5); 
		LCD_WR_DATA(0x03); 

		LCD_WR_REG(0xC8); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x32);
		LCD_WR_DATA(0x36);
        LCD_WR_DATA(0x45);
		LCD_WR_DATA(0x06);
		LCD_WR_DATA(0x16);
		LCD_WR_DATA(0x37);
		LCD_WR_DATA(0x75);
		LCD_WR_DATA(0x77);
		LCD_WR_DATA(0x54);
		LCD_WR_DATA(0x0C);
		LCD_WR_DATA(0x00);

       #if HORIZONTAL    //                     8 4 2 1 8 4 2 1
        LCD_WR_REG(0x36);//设置显示模式         7 6 5 4 3 2 1 0
        LCD_WR_DATA(0xaa);//竖屏0x0A,heng屏0x38 1 0 1 1 1 0 1 0
                
       #else
        LCD_WR_REG(0x36);//设置显示模式
        LCD_WR_DATA(0x0a);//竖屏0x0A,heng屏0x38
       
		#endif
        LCD_WR_REG(0x3A);
		LCD_WR_DATA(0x55);

		LCD_WR_REG(0x2A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3F);

		LCD_WR_REG(0x2B);//
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0xDF);
        delay_nms(50); // delay 50 ms 
          
        LCD_WR_REG(0x29); //display on 0x28:display off      
    
	//LCD_SetMode(2);
	LCD_Clear(WHITE);
	LCD_SetFont(&Font16x24);
								 
}
/**************************************************************************************************************
 * 函数名：LCD_SetMode()
 * 输入  ：uint8_t mode 设置屏幕刷屏模式
 * 输出  ：void
 * 描述  ：设置LCD刷屏模式
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetMode(uint8_t mode)
{
	switch(mode)//常用4(横屏)和7(竖屏)
	{
		case 1:LCD_WriteReg(0x36,0x0a);break;//向左向上:←↖
		case 2:LCD_WriteReg(0x36,0x38);break;//向上向左:↑↖
        
// 		case 3:LCD_WriteReg(0x36,0x1010);break;//向右向上:→↗
// 		case 4:LCD_WriteReg(0x36,0x1018);break;//向上向右:↑↗
// 		case 5:LCD_WriteReg(0x36,0x1020);break;//向左向下:←↙
// 		case 6:LCD_WriteReg(0x36,0x1028);break;//向下向左:↓↙
// 		case 7:LCD_WriteReg(0x36,0x1030);break;//向右向下:→↘
// 		case 8:LCD_WriteReg(0x36,0x1038);break;//向下向右:↓↘
	}
}
/**************************************************************************************************************
 * 函数名：LCD_Clear()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD清屏函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_Clear(uint16_t Color)
{
  uint32_t i,j;

#if HORIZONTAL//LCD_RAM=
  LCD_REG = 0x002A;

	LCD_RAM = 0;
	LCD_RAM = 0;
	LCD_RAM = 0x0001;
	LCD_RAM = 0x00DF;//480

	LCD_REG = 0x002B;
         
	LCD_RAM = 0;
	LCD_RAM = 0;                 //设定X坐标
	LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320
	#else
	 LCD_REG = 0x002A;
	LCD_RAM = 0;
  LCD_RAM = 0;                 //设定X坐标
  LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320

  LCD_REG = 0x002B;
	LCD_RAM = (479-0)>>8;
  LCD_RAM = 0x00FF&(479-0);           //设定Y坐标
  LCD_RAM = 0x0001;
	LCD_RAM = 0x00DF;//480
	
	#endif
  LCD_WR_REG(0x2C); /*开始写GRAM */
  for(i = 0; i < (320*480); i++)
  {
		LCD_RAM=Color;__NOP();__NOP();
  } 
}

/**************************************************************************************************************
 * 函数名：LCD_SetCursor()
 * 输入  ：uint16_t Xpos, uint16_t Ypos 设定的屏幕的X值和Y值
 * 输出  ：void
 * 描述  ：LCD设置光标位置函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
#if HORIZONTAL//LCD_RAM=
  LCD_REG = 0x002A;

	LCD_RAM = (479 - Xpos)>>8;
	LCD_RAM = 0x00FF&(479 - Xpos);
	LCD_RAM = (479 - Xpos)>>8;
	LCD_RAM = 0x00FF&(479 - Xpos);

	LCD_REG = 0x002B;
         
	LCD_RAM = Ypos>>8;
	LCD_RAM = 0x00FF&Ypos;                 //设定X坐标
	LCD_RAM = Ypos>>8;
	LCD_RAM = 0x00FF&Ypos; 
	#else
	 LCD_REG = 0x002A;
	LCD_RAM = Xpos>>8;
  LCD_RAM = 0x00FF&Xpos;                 //设定X坐标
  LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320

  LCD_REG = 0x002B;
	LCD_RAM = (479-Ypos)>>8;
  LCD_RAM = 0x00FF&(479-Ypos);           //设定Y坐标
  LCD_RAM = 0x0001;
	LCD_RAM = 0x00DF;//480
	
	#endif
	//LCD_REG = 0x2C;
}
/**************************************************************************************************************
 * 函数名：LCD_SetWindow()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width 区域的起点，宽和高
 * 输出  ：void
 * 描述  ：设置某个特定的填充区域
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint8_t Height)
{
  /*设置水平方向X开始坐标*/
  if(Xpos >= Height)
  {
    LCD_WriteReg(0x2A, (Xpos - Height + 1));
  }
  else
  {
    LCD_WriteReg(0x2A, 0);
  }
  /*设置水平方向X结束坐标*/
  LCD_WriteReg(0x2A, Xpos);
  /*设置竖直方向Y开始坐标*/
  if(Ypos >= Width)
  {
    LCD_WriteReg(0x2B, (Ypos - Width + 1));
  }  
  else
  {
    LCD_WriteReg(0x2B, 0);
  }
  /*设置竖直方向Y结束坐标*/
  LCD_WriteReg(0x2B, Ypos);
  LCD_SetCursor(Xpos, Ypos);
}
/**************************************************************************************************************
 * 函数名：LCD_SetColors()
 * 输入  ：_TextColor 前景色,_BackColor 背景色
 * 输出  ：void
 * 描述  ：设置LCD的前景色和背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor; 
  BackColor = _BackColor;
}
/**************************************************************************************************************
 * 函数名：LCD_GetColors()
 * 输入  ：*_TextColor 前景色的指针,*_BackColor 背景色的指针
 * 输出  ：void
 * 描述  ：获取LCD的前景色和背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}
/**************************************************************************************************************
 * 函数名：LCD_SetTextColor()
 * 输入  ：uint16_t Color 前景色
 * 输出  ：void
 * 描述  ：设置LCD的前景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}


/**************************************************************************************************************
 * 函数名：LCD_SetBackColor()
 * 输入  ：uint16_t Color 背景色
 * 输出  ：void
 * 描述  ：设置LCD的背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}
/**************************************************************************************************************
 * 函数名：LCD_SetFont()
 * 输入  ：sFONT *fonts 要设置的字体
 * 输出  ：void
 * 描述  ：设置LCD的字体
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}
/**************************************************************************************************************
 * 函数名：LCD_GetFont()
 * 输入  ：void
 * 输出  ：sFONT * 获取字体
 * 描述  ：设置LCD的字体
 * 调用  ：外部调用        
 *****************************************************************************************************************/
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
/**************************************************************************************************************
 * 函数名：DrawPixel()
 * 输入  ：int16_t x, int16_t y  点的坐标
 * 输出  ：void
 * 描述  ：画一个象素点
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void DrawPixel(uint16_t x, uint16_t y)
{ 
  if(x < 0 || x > LCD_XMAX || y < 0 || y > LCD_YMAX)
  {
    return ;  
  }
  LCD_SetCursor(x, y);
  LCD_WriteRAM_Start(); /* Prepare to write GRAM */
  LCD_WriteRAM(TextColor);
}
void GUI_SetPoint(u16 xpos, u16 ypos,u16 color)
{

      #if HORIZONTAL//LCD_RAM=
    LCD_REG = 0x002A;

	LCD_RAM = (479 - xpos)>>8;
	LCD_RAM = 0x00FF&(479 - xpos);
	LCD_RAM = (479 - xpos)>>8;
	LCD_RAM = 0x00FF&(479 - xpos);

	LCD_REG = 0x002B;
         
	LCD_RAM = ypos>>8;
	LCD_RAM = 0x00FF&ypos;                 //设定X坐标
	LCD_RAM = ypos>>8;
	LCD_RAM = 0x00FF&ypos ;

	#else
	 LCD_REG = 0x002A;
	LCD_RAM = xpos>>8;
  LCD_RAM = 0x00FF&xpos;                 //设定X坐标
//  LCD_RAM = 0x0001;
//	LCD_RAM = 0x003F;//320

  LCD_REG = 0x002B;
	LCD_RAM = (479-ypos)>>8;
  LCD_RAM = 0x00FF&(479-ypos);           //设定Y坐标
//  LCD_RAM = 0x0001;
//	LCD_RAM = 0x00DF;//480
	
	#endif
    LCD_REG = 0x2C; /* Prepare to write GRAM */
    LCD_RAM = color;
}

u16 LCD_BGR2RGB(u16 c)
{
	u16 r,g,b,rgb;
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;
	rgb=(b<<11)+(g<<5)+(r<<0);
  	return( rgb );
}
u16 GUI_GetPoint(u16 xpos,u16 ypos)
{   
  #if HORIZONTAL//LCD_RAM=
		u16 value;
    LCD_REG = 0x002A;

	LCD_RAM = (479 - xpos)>>8;
	LCD_RAM = 0x00FF&(479 - xpos);
	LCD_RAM = (479 - xpos)>>8;
	LCD_RAM = 0x00FF&(479 - xpos);

	LCD_REG = 0x002B;
         
	LCD_RAM = ypos>>8;
	LCD_RAM = 0x00FF&ypos;                 //设定X坐标
	LCD_RAM = ypos>>8;
	LCD_RAM = 0x00FF&ypos ;
	#else
		 LCD_REG = 0x002A;
		LCD_RAM = xpos>>8;
		LCD_RAM = 0x00FF&xpos;                 //设定X坐标
		LCD_RAM = 0x0001;
		LCD_RAM = 0x003F;//320

		LCD_REG = 0x002B;
		LCD_RAM = (479-ypos)>>8;
		LCD_RAM = 0x00FF&(479-ypos);           //设定Y坐标
		LCD_RAM = 0x0001;
		LCD_RAM = 0x00DF;//480
		
	#endif
	
	LCD_REG = 0x2E;
	value = LCD_RAM;//假读	
  /*读取16bit RAM值*/
  return LCD_BGR2RGB(LCD_RAM);    
}
/**************************************************************************************************************
 * 函数名：LCD_DrawHLine()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Length 起点X和Y坐标及长度
 * 输出  ：void
 * 描述  ：画水平线
 * 调用  ：外部调用        
 *****************************************************************************************************************/
#if HORIZONTAL
void TFTLCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
	uint32_t i = 0;
    LCD_REG = 0x002A;

	LCD_RAM = (479 - Xpos - Length)>>8;
	LCD_RAM = 0x00FF&(480 - Xpos - Length);
	LCD_RAM = (479 - Xpos)>>8;
	LCD_RAM = 0x00FF&(479 - Xpos);

	LCD_REG = 0x002B;
         
	LCD_RAM = Ypos>>8;
	LCD_RAM = 0x00FF&Ypos;                 //设定X坐标
	LCD_RAM = Ypos>>8;
	LCD_RAM = 0x00FF&Ypos ;
         
    LCD_REG = 0x2C; /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
		
      LCD_RAM = TextColor;
      
    }
}
#else
void TFTLCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
	uint32_t i = 0;
    LCD_SetCursor(Xpos, Ypos);
    
    for(i = 0; i < Length; i++)
    {
	  LCD_REG = 0x2C; /* Prepare to write GRAM */
      LCD_RAM = TextColor;
      
      LCD_SetCursor(++Xpos, Ypos);
    }
}
#endif
/**************************************************************************************************************
 * 函数名：LCD_DrawVLine()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Length 起点X和Y坐标及长度
 * 输出  ：void
 * 描述  ：画垂直线
 * 调用  ：外部调用        
 *****************************************************************************************************************/
#if HORIZONTAL
void TFTLCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
	uint32_t i = 0;
    LCD_REG = 0x002A;

	LCD_RAM = (479 - Xpos)>>8;
	LCD_RAM = 0x00FF&(479 - Xpos);
	LCD_RAM = (479 - Xpos)>>8;
	LCD_RAM = 0x00FF&(479 - Xpos);

	LCD_REG = 0x002B;
         
	LCD_RAM = Ypos>>8;
	LCD_RAM = 0x00FF&Ypos;                 //设定X坐标
	LCD_RAM = (Ypos+Length)>>8;
	LCD_RAM = 0x00FF&(Ypos+Length) ;
         
    LCD_REG = 0x2C; /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
		
      LCD_RAM = TextColor;
      
    }
}
#else

#endif
/**************************************************************************************************************
 * 函数名：LCD_DrawRect()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint8_t Height 矩形左上角点的坐标及宽和高
 * 输出  ：void
 * 描述  ：画矩形函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  TFTLCD_DrawHLine(Xpos, Ypos, Width);
  TFTLCD_DrawHLine(Xpos, Ypos+ Height, Width); 
  TFTLCD_DrawVLine(Xpos, Ypos, Height);
  TFTLCD_DrawVLine(Xpos+ Width,Ypos, Height);
}
/**************************************************************************************************************
 * 函数名：LCD_DrawCircle()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Radius 圆心坐标点及半径
 * 输出  ：void
 * 描述  ：画圆函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;/* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}
/**************************************************************************************************************
 * 函数名：LCD_DrawMonoPict()
 * 输入  ：const uint32_t *Pict 画一个全屏单色的取膜数据
 * 输出  ：void
 * 描述  ：画一个单色的全屏图片函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawMonoPict(const uint32_t *Pict)
{
  uint32_t index = 0, i = 0;
  LCD_SetCursor(0, (LCD_PIXEL_WIDTH - 1)); 
  LCD_WriteRAM_Start(); /* Prepare to write GRAM */
  for(index = 0; index < 2400; index++)
  {
    for(i = 0; i < 32; i++)
    {
      if((Pict[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
  }
}
/**************************************************************************************************************
 * 函数名：LCD_FillRect()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height 填充矩形左上角点、宽和高
 * 输出  ：void
 * 描述  ：画一个填充的矩形
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void TFTLCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{ 
	uint16_t tempcolor,i;
  LCD_SetTextColor(TextColor);
  TFTLCD_DrawHLine(Xpos, Ypos, Width);
  TFTLCD_DrawHLine(Xpos, Ypos+ Height, Width);
  
  TFTLCD_DrawVLine(Xpos, Ypos, Height);
  TFTLCD_DrawVLine(Xpos+Width, Ypos, Height);

  Width --;
  Xpos++;
  tempcolor=TextColor;
  LCD_SetTextColor(BackColor);

//   while(Height--)
//   {
//     LCD_DrawHLine(Xpos, ++Ypos, Width);    
//   }
  for(i=Ypos;i<Ypos+Height;i++)
  {
       TFTLCD_DrawHLine(Xpos, i, Width); 
  }
  LCD_SetTextColor(tempcolor);
}
/**************************************************************************************************************
 * 函数名：LCD_FillCircle()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Radius 填充圆的圆心和半径
 * 输出  ：void
 * 描述  ：画一个填充圆
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;    /* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  uint16_t tempcolor;
  D = 3 - (Radius << 1);

  CurX = 0;
  CurY = Radius;
  tempcolor=TextColor;
  LCD_SetTextColor(BackColor);

  while (CurX <= CurY)
  {
    if(CurY > 0) 
    {
      TFTLCD_DrawHLine(Xpos - CurY, Ypos - CurX, 2*CurY);
      TFTLCD_DrawHLine(Xpos - CurY, Ypos + CurX, 2*CurY);
    }

    if(CurX > 0) 
    {
      TFTLCD_DrawHLine(Xpos - CurX, Ypos -CurY, 2*CurX);
      TFTLCD_DrawHLine(Xpos - CurX, Ypos + CurY, 2*CurX);
    }
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }

  LCD_SetTextColor(tempcolor);
  LCD_DrawCircle(Xpos, Ypos, Radius);
}
/**************************************************************************************************************
 * 函数名：LCD_DrawUniLine()
 * 输入  ：uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 起始点坐标和终点坐标
 * 输出  ：void
 * 描述  ：画任意方向的直线
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    DrawPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}

#if HORIZONTAL
/**************************************************************************************************************
 * 函数名：LCD_DrawChar()
 * 输入  ：const uint16_t *c   字符编码
 * 输出  ：void
 * 描述  ：LCD画一个字符
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, i = 0;
  uint16_t  Yaddress = 0;
  Yaddress = Ypos;
  
  LCD_SetCursor(Xpos, Yaddress);
  
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(i = 0; i < LCD_Currentfonts->Width; i++)
    {
      
      LCD_SetCursor(Xpos+i, Yaddress+index);
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))

      { 
          LCD_REG = 0x2C; /* Prepare to write GRAM */
      LCD_RAM = BackColor;
      }
      else
      { 
          //LCD_DrawHLine(Xpos+i, Yaddress+index，1);
      LCD_REG = 0x2C; /* Prepare to write GRAM */
      LCD_RAM = TextColor;
      } 
    }
  }
}

#else

/**************************************************************************************************************
 * 函数名：LCD_DrawChar()
 * 输入  ：const uint16_t *c   字符编码
 * 输出  ：void
 * 描述  ：LCD画一个字符
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, i = 0;
  uint16_t  Yaddress = 0;
  Yaddress = Ypos;
  
  LCD_SetCursor(Xpos, Yaddress);
  
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {

    for(i = 0; i < LCD_Currentfonts->Width; i++)
    {
  
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))

      {
        LCD_RAM = BackColor;
      }
      else
      {
        LCD_RAM = TextColor;
      } 
    }
    Yaddress++;
    LCD_SetCursor(Xpos, Yaddress);
  }
}
#endif

/**************************************************************************************************************
 * 函数名：LCD_DisplayChar()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint8_t Ascii 显示的位置和字符
 * 输出  ：void
 * 描述  ：LCD显示一个字符
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Xpos, Ypos, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}
/**************************************************************************************************************
 * 函数名：LCD_DrawString()
 * 输入  ：u16 xpos, u16 ypos, u8 *ptr 显示的位置和字符串
 * 输出  ：void
 * 描述  ：LCD显示一串字符
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DrawString(u16 xpos, u16 ypos, char *ptr)
{
  	u16 refypos=xpos;

  	while(*ptr!=0)
  	{
    	LCD_DisplayChar(refypos,ypos,*ptr);
    	refypos+=LCD_Currentfonts->Width;
    	ptr++;
  	}
}

//m^n函数
u32 mypow(u8 m,u8 n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
/**************************************************************************************************************
 * 函数名：LCD_DrawNum()
 * 输入  ：u16 x, u16 y, s32 num, u8 len   显示的位置，数字及长度
 * 输出  ：void
 * 描述  ：LCD显示一个数字
 * 调用  ：外部调用        
 *****************************************************************************************************************/	 
void LCD_DrawNum(u16 x, u16 y, s32 num, u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;
	t=0;
	if(num<0)
	{
		LCD_DisplayChar(x+(LCD_Currentfonts->Width)*t,y,'-');
		num=-num;
		t++;
	}						   
	for(;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_DisplayChar(x+(LCD_Currentfonts->Width)*t,y,' ');
				continue;
			}else enshow=1; 		 	 
		}
	 	LCD_DisplayChar(x+(LCD_Currentfonts->Width)*t,y,temp+'0'); 
	}
}
/*---------------------------------------------------------------------------
功能：画中文字符  16*16
---------------------------------------------------------------------------*/
void LCD_Show_Chinese(unsigned int row, unsigned int column, unsigned long index)
{
	unsigned char i = 0, j = 0;
	unsigned int z = row;
	LCD_SetCursor(z, j);

	for (j = 0; j < 16; j++)//列
	{
	LCD_WriteRAM_Start();//写GRAM前 先写0x002C
		for (i = 0; i < 8; i++)//前半行
		{
			if (hzdot[index] & (0x80>>i))
			{
				LCD_SetTextColor(TextColor);		
			}
			else
			{
				LCD_SetBackColor(BackColor);	
			}	
		}

		for (i = 0; i < 8; i++)//后半行
		{
			if (hzdot[index+1] & (0x80>>i))
			{
				LCD_SetTextColor(TextColor);		
			}
			else
			{
				LCD_SetBackColor(BackColor);	
			}	
		}

		index += 2;//2*16=32
		z++;
	}
    LCD_SetCursor(row+16, column);//转到下一个字符处
}

/*---------------------------------------------------------------------------
功能：显示一个中文字符  16*16
---------------------------------------------------------------------------*/
void lcd_display_hzchar(unsigned int row, unsigned int column, unsigned char ascii[2])
{
	unsigned char i = 0, j = 0;
	unsigned int z = column;
	unsigned char region   = ascii[0]-0xa0;//????  gb2312ascii[0]
	unsigned char location = ascii[1]-0xa0;//????
	unsigned long index = (94*(region-1)+location-1)*32;//?????
    LCD_SetCursor(row,z);
	for (j = 0; j < 16; j++)//?
	{
		LCD_SetCursor(row, z);

		LCD_WriteRAM_Start();//
		for (i = 0; i < 8; i++)//
		{
			if (hzdot[index] & (0x80>>i)) //DrawPixel(uint16_t x, uint16_t y);
			{
				DrawPixel(row+i, z);		
			}
			else
			{
				LCD_SetBackColor(BackColor);	
			}	
		}

		for (i; i < 16; i++)//???
		{
			if (hzdot[index+1] & (0x80>>(i-8)))
			{
				DrawPixel(row+i, z);		
			}
			else
			{
				LCD_SetBackColor(BackColor);	
			}	
		}

		index += 2;//2*16=32
		z++;
	}
    LCD_SetCursor(row+16, column);//
}
#if !HORIZONTAL //水平显示时用
/*---------------------------------------------------------------------------
  任意显示字符函数
---------------------------------------------------------------------------*/
 void LCD_Display(u16 row, u16 column, u8 *pdata)
 {
 	unsigned int i = column;
         unsigned int j = row;
 	while (*pdata)
 	{
 		if (*pdata < 128)
 		{
			LCD_DisplayChar(j, i, *pdata);
			j += LCD_Currentfonts->Width;
			pdata += 1;                               
 		}
 		else
 		{       
			lcd_display_hzchar(j, i, pdata);
		  	j += 16;
			pdata += 2;              
 		}
                 if(j>319)//
                  {
                    j=0;
                    i+=16;
                    //delay_nms(20);     //
                  }
                  if(i>479)//
                  {
                    i=0;   
                    
                    //delay_nms(5000);     //
                    LCD_Clear(YELLOW);  //
                  }
                if ((j > 304)&&(*pdata > 128)&& (j!=320))//
                 {
                   LCD_DisplayChar(j, i, ' ');//
                   i = 0 ;
                   j+=LCD_Currentfonts->Width;
                  }
              }
              LCD_SetCursor(0,0);
 	}
#else
/*---------------------------------------------------------------------------
  任意显示字符函数
---------------------------------------------------------------------------*/
 void LCD_Display(u16 row, u16 column, u8 *pdata)
 {
 	unsigned int i = column;
         unsigned int j = row;
 	while (*pdata)
 	{
 		if (*pdata < 128)
 		{
			LCD_DisplayChar(j, i, *pdata);
			j += LCD_Currentfonts->Width;
			pdata += 1;                               
 		}
 		else
 		{       
			lcd_display_hzchar(j, i, pdata);
		  	j += 16;
			pdata += 2;              
 		}
                 if(j>479)//
                  {
                    j=0;
                    i+=16;
                    //delay_nms(20);     //
                  }
                  if(i>319)//
                  {
                    i=0;   
                    
                   // delay_nms(5000);     //
                    LCD_Clear(YELLOW);  //
                  }
                if ((j > 464)&&(*pdata > 128)&& (j!=480))//
                 {
                   LCD_DisplayChar(j, i, ' ');//
                   i = 0 ;
                   j+=LCD_Currentfonts->Width;
                  }
              }
              LCD_SetCursor(0,0);
}
#endif
/**************************************************************************************************************
 * 函数名：ClearAera()
 * 输入  ：uint16_t x,uint16_t y,uint16_t w,uint16_t h   清除的位置，长度和宽度
 * 输出  ：void
 * 描述  ：LCD清除某个区域
 * 调用  ：外部调用        
 *****************************************************************************************************************/	
void ClearAera(uint16_t x,uint16_t y,uint16_t w,uint16_t h)
{
	uint16_t tempcolor;
  tempcolor = TextColor;
  LCD_SetTextColor(BackColor);
  while(h--)
  {
    TFTLCD_DrawHLine(x, y++, w);    
  }
  LCD_SetTextColor(tempcolor);
}
/**************************************************************************************************************
 * 函数名：LCD_DrawImage()
 * 输入  ：u8 *p   图片取膜数据
 * 输出  ：void
 * 描述  ：LCD画一张全屏图片
 * 调用  ：外部调用        
 *****************************************************************************************************************/	
void LCD_DrawImage(uint16_t Xpos, uint16_t Ypos,const u8 *p)
{ 
	uint16_t i,j,temp;
    uint16_t pic_width;
    uint16_t pic_high;
    pic_width=*(p+3)<<8|*(p+2);
    pic_high=*(p+5)<<8|*(p+4);
	LCD_REG = 0x002A;

	LCD_RAM = 0;
	LCD_RAM = 0;
	LCD_RAM = 0x0001;
	LCD_RAM = 0x00DF;//480

	LCD_REG = 0x002B;
         
	LCD_RAM = 0;
	LCD_RAM = 0;                 //设定X坐标
	LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320
    LCD_WriteRAM_Start(); /* Prepare to write GRAM */
	for(i=Ypos;i<pic_high+Ypos;i++) //480
		{
			for(j=Xpos;j< pic_width+Xpos;j++)
				{
					LCD_RAM=(uint16_t)(*(p+1+8)<<8|*(p+8));
					p+=2;
				}
		}
}
//刷屏测试
void LCD_brush_screen()
{
    uint16_t COLOR[]={WHITE,BLACK,RED,GREEN,BLUE,YELLOW,BROWN,GRAY,DARKBLUE};
    uint8_t i;
    for(i=0;i<9;i++)
        {
            LCD_Clear(COLOR[i]);
            delay_nms(500);
        }
}



//END
