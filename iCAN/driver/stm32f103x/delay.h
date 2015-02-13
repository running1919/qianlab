/**
* @file delay.h
*
* delay header
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __DELAY_H__
#define __DELAY_H__

void sysMsTickProc(void);
uint32_t sysTickGet(void);
uint32_t sysTickDelta(uint32_t start);
void sysDelay(uint32_t ms);

#define delay(ms)   sysDelay(ms)

#endif

