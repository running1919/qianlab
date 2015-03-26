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

typedef struct __timeval
{
    time_t tv_sec;       /* Seconds. */
    uint16_t tv_msec;    /* Millseconds. */
} timeval;


typedef struct __VSysClock{
  uint16_t wYear;    /* 1970- */
  uint8_t byMonth;   /* 1-12  */
  uint8_t byDay;     /* 1-31  */
  uint8_t byWeek;    /* 0-6   */
  uint8_t byHour;    /* 0-23  */
  uint8_t byMinute;  /* 0-59  */
  uint8_t bySecond;  /* 0-59  */
  uint16_t wMSecond; /* 0-999 */
} VSysClock; 


void clockInit(void);
timeval getTime(void);
void setTime(timeval tv);
void setAlarm(time_t t, void (*proc)(void));
void cancelAlarm(void);


VSysClock getSysTime(void);
void setSysTime(VSysClock* vsc);


/**
* \brief rtc interrupt handler
* \note user can't call this func
*/
void rtcIrqHandler(void);

#endif

