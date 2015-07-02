/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
#include "mmc_sd.h"		/* Example: MMC/SDC contorl */
#include "spi.h"
//#include "malloc.h"
/* Definitions of physical drive number for each media */
#define ATA		1
#define MMC		0
#define USB		2


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	u8 stat;


	switch (drv) {
	case ATA :
		//result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;

	case MMC :
		stat = SD_Init();

		// translate the reslut code here
        if(stat)
        {
            SPI1_SetSpeed(SPI_SPEED_256);//低速
            SPI1_ReadWriteByte(0xff);//提供额外的8个时钟
            SPI1_SetSpeed(SPI_SPEED_4);  //高速
        }
		break;

	case USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
    if(stat)
    {
       return STA_NOINIT;
    }
    else return 0;//初始化成功
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
	u8 res=0;
	
    if (!count)return RES_PARERR;
	switch (drv) {
	case ATA :
		// translate the arguments here

		//result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

		return RES_OK;

	case MMC :
		// translate the arguments here
            if(count==1)
            {
                 res =SD_ReadSingleBlock(sector, buff);
               
            }
            else
            {
                 res =SD_ReadMultiBlock(sector, buff, count); //读多个sector                 
            }
             	
            if(res)
                {
                    SPI1_SetSpeed(SPI_SPEED_256);//低速
                    SPI1_ReadWriteByte(0xff);//提供额外的8个时钟
                    SPI1_SetSpeed(SPI_SPEED_4);  //高速
                }
		break;

	case USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return RES_OK;
	}
    if(res==0x00)return RES_OK;
    else return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
	u8 res;
	
    if(!count)return RES_PARERR;
	switch (drv) {
	case ATA :
		// translate the arguments here

		//result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return RES_OK;

	case MMC :
		// translate the arguments here
        if(count==1)
            {
                 res =SD_WriteSingleBlock(sector, (uint8_t *)(&buff[0]) );
               
            }
            else
            {
                 res =SD_WriteMultiBlock(sector,  (uint8_t *)(&buff[0]), count); //读多个sector                 
            }
             	
            if(res)
                {
                    SPI1_SetSpeed(SPI_SPEED_256);//低速
                    SPI1_ReadWriteByte(0xff);//提供额外的8个时钟
                    SPI1_SetSpeed(SPI_SPEED_4);  //高速
                }
		break;

		
	case USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return RES_OK;
    default:res=1;
	}
	if(res==0x00)return RES_OK;
    else return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;


	if(drv==MMC)//SD卡
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				SD_CS(0);
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				SD_CS(1);
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetCapacity();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else  res=RES_ERROR;//其他的不支持
    return res;
}
/*-----------------------------------------------------------------------*/
/* User defined to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)     */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)           */ 
/*-----------------------------------------------------------------------*/                                                                                                                                                                                                                                               
DWORD get_fattime (void)
{
#if 0
    struct tm t;
    DWORD date;
    
    t = Time_GetCalendarTime();
    t.tm_year -= 1980;  //????1980??
    t.tm_mon++;          //0-11???1-12?
    t.tm_sec /= 2;       //?????0-29
    
    date = 0;
    date = (t.tm_year << 25) | (t.tm_mon<<21) | (t.tm_mday<<16)|\
            (t.tm_hour<<11) | (t.tm_min<<5) | (t.tm_sec);

    return date;
#else
	return 0;
#endif
}

#endif
