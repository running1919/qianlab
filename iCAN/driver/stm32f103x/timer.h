/**
* @file timer.h
*
* stm32f10x timer driver header
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __TIMER_H__
#define __TIMER_H__


void timerInit(void);
void timerStart(void);
void timerStop(void);
void timerRegIrqFunc(void (*func)(void), uint32_t peridMs, int32_t repeatTimes);

void irqTimHandler(void);

#endif

