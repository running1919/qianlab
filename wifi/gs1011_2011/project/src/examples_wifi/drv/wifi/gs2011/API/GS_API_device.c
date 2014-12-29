/**
 * @file GS_API_device.c
 *
 * Methods for handling device specific functionality 
 */

#include "GS_API.h"
#include "../platform/GS_HAL.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../AT/AtCmdLib.h"


/** Public Method Implementations **/
bool GS_API_CommWorking( void ){
     return AtLibGs_Check() == HOST_APP_MSG_ID_OK;
}

void GS_API_PrintModuleInformation( void ){
     if(AtLibGs_Version() == HOST_APP_MSG_ID_OK){
          char  appVer[10], gepsVer[10], wlanVer[10];
          if(AtLib_ParseVersionResponse(appVer, gepsVer, wlanVer)){
               GS_API_Printf("APP Version:");
               GS_API_Printf(appVer);
               GS_API_Printf("GEPS Version:");
               GS_API_Printf(gepsVer);
               GS_API_Printf("WLAN Version:");
               GS_API_Printf(wlanVer);
          } else {
               GS_API_Printf("Version Fail");
          }
     }
}


bool GS_API_UpdateFirmware(char* updateIp, char* updatePort, char* wlanPath, char* app0Path, char* app1Path){
     HOST_APP_MSG_ID_E message;
     
     // Set User Agent 
     AtLibGs_SetHttpConf(HOST_APP_HTTPCONF_HEADER_USER_AGENT, "UserAgent: Gainspan Demo");

     // Set Connection 
     AtLibGs_SetHttpConf(HOST_APP_HTTPCONF_HEADER_CONNECTION, "close");

     // Set Host Header 
     AtLibGs_SetHttpConf(HOST_APP_HTTPCONF_HEADER_HOST, updateIp);

     // Set Firmware Update Server IP 
     message = AtLibGs_SetFirmwareConf(HOST_APP_FWUP_CONF_SERVER_IP, updateIp);
     
     if(message == HOST_APP_MSG_ID_INVALID_INPUT){
       GS_API_Printf("FW Update not Supported");
       return false;
     }

     // Set Firmware Update Server Port 
     AtLibGs_SetFirmwareConf(HOST_APP_FWUP_CONF_SERVER_PORT, updatePort);

     // Set Wlan Path 
     AtLibGs_SetFirmwareConf(HOST_APP_FWUP_CONF_WLAN_URL, wlanPath);

     // Set App0 Path 
     AtLibGs_SetFirmwareConf(HOST_APP_FWUP_CONF_APP0_URL, app0Path);

     // Set App1 Path 
     AtLibGs_SetFirmwareConf(HOST_APP_FWUP_CONF_APP1_URL, app1Path);

     // Call Firmware Update 
     AtLibGs_StartFirmwareUpdate(HOST_APP_FWUPDATE_ALL);
     
     // Clear messages
     AtLib_FlushIncomingMessage();
     
     // Wait until we have a message from the module
     do{
          message = AtLib_ResponseHandleNoBlock();
     }while(message == HOST_APP_MSG_ID_NONE);
     
     return (message == HOST_APP_MSG_ID_FW_UPDATE_OK);
}

void GS_API_GotoDeepSleep(){
     // Put device in standby 
     AtLibGs_EnableDeepSleep();
}

void GS_API_WakeupDeepSleep(void){
     // Send a Byte 
     GS_HAL_send((uint8_t*)"\r\n", 2);

     // Give device time to wake up 
     MSTimerDelay(500);

     // Flush and handle data 
     GS_API_CheckForData();
}

