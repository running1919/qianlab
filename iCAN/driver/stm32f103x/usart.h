/**
* @file usart.h
*
* stm32f10x realize printf func
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __USART_H__
#define __USART_H__

#define DEBUG_ENABLE

/* if defined DBUG_SEMIHOSTED, the characters will output
 * to debug console.
 * Note: The IAR 'General Options' -> 'Library Configration'
 * -> 'semihosted' must be enable.
 */
//#define DBUG_SEMIHOSTED

void usartConsoleInit(void);
void usartPutchar(int c);
int  usartGetchar(void);
void printStr(const char *format, ...);

#endif

