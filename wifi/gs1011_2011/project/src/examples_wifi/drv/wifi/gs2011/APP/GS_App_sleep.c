/**
 * @file GS_App_sleep.c
 *
 * Public method implementation for sleep example
 */
#include <stdint.h>
#include "../API/GS_API.h"
#include "../platform/GS_HAL.h"


void GS_App_sleep(uint32_t sleepMS){
     // Put device in sleep 
     GS_API_GotoDeepSleep();

     // Do something while device is sleeping 
     MSTimerDelay(sleepMS);

     // Wake device up 
     GS_API_WakeupDeepSleep();
}

