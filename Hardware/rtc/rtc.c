#include "rtc.h"
#include "gui.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "m_gui.h"

RTC_InitTypeDef  RTC_InitStructure;
RTC_TimeTypeDef  RTC_TimeStruct;
RTC_DateTypeDef  RTC_DateStruct;
const char Week_Char[7][3] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
void RTC_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  /* Enable the PWR clock */
	PWR_BackupAccessCmd(ENABLE);   /* Allow access to RTC */
	RCC_BackupResetCmd(ENABLE); /* Reset RTC Domain */
	RCC_BackupResetCmd(DISABLE);
	RCC_LSEConfig(RCC_LSE_ON); /* Enable the LSE OSC */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}/* Wait till LSE is ready */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); /* Select the RTC Clock Source */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF; /* Configure the RTC data register and RTC prescaler */

	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
	RTC_WaitForSynchro();//等待RTC APB寄存器同步

	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* Set the time to 00h 00mn 00s AM */
	RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours   = 1;
	RTC_TimeStruct.RTC_Minutes = 1;
	RTC_TimeStruct.RTC_Seconds = 1;  
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	
	RTC_DateStruct.RTC_Year = 13;
	RTC_DateStruct.RTC_Month = 1;
	RTC_DateStruct.RTC_Date = 1;
	RTC_DateStruct.RTC_WeekDay = 2;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

void Time_Set(unsigned char Hour, unsigned char Minute, unsigned char Second)
{
	PWR_BackupAccessCmd(ENABLE);
	RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours   = Hour;
	RTC_TimeStruct.RTC_Minutes = Minute;
	RTC_TimeStruct.RTC_Seconds = Second;  
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

void Date_Set(unsigned char Year, unsigned char Month, unsigned char Date, unsigned char Week)
{
	PWR_BackupAccessCmd(ENABLE);
	RTC_DateStruct.RTC_Year = Year;
	RTC_DateStruct.RTC_Month = Month;
	RTC_DateStruct.RTC_Date = Date;
	RTC_DateStruct.RTC_WeekDay = Week;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);	
}

void Time_Display(void)
{
	char Time[] = "00:00:00";
	char Date[] = "2013/01/01 Mon";
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	Time[0] = RTC_TimeStruct.RTC_Hours / 10 + '0';
	Time[1] = RTC_TimeStruct.RTC_Hours % 10 + '0';
	Time[3] = RTC_TimeStruct.RTC_Minutes / 10 + '0';
	Time[4] = RTC_TimeStruct.RTC_Minutes % 10 + '0';
	Time[6] = RTC_TimeStruct.RTC_Seconds / 10 + '0';
	Time[7] = RTC_TimeStruct.RTC_Seconds % 10 + '0';
	
	Date[2] = RTC_DateStruct.RTC_Year / 10 + '0';
	Date[3] = RTC_DateStruct.RTC_Year % 10 + '0';
	Date[5] = RTC_DateStruct.RTC_Month / 10 + '0';
	Date[6] = RTC_DateStruct.RTC_Month % 10 + '0';
	Date[8] = RTC_DateStruct.RTC_Date / 10 + '0';
	Date[9] = RTC_DateStruct.RTC_Date % 10 + '0';
	Date[11] = Week_Char[RTC_DateStruct.RTC_WeekDay - 1][0];
	Date[12] = Week_Char[RTC_DateStruct.RTC_WeekDay - 1][1];
	Date[13] = Week_Char[RTC_DateStruct.RTC_WeekDay - 1][2];
	printf("Time: %s\n\rDate: %s\n\r\n\r", Time, Date);
	MY_GUI_sprintf_string_h(220, 0, Date, YELLOW, BLACK);
	MY_GUI_sprintf_string_h(220, 150, Time, YELLOW, BLACK);
}




