/**
* @file can_sma9x25.c
*
* sma9x25 can driver
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/
#include <board.h>
#include "can_sam9x25.h"
    
#include "../include/can_type.h"
#include "../include/ican_type.h"
#include "../include/can_config.h"
#include "../lib/ican_timer.h"
#include "../lib/ican_stdio.h"
    
#include <stdio.h>
#include <string.h>


/** PIO for CAN */
static const Pin pinsCAN[] = {PINS_CAN0,PINS_CAN1};
/** CAN Driver instance */
static sCand cand[NUM_CAN_IF];
/** CAN Transfer Buffer */
static sTransMailbox transMbs[NUM_CAN_IF][SEND_MB_NUM];
static sTransMailbox recvsMbs[NUM_CAN_IF][RECV_MB_NUM];

static canRxfifo rxMsgFiFO[NUM_CAN_IF];
static canMsg rxMsgBuf[NUM_CAN_IF][ICAN_SPLIT_MAX_SEGS * 4];

/** CAN Interrupt Mask */
static uint32_t canIntMskInit = 0;


static void CAN0_IrqHandler(void)
{
    CAND_Handler(&cand[0]);
}


static void CAN1_IrqHandler(void)
{
    CAND_Handler(&cand[1]);
}


static uint8_t _WaitXfrTO(sCand* pCand, sCandTransfer *pXfr)
{
    volatile uint32_t t = ican_tmr_ms_get();
    while (ican_tmr_ms_delta(t) < CAN_TO) {
        if (CAND_IsTransferDone(pCand, pXfr))
            return 1;
    }
    return 0;
}

void can_configure(uint8_t channel, uint16_t baudrate)
{
    uint16_t bdrate = 0;

    switch (baudrate) {
    case RATE_1000K:
        bdrate = 1000;
        break;

    case RATE_800K:
        bdrate = 800;
        break;
        
    case RATE_500K:
        bdrate = 500;
        break;

    case RATE_250K:
        bdrate = 250;
        break;

    case RATE_125K:
        bdrate = 125;
        break;

    case RATE_50K:
        bdrate = 50;
        break;

    case RATE_25K:
        bdrate = 25;
            break;

    case RATE_10K:
        bdrate = 10;
        break;
        
    default:
        bdrate = 50;
    }

    PIO_Configure(pinsCAN, PIO_LISTSIZE(pinsCAN));

    switch (channel) {
    case 0 :
        CAND_Init(&cand[0], CAN0, ID_CAN0, bdrate, BOARD_MCK);
        IRQ_ConfigureIT(ID_CAN0, 0, CAN0_IrqHandler);
        IRQ_EnableIT(ID_CAN0);
        break;

    case 1 :
        CAND_Init(&cand[1], CAN1, ID_CAN1, bdrate, BOARD_MCK);
        IRQ_ConfigureIT(ID_CAN1, 0, CAN1_IrqHandler);
        IRQ_EnableIT(ID_CAN1);

    default :
        ican_printf("No CAN%d Interface\n\r");
        break;
    }    
}


