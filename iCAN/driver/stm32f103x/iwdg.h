/**
* @file iwdg.h
*
* stm32f103x iwdg high interface based on stm32f10x_iwdg.h
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __IWDG_H__
#define __IWDG_H__

#include <stdint.h>
#include <stdbool.h>

bool iwdgOpen(uint32_t msfeed);
void iwdgFeed(void);

#endif

