/**
* @file delay.c
*
* delay func
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <time.h>
#include "delay.h"

static uint32_t volatile sysMsTick = 0;

void sysMsTickProc(void)
{
    ++sysMsTick;
}


uint32_t sysTickGet(void)
{
    return sysMsTick;
}


uint32_t sysTickDelta(uint32_t start)
{
    return sysTickGet() - start;
}


void sysDelay(uint32_t ms)
{
    uint32_t start = sysTickGet();

    while (sysTickDelta(start) < ms) {
    }
}


/* STD C TIME FUNC */
clock_t clock(void)
{
    return sysMsTick;
}

