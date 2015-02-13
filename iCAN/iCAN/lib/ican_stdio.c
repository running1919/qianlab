/**
* @file ican_stdio.c
*
* ican stdio
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include "ican_stdio.h"
#include "../include/ican_config.h"


void ican_printf(const char *format, ...)
{
#if DEBUG_ICAN
     uint8_t buffer[512];
     va_list args;

     // Start processing the arguments 
     va_start(args, format);

     // Output the parameters into a string. Note:printf max 512 bytes
     vsnprintf((char *)buffer, sizeof(buffer) - 1, format, args);

     // Output the string to the console 
     printf((char *)buffer);     

     // End processing of the arguments 
     va_end(args);
#endif
}