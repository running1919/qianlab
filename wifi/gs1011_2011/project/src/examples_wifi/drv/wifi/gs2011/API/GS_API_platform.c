/**
 * @file GS_API_platform.c
 *
 * Methods for handling platform specific functionality 
 */
#include <stdarg.h>
#include <stdio.h>

#include "GS_API.h"
#include "GS_API_private.h"
#include "../platform/GS_HAL.h"
#include "../AT/AtCmdLib.h"


/** Public Method Implementation **/
void GS_API_Printf(const char *format, ...)
{
     uint8_t buffer[1600];
     va_list args;

     // Start processing the arguments 
     va_start(args, format);

     // Output the parameters into a string 
     vsprintf((char *)buffer, format, args);

     // Output the string to the console 
     GS_HAL_print((char *)buffer);

     // End processing of the arguments 
     va_end(args);
}


void GS_API_Init(void){
     GS_HAL_init();

     // Send a \r\n to sync communication
     GS_HAL_send("\r\n", 2);
     AtLib_FlushIncomingMessage();

#if 1
     // Try to reset
     if(AtLibGs_Reset() == HOST_APP_MSG_ID_APP_RESET)
          GS_API_Printf("Reset OK");
     else
          GS_API_Printf("Reset Fail");

      GS_HAL_reset_buf();
#endif

     GS_HAL_send("\r\n", 2);
     AtLib_FlushIncomingMessage();
     // Turn off echo
     AtLibGs_SetEcho(0);

     // Turn on Bulk Data
     AtLibGs_BData(1);

     // Read saved network parameters
     gs_api_readNetworkConfig();
}
