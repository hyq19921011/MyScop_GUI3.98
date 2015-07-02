#include <stdlib.h>
#include "touch.h" 
#include "TFTlcd.h"
#include "delay.h"	
#include "usart.h"

uint16_t Phy_X,Phy_Y;                        //触笔的物理坐标
uint16_t ADC_X,ADC_Y;                        //触笔的ADC值
float    xoff,yoff,xfac,yfac;                //触笔的校正值及换算比值



/******************************************************************************************
*函数名：ADS_Write_Byte(u8 num)
* 参数：u8 num 
* 返回值：无
* 功能：向7843写入1byte数据 
*********************************************************************************************/  
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;  
	
	for(count=0;count<8;count++)  
	{ 
		if(num&0x80)SET_TDIN(); 
		else CLR_TDIN();   
		num<<=1;    
		CLR_TCLK();//上升沿有效	  
		SET_TCLK();      
	} 			    
} 	


/******************************************************************************************
*函数名：ADS_Read_AD(u8 CMD)	
* 参数：u8 CMD
* 返回值：无
* 功能：从7846/7843/XPT2046/UH7843/UH7846读取adc值
*********************************************************************************************/   
uint16_t ADS_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 	
	CLR_TCLK();         //先拉低时钟 
    //CLR_TDIN();         //拉低数据线
	CLR_TCS();          //选中ADS7843	 
	ADS_Write_Byte(CMD);//发送命令字
	delay_nus(6);       //ADS7846的转换时间最长为6us
    delay_nus(1);    
	SET_TCLK();         //给1个时钟，清除BUSY   	    
	CLR_TCLK(); 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
				
		CLR_TCLK();       //下降沿有效
        SET_TCLK(); 
		if(TDOUT_IN())Num++; 		 
	}  	 
	Num>>=4;            //只有高12位有效.
	SET_TCS();          //释放ADS7843	 	
	return(Num); 
}

