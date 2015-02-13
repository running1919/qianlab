/**
* @file iwdg.c
*
* stm32f103x iwdg high interface based on stm32f10x_iwdg.c
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include "stm32f10x.h"
#include "iwdg.h"


/**
* \note Max Feed Dog time about 13s, min is 4 ms
*/
bool iwdgOpen(uint32_t msfeed)
{
    uint32_t reloadval = 0;

    if (msfeed > 13000 || msfeed < 4) {
        return false;
    }

    reloadval = ((uint32_t)0x1000 * msfeed - 13000) / msfeed;

    /* Check if the system has resumed from IWDG reset */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) {    
        /* Clear reset flags */
        RCC_ClearFlag();
    }

    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/128 */
    IWDG_SetPrescaler(IWDG_Prescaler_128);

    IWDG_SetReload(reloadval);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Disable IWDG when CPU work in debug mode */
    DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);
    
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

    return true;
}


void iwdgFeed(void)
{
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
}

