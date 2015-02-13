/**
* @file ican_timer.h
*
* ican timer
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_TMR_H__
#define __ICAN_TMR_H__

#include <stdint.h>
#include <stdbool.h>

void ican_tmr_init(void);
void ican_tmr_irq_proc(void);
uint32_t ican_tmr_ms_get(void);
uint32_t ican_tmr_ms_delta(uint32_t start);
void ican_tmr_ms_delay(uint32_t ms);

#endif
