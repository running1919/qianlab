/**
@file GS_App_tcp.c

Method implementation for TCP server and client task apps
*/
#include "GS_App.h"
#include "../API/GS_API.h"
#include "../platform/GS_HAL.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/** Private Defines **/
#define TCP_SERVER_PORT  "22222" ///< Port TCP Server will use
#define TCP_CLIENT_SEND_INTERVAL 1000 ///< Time to wait between sending temperature
#define TCP_MAX_CLIENT_CONNECTION (15) ///< Maximum possible TCP Server client connections (16 available - 1 TCP server connection)
#define TCP_CLIENT_DATA_STR  "Temperature: %3dC\r\n" ///< Formatted string for sending Temperature
#define START_TCP_CLIENT_STR "STARTTCPCLIENT" ///< Start TCP client command string
#define STOP_TCP_CLIENT_STR  "STOPTCPCLIENT"  ///< Stop TCP client command string
#define TCP_FIRMWARE_UPGRADE_STR "FWUPDATE" ///< Firmware Update command string
#define TCP_COMMAND_ACK_STR  "OK\r\n" ///< Command recognized message
#define TCP_COMMAND_NAK_STR  "NOT OK\r\n" ///< Unknown command message

typedef enum{
  TCP_NO_CMD,
  START_TCP_CLIENT,
  STOP_TCP_CLIENT,
  TCP_FIRMWARE_UPGRADE,
  TCP_UNKNOWN_CMD
} TCP_SERVER_COMMANDS;

/** Private Variables **/
static uint8_t tcpServerCID = GS_API_INVALID_CID; ///< Connection ID for TCP server
static uint8_t tcpServerClientCids[TCP_MAX_CLIENT_CONNECTION]; ///< Connection IDs for clients that made a connection to the TCP server
static uint8_t tcpClientCID = GS_API_INVALID_CID; ///< Connection ID for TCP client

/** Private Method Declarations **/
static void gs_app_handle_tcp_server_data(uint8_t cid, uint16_t data);
void gs_app_start_tcp_client(uint8_t* serverIp, uint8_t* serverPort);
static void gs_app_handle_tcp_client_data(uint8_t cid, uint16_t data);


/** Public Method Implmentation **/
void GS_App_start_tcp_task(void){
  // Reset all variables 
  memset(tcpServerClientCids, GS_API_INVALID_CID, sizeof(tcpServerClientCids));
  tcpServerCID = GS_API_INVALID_CID;  
  
  // Create the TCP Server connection
  tcpServerCID = GS_API_CreateTcpServerConnection(TCP_SERVER_PORT, gs_app_handle_tcp_server_data);
  if(tcpServerCID != GS_API_INVALID_CID){
    GS_API_Printf("TCP PORT: %s", TCP_SERVER_PORT);
  }
}

void GS_App_stop_tcp_task(void){
  uint8_t index;
  // Close the server CID
  if(tcpServerCID != GS_API_INVALID_CID){
    GS_API_CloseConnection(tcpServerCID);
  }
  
  // Close the client CID
  if(tcpClientCID != GS_API_INVALID_CID){
    GS_API_CloseConnection(tcpClientCID);
  }
  
  // Close any server client connections
  for(index = 0; index < TCP_MAX_CLIENT_CONNECTION; index++){
    if(tcpServerClientCids[index] != GS_API_INVALID_CID){
      GS_API_CloseConnection(tcpServerClientCids[index]);
    }
  }
  
  
}

uint16_t recvlen = 0;
uint8_t rxBuf[8192];

bool GS_App_tcp_task(void){    
  // Check for  a valid server connection
  if(tcpServerCID != GS_API_INVALID_CID || tcpClientCID != GS_API_INVALID_CID){
    // Check for incoming data
    GS_API_CheckForData();

    static uint32_t sendpackets = 0;
    static uint8_t failedcounts = 0;
    
    uint16_t sendlen = 0;
    uint16_t sendstart = 1;

    if (recvlen > sizeof(rxBuf) - 1) {
        recvlen = 0;
    }

    while (recvlen > 0 && rxBuf[0] < 17 && rxBuf[0] != 0) {          
        if (recvlen > 1460) {
            sendlen = 1460;
        }
        else {
            sendlen = recvlen;
        }
        rxBuf[sizeof(rxBuf) - 1] = 0;

        sendpackets++;
        if (!GS_API_SendTcpData(rxBuf[0], &rxBuf[sendstart], sendlen)) {
        //if(!AtLib_SendTcpData(rxBuf[0] + '0', &rxBuf[sendstart], sendlen)){
            // Sending failed, disable this connection ID
            tcpServerCID = GS_API_INVALID_CID;
            tcpClientCID = GS_API_INVALID_CID;
            GS_API_Printf("Send TCP Data failed, %d", ++failedcounts);
        }
        else {
            //GS_API_Printf("Send ^_^ Sucess %d packets\n", sendpackets);
            if (sendpackets % 1000 == 0) {
                GS_API_Printf("Send %d packets\n", sendpackets);
            }
        }

        recvlen -= sendlen;
        sendstart += sendlen;

        if (recvlen == 0) {
            rxBuf[0] = 0xFF;            
        }
	}    
    return 0;
  }
  return true;
}

