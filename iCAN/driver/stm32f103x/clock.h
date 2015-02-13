/**
* @file clock.h
*
* stm32f10x clock header
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <time.h>

void clockInit(void);
time_t getTime(void);
void setTime(time_t t);
void setAlarm(time_t t);
void cancelAlarm(void);

/**
* \brief rtc interrupt handler
* \note user can't call this func
*/
void rtcIrqHandler(void);

#endif

