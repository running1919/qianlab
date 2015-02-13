/**
* @file can_sma9x25.h
*
* sma9x25 can driver
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __CAN_SAM9X25_H
#define __CAN_SAM9X25_H



/** CAN operation timeout (ms) */
#define CAN_TO          500

#define MB_UNUSED       0
#define MB_USED         1

#define RECV_MB_START   2
#define RECV_MB_END     7


typedef struct _Trans_Mailbox {
    sCandMbCfg mbcfg;
    sCandTransfer transfer;
    uint8_t request_state;
} sTransMailbox;

#endif