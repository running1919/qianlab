/**
* @file timer.c
*
* stm32f10x TIM2 Config as User timer
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f10x.h"
#include "timer.h"


static void (*irqTimCall)(void) = NULL;
static int32_t recallTimes = 0;
static uint32_t msCounter = 0;
static uint32_t userPerid = 0;


void timerInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;      

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
    NVIC_Init(&NVIC_InitStructure);

    /* Prescaler configuration */
    //TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);
}


void timerStart(void)
{
    /* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);
}


void timerStop(void)
{
    /* TIM2 disable counter */
    TIM_Cmd(TIM2, DISABLE);
}


/**
* \brief register timer interrupt user func
* \param func user func pointer
* \param peridMs every timer call user func perid, unit ms
* \param repeatTimes repeat call user func times. if value == 0,
*        then timer out doesn't call user func; if value < 0, then
*        timer out always call user func;
*/
void timerRegIrqFunc(void (*func)(void), uint32_t peridMs, int32_t repeatTimes)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    if (func == NULL || peridMs == 0 || repeatTimes == 0) {
        return;
    }

    TIM_DeInit(TIM2);
    irqTimCall = func;
    userPerid = peridMs;
    recallTimes = repeatTimes;
    msCounter = 0;

    /*  Time base configuration 
     *  config base period is 1ms
     *  Note: CK_INT = 2 * PCLK1 = 72MHZ
     */
    TIM_TimeBaseStructure.TIM_Period = 1000;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Global);

    TIM_ClearFlag(TIM2, TIM_IT_Update);

    /* TIM IT enable */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);    
}


void irqTimHandler(void)
{
    TIM_ClearFlag(TIM2, TIM_IT_Update);

    ++msCounter;
    if (msCounter != userPerid) {
        return;
    }
    msCounter = 0;

    if (irqTimCall != NULL && recallTimes != 0) {
        irqTimCall();
        if (recallTimes < 0) {
            return;
        }
        --recallTimes;

        if (recallTimes == 0) {
            irqTimCall = NULL;            
            TIM_Cmd(TIM2, DISABLE);
            TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
        }
    }
}

