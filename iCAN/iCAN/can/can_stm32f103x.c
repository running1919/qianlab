/**
* @file can_stm32f103x.c
*
* stm32f103x can driver
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include "../../driver/stm32f103x/cand.h"
#include "can_stm32f103x.h"

#include "../include/can_type.h"
#include "../include/ican_type.h"
#include "../include/can_config.h"
#include "../lib/ican_timer.h"
#include "../lib/ican_stdio.h"

#include <stdio.h>
#include <string.h>

static CAN_InitTypeDef InitType;
static CAN_FilterInitTypeDef FilterInitType;

/** CAN Transfer Buffer */
static CanRxMsg rxMsgBuf[ICAN_SPLIT_MAX_SEGS];
static sRxFIFO RxBuf;

static uint8_t trans_mailboxid = 0;
static CanTxMsg transMsg;

void can_configure(uint8_t channel, uint16_t baudrate)
{
    switch (baudrate) {
    case RATE_1000K:
        InitType.CAN_Prescaler = 2;
        InitType.CAN_SJW = CAN_SJW_4tq;
        InitType.CAN_BS1 = CAN_BS1_8tq;
        InitType.CAN_BS2 = CAN_BS2_8tq;
        break;

    case RATE_800K://can't passed test with sam9x25
        InitType.CAN_Prescaler = 3;
        InitType.CAN_SJW = CAN_SJW_3tq;
        InitType.CAN_BS1 = CAN_BS1_8tq;
        InitType.CAN_BS2 = CAN_BS2_6tq;       
        break;
        
    case RATE_500K:
        InitType.CAN_Prescaler = 4;
        InitType.CAN_SJW = CAN_SJW_3tq;
        InitType.CAN_BS1 = CAN_BS1_8tq;
        InitType.CAN_BS2 = CAN_BS2_8tq;
        break;

    case RATE_250K:
        InitType.CAN_Prescaler = 12;
        InitType.CAN_SJW = CAN_SJW_2tq;
        InitType.CAN_BS1 = CAN_BS1_5tq;
        InitType.CAN_BS2 = CAN_BS2_6tq;
        break;

    case RATE_125K:
        InitType.CAN_Prescaler = 18;
        InitType.CAN_SJW = CAN_SJW_3tq;
        InitType.CAN_BS1 = CAN_BS1_7tq;
        InitType.CAN_BS2 = CAN_BS2_8tq;
        break;

    case RATE_50K://can't passed test with sam9x25
        InitType.CAN_Prescaler = 60;
        InitType.CAN_SJW = CAN_SJW_2tq;
        InitType.CAN_BS1 = CAN_BS1_6tq;
        InitType.CAN_BS2 = CAN_BS2_5tq;
        break;

    case RATE_25K://can't passed test with sam9x25
        InitType.CAN_Prescaler = 120;
        InitType.CAN_SJW = CAN_SJW_2tq;
        InitType.CAN_BS1 = CAN_BS1_6tq;
        InitType.CAN_BS2 = CAN_BS2_5tq;
        break;

    case RATE_10K://can't passed test with sam9x25
        InitType.CAN_Prescaler = 300;
        InitType.CAN_SJW = CAN_SJW_2tq;
        InitType.CAN_BS1 = CAN_BS1_6tq;
        InitType.CAN_BS2 = CAN_BS2_5tq;
        break;
        
    default:
        InitType.CAN_Prescaler = 2;
        InitType.CAN_SJW = CAN_SJW_4tq;
        InitType.CAN_BS1 = CAN_BS1_8tq;
        InitType.CAN_BS2 = CAN_BS2_8tq;
    }

    CAND_NVIC_Config();
    CAND_PIO_Config();

    /* CAN cell init */
    InitType.CAN_TTCM = DISABLE;
    InitType.CAN_ABOM = DISABLE;
    InitType.CAN_AWUM = DISABLE;
    InitType.CAN_NART = DISABLE;
    InitType.CAN_RFLM = ENABLE;
    InitType.CAN_TXFP = DISABLE;
    InitType.CAN_Mode = CAN_Mode_Normal;
    //InitType.CAN_Mode = CAN_Mode_LoopBack;
    
    switch (channel) {
    case 0 :
        RxBuf.rxMsg = rxMsgBuf;
        RxBuf.fifonum = ICAN_SPLIT_MAX_SEGS;
        RxBuf.rp = 0;
        RxBuf.wp = 0;
        CAND_Init(&InitType, &FilterInitType, &RxBuf);
        break;

    default :
        ican_printf("No CAN%d Interface\n\r");
        break;
    }    
}


bool can_active(uint8_t channel, uint8_t recv_macid)
{
    volatile uint32_t tick;   

    if (channel > NUM_CAN_IF) {
        ican_printf("No CAN%d Interface\n\r");
    }

    /* CAN filter init */    
    FilterInitType.CAN_FilterMode = CAN_FilterMode_IdMask;
    FilterInitType.CAN_FilterScale = CAN_FilterScale_32bit;
    FilterInitType.CAN_FilterIdHigh = (uint16_t)(recv_macid) << 8;
    FilterInitType.CAN_FilterIdLow = 0x0000;
    FilterInitType.CAN_FilterMaskIdHigh = 0xFF00;
    FilterInitType.CAN_FilterMaskIdLow = 0x0000;
    FilterInitType.CAN_FilterActivation = ENABLE;
    
    tick = ican_tmr_ms_get();
    while (ican_tmr_ms_delta(tick) < CAN_TO) {
        CAND_Activate();
        ican_tmr_ms_delay(100);
        if (CAND_IsReady()) { 
            return true;
        }
    }
    
    return false;
}


