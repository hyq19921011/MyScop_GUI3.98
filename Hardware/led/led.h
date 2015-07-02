#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"
typedef enum 
{
  LED0 = 0,
  LED1 = 1,
  LED2 = 2,
  LED3 = 3
} Led_Def;


static GPIO_TypeDef* LED_PORT[4]={GPIOD, GPIOD, GPIOD, GPIOD};
static const u16 LED_PIN[4]={GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14,GPIO_Pin_15};


// #define LED0(x) (x?(GPIO_SetBits(GPIOD,GPIO_Pin_12)):(GPIO_ResetBits(GPIOD,GPIO_Pin_12)))
// #define LED1(x) (x?(GPIO_SetBits(GPIOD,GPIO_Pin_13)):(GPIO_ResetBits(GPIOD,GPIO_Pin_13)))

extern void LED_Init(void);
extern void LEDOn(Led_Def Led);
extern void LEDOff(Led_Def Led);
extern void LEDTog(Led_Def Led);
extern void PE_OUT(u8 i);
void PE_Tog(uint16_t GPIO_Pin);
#endif