bool can_active(uint8_t channel, uint8_t recv_macid)
{
    uint8_t i;
    volatile uint32_t tick;
    sCand *pCand;
    sCandMbCfg *mbCfg;
    sCandTransfer *trfCfg;

    if (channel > NUM_CAN_IF) {
        ican_printf("No CAN%d Interface\n\r");
    }

    pCand = &cand[channel];
    
    CAND_Activate(pCand);
    tick = ican_tmr_ms_get();
    while (ican_tmr_ms_delta(tick) < CAN_TO) {
        if (CAND_IsReady(pCand)) {
            /* init recv mailboxs */
            for (i = 0; i < RECV_MB_NUM; i++) {
                mbCfg = &recvsMbs[channel][i].mbcfg;
                trfCfg = &recvsMbs[channel][i].transfer;
                
                mbCfg->bMsgType = ICAN_MAIL_BOX_RX;
                mbCfg->bTxPriority = 0;
                mbCfg->dwMsgMask = (0xff << 21) | CAN_MAM_MIDE;
                trfCfg->bState = 0;
                trfCfg->dwMsgID = (recv_macid << 21) | CAN_MID_MIDE;
                trfCfg->bMailbox = i + RECV_MB_START;
                CAND_ConfigureTransfer(pCand, mbCfg, trfCfg);
                CAND_StartTransfers(pCand, 0x1u << (i + RECV_MB_START));
            }

            /* init send mailboxs */
            for (i = 0; i < SEND_MB_NUM; i++) {
                transMbs[channel][i].transfer.bMsgLen = 0;
                transMbs[channel][i].transfer.bState = 0;
                transMbs[channel][i].mbcfg.bMsgType = 0;
            }

            /* init revc fifo and register fifo */
            rxMsgFiFO[channel].rxmsg = rxMsgBuf[channel];
            rxMsgFiFO[channel].rp = 0;
            rxMsgFiFO[channel].wp = 0;
            rxMsgFiFO[channel].fifoNum = sizeof(rxMsgBuf[channel]) /sizeof(canMsg);
            CAND_RegFifo(channel, &rxMsgFiFO[channel]);

            /* get interrupt mask */
            canIntMskInit = CAN_GetItMask(cand[channel].pHw);
            canIntMskInit &= 0x1fff0000;
            
            return true;
        }
    }
    
    return false;
}


bool can_wakeup(uint8_t channel)
{
    volatile uint32_t tick;
    sCand *pCand; 

    if (channel > NUM_CAN_IF) {
        ican_printf("No CAN%d Interface\n\r", channel);
    }

    pCand = &cand[channel];
    
    CAND_Activate(pCand);
    tick = ican_tmr_ms_get();
    while (ican_tmr_ms_delta(tick) < CAN_TO) {
        if (CAND_IsReady(pCand)) {
            canIntMskInit = CAN_GetItMask(cand[channel].pHw);
            canIntMskInit &= 0x1fff0000;
            
            return true;
        }
    }
    ican_printf("CAN%d can't wakeup\n\r", channel);
    return false;
}


void can_sleep(uint8_t channel)
{
    CAND_Sleep(&cand[channel]);
}


void can_off(uint8_t channel)
{
    /* POWER OFF */
    switch (channel) {
        case 0 :
            PMC_DisablePeripheral(ID_CAN0);
            break;

        case 1 :
            PMC_DisablePeripheral(ID_CAN1);
            break;
            
        default :
            break;
    }
}


bool can_is_except(uint8_t channel)
{
    uint32_t dwItMsk = 0;
    
    dwItMsk = CAN_GetItMask(cand[channel].pHw);

    if ((uint32_t)(dwItMsk & 0x1fff0000) != canIntMskInit) {
        return true;
    }
    else {
        return false;
    }
}


bool can_mb_request(uint8_t channel, uint8_t* mailbox_id, uint8_t mail_type)
{
    uint8_t i;

    if (channel > NUM_CAN_IF || mailbox_id == NULL) {
        return false;
    }

    for (i = 0; i < SEND_MB_NUM; i++) {
        if (transMbs[channel][i].request_state == MB_UNUSED) {
            *mailbox_id = i;
            transMbs[channel][i].request_state = MB_USED;
            transMbs[channel][i].mbcfg.bMsgType = mail_type;
            transMbs[channel][i].mbcfg.dwMsgMask = CAN_MAM_MIDE | (0xff << 21);
            transMbs[channel][i].mbcfg.bTxPriority = 0;
            transMbs[channel][i].transfer.bMailbox = i;
            transMbs[channel][i].transfer.bState = 0;
            return true;
        }
    }

    return false;
}


