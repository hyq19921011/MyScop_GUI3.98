#include "TFTlcd.h"
#include "usart.h"
#include "GBK2312.h"
#include <stdio.h>
#include "pwm.h"
#include <math.h>
/*******************Һ����Ӳ������**********************

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
/* ѡ��BANK1-NORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
 * FSMC_A16 ��LCD��DC(�Ĵ���/����ѡ��)��
 * �Ĵ�������ַ = 0X60000000
 * RAM����ַ = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * ������Ҫ���ڿ���RS����д�Ĵ���д���ݲ�������Ϊ���ӵ�Һ��RS����A16�ܽţ�����Ҫʹ��ַ��A16Ϊ�ߵ�ƽ����д��ַ��2��16�η�����������ΪҺ����16bit��
 *�����ٳ���2���ó��ĵ�ַ����BANK���ڵĵ�ַ���ɡ���RS��A0������RAM����ַΪbank���ڵ�ַ����2��0�η���2��������8bit������Ҫ����2
 */

/**************************************************************************************************************
 * ��������GPIO_AF_FSMC_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD���ӵ�GPIO�������ӵ�FSMC
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void GPIO_AF_FSMC_Config(void)
{
  /*����GPIOD��Ӧ�ĹܽŸ������ӵ�FSMC*/
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);	    //����GPIOD��Pin0�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);			//����GPIOD��Pin1�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);			//����GPIOD��Pin4�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);			//����GPIOD��Pin5�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);			//����GPIOD��Pin7�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);			//����GPIOD��Pin8�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);			//����GPIOD��Pin9�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);		//����GPIOD��Pin10�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);		//����GPIOD��Pin11�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);		//����GPIOD��Pin14�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);		//����GPIOD��Pin15�������ӵ�FSMC
   /*����GPIOE��Ӧ�ĹܽŸ������ӵ�FSMC*/
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);			//����GPIOE��Pin7�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);			//����GPIOE��Pin8�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);			//����GPIOE��Pin9�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);		//����GPIOE��Pin10�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);		//����GPIOE��Pin11�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);		//����GPIOE��Pin12�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	    //����GPIOE��Pin13�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);		//����GPIOE��Pin14�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);		//����GPIOE��Pin15�������ӵ�FSMC
}
/**************************************************************************************************************
 * ��������LCD_GPIO_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD��GPIO�ܽ���FSMC����
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;		                                              //����GPIO��ʼ���ṹ��
     
    /* ʹ��FSMC��AHB3ʱ�ӣ�����ע�⣬��ͬоƬ���߻᲻ͬ */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    
    /* ʹ��GPIOB��GPIOD��GPIOE��AHB1ʱ��*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE , ENABLE);
   	 /*����Һ�����ı�����ƹܽ�*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;											  //���ùܽ�ģʽΪ���ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //���÷�ת�ٶ�Ϊ100Mhz��M4���ǿ찡
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											  //�����������Ϊ�������
    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4);									//���ùܽ�ΪPin_5,�����Լ�Һ���ӷ�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  //������/��������Ϊ��������������		
    GPIO_Init(GPIOB, &GPIO_InitStructure);													  //��ʼ��GPIOB��Pin_5
    		   
    /* ���û���FSMC��Һ������������
	 * Һ���������ߣ�FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //���÷�ת�ٶ�Ϊ100Mhz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;											  //���ùܽ�ģʽΪ����ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8  | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                  
    GPIO_Init(GPIOD, &GPIO_InitStructure);													  //��ʼ��GPIOB��Pin_0��Pin_��Pin_2��
	                                                                    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* ���û���FSMC��Һ����������
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
    GPIO_AF_FSMC_Config();				             //��F407�б��������������������ã����������ӵ�FSMC
    GPIO_SetBits(GPIOB, GPIO_Pin_4);			 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 
    	
}

/**************************************************************************************************************
 * ��������LCD_FSMC_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD��FSMC
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void LCD_FSMC_Config(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  p;
	  LCD_GPIO_Config();                                                     //LCD�ܽ�����
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 1;                                           //��ַ����ʱ��
  	p.FSMC_AddressHoldTime = 0;                                            //��ַ����ʱ��
  	p.FSMC_DataSetupTime = 6;                                              //���ݽ���ʱ��
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
  	p.FSMC_AccessMode = FSMC_AccessMode_A;                                  // ��ģʽA����LCD
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
//д�Ĵ�������
//regval:�Ĵ���ֵ
void LCD_WR_REG(uint8_t regval)
{ 
	LCD_REG=regval;//д��Ҫд�ļĴ������	 
}
//дLCD����
//data:Ҫд���ֵ
void LCD_WR_DATA(uint16_t data)
{										    	   
	LCD_RAM=data;		 
}

/**************************************************************************************************************
 * ��������LCD_WriteReg()
 * ����  ��uint8_t LCD_Reg �Ĵ�����ַ, uint16_t LCD_RegValue �Ĵ�����ֵ
 * ���  ��void
 * ����  ��дLCD�Ĵ�������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /*д�Ĵ�����ַ*/
  LCD_REG = LCD_Reg;
  /*д��Ĵ���ֵ*/
  LCD_RAM = LCD_RegValue;
}
/**************************************************************************************************************
 * ��������LCD_ReadReg()
 * ����  ��uint8_t LCD_Reg ��Ҫ��ȡ�ļĴ�����ַ
 * ���  ��uint16_t �Ĵ�����ֵ
 * ����  ����ȡLCD�Ĵ�����ֵ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /*д�Ĵ�����ַ*/
  LCD_REG = LCD_Reg;
  delay_nus(1);
  /*�����Ĵ���ֵ������*/
  return (LCD_RAM);
}


