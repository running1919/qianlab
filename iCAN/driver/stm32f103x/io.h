/**
* @file io.h
*
* stm32f10x io interface
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __IO_H__
#define __IO_H__

typedef enum __IoLevel {
    Low  = 0,
    High = 1,
} IoLevel;

/* IO Pin Type Define */
typedef uint8_t IOPin[3];


void ioInit(void);
void ioConfigInput(uint8_t pinArray, uint8_t pinNum);
void ioConfigOutput(uint8_t pinArray, uint8_t pinNum);
uint8_t ioReadI(uint8_t pinArray, uint8_t pinNum);
uint8_t ioReadO(uint8_t pinArray, uint8_t pinNum);
void ioWrite(uint8_t pinArray, uint8_t pinNum, uint8_t level);

#endif

