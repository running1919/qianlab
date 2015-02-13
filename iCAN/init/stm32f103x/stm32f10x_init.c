/**
* @file stm32f10x.c
*
* stm32f10x system init interface
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <core_cm3.h>


extern uint32_t SystemCoreClock;


void __SysNvic_Config(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
}


void __SysTick_config(void)
{
  if (SysTick_Config(SystemCoreClock / 1000)) {//ms
    /* Capture error */ 
    while (1);
  }
}

void sysInit(void)
{
    __SysTick_config();
    __SysNvic_Config();
}
