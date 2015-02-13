/**
* @file ican_timer.c
*
* ican timer
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <stdio.h>

#include "ican_timer.h"


static volatile uint32_t ican_mstimer;

uint32_t ican_tmr_ms_get(void)
{
    return ican_mstimer;
}


uint32_t ican_tmr_ms_delta(uint32_t start)
{
    return ican_tmr_ms_get() - start;
}


void ican_tmr_ms_delay(uint32_t ms)
{
    uint32_t start = ican_tmr_ms_get();

    while (ican_tmr_ms_delta(start) < ms) {
    }
}


/**
* \brief process ican timer
* \note this func must be called in ms timer handler of system 
*/
void ican_tmr_irq_proc(void)
{
    ++ican_mstimer;
}


void ican_tmr_init(void)
{
    ican_mstimer = 0;
}