/**************************************************************************************************************
 * ��������LCD_WriteRAM()
 * ����  ��uint16_t RGB_data  16bit��ɫ����
 * ���  ��void
 * ����  ����LCD��RAMд����
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_WriteRAM(uint16_t RGB_data)
{
  /*��LCD��RAMд����*/
  LCD_RAM = RGB_data;
}

/**************************************************************************************************************
 * ��������LCD_ReadRAM()
 * ����  ��void
 * ���  ��uint16_t  16bit��ɫֵ
 * ����  ����ȡLCD��RAM����
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
uint16_t LCD_ReadRAM(void)
{ 
  LCD_REG = 0x2E; 
  /*��ȡ16bit RAMֵ*/
  return LCD_RAM;
}

/**************************************************************************************************************
 * ��������LCD_Init()
 * ����  ��void
 * ���  ��void
 * ����  ��LCD��ʼ������
 * ����  ���ⲿ����        
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
        LCD_WR_REG(0x0011); //�Ƴ�˯��ģʽʹ�ܸ�����ģ�� 
        delay_nms(50); // delay 50 ms 		
		LCD_WR_REG(0x0013);  //��������ģʽ
        delay_nms(50);
        
		LCD_WR_REG(0xD0); //���õ�Դ����
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
        LCD_WR_REG(0x36);//������ʾģʽ         7 6 5 4 3 2 1 0
        LCD_WR_DATA(0xaa);//����0x0A,heng��0x38 1 0 1 1 1 0 1 0
                
       #else
        LCD_WR_REG(0x36);//������ʾģʽ
        LCD_WR_DATA(0x0a);//����0x0A,heng��0x38
       
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
 * ��������LCD_SetMode()
 * ����  ��uint8_t mode ������Ļˢ��ģʽ
 * ���  ��void
 * ����  ������LCDˢ��ģʽ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetMode(uint8_t mode)
{
	switch(mode)//����4(����)��7(����)
	{
		case 1:LCD_WriteReg(0x36,0x0a);break;//��������:���I
		case 2:LCD_WriteReg(0x36,0x38);break;//��������:���I
        
// 		case 3:LCD_WriteReg(0x36,0x1010);break;//��������:���J
// 		case 4:LCD_WriteReg(0x36,0x1018);break;//��������:���J
// 		case 5:LCD_WriteReg(0x36,0x1020);break;//��������:���L
// 		case 6:LCD_WriteReg(0x36,0x1028);break;//��������:���L
// 		case 7:LCD_WriteReg(0x36,0x1030);break;//��������:���K
// 		case 8:LCD_WriteReg(0x36,0x1038);break;//��������:���K
	}
}
/**************************************************************************************************************
 * ��������LCD_Clear()
 * ����  ��void
 * ���  ��void
 * ����  ��LCD��������
 * ����  ���ⲿ����        
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
	LCD_RAM = 0;                 //�趨X����
	LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320
	#else
	 LCD_REG = 0x002A;
	LCD_RAM = 0;
  LCD_RAM = 0;                 //�趨X����
  LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320

  LCD_REG = 0x002B;
	LCD_RAM = (479-0)>>8;
  LCD_RAM = 0x00FF&(479-0);           //�趨Y����
  LCD_RAM = 0x0001;
	LCD_RAM = 0x00DF;//480
	
	#endif
  LCD_WR_REG(0x2C); /*��ʼдGRAM */
  for(i = 0; i < (320*480); i++)
  {
		LCD_RAM=Color;__NOP();__NOP();
  } 
}