#if !HORIZONTAL
/******************************************************************************************
*函数名：AdjustTouchPanel()
* 参数：void
* 返回值：void
* 功能：触摸屏校准函数
*********************************************************************************************/
void AdjustTouchPanel(void)
{
	uint16_t touchXY[5][2];
	uint8_t cnt=0;
	uint16_t temp1,temp2,temp3;
	
	LCD_Clear(WHITE);                                //清屏
	LCD_SetColors(BLUE,WHITE);                       //设置前景色和背景色
	DrawAdjustPoint(20,20,BLUE);                          //绘制第一个准星
	LCD_DrawString(40,50,"Touch Sight");
	while(1)
		{
			if(PEN_State()==RESET)
				{
				 	if(Touch_GetPhyXY())
						{
							touchXY[cnt][0]=ADC_X;             //获取X的ADC值
							touchXY[cnt][1]=ADC_Y;             //获取y的ADC值
							
                            //printf("%4d  \n\r",ADC_X);//touchXY[%d][0]=
                            //printf("%d",cnt,); touchXY[%d][1]=
                            while(PEN_State()==RESET);           //等待触笔松开
							delay_nms(50);                     //消抖
							cnt++;                             //跳转到下个点
						}
label:				switch(cnt)
						{
							case 0:
								DrawAdjustPoint(160,240,WHITE);        //清除第5个准星位置
								DrawAdjustPoint(20,20,BLUE);          //绘制第一个准星
							 break;
							case 1:
                                LCD_DrawNum(0,20,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(70,20,touchXY[cnt-1][1],4);
								DrawAdjustPoint(20,20,WHITE);             //清除第1个准星位置
								DrawAdjustPoint(20,460,BLUE);          //绘制第二个准星
							  break;
							case 2:
                                LCD_DrawNum(0,460,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(70,460,touchXY[cnt-1][1],4);
								DrawAdjustPoint(20,460,WHITE);          //清除第2个准星位置
								DrawAdjustPoint(300,460,BLUE);        //绘制第三个准星  
							  break;
							case 3:
                                LCD_DrawNum(170,460,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(240,460,touchXY[cnt-1][1],4);
								DrawAdjustPoint(300,460,WHITE);         //清除第3个准星位置
								DrawAdjustPoint(300,20,BLUE);          //绘制第四个准星
							  break;
							case 4:
                                LCD_DrawNum(170,20,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(240,20,touchXY[cnt-1][1],4);
								DrawAdjustPoint(300,20,WHITE);          //清除第4个准星位置
								DrawAdjustPoint(160,240,BLUE);        //绘制第五个准星
							  break;
							case 5:
                                LCD_DrawNum(90,180,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(150,180,touchXY[cnt-1][1],4);
								 temp1=touchXY[3][0]-touchXY[0][0];                               //获取上方两点的X的ADC值的差
                                 temp2=touchXY[2][0]-touchXY[1][0];                               //获得下方两点的X的ADC值的差
								 if(ABS(temp1-temp2)>ADJUST_DE)                                    //对比偏差是否大于阀值，若大于则不符合
									 {
                                        cnt=0;                                                          //归零
                                        goto label;                                                     //跳转重新开始
									 }
									 temp3=temp1+temp2;                                              //将上下X的ADC的值的差值相加
									 xfac=280.0/(temp3>>1);//右移n位除以2的n次方                                          //获取x的比值
									 temp3=temp3>>2;                                                 //将上下X的ADC的值的差值相加除以4和中心点的X的ADC值对比
								  if(ABS(touchXY[4][0]-touchXY[0][0]-temp3)>ADJUST_DE)             //对比偏差是否大于阀值，若大于则不符合
									 {
                                        cnt=0;                                                          //归零
                                        goto label;                                                     //跳转重新开始
									 }
							   temp1=touchXY[0][1]-touchXY[1][1];  //这里也是                               //获取左边两点的Y的ADC值的差
							   temp2=touchXY[3][1]-touchXY[2][1]; //这里 我用了 abs函数                                //获取右边两点的Y的ADC值的差
								 if(ABS(temp1-temp2)>ADJUST_DE)                                     //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                           //归零
							     goto label;                                                      //跳转重新开始
									 }
									 temp3=temp1+temp2;                                               //将左右Y的ADC差相加
									 yfac = 440.0/(temp3>>1);                                         //获取y的比值
									 temp3=temp3>>2;                                                  //将左右Y的ADC差的和除以4和中心点的Y的ADC值对比
								  if(ABS(touchXY[0][1]-touchXY[4][1]-temp3)>ADJUST_DE)              //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                           //归零
							     goto label;                                                      //跳转重新开始
									 }
									 xoff=(320-xfac*(touchXY[0][0]+touchXY[3][0]))/2;                 //获取到X的修正值
									 yoff=(480-yfac*(touchXY[0][1]+touchXY[1][1]))/2;                 //获取到Y的修正值
									 LCD_DrawString(40,180," Finish");                                //校验完成
									 delay_nms(500);                                                  //延时
									 return ;                                                         //返回
						}
				}
		}
}
/******************************************************************************************
*函数名：Touch_GetPhyXY()
* 参数：void
* 返回值：uint8_t 返回1表示成功，返回0表示失败
* 功能：读取XY物理位置
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void)
{
	u8  i, j, k, min;
	u16 temp;
	u16 tempXY[2][SAMP_CNT], XY[2];// 采样
	
	for(i=0; i<SAMP_CNT; i++)
	{
				tempXY[0][i] = ADS_Read_AD(CMD_RDX);
                //LCD_DrawNum(70,150,ADC_X,5);      //输出X坐标
				tempXY[1][i] = ADS_Read_AD(CMD_RDY);
                //LCD_DrawNum(150,150,ADC_Y,5);
	}
	// 滤波
	for(k=0; k<2; k++)
	{ 
		// 降序排列
		for(i=0; i<SAMP_CNT-1; i++)
		{
			min=i;
			for (j=i+1; j<SAMP_CNT; j++)
			{
				if (tempXY[k][min] > tempXY[k][j]) 
					min=j;
				}
				temp = tempXY[k][i];
				tempXY[k][i] = tempXY[k][min];
				tempXY[k][min] = temp;
		}
	 // 设定阈值
		if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
			return 0;
	// 求中间值的均值
		XY[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1])>>1;
	}
		if(XY[0]==0||XY[1]==2047)
			{
                
				return 0;
			}
		ADC_X=XY[0];                             //他们是原始坐标 物理坐标
		ADC_Y=XY[1];
		// 矫正坐标
		Phy_X = (uint16_t)(ADC_X*xfac+xoff);     // xfac 0.17878    xoff -23.7
		Phy_Y = 480-(uint16_t)(ADC_Y*yfac+yoff); //yfac 0.24985    yoff     -13.1
		return 1;
}
#else
/******************************************************************************************
*函数名：AdjustTouchPanel()
* 参数：void
* 返回值：void
* 功能：触摸屏校准函数
*********************************************************************************************/
void AdjustTouchPanel(void)
{
	uint16_t touchXY[5][2];
	uint8_t cnt=0;
	uint16_t temp1,temp2,temp3;
	
	LCD_Clear(WHITE);                                //清屏
	LCD_SetColors(BLUE,WHITE);                       //设置前景色和背景色
	DrawAdjustPoint(20,20,BLUE);                          //绘制第一个准星
	LCD_DrawString(220,40,"Touch Sight");
	while(1)
		{
			if(PEN_State()==RESET)
				{
				 	if(Touch_GetPhyXY())
						{
							touchXY[cnt][0]=ADC_X;             //获取X的ADC值
							touchXY[cnt][1]=ADC_Y;             //获取y的ADC值
							
                            //printf("%4d  \n\r",ADC_X);//touchXY[%d][0]=
                            //printf("%d",cnt,); touchXY[%d][1]=
                            while(PEN_State()==RESET);           //等待触笔松开
							delay_nms(50);                     //消抖
							cnt++;                             //跳转到下个点
						}
label:				switch(cnt)
						{
							case 0:
								DrawAdjustPoint(240,160,WHITE);        //清除第5个准星位置
								DrawAdjustPoint(20,20,BLUE);          //绘制第一个准星
							 break;
							case 1:
                                LCD_DrawNum(0,20,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(70,20,touchXY[cnt-1][1],4);
                                DrawAdjustPoint(20,20,WHITE);             //清除第1个准星位置
								DrawAdjustPoint(20,300,BLUE);          //绘制第二个准星
							  break;
							case 2:
                                LCD_DrawNum(0,300,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(70,300,touchXY[cnt-1][1],4);
								DrawAdjustPoint(20,300,WHITE);          //清除第2个准星位置
								DrawAdjustPoint(460,300,BLUE);        //绘制第三个准星  
							  break;
							case 3:
                                LCD_DrawNum(330,300,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(400,300,touchXY[cnt-1][1],4);
								DrawAdjustPoint(460,300,WHITE);         //清除第3个准星位置
								DrawAdjustPoint(460,20,BLUE);          //绘制第四个准星
							  break;
							case 4:
                                LCD_DrawNum(330,20,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(400,20,touchXY[cnt-1][1],4);
								DrawAdjustPoint(460,20,WHITE);          //清除第4个准星位置
								DrawAdjustPoint(240,160,BLUE);        //绘制第五个准星
							  break;
							case 5:
                                LCD_DrawNum(200,180,touchXY[cnt-1][0],4);      //输出X坐标
                                LCD_DrawNum(270,180,touchXY[cnt-1][1],4);
								 temp1=touchXY[3][1]-touchXY[0][1];                               //获取上方两点的X的ADC值的差
                                 temp2=touchXY[2][1]-touchXY[1][1];                               //获得下方两点的X的ADC值的差
								 if(ABS(temp1-temp2)>ADJUST_DE)                                    //对比偏差是否大于阀值，若大于则不符合
									 {
                                        cnt=0;                                                          //归零
                                        goto label;                                                     //跳转重新开始
									 }
									 temp3=temp1+temp2;                                              //将上下X的ADC的值的差值相加
									 xfac=440.0/(temp3>>1);//右移n位除以2的n次方                                          //获取x的比值
									 temp3=temp3>>2;                                                 //将上下X的ADC的值的差值相加除以4和中心点的X的ADC值对比
								  if(ABS(touchXY[4][0]-touchXY[0][0]-temp3)>ADJUST_DE)             //对比偏差是否大于阀值，若大于则不符合
									 {
                                        cnt=0;                                                          //归零
                                        goto label;                                                     //跳转重新开始
									 }
							   temp1=touchXY[1][0]-touchXY[0][0];                                 //获取左边两点的Y的ADC值的差
							   temp2=touchXY[2][0]-touchXY[3][0];                                 //获取右边两点的Y的ADC值的差
								 if(ABS(temp1-temp2)>ADJUST_DE)                                     //对比偏差是否大于阀值，若大于则不符合
									 {
                                        cnt=0;                                                           //归零
                                        goto label;                                                      //跳转重新开始
									 }
									 temp3=temp1+temp2;                                               //将左右Y的ADC差相加
									 yfac = 280.0/(temp3>>1);                                         //获取y的比值
									 temp3=temp3>>2;                                                  //将左右Y的ADC差的和除以4和中心点的Y的ADC值对比
								  if(ABS(touchXY[4][1]-touchXY[0][1]-temp3)>ADJUST_DE)              //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                           //归零
							     goto label;                                                      //跳转重新开始
									 }
									 xoff=(480-xfac*(touchXY[0][1]+touchXY[3][1]))/2;                 //获取到X的修正值
									 yoff=(320-yfac*(touchXY[0][0]+touchXY[1][0]))/2;                 //获取到Y的修正值
									 LCD_DrawString(40,180," Finish");                                //校验完成
									 delay_nms(500);                                                  //延时
									 return ;                                                         //返回
						}
				}
		}
}
/******************************************************************************************
*函数名：Touch_GetPhyXY()
* 参数：void
* 返回值：uint8_t 返回1表示成功，返回0表示失败
* 功能：读取XY物理位置
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void)
{
	u8  i, j, k, min;
	u16 temp;
	u16 tempXY[2][SAMP_CNT], XY[2];// 采样
	
	for(i=0; i<SAMP_CNT; i++)
	{
				tempXY[0][i] = ADS_Read_AD(CMD_RDX);
                //LCD_DrawNum(70,150,ADC_X,5);      //输出X坐标
				tempXY[1][i] = ADS_Read_AD(CMD_RDY);
                //LCD_DrawNum(150,150,ADC_Y,5);
	}
	// 滤波
	for(k=0; k<2; k++)
	{ 
		// 降序排列
		for(i=0; i<SAMP_CNT-1; i++)
		{
			min=i;
			for (j=i+1; j<SAMP_CNT; j++)
			{
				if (tempXY[k][min] > tempXY[k][j]) 
					min=j;
				}
				temp = tempXY[k][i];
				tempXY[k][i] = tempXY[k][min];
				tempXY[k][min] = temp;
		}
	 // 设定阈值
		if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
			return 0;
	// 求中间值的均值
		XY[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1])>>1;
	}
		if(XY[0]==0||XY[1]==2047)
			{
                
				return 0;
			}
		ADC_X=XY[0];                             //他们是原始坐标 物理坐标
		ADC_Y=XY[1];
		// 矫正坐标
// 		Phy_X = (uint16_t)(ADC_Y*xfac+xoff);     //这两个是按下触摸后对应液晶屏上的坐标  逻辑坐标 
// 		Phy_Y = (uint16_t)(ADC_X*yfac+yoff); 
          Phy_X = (uint16_t)(ADC_Y*0.25-14.37);     //这两个是按下触摸后对应液晶屏上的坐标  逻辑坐标 
		Phy_Y = (uint16_t)(ADC_X*0.18-17.35);
		return 1;
}
#endif

/******************************************************************************************
*函数名：DrawAdjustPoint()
* 参数：uint16_t x,uint16_t y xy坐标
* 返回值：void
* 功能：绘制触摸校准时使用的准星
*********************************************************************************************/
void DrawAdjustPoint(uint16_t x,uint16_t y,uint16_t text_color)
{ 
    LCD_SetTextColor(text_color);
	/*绘制中心点*/
	DrawPixel(x,y);
	DrawPixel(x-1,y);
	DrawPixel(x,y-1);
	DrawPixel(x-1,y-1);
	DrawPixel(x+1,y-1);
	DrawPixel(x,y+1);
    DrawPixel(x-1,y+1);
	DrawPixel(x+1,y+1);
	DrawPixel(x+1,y);
	/*绘制两个同心圆*/
	LCD_DrawCircle(x,y,12);
	LCD_DrawCircle(x,y,10);
	/*绘制十字线*/
	LCD_DrawHLine(x-14,y,28);
	LCD_DrawVLine(x,y-14,28);
	/*绘制小方框*/
	LCD_DrawRect(x-3,y-3,6,6);
}


/******************************************************************************************
*函数名：DrawTouchPoint()
* 参数：uint16_t x,uint16_t y xy坐标
* 返回值：void
* 功能：绘制触摸笔笔尖的点
*********************************************************************************************/
void DrawTouchPoint(uint16_t x,uint16_t y)
{
	DrawPixel(x,y);                  //绘制中心点
	/*在四周分别绘制点*/
	DrawPixel(x-1,y);                
	DrawPixel(x,y-1);
	DrawPixel(x-1,y-1);
	DrawPixel(x+1,y-1);
	DrawPixel(x,y+1);
    DrawPixel(x-1,y+1);
	DrawPixel(x+1,y+1);
	DrawPixel(x+1,y);
}


/******************************************************************************************
*函数名：Touch_Init()
* 参数：void
* 返回值：void
* 功能：触摸芯片初始化
*********************************************************************************************/
void Touch_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  	GPIO_SetBits(GPIOD, GPIO_Pin_12);                                  //TCS不选中 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;                  //pen_int:PD13   
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//设置为上拉
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                          //DOUT PA0  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void Touch_Painting(void)
{
label1:	
	LCD_Clear(WHITE);
//LCD_brush_screen();    //刷屏   
	//LCD_DrawString(30,300,"X:");            //输出字符
	//LCD_DrawString(140,300,"Y:");           //输出字符
    LCD_SetColors(RED,RED);              //设置画笔的前景色和背景色
	LCD_DrawString(  0,0,"  ");            //输出字符
    LCD_SetColors(GREEN,GREEN);
	LCD_DrawString( 0,24,"  ");          //输出字符
    LCD_SetColors(BLUE,BLUE);
	LCD_DrawString(0,48,"  ");            //输出字符
    LCD_SetColors(BLACK,BLACK);
    LCD_DrawString(0,72,"  ");
    LCD_SetColors(BLACK,YELLOW);    //输出字符
	LCD_DrawString(0,96,"RST");            //输出字符
    while(1)
    {
        if(PEN_State()==RESET)                //检测是否按下画笔
		{
			if(Touch_GetPhyXY())								//获取画笔位置
			{              
				DrawTouchPoint(Phy_X,Phy_Y);      //画点
               
				//LCD_DrawNum(70,300,Phy_X,3);      //输出X坐标
				//LCD_DrawNum(180,300,Phy_Y,3);     //输出Y坐标 
				
				if((Phy_X>  0&&Phy_X< 35)&&(Phy_Y>0&&Phy_Y<24)){
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,0,"  ");
                    delay_nms(50);
                    LCD_SetColors(  RED,RED);  //改变画笔颜色
                    LCD_DrawString(  0,0,"  ");
                    }
				if((Phy_X> 0&&Phy_X<35)&&(Phy_Y>24&&Phy_Y<48)) {
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,24,"  ");
                    delay_nms(50);
                    LCD_SetColors(GREEN,GREEN); //改变画笔颜色
                    LCD_DrawString(  0,24,"  ");
                    } 
				if((Phy_X> 0&&Phy_X<35)&&(Phy_Y>48&&Phy_Y<72)) {
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,48,"  ");
                    delay_nms(50);
                    LCD_SetColors(BLUE,BLUE);
                    LCD_DrawString(  0,48,"  ");
                     }
                if((Phy_X>0&&Phy_X<48)&&(Phy_Y>72&&Phy_Y<96))  {
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,72,"  ");
                    delay_nms(50);
                    LCD_SetColors(BLACK,BLACK);
                    LCD_DrawString(  0,72,"  ");
                    }
                if((Phy_X>0&&Phy_X<48)&&(Phy_Y>96&&Phy_Y<120))  //判断是否按下清屏
				{   
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,96,"   ");
                    delay_nms(50);
					goto label1;                                   //返回到清屏位置
				}
			}
		}
    }        
}

void Change_pwm() //调节背光亮度的
{
   if(PEN_State()==RESET)
   {
       if(Touch_GetPhyXY())
       {
           if((Phy_X>  0&&Phy_X< 35)&&(Phy_Y>0&&Phy_Y<24))
           {
                
           }
       }
   }   
}