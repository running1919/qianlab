/**
* @file flash.h
*
* stm32f10x internal flash interface header
*
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __FLASH_H__
#define __FLASH_H__


void flashInit(void);
uint32_t flashRead(uint32_t offset, uint8_t* buff, uint32_t len);
uint32_t flashWrite(uint32_t offset, uint8_t* buff, uint32_t len);

#endif