void can_mb_free(uint8_t channel, uint8_t mailbox_id)
{
    sCand *pCand = &cand[channel];

    transMbs[channel][mailbox_id].request_state = MB_UNUSED;
    transMbs[channel][mailbox_id].mbcfg.bMsgType = 0;    
    transMbs[channel][mailbox_id].transfer.bState = 0;
    CAND_ResetMailbox(pCand, mailbox_id, NULL);
}


void can_set_id(uint8_t channel, uint8_t mailbox_id, const ican_id* id)
{
    uint32_t msg_id = 0;

    msg_id = id->source_id;
    msg_id |= (uint32_t)(id->func_id) << 8;
    msg_id |= (uint32_t)(id->ack) << 12;
    msg_id |= (uint32_t)(id->src_mac_id) << 13;
    msg_id |= (uint32_t)(id->dest_mac_id) << 21;

    msg_id |= CAN_MID_MIDE;
    transMbs[channel][mailbox_id].transfer.dwMsgID = msg_id;    
}


void can_get_id(uint32_t msg_id, ican_id* id)
{
    if (id == NULL) {
        return;
    }

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
bool can_check_inbox(uint8_t channel)
{
    if (rxMsgFiFO[channel].rp != rxMsgFiFO[channel].wp) {
        return true;        
    }

    return false;
}

/**
* \note clear buff after get data
*/
void can_read_mail(uint8_t channel, ican_frame * iframe)
{
    uint8_t len = 0;
    canMsg *pCanMsg = &rxMsgFiFO[channel].rxmsg[rxMsgFiFO[channel].rp];    

    len = pCanMsg->msgLen;
    if (len > 0) {
        can_get_id(pCanMsg->msgID, &iframe->id);
        iframe->dlc = len;
        memcpy(&iframe->frame_data[0], pCanMsg->msgData, len);
        pCanMsg->msgLen = 0;       
    }
    rxMsgFiFO[channel].rp = (rxMsgFiFO[channel].rp + 1) % rxMsgFiFO[channel].fifoNum;
}



//0 is busy, 1 is ilde
bool can_check_outbox(uint8_t channel, uint8_t mailbox_id)
{
    sCand* pCand = &cand[channel];
    sCandTransfer *mbtrf = &transMbs[channel][mailbox_id].transfer;
    
    if (!CAND_IsTransferDone(pCand, mbtrf)) {
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
    sCandTransfer *mbtrf;

    mbtrf = &transMbs[channel][mailbox_id].transfer;
    if (size > 8) {
        len = 8;
    }
    memcpy(mbtrf->msgData, buff, size);
    mbtrf->bMsgLen = len;
}


void can_clear_mailbox(uint8_t channel, uint8_t mailbox_id, bool all)
{
    uint8_t i;
    sCandTransfer *mbtrf;

    mbtrf = &transMbs[channel][mailbox_id].transfer;
    
    if (all) {
        for (i = 0; i < CAN_NUM_MAILBOX; i++) {
            transMbs[channel][i].transfer.bMsgLen = 0;
        }
        return;
    }

    mbtrf->bMsgLen = 0;
    memset(mbtrf->msgData, 0xFF, 8);
}


bool can_transfer_start(uint8_t channel, uint8_t mailbox_id)
{
    uint8_t state = 0, count = 0;
    sCand* pCand = &cand[channel];
    sCandMbCfg* mbcfg = &transMbs[channel][mailbox_id].mbcfg;
    sCandTransfer* mbtrf = &transMbs[channel][mailbox_id].transfer;

    if (mailbox_id >= RECV_MB_START || mbtrf->bMsgLen == 0) {
        return true;
    }

    CAND_ResetMailbox(pCand, mailbox_id, mbcfg);
    do {
        state = CAND_Transfer(&cand[channel], mbtrf);
        if (state != CAND_OK) {
            printf("Can[%d] busy...%x\n\r", channel, state);
            if (count++ < 5) {
                continue;
            }
        }
        break;
    } while (1);
    mbtrf->bMsgLen = 0;
    return _WaitXfrTO(pCand, mbtrf);
}

