#include <stdlib.h>
#include "touch.h" 
#include "TFTlcd.h"
#include "delay.h"	
#include "usart.h"

uint16_t Phy_X,Phy_Y;                        //���ʵ���������
uint16_t ADC_X,ADC_Y;                        //���ʵ�ADCֵ
float    xoff,yoff,xfac,yfac;                //���ʵ�У��ֵ�������ֵ



/******************************************************************************************
*��������ADS_Write_Byte(u8 num)
* ������u8 num 
* ����ֵ����
* ���ܣ���7843д��1byte���� 
*********************************************************************************************/  
void ADS_Write_Byte(u8 num)    
{  
	u8 count=0;  
	
	for(count=0;count<8;count++)  
	{ 
		if(num&0x80)SET_TDIN(); 
		else CLR_TDIN();   
		num<<=1;    
		CLR_TCLK();//��������Ч	  
		SET_TCLK();      
	} 			    
} 	


/******************************************************************************************
*��������ADS_Read_AD(u8 CMD)	
* ������u8 CMD
* ����ֵ����
* ���ܣ���7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ
*********************************************************************************************/   
uint16_t ADS_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 	
	CLR_TCLK();         //������ʱ�� 
    //CLR_TDIN();         //����������
	CLR_TCS();          //ѡ��ADS7843	 
	ADS_Write_Byte(CMD);//����������
	delay_nus(6);       //ADS7846��ת��ʱ���Ϊ6us
    delay_nus(1);    
	SET_TCLK();         //��1��ʱ�ӣ����BUSY   	    
	CLR_TCLK(); 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
				
		CLR_TCLK();       //�½�����Ч
        SET_TCLK(); 
		if(TDOUT_IN())Num++; 		 
	}  	 
	Num>>=4;            //ֻ�и�12λ��Ч.
	SET_TCS();          //�ͷ�ADS7843	 	
	return(Num); 
}

