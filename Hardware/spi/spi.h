#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx_conf.h"

#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_256 4

void SPI1_Init(void);
void SPI2_Init(void);
void CSPin_init(void);
void SPI1_SetSpeed(u8 SpeedSet);
void SPI2_SetSpeed(u8 SpeedSet);
u8 SPI1_ReadWriteByte(u8 byte);
u8 SPI2_ReadWriteByte(u8 byte);

#endif

