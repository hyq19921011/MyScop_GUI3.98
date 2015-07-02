#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "stm32f4xx_conf.h" 


//�봥����оƬ��������	   
#define CLR_TDIN()		GPIO_ResetBits(GPIOD, GPIO_Pin_3)
#define SET_TDIN()		GPIO_SetBits(GPIOD, GPIO_Pin_3)
#define CLR_TCLK()		GPIO_ResetBits(GPIOD, GPIO_Pin_6)
#define SET_TCLK()		GPIO_SetBits(GPIOD, GPIO_Pin_6)
#define CLR_TCS()		GPIO_ResetBits(GPIOD, GPIO_Pin_12)
#define SET_TCS()		GPIO_SetBits(GPIOD, GPIO_Pin_12)

#define TDOUT_IN()  	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)   //GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define PEN_State()		GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13)   //��ⴥ������Ӧ�ź�

#define SAMP_CNT      8                           //���ô���
#define SAMP_CNT_DIV2 4                           //����ȡֵ����
#define ADJUST_DE     100                          //У��ķ���ֵ��ԽСԽ��׼��Խ��У��ɹ�
#define CMD_RDY       0X90  
#define CMD_RDX	      0XD0    											 	    

/******************************************************************************************
*��������Touch_Init()
* ������void
* ����ֵ��void
* ���ܣ�����оƬ��ʼ��
*********************************************************************************************/
void Touch_Init(void);
/******************************************************************************************
*��������DrawTouchPoint()
* ������uint16_t x,uint16_t y xy����
* ����ֵ��void
* ���ܣ����ƴ����ʱʼ�ĵ�
*********************************************************************************************/
void DrawTouchPoint(uint16_t x,uint16_t y);
/******************************************************************************************
*��������DrawAdjustPoint()
* ������uint16_t x,uint16_t y xy����
* ����ֵ��void
* ���ܣ����ƴ���У׼ʱʹ�õ�׼��
*********************************************************************************************/
void DrawAdjustPoint(uint16_t x,uint16_t y,uint16_t text_color);
/******************************************************************************************
*��������AdjustTouchPanel()
* ������void
* ����ֵ��void
* ���ܣ�������У׼����
*********************************************************************************************/
void AdjustTouchPanel(void);
/******************************************************************************************
*��������ADS_Read()
* ������u8 CMD ָ���x���y
* ����ֵ��void
* ���ܣ���ȡADS��ADCֵ
*********************************************************************************************/
uint16_t ADS_Read_AD(u8 CMD)	;
/******************************************************************************************
*��������Touch_GetPhyXY()
* ������void
* ����ֵ��uint8_t ����1��ʾ�ɹ�������0��ʾʧ��
* ���ܣ���ȡXY����λ��
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void);
void Touch_Painting(void);//����
#endif


