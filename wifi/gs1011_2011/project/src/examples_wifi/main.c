/**
 * \file
 *
 * This file contains all the specific code for the spi_wifi app.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <board.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "./drv/wifi/wifi.h"

//void (*sysint_handlers[])(void);

extern volatile uint32_t G_msTimer;//该变量需要放在系统的毫秒定时器中，每1ms自动加1

void sysint_handler(void)
{
    /* Clear interrupt */
    PIT_GetPIVR();
    AIC->AIC_ICCR = 1 << ID_SYS;

    TimeTick_Increment(1);
    G_msTimer ++;//gs2011 timer use it
}

#define TCPCON  1
#define SERVER  1
#define APMODE  1

#if APMODE
static void runAPMode(void)
{
    GS_App_start_provisioning_task();

#if TCPCON

/*build TCP socket*/
#if SERVER // server mode
    GS_App_start_tcp_task();
#else      //client mode
    gs_app_start_tcp_client("192.168.1.135", "55555");
#endif

/*build UDP socket*/
#else
//\\\To Do...
#endif
}

#else
/*return > 0, join successfully */
static void runSTMode(void)
{
    if (GS_App_join_network()) {
#if TCPCON
/*build TCP socket*/

#if SERVER // server mode
        GS_App_start_tcp_task();
#else      //client mode
        gs_app_start_tcp_client("192.168.1.135", "55555");
#endif//SERVER

/*build UDP socket*/
#else
    //\\\To Do...
#endif// TCPCON
    }
    else {//if (GS_App_join_network())
        while (1);
    }
}
#endif//APMODE

static void gainSpanInit(void)
{
    GS_API_Init();

#if APMODE
    runAPMode();
#else
    runSTMode();
#endif
}

static void daemonSockets(void)
{
    while (1) {
    /*You should redefine this function*/
        GS_App_tcp_task();
    }
}

void main(void)
{
    /*Disable watchdog */
    WDT_Disable(WDT);
    
    GS_API_Printf("\n\nStart system & Test WiFi chip\n");

    /* Open sys interrupt and pit interrupt */
    IRQ_ConfigureIT(ID_SYS, 5, sysint_handler);
    IRQ_EnableIT(ID_SYS);
    TimeTick_Configure(BOARD_MCK);    

    IRQ_DisableIT(ID_FIQ);

    gainSpanInit();
    
    daemonSockets();
}