/** Private Method Implementations **/

/**
@brief Handles incoming data for the TCP Server

This method handles incoming data for all TCP Server client connections
It will look for an end of line character, parse the line sent and 
look for a valid incoming command.
@private
@param cid Connection ID the data is coming from
@param data Byte of data coming from the TCP connection
*/
#if 1//if call AtLibGs_BData(1);
static void gs_app_handle_tcp_server_data(uint8_t cid, uint16_t dataLen)
{
    uint16_t curRecvdBytes;
    uint16_t recvBytes;
    uint8_t dummyBuf[1600];

    if (dataLen == 0) {
        return;
    }
    recvBytes = dataLen;

    /*Buffer just stores only one activing tcp socket*/
    if (cid != rxBuf[0]) {
        recvlen = 0;
        rxBuf[0] = 0xFF;
    }

    /*Save curruent CID*/
    rxBuf[0] = cid;

    /*Save data if Buffer not overflow*/
    if ((recvlen + dataLen) < (sizeof(rxBuf) - 1)) {
        while (recvBytes > 0) {
            /*Get size in Cur Recevie Buffer's Data which has stored */
            curRecvdBytes = GS_HAL_unproc_bytes();

            if (recvBytes <= curRecvdBytes) {
                GS_HAL_recv(&rxBuf[1 + recvlen], recvBytes, 1);
                recvBytes = 0;
            }
            else {
                GS_HAL_recv(&rxBuf[1 + recvlen], curRecvdBytes, 1);
                recvBytes -= curRecvdBytes;
            }

            recvlen += dataLen;
        }
    }
    /*Handle Buffer Overflow.Note:the 'Data Remaining' must be processed!!!*/
    else {
        GS_API_Printf("Tcp Handle Buffer Over Flow!!!");

         while (recvBytes > 0) {
            curRecvdBytes = GS_HAL_unproc_bytes();

            if (recvBytes <= curRecvdBytes) {
                GS_HAL_recv(dummyBuf, recvBytes, 1);
                recvBytes = 0;
            }
            else {
                GS_HAL_recv(dummyBuf, curRecvdBytes, 1);
                recvBytes -= curRecvdBytes;
            }
        }
    }
}

#else

static void gs_app_handle_tcp_server_data(uint8_t cid, uint16_t data){
    /*Buffer just stores only one activing tcp socket*/
    if (cid != rxBuf[0]) {
        recvlen = 0;
        rxBuf[0] = 0xFF;
    }

    /*Save curruent CID*/
    rxBuf[0] = cid;
    if (recvlen < sizeof(rxBuf) - 1) {
        rxBuf[1 + recvlen++] = (uint8_t)data;
    }
    else {
        GS_API_Printf("tcp handle Over Flow!!!");

    }
}

#endif

/**
@brief Starts a TCP client to connect to the specified server

Tries to connect using TCP to the IP and port passed in.
@private
@param serverIp IP of TCP server to connect to
@param serverPort port of TCP server to connect to
*/
void gs_app_start_tcp_client(uint8_t* serverIp, uint8_t* serverPort){
  
  // Check to see if we are already connected
  if(tcpClientCID != GS_API_INVALID_CID){
    GS_API_Printf("TCP Client Already Started");
    return;
  }
  
  // We're not connected, so lets create a TCP client connection
  tcpClientCID = GS_API_CreateTcpClientConnection((char*)serverIp, (char*)serverPort, gs_app_handle_tcp_client_data);
  
  // Now lets send data for the first time
  //gs_app_send_tcp_client_data();
}

/**
@brief Handles incoming data for the TCP Client

Does nothing, as we are using the TCP client connection as outgoing only

@private
@param cid Connection ID the data is coming from
@param data Byte of data coming from the TCP connection
*/
static void gs_app_handle_tcp_client_data(uint8_t cid, uint16_t data){
    rxBuf[0] = cid;    
    rxBuf[1 + recvlen++] = data;
}