bool can_wakeup(uint8_t channel)
{
    volatile uint32_t tick;

    if (channel > NUM_CAN_IF) {
        ican_printf("No CAN%d Interface\n\r");
    }


    tick = ican_tmr_ms_get();
    while (ican_tmr_ms_delta(tick) < CAN_TO) {
        if (CAND_Wakeup()) {            
            return true;
        }
    }
    ican_printf("CAN%d can't wakeup\n\r", channel);
    return false;
}


void can_sleep(uint8_t channel)
{
    volatile uint32_t tick;

    if (channel > NUM_CAN_IF) {
        ican_printf("No CAN%d Interface\n\r", channel);
    }


    tick = ican_tmr_ms_get();
    while (ican_tmr_ms_delta(tick) < CAN_TO) {
        if (CAND_Sleep()) {            
            return;
        }
    }

    ican_printf("CAN%d can't sleep\n\r", channel);
}


void can_off(uint8_t channel)
{
    /* POWER OFF */
    CAND_Off();
}


bool can_is_except(uint8_t channel)
{
    if (CAND_IsExcept() > 0) {
        return true;
    }

    return false;
}


bool can_mb_request(uint8_t channel, uint8_t* mailbox_id, uint8_t mail_type)
{
    if (channel > NUM_CAN_IF || mailbox_id == NULL) {
        return false;
    }

    transMsg.IDE = CAN_ID_EXT;// CAN_Id_Extended
    transMsg.RTR = CAN_RTR_DATA;
    transMsg.DLC = 0;

    return true;
}


void can_mb_free(uint8_t channel, uint8_t mailbox_id)
{
    transMsg.IDE = 0;
    transMsg.RTR = 0;
    transMsg.DLC = 0;

    CAND_RequestComplete(mailbox_id);
}


void can_set_id(uint8_t channel, uint8_t mailbox_id, const ican_id* id)
{
    uint32_t msg_id = 0;

    msg_id = id->source_id;
    msg_id |= (uint32_t)(id->func_id) << 8;
    msg_id |= (uint32_t)(id->ack) << 12;
    msg_id |= (uint32_t)(id->src_mac_id) << 13;
    msg_id |= (uint32_t)(id->dest_mac_id) << 21;

    transMsg.ExtId = msg_id;    
}


void can_get_id(uint8_t channel, uint8_t mailbox_id, ican_id* id)
{
    if (id == NULL) {
        return;
    }

    CanRxMsg* rxmsg = RxBuf.rxMsg;
    uint32_t msg_id = rxmsg[mailbox_id].ExtId;

    msg_id = msg_id & 0x1fffffff;

    id->dest_mac_id = msg_id >> 21; //8bit
    id->src_mac_id  = (msg_id & (0xff << 13)) >> 13; //8bit
    id->ack         = (msg_id & (0x1 << 12)) >> 12; //1bit
    id->func_id     = (msg_id & (0xf << 8)) >> 8; //4bit
    id->source_id   = msg_id & 0xff; //8bit
}

/**
* \return msg len
*/
uint8_t can_check_inbox(uint8_t channel, uint8_t* mailbox_id)
{
    if (RxBuf.rp != RxBuf.wp) {
        *mailbox_id = RxBuf.rp;
        return RxBuf.rxMsg[RxBuf.rp].DLC;
    }
#if 0
    uint8_t i ,j;
    CanRxMsg* rxmsg = RxBuf.rxMsg;

    j = 0;
    for (i = 0; i < 6;) {
        if (rxmsg[i].DLC > 0) {
            if (j != 3) {
                ++j;
                continue;
            }
            *mailbox_id = i;
            return rxmsg[i].DLC;
        }
        ++i;
    }
#endif
    return 0;
}

/**
* \note clear buff after get data
*/
void can_read_mail(uint8_t channel, uint8_t mailbox_id, uint8_t* buff, uint8_t size)
{
    uint8_t len = 0;
    CanRxMsg* rxmsg = RxBuf.rxMsg;    

    if (rxmsg[mailbox_id].DLC > 0) {
        len = size;
        if (size > rxmsg[mailbox_id].DLC) {
            len = rxmsg[mailbox_id].DLC;
        }    

        memcpy(buff, &rxmsg[mailbox_id].Data, len);
        rxmsg[mailbox_id].DLC = 0;
        RxBuf.alldlc -= len;
        RxBuf.rp = (RxBuf.rp + 1) % RxBuf.fifonum;
    }
}



//0 is busy, 1 is ilde
bool can_check_outbox(uint8_t channel, uint8_t mailbox_id)
{
    if (!CAND_IsTransferDone(trans_mailboxid)) {
        return false;
    }
    else {
        return true;
    }
}


/**
* \note just put data, but not transfer
*/
void can_write_mail(uint8_t channel, uint8_t mailbox_id, const uint8_t* buff, uint8_t size)
{
    uint8_t len = size;

    if (size > 8) {
        len = 8;
    }
    memcpy(&transMsg.Data, buff, size);
    
    transMsg.DLC = len;
}


void can_clear_trans_buf(uint8_t channel, uint8_t mailbox_id, bool all)
{

}


bool can_transfer_start(uint8_t channel, uint8_t mailbox_id)
{
    uint32_t curtick = 0;
    if (transMsg.DLC == 0) {
        return true;
    }

    if (CAND_IsExcept() > 0) {
        return false;
    }

    trans_mailboxid = CAND_Transfer(&transMsg);
    if (trans_mailboxid != (uint8_t)0xFF) {
        transMsg.DLC = 0;

        curtick = ican_tmr_ms_get();
        while (!CAND_IsTransferDone(trans_mailboxid)) {
            if (ican_tmr_ms_delta(curtick) > CAN_TO) {
                return false;
            }
        }
        return true;
    }

    return false;
}