/**************************************************************************************************************
 * ��������LCD_SetCursor()
 * ����  ��uint16_t Xpos, uint16_t Ypos �趨����Ļ��Xֵ��Yֵ
 * ���  ��void
 * ����  ��LCD���ù��λ�ú���
 * ����  ���ⲿ����        
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
	LCD_RAM = 0x00FF&Ypos;                 //�趨X����
	LCD_RAM = Ypos>>8;
	LCD_RAM = 0x00FF&Ypos; 
	#else
	 LCD_REG = 0x002A;
	LCD_RAM = Xpos>>8;
  LCD_RAM = 0x00FF&Xpos;                 //�趨X����
  LCD_RAM = 0x0001;
	LCD_RAM = 0x003F;//320

  LCD_REG = 0x002B;
	LCD_RAM = (479-Ypos)>>8;
  LCD_RAM = 0x00FF&(479-Ypos);           //�趨Y����
  LCD_RAM = 0x0001;
	LCD_RAM = 0x00DF;//480
	
	#endif
	//LCD_REG = 0x2C;
}
/**************************************************************************************************************
 * ��������LCD_SetWindow()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width �������㣬��͸�
 * ���  ��void
 * ����  ������ĳ���ض����������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint8_t Height)
{
  /*����ˮƽ����X��ʼ����*/
  if(Xpos >= Height)
  {
    LCD_WriteReg(0x2A, (Xpos - Height + 1));
  }
  else
  {
    LCD_WriteReg(0x2A, 0);
  }
  /*����ˮƽ����X��������*/
  LCD_WriteReg(0x2A, Xpos);
  /*������ֱ����Y��ʼ����*/
  if(Ypos >= Width)
  {
    LCD_WriteReg(0x2B, (Ypos - Width + 1));
  }  
  else
  {
    LCD_WriteReg(0x2B, 0);
  }
  /*������ֱ����Y��������*/
  LCD_WriteReg(0x2B, Ypos);
  LCD_SetCursor(Xpos, Ypos);
}
/**************************************************************************************************************
 * ��������LCD_SetColors()
 * ����  ��_TextColor ǰ��ɫ,_BackColor ����ɫ
 * ���  ��void
 * ����  ������LCD��ǰ��ɫ�ͱ���ɫ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor; 
  BackColor = _BackColor;
}
/**************************************************************************************************************
 * ��������LCD_GetColors()
 * ����  ��*_TextColor ǰ��ɫ��ָ��,*_BackColor ����ɫ��ָ��
 * ���  ��void
 * ����  ����ȡLCD��ǰ��ɫ�ͱ���ɫ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}
/**************************************************************************************************************
 * ��������LCD_SetTextColor()
 * ����  ��uint16_t Color ǰ��ɫ
 * ���  ��void
 * ����  ������LCD��ǰ��ɫ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}


/**************************************************************************************************************
 * ��������LCD_SetBackColor()
 * ����  ��uint16_t Color ����ɫ
 * ���  ��void
 * ����  ������LCD�ı���ɫ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}
/**************************************************************************************************************
 * ��������LCD_SetFont()
 * ����  ��sFONT *fonts Ҫ���õ�����
 * ���  ��void
 * ����  ������LCD������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}
/**************************************************************************************************************
 * ��������LCD_GetFont()
 * ����  ��void
 * ���  ��sFONT * ��ȡ����
 * ����  ������LCD������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
/**************************************************************************************************************
 * ��������DrawPixel()
 * ����  ��int16_t x, int16_t y  �������
 * ���  ��void
 * ����  ����һ�����ص�
 * ����  ���ⲿ����        
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
	LCD_RAM = 0x00FF&ypos;                 //�趨X����
	LCD_RAM = ypos>>8;
	LCD_RAM = 0x00FF&ypos ;

	#else
	 LCD_REG = 0x002A;
	LCD_RAM = xpos>>8;
  LCD_RAM = 0x00FF&xpos;                 //�趨X����
//  LCD_RAM = 0x0001;
//	LCD_RAM = 0x003F;//320

  LCD_REG = 0x002B;
	LCD_RAM = (479-ypos)>>8;
  LCD_RAM = 0x00FF&(479-ypos);           //�趨Y����
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
	LCD_RAM = 0x00FF&ypos;                 //�趨X����
	LCD_RAM = ypos>>8;
	LCD_RAM = 0x00FF&ypos ;
	#else
		 LCD_REG = 0x002A;
		LCD_RAM = xpos>>8;
		LCD_RAM = 0x00FF&xpos;                 //�趨X����
		LCD_RAM = 0x0001;
		LCD_RAM = 0x003F;//320

		LCD_REG = 0x002B;
		LCD_RAM = (479-ypos)>>8;
		LCD_RAM = 0x00FF&(479-ypos);           //�趨Y����
		LCD_RAM = 0x0001;
		LCD_RAM = 0x00DF;//480
		
	#endif
	
	LCD_REG = 0x2E;
	value = LCD_RAM;//�ٶ�	
  /*��ȡ16bit RAMֵ*/
  return LCD_BGR2RGB(LCD_RAM);    
}
/**************************************************************************************************************
 * ��������LCD_DrawHLine()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Length ���X��Y���꼰����
 * ���  ��void
 * ����  ����ˮƽ��
 * ����  ���ⲿ����        
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
	LCD_RAM = 0x00FF&Ypos;                 //�趨X����
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
 * ��������LCD_DrawVLine()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Length ���X��Y���꼰����
 * ���  ��void
 * ����  ������ֱ��
 * ����  ���ⲿ����        
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
	LCD_RAM = 0x00FF&Ypos;                 //�趨X����
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
 * ��������LCD_DrawRect()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint8_t Height �������Ͻǵ�����꼰��͸�
 * ���  ��void
 * ����  �������κ���
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  TFTLCD_DrawHLine(Xpos, Ypos, Width);
  TFTLCD_DrawHLine(Xpos, Ypos+ Height, Width); 
  TFTLCD_DrawVLine(Xpos, Ypos, Height);
  TFTLCD_DrawVLine(Xpos+ Width,Ypos, Height);
}
/**************************************************************************************************************
 * ��������LCD_DrawCircle()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Radius Բ������㼰�뾶
 * ���  ��void
 * ����  ����Բ����
 * ����  ���ⲿ����        
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
 * ��������LCD_DrawMonoPict()
 * ����  ��const uint32_t *Pict ��һ��ȫ����ɫ��ȡĤ����
 * ���  ��void
 * ����  ����һ����ɫ��ȫ��ͼƬ����
 * ����  ���ⲿ����        
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
 * ��������LCD_FillRect()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height ���������Ͻǵ㡢��͸�
 * ���  ��void
 * ����  ����һ�����ľ���
 * ����  ���ⲿ����        
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
 * ��������LCD_FillCircle()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Radius ���Բ��Բ�ĺͰ뾶
 * ���  ��void
 * ����  ����һ�����Բ
 * ����  ���ⲿ����        
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
 * ��������LCD_DrawUniLine()
 * ����  ��uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ��ʼ��������յ�����
 * ���  ��void
 * ����  �������ⷽ���ֱ��
 * ����  ���ⲿ����        
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
 * ��������LCD_DrawChar()
 * ����  ��const uint16_t *c   �ַ�����
 * ���  ��void
 * ����  ��LCD��һ���ַ�
 * ����  ���ⲿ����        
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
          //LCD_DrawHLine(Xpos+i, Yaddress+index��1);
      LCD_REG = 0x2C; /* Prepare to write GRAM */
      LCD_RAM = TextColor;
      } 
    }
  }
}

