/**
* @file timer.h
*
* stm32f10x timer driver header
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <stdbool.h>

void timerInit(void);
void timerStart(void);
void timerStop(void);
bool timerRegIrqFunc(void (*func)(void), uint32_t peridMs, int32_t repeatTimes);
void timerUnRegIrqFunc(void (*func)(void));
void timerUnRegIrqFuncsAll(void);

void irqTimHandler(void);

#endif

