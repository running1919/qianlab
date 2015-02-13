/**
* @file clock.c
*
* stm32f10x clock interface
*
* \note Time represents the [1970, 2106)range
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <stdint.h>
#include <stdbool.h>

#include "stm32f10x.h"
#include "clock.h"
#include "usart.h"


//#define RTC_CLK_USE_LSE


/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
static void rtcNvicConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


#ifdef RTC_CLK_USE_LSE
static void rtcConfig(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}


void clockInit(void)
{
    /* NVIC configuration */
    rtcNvicConfig();

    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        /* RTC Configuration */
        rtcConfig();

        /* Adjust time by values entered by the user on the hyperterminal */
        //setTime(t);

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else  {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
            printStr("\r\n\n Power On Reset occurred....");
        }

        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
            printStr("\r\n\n External Reset occurred....");
        }

        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();

        /* Enable the RTC Second */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }

#ifdef RTCClockOutput_Enable
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Disable the Tamper Pin */
    BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

    /* Enable RTC Clock Output on Tamper Pin */
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

    /* Clear reset flags */
    RCC_ClearFlag();

    /* RTC Clock Calibration */
    //...
}

#else

static void rtcConfig(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(40000); //LSI default is 40k HZ

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* To output second signal on Tamper pin, the tamper functionality
     * must be disabled (by default this functionality is disabled)
     */
    //BKP_TamperPinCmd(DISABLE);

    /* Enable the RTC Second Output on Tamper Pin */
    //BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
}


void clockInit(void)
{
    /* NVIC configuration */
    rtcNvicConfig();

    /* RTC Configuration */
    rtcConfig();

    /* RTC Clock Calibration */
    //...
}
#endif


time_t getTime(void)
{
    return RTC_GetCounter();
}


void setTime(time_t t)
{
    RTC_WaitForLastTask();
    RTC_SetCounter(t);
    RTC_WaitForLastTask();
}


void setAlarm(time_t t)
{
    RTC_WaitForLastTask();
    RTC_SetAlarm(t);
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();
}


void cancelAlarm(void)
{
    RTC_WaitForLastTask();
    RTC_SetAlarm(0);
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, DISABLE);
    RTC_WaitForLastTask();
    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_WaitForLastTask();
}

void rtcIrqHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
        RTC_ClearITPendingBit(RTC_FLAG_SEC);
    }

    /* ignore overflow */
    //if (RTC_GetITStatus(RTC_IT_OW) != RESET) {
    //    RTC_ClearITPendingBit(RTC_FLAG_OW);
    //}

    if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {
        RTC_ClearITPendingBit(RTC_FLAG_ALR);
        printStr("ALARM---\n\r");
    }
}


/* STD C TIME FUNC */
__time32_t __time32(__time32_t *p)
{
    //return sysTime;
    return RTC_GetCounter();
}

