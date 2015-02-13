/**
* @file cand.h
*
* stm32f103x can high interface based on stm32f10x_can.h
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/


#ifndef __CAND_H__
#define __CAND_H__

#include "stm32f10x.h"
#include <stdbool.h>

typedef struct __RxFIFO {
    uint8_t fifonum;
    uint8_t usednum;
    uint8_t alldlc;
    CanRxMsg* rxMsg;    
} sRxFIFO;


void CAND_NVIC_Config(void);
void CAND_PIO_Config(void);
void CAND_Init(CAN_InitTypeDef* InitS, CAN_FilterInitTypeDef* FilterInit, sRxFIFO* Rxbuf);
void CAND_Activate(void);
bool CAND_Wakeup(void);
bool CAND_Sleep(void);
void CAND_Off(void);
bool CAND_IsReady(void);
bool CAND_IsTransferDone(uint8_t TransmitMailboxId);
void CAND_RequestComplete(uint8_t TransmitMailboxId);

uint8_t CAND_IsExcept(void);
uint8_t CAND_Transfer(CanTxMsg* TxMessage);

void CANx_RxHandler(void);
void CANx_SceHandler(void);


#endif

