/*------------------------------------------------------------------------/
/RTC control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "chip.h"
#include "board.h"
#include "rtc.h"


int rtc_initialize (void)
{
	static int init;
	RTC_TIME_T rtcTime;

	if (init) /* Already initialized */
		return 1;

	/* RTC Block section ------------------------------------------------------ */
	Chip_RTC_Init(LPC_RTC);
	Chip_RTC_Enable(LPC_RTC, ENABLE);
	Chip_RTC_CalibCounterCmd(LPC_RTC, ENABLE);


	/* Set current time for RTC */
	/* Current time is 12:06:00AM, 2016-03-24 */
	rtcTime.time[RTC_TIMETYPE_SECOND]     = 0;
	rtcTime.time[RTC_TIMETYPE_MINUTE]     = 06;
	rtcTime.time[RTC_TIMETYPE_HOUR]       = 12;
	rtcTime.time[RTC_TIMETYPE_DAYOFWEEK]  = 5;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = 23;
	rtcTime.time[RTC_TIMETYPE_MONTH]      = 3;
	rtcTime.time[RTC_TIMETYPE_YEAR]       = 2016;
	Chip_RTC_SetFullTime(LPC_RTC, &rtcTime);

	/* Enable rtc (starts increase the tick counter and second counter register) */
	Chip_RTC_Enable(LPC_RTC, ENABLE);
	init = 1;

	return 1;
}

int rtc_gettime (RTC *rtc)
{
	RTC_TIME_T rtcTime;

	Chip_RTC_GetFullTime(LPC_RTC, &rtcTime);

	rtc->sec = rtcTime.time[RTC_TIMETYPE_SECOND];
	rtc->min = rtcTime.time[RTC_TIMETYPE_MINUTE];
	rtc->hour = rtcTime.time[RTC_TIMETYPE_HOUR];
	rtc->wday = rtcTime.time[RTC_TIMETYPE_DAYOFWEEK];
	rtc->mday = rtcTime.time[RTC_TIMETYPE_DAYOFMONTH];
	rtc->month = rtcTime.time[RTC_TIMETYPE_MONTH];
	rtc->year = rtcTime.time[RTC_TIMETYPE_YEAR];
  return 1;
}

int rtc_settime (const RTC *rtc)
{
	RTC_TIME_T rtcTime;

	rtcTime.time[RTC_TIMETYPE_SECOND]     = rtc->sec;
	rtcTime.time[RTC_TIMETYPE_MINUTE]     = rtc->min;
	rtcTime.time[RTC_TIMETYPE_HOUR]       = rtc->hour;
	rtcTime.time[RTC_TIMETYPE_DAYOFWEEK]  = rtc->wday;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->mday;
	rtcTime.time[RTC_TIMETYPE_MONTH]      = rtc->month;
	rtcTime.time[RTC_TIMETYPE_YEAR]	      = rtc->year;

	Chip_RTC_SetFullTime(LPC_RTC, &rtcTime);

  return 1;
}

/**
 * @brief	User Provided Timer Function for FatFs module
 * @return	Nothing
 * @note	This is a real time clock service to be called from FatFs module.
 * Any valid time must be returned even if the system does not support a real time clock.
 * This is not required in read-only configuration.
 */
DWORD get_fattime()
{
	RTC rtc;

	/* Get local time */
	rtc_gettime(&rtc);

	/* Pack date and time into a DWORD variable */
	return ((DWORD) (rtc.year - 1980) << 25)
		   | ((DWORD) rtc.month << 21)
		   | ((DWORD) rtc.mday << 16)
		   | ((DWORD) rtc.hour << 11)
		   | ((DWORD) rtc.min << 5)
		   | ((DWORD) rtc.sec >> 1);
}