#else

/**************************************************************************************************************
 * ��������LCD_DrawChar()
 * ����  ��const uint16_t *c   �ַ�����
 * ���  ��void
 * ����  ��LCD��һ���ַ�
 * ����  ���ⲿ����        
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
 * ��������LCD_DisplayChar()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint8_t Ascii ��ʾ��λ�ú��ַ�
 * ���  ��void
 * ����  ��LCD��ʾһ���ַ�
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Xpos, Ypos, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}
/**************************************************************************************************************
 * ��������LCD_DrawString()
 * ����  ��u16 xpos, u16 ypos, u8 *ptr ��ʾ��λ�ú��ַ���
 * ���  ��void
 * ����  ��LCD��ʾһ���ַ�
 * ����  ���ⲿ����        
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

//m^n����
u32 mypow(u8 m,u8 n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
/**************************************************************************************************************
 * ��������LCD_DrawNum()
 * ����  ��u16 x, u16 y, s32 num, u8 len   ��ʾ��λ�ã����ּ�����
 * ���  ��void
 * ����  ��LCD��ʾһ������
 * ����  ���ⲿ����        
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
���ܣ��������ַ�  16*16
---------------------------------------------------------------------------*/
void LCD_Show_Chinese(unsigned int row, unsigned int column, unsigned long index)
{
	unsigned char i = 0, j = 0;
	unsigned int z = row;
	LCD_SetCursor(z, j);

	for (j = 0; j < 16; j++)//��
	{
	LCD_WriteRAM_Start();//дGRAMǰ ��д0x002C
		for (i = 0; i < 8; i++)//ǰ����
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

		for (i = 0; i < 8; i++)//�����
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
    LCD_SetCursor(row+16, column);//ת����һ���ַ���
}

/*---------------------------------------------------------------------------
���ܣ���ʾһ�������ַ�  16*16
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
#if !HORIZONTAL //ˮƽ��ʾʱ��
/*---------------------------------------------------------------------------
  ������ʾ�ַ�����
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
  ������ʾ�ַ�����
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
 * ��������ClearAera()
 * ����  ��uint16_t x,uint16_t y,uint16_t w,uint16_t h   �����λ�ã����ȺͿ��
 * ���  ��void
 * ����  ��LCD���ĳ������
 * ����  ���ⲿ����        
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
 * ��������LCD_DrawImage()
 * ����  ��u8 *p   ͼƬȡĤ����
 * ���  ��void
 * ����  ��LCD��һ��ȫ��ͼƬ
 * ����  ���ⲿ����        
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
	LCD_RAM = 0;                 //�趨X����
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
//ˢ������
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