#if !HORIZONTAL
/******************************************************************************************
*��������AdjustTouchPanel()
* ������void
* ����ֵ��void
* ���ܣ�������У׼����
*********************************************************************************************/
void AdjustTouchPanel(void)
{
	uint16_t touchXY[5][2];
	uint8_t cnt=0;
	uint16_t temp1,temp2,temp3;
	
	LCD_Clear(WHITE);                                //����
	LCD_SetColors(BLUE,WHITE);                       //����ǰ��ɫ�ͱ���ɫ
	DrawAdjustPoint(20,20,BLUE);                          //���Ƶ�һ��׼��
	LCD_DrawString(40,50,"Touch Sight");
	while(1)
		{
			if(PEN_State()==RESET)
				{
				 	if(Touch_GetPhyXY())
						{
							touchXY[cnt][0]=ADC_X;             //��ȡX��ADCֵ
							touchXY[cnt][1]=ADC_Y;             //��ȡy��ADCֵ
							
                            //printf("%4d  \n\r",ADC_X);//touchXY[%d][0]=
                            //printf("%d",cnt,); touchXY[%d][1]=
                            while(PEN_State()==RESET);           //�ȴ������ɿ�
							delay_nms(50);                     //����
							cnt++;                             //��ת���¸���
						}
label:				switch(cnt)
						{
							case 0:
								DrawAdjustPoint(160,240,WHITE);        //�����5��׼��λ��
								DrawAdjustPoint(20,20,BLUE);          //���Ƶ�һ��׼��
							 break;
							case 1:
                                LCD_DrawNum(0,20,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(70,20,touchXY[cnt-1][1],4);
								DrawAdjustPoint(20,20,WHITE);             //�����1��׼��λ��
								DrawAdjustPoint(20,460,BLUE);          //���Ƶڶ���׼��
							  break;
							case 2:
                                LCD_DrawNum(0,460,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(70,460,touchXY[cnt-1][1],4);
								DrawAdjustPoint(20,460,WHITE);          //�����2��׼��λ��
								DrawAdjustPoint(300,460,BLUE);        //���Ƶ�����׼��  
							  break;
							case 3:
                                LCD_DrawNum(170,460,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(240,460,touchXY[cnt-1][1],4);
								DrawAdjustPoint(300,460,WHITE);         //�����3��׼��λ��
								DrawAdjustPoint(300,20,BLUE);          //���Ƶ��ĸ�׼��
							  break;
							case 4:
                                LCD_DrawNum(170,20,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(240,20,touchXY[cnt-1][1],4);
								DrawAdjustPoint(300,20,WHITE);          //�����4��׼��λ��
								DrawAdjustPoint(160,240,BLUE);        //���Ƶ����׼��
							  break;
							case 5:
                                LCD_DrawNum(90,180,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(150,180,touchXY[cnt-1][1],4);
								 temp1=touchXY[3][0]-touchXY[0][0];                               //��ȡ�Ϸ������X��ADCֵ�Ĳ�
                                 temp2=touchXY[2][0]-touchXY[1][0];                               //����·������X��ADCֵ�Ĳ�
								 if(ABS(temp1-temp2)>ADJUST_DE)                                    //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
                                        cnt=0;                                                          //����
                                        goto label;                                                     //��ת���¿�ʼ
									 }
									 temp3=temp1+temp2;                                              //������X��ADC��ֵ�Ĳ�ֵ���
									 xfac=280.0/(temp3>>1);//����nλ����2��n�η�                                          //��ȡx�ı�ֵ
									 temp3=temp3>>2;                                                 //������X��ADC��ֵ�Ĳ�ֵ��ӳ���4�����ĵ��X��ADCֵ�Ա�
								  if(ABS(touchXY[4][0]-touchXY[0][0]-temp3)>ADJUST_DE)             //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
                                        cnt=0;                                                          //����
                                        goto label;                                                     //��ת���¿�ʼ
									 }
							   temp1=touchXY[0][1]-touchXY[1][1];  //����Ҳ��                               //��ȡ��������Y��ADCֵ�Ĳ�
							   temp2=touchXY[3][1]-touchXY[2][1]; //���� ������ abs����                                //��ȡ�ұ������Y��ADCֵ�Ĳ�
								 if(ABS(temp1-temp2)>ADJUST_DE)                                     //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                           //����
							     goto label;                                                      //��ת���¿�ʼ
									 }
									 temp3=temp1+temp2;                                               //������Y��ADC�����
									 yfac = 440.0/(temp3>>1);                                         //��ȡy�ı�ֵ
									 temp3=temp3>>2;                                                  //������Y��ADC��ĺͳ���4�����ĵ��Y��ADCֵ�Ա�
								  if(ABS(touchXY[0][1]-touchXY[4][1]-temp3)>ADJUST_DE)              //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                           //����
							     goto label;                                                      //��ת���¿�ʼ
									 }
									 xoff=(320-xfac*(touchXY[0][0]+touchXY[3][0]))/2;                 //��ȡ��X������ֵ
									 yoff=(480-yfac*(touchXY[0][1]+touchXY[1][1]))/2;                 //��ȡ��Y������ֵ
									 LCD_DrawString(40,180," Finish");                                //У�����
									 delay_nms(500);                                                  //��ʱ
									 return ;                                                         //����
						}
				}
		}
}
/******************************************************************************************
*��������Touch_GetPhyXY()
* ������void
* ����ֵ��uint8_t ����1��ʾ�ɹ�������0��ʾʧ��
* ���ܣ���ȡXY����λ��
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void)
{
	u8  i, j, k, min;
	u16 temp;
	u16 tempXY[2][SAMP_CNT], XY[2];// ����
	
	for(i=0; i<SAMP_CNT; i++)
	{
				tempXY[0][i] = ADS_Read_AD(CMD_RDX);
                //LCD_DrawNum(70,150,ADC_X,5);      //���X����
				tempXY[1][i] = ADS_Read_AD(CMD_RDY);
                //LCD_DrawNum(150,150,ADC_Y,5);
	}
	// �˲�
	for(k=0; k<2; k++)
	{ 
		// ��������
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
	 // �趨��ֵ
		if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
			return 0;
	// ���м�ֵ�ľ�ֵ
		XY[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1])>>1;
	}
		if(XY[0]==0||XY[1]==2047)
			{
                
				return 0;
			}
		ADC_X=XY[0];                             //������ԭʼ���� ��������
		ADC_Y=XY[1];
		// ��������
		Phy_X = (uint16_t)(ADC_X*xfac+xoff);     // xfac 0.17878    xoff -23.7
		Phy_Y = 480-(uint16_t)(ADC_Y*yfac+yoff); //yfac 0.24985    yoff     -13.1
		return 1;
}
#else
/******************************************************************************************
*��������AdjustTouchPanel()
* ������void
* ����ֵ��void
* ���ܣ�������У׼����
*********************************************************************************************/
void AdjustTouchPanel(void)
{
	uint16_t touchXY[5][2];
	uint8_t cnt=0;
	uint16_t temp1,temp2,temp3;
	
	LCD_Clear(WHITE);                                //����
	LCD_SetColors(BLUE,WHITE);                       //����ǰ��ɫ�ͱ���ɫ
	DrawAdjustPoint(20,20,BLUE);                          //���Ƶ�һ��׼��
	LCD_DrawString(220,40,"Touch Sight");
	while(1)
		{
			if(PEN_State()==RESET)
				{
				 	if(Touch_GetPhyXY())
						{
							touchXY[cnt][0]=ADC_X;             //��ȡX��ADCֵ
							touchXY[cnt][1]=ADC_Y;             //��ȡy��ADCֵ
							
                            //printf("%4d  \n\r",ADC_X);//touchXY[%d][0]=
                            //printf("%d",cnt,); touchXY[%d][1]=
                            while(PEN_State()==RESET);           //�ȴ������ɿ�
							delay_nms(50);                     //����
							cnt++;                             //��ת���¸���
						}
label:				switch(cnt)
						{
							case 0:
								DrawAdjustPoint(240,160,WHITE);        //�����5��׼��λ��
								DrawAdjustPoint(20,20,BLUE);          //���Ƶ�һ��׼��
							 break;
							case 1:
                                LCD_DrawNum(0,20,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(70,20,touchXY[cnt-1][1],4);
                                DrawAdjustPoint(20,20,WHITE);             //�����1��׼��λ��
								DrawAdjustPoint(20,300,BLUE);          //���Ƶڶ���׼��
							  break;
							case 2:
                                LCD_DrawNum(0,300,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(70,300,touchXY[cnt-1][1],4);
								DrawAdjustPoint(20,300,WHITE);          //�����2��׼��λ��
								DrawAdjustPoint(460,300,BLUE);        //���Ƶ�����׼��  
							  break;
							case 3:
                                LCD_DrawNum(330,300,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(400,300,touchXY[cnt-1][1],4);
								DrawAdjustPoint(460,300,WHITE);         //�����3��׼��λ��
								DrawAdjustPoint(460,20,BLUE);          //���Ƶ��ĸ�׼��
							  break;
							case 4:
                                LCD_DrawNum(330,20,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(400,20,touchXY[cnt-1][1],4);
								DrawAdjustPoint(460,20,WHITE);          //�����4��׼��λ��
								DrawAdjustPoint(240,160,BLUE);        //���Ƶ����׼��
							  break;
							case 5:
                                LCD_DrawNum(200,180,touchXY[cnt-1][0],4);      //���X����
                                LCD_DrawNum(270,180,touchXY[cnt-1][1],4);
								 temp1=touchXY[3][1]-touchXY[0][1];                               //��ȡ�Ϸ������X��ADCֵ�Ĳ�
                                 temp2=touchXY[2][1]-touchXY[1][1];                               //����·������X��ADCֵ�Ĳ�
								 if(ABS(temp1-temp2)>ADJUST_DE)                                    //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
                                        cnt=0;                                                          //����
                                        goto label;                                                     //��ת���¿�ʼ
									 }
									 temp3=temp1+temp2;                                              //������X��ADC��ֵ�Ĳ�ֵ���
									 xfac=440.0/(temp3>>1);//����nλ����2��n�η�                                          //��ȡx�ı�ֵ
									 temp3=temp3>>2;                                                 //������X��ADC��ֵ�Ĳ�ֵ��ӳ���4�����ĵ��X��ADCֵ�Ա�
								  if(ABS(touchXY[4][0]-touchXY[0][0]-temp3)>ADJUST_DE)             //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
                                        cnt=0;                                                          //����
                                        goto label;                                                     //��ת���¿�ʼ
									 }
							   temp1=touchXY[1][0]-touchXY[0][0];                                 //��ȡ��������Y��ADCֵ�Ĳ�
							   temp2=touchXY[2][0]-touchXY[3][0];                                 //��ȡ�ұ������Y��ADCֵ�Ĳ�
								 if(ABS(temp1-temp2)>ADJUST_DE)                                     //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
                                        cnt=0;                                                           //����
                                        goto label;                                                      //��ת���¿�ʼ
									 }
									 temp3=temp1+temp2;                                               //������Y��ADC�����
									 yfac = 280.0/(temp3>>1);                                         //��ȡy�ı�ֵ
									 temp3=temp3>>2;                                                  //������Y��ADC��ĺͳ���4�����ĵ��Y��ADCֵ�Ա�
								  if(ABS(touchXY[4][1]-touchXY[0][1]-temp3)>ADJUST_DE)              //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                           //����
							     goto label;                                                      //��ת���¿�ʼ
									 }
									 xoff=(480-xfac*(touchXY[0][1]+touchXY[3][1]))/2;                 //��ȡ��X������ֵ
									 yoff=(320-yfac*(touchXY[0][0]+touchXY[1][0]))/2;                 //��ȡ��Y������ֵ
									 LCD_DrawString(40,180," Finish");                                //У�����
									 delay_nms(500);                                                  //��ʱ
									 return ;                                                         //����
						}
				}
		}
}
/******************************************************************************************
*��������Touch_GetPhyXY()
* ������void
* ����ֵ��uint8_t ����1��ʾ�ɹ�������0��ʾʧ��
* ���ܣ���ȡXY����λ��
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void)
{
	u8  i, j, k, min;
	u16 temp;
	u16 tempXY[2][SAMP_CNT], XY[2];// ����
	
	for(i=0; i<SAMP_CNT; i++)
	{
				tempXY[0][i] = ADS_Read_AD(CMD_RDX);
                //LCD_DrawNum(70,150,ADC_X,5);      //���X����
				tempXY[1][i] = ADS_Read_AD(CMD_RDY);
                //LCD_DrawNum(150,150,ADC_Y,5);
	}
	// �˲�
	for(k=0; k<2; k++)
	{ 
		// ��������
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
	 // �趨��ֵ
		if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
			return 0;
	// ���м�ֵ�ľ�ֵ
		XY[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1])>>1;
	}
		if(XY[0]==0||XY[1]==2047)
			{
                
				return 0;
			}
		ADC_X=XY[0];                             //������ԭʼ���� ��������
		ADC_Y=XY[1];
		// ��������
// 		Phy_X = (uint16_t)(ADC_Y*xfac+xoff);     //�������ǰ��´������ӦҺ�����ϵ�����  �߼����� 
// 		Phy_Y = (uint16_t)(ADC_X*yfac+yoff); 
          Phy_X = (uint16_t)(ADC_Y*0.25-14.37);     //�������ǰ��´������ӦҺ�����ϵ�����  �߼����� 
		Phy_Y = (uint16_t)(ADC_X*0.18-17.35);
		return 1;
}
#endif

/******************************************************************************************
*��������DrawAdjustPoint()
* ������uint16_t x,uint16_t y xy����
* ����ֵ��void
* ���ܣ����ƴ���У׼ʱʹ�õ�׼��
*********************************************************************************************/
void DrawAdjustPoint(uint16_t x,uint16_t y,uint16_t text_color)
{ 
    LCD_SetTextColor(text_color);
	/*�������ĵ�*/
	DrawPixel(x,y);
	DrawPixel(x-1,y);
	DrawPixel(x,y-1);
	DrawPixel(x-1,y-1);
	DrawPixel(x+1,y-1);
	DrawPixel(x,y+1);
    DrawPixel(x-1,y+1);
	DrawPixel(x+1,y+1);
	DrawPixel(x+1,y);
	/*��������ͬ��Բ*/
	LCD_DrawCircle(x,y,12);
	LCD_DrawCircle(x,y,10);
	/*����ʮ����*/
	LCD_DrawHLine(x-14,y,28);
	LCD_DrawVLine(x,y-14,28);
	/*����С����*/
	LCD_DrawRect(x-3,y-3,6,6);
}


/******************************************************************************************
*��������DrawTouchPoint()
* ������uint16_t x,uint16_t y xy����
* ����ֵ��void
* ���ܣ����ƴ����ʱʼ�ĵ�
*********************************************************************************************/
void DrawTouchPoint(uint16_t x,uint16_t y)
{
	DrawPixel(x,y);                  //�������ĵ�
	/*�����ֱܷ���Ƶ�*/
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
*��������Touch_Init()
* ������void
* ����ֵ��void
* ���ܣ�����оƬ��ʼ��
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
  	GPIO_SetBits(GPIOD, GPIO_Pin_12);                                  //TCS��ѡ�� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;                  //pen_int:PD13   
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����Ϊ����
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
//LCD_brush_screen();    //ˢ��   
	//LCD_DrawString(30,300,"X:");            //����ַ�
	//LCD_DrawString(140,300,"Y:");           //����ַ�
    LCD_SetColors(RED,RED);              //���û��ʵ�ǰ��ɫ�ͱ���ɫ
	LCD_DrawString(  0,0,"  ");            //����ַ�
    LCD_SetColors(GREEN,GREEN);
	LCD_DrawString( 0,24,"  ");          //����ַ�
    LCD_SetColors(BLUE,BLUE);
	LCD_DrawString(0,48,"  ");            //����ַ�
    LCD_SetColors(BLACK,BLACK);
    LCD_DrawString(0,72,"  ");
    LCD_SetColors(BLACK,YELLOW);    //����ַ�
	LCD_DrawString(0,96,"RST");            //����ַ�
    while(1)
    {
        if(PEN_State()==RESET)                //����Ƿ��»���
		{
			if(Touch_GetPhyXY())								//��ȡ����λ��
			{              
				DrawTouchPoint(Phy_X,Phy_Y);      //����
               
				//LCD_DrawNum(70,300,Phy_X,3);      //���X����
				//LCD_DrawNum(180,300,Phy_Y,3);     //���Y���� 
				
				if((Phy_X>  0&&Phy_X< 35)&&(Phy_Y>0&&Phy_Y<24)){
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,0,"  ");
                    delay_nms(50);
                    LCD_SetColors(  RED,RED);  //�ı仭����ɫ
                    LCD_DrawString(  0,0,"  ");
                    }
				if((Phy_X> 0&&Phy_X<35)&&(Phy_Y>24&&Phy_Y<48)) {
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,24,"  ");
                    delay_nms(50);
                    LCD_SetColors(GREEN,GREEN); //�ı仭����ɫ
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
                if((Phy_X>0&&Phy_X<48)&&(Phy_Y>96&&Phy_Y<120))  //�ж��Ƿ�������
				{   
                    LCD_SetBackColor(WHITE);
                    LCD_DrawString(  0,96,"   ");
                    delay_nms(50);
					goto label1;                                   //���ص�����λ��
				}
			}
		}
    }        
}

void Change_pwm() //���ڱ������ȵ�
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