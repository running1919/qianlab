/**
* @file timer.c
*
* stm32f10x TIM2 Config as User timer
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/
#include <string.h>

#include "stm32f10x.h"
#include "timer.h"

typedef enum __taskState {
    TaskUnready = 0,
    TaskReady = 1,
    TaskRunning = 2,
} taskState;

typedef struct __timTask {
    void (*irqTimCall)(void);
    uint32_t userPerid;
    int32_t recallTimes;
    uint32_t msCounter;

    taskState state;
} timTask;

#define USER_TIM_TASK_MAX       10

static timTask userTimTask[USER_TIM_TASK_MAX];
static uint8_t userTaskRegNum = 0;

static bool timStart = false;

void timerInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

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

    TIM_DeInit(TIM2);

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

    memset(userTimTask, 0x0, sizeof(userTimTask));
}


void timerStart(void)
{
    if (!timStart) {
        /* TIM IT enable */
        TIM_ClearFlag(TIM2, TIM_IT_Update);
        TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

        /* TIM2 enable counter */
        TIM_Cmd(TIM2, ENABLE);

        timStart = true;
    }
}


void timerStop(void)
{
    TIM_Cmd(TIM2, DISABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

    timStart = false;
}


/**
* \brief register timer interrupt user func
* \param func user func pointer
* \param peridMs every timer call user func perid, unit ms
* \param repeatTimes repeat call user func times. if value == 0,
*        then timer out doesn't call user func; if value < 0, then
*        timer out always call user func;
*/
bool timerRegIrqFunc(void (*func)(void), uint32_t peridMs, int32_t repeatTimes)
{
    uint8_t i;

    if (func == NULL || peridMs == 0 || repeatTimes == 0
        || userTaskRegNum >= USER_TIM_TASK_MAX) {
        return false;
    }

    for (i = 0; i < USER_TIM_TASK_MAX; i++) {
        if (userTimTask[i].irqTimCall != NULL) {
            continue;
        }

        userTimTask[i].irqTimCall = func;
        userTimTask[i].userPerid = peridMs;
        userTimTask[i].recallTimes = repeatTimes;
        userTimTask[i].msCounter = 0;
        ++userTaskRegNum;

        /* if timerStart() was called,
         * maybe the user func will be soon call from now
         */
        userTimTask[i].state = TaskReady;

        break;
    }

    if (i == USER_TIM_TASK_MAX) {
        return false;
    }    

    return true;
}


void timerUnRegIrqFunc(void (*func)(void))
{
    uint8_t i;

    if (userTaskRegNum == 0 || func == NULL) {
        return;
    }

    for (i = 0; i < USER_TIM_TASK_MAX; i++) {
        if (userTimTask[i].irqTimCall == func) {
            while (userTimTask[i].state == TaskRunning);
            userTimTask[i].irqTimCall = NULL;
            --userTaskRegNum;
            break;
        }
    }
}


void timerUnRegIrqFuncsAll(void)
{
    uint8_t i;

    for (i = 0; i < USER_TIM_TASK_MAX; i++) {
        while (userTimTask[i].state == TaskRunning);
        userTimTask[i].irqTimCall = NULL;
        userTimTask[i].state = TaskUnready;
    }

    userTaskRegNum = 0;
    timStart = false;
}


void irqTimHandler(void)
{
    uint8_t i;
    TIM_ClearFlag(TIM2, TIM_IT_Update);

    if (userTaskRegNum == 0) {
        memset(userTimTask, 0x0, sizeof(userTimTask));
        TIM_Cmd(TIM2, DISABLE);
        TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
        timStart = false;
        return;
    }

    for (i = 0; i < USER_TIM_TASK_MAX; i++) {
        if (userTimTask[i].irqTimCall == NULL
            || userTimTask[i].state == TaskUnready) {
            userTimTask[i].state = TaskUnready;
            continue;
        }

        userTimTask[i].state = TaskRunning;
   
        ++userTimTask[i].msCounter;
        if (userTimTask[i].msCounter != userTimTask[i].userPerid) {
            userTimTask[i].state = TaskReady;
            continue;
        }

        userTimTask[i].msCounter = 0;

        if (userTimTask[i].recallTimes != 0) {
            userTimTask[i].irqTimCall();

            if (userTimTask[i].recallTimes < 0) {
                userTimTask[i].state = TaskReady;
                continue;
            }
            --userTimTask[i].recallTimes;

            if (userTimTask[i].recallTimes == 0) {
                userTimTask[i].irqTimCall = NULL;
                userTimTask[i].state = TaskUnready;
                --userTaskRegNum;
            }
        }
        else {
            userTimTask[i].irqTimCall = NULL;
            userTimTask[i].state = TaskUnready;
            --userTaskRegNum;
        }        
    }
}

