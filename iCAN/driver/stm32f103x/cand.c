/**
* @file cand.c
*
* stm32f103x can high interface based on stm32f10x_can.c
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "io.h"
#include "usart.h"
#include "cand.h"

#define CANx                       CAN1
#define GPIO_CAN                   GPIOA
#define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOA
#define GPIO_Remapping_CAN         GPIO_Remap1_CAN1
#define GPIO_Pin_CAN_RX            GPIO_Pin_11
#define GPIO_Pin_CAN_TX            GPIO_Pin_12

#define CAN_ERR_INT                ( CAN_IT_ERR \
                                   | CAN_IT_LEC \
                                   | CAN_IT_BOF \
                                   | CAN_IT_EPV \
                                   | CAN_IT_EWG )


static bool InitSucess = false;
static CAN_InitTypeDef* InitStruct = NULL;
static CAN_FilterInitTypeDef* FilterInitStruct = NULL;
static sRxFIFO* RxFIFOBuf = NULL;
static uint8_t CanExceptVal = 0;


#if 0
/**
 * \brief Calculate and configure the baudrate
 * \param pCan       Pointer to Can instance.
 * \param dwBaudrate Baudrate value (kB/s)
 *                   allowed: 100, 800, 500, 250, 125, 50, 25, 10
 * \param dwMck      MCK.
 * \return 1 in success, otherwise return 0.
 */
uint8_t CAN_CalcBaudrate(CAN_InitTypeDef *pCan_Init, uint32_t dwBaudrate, uint32_t dwMck)
{
    uint32_t BRP, PROPAG, PHASE1, PHASE2, SJW;
    uint8_t  TQ;
    uint32_t t1t2;
    if (dwBaudrate >= 1000) TQ = 8;
    else                    TQ = 16;
    BRP = (dwMck / (dwBaudrate * 1000 * TQ)) - 1;
    if (BRP == 0) {
        return 0;
    }

    /* Timing delay:
       Delay Bus Driver     - 50ns
       Delay Receiver       - 30ns
       Delay Bus Line (20m) - 110ns */
    if ( (TQ * dwBaudrate * 2 * (50+30+110)/1000000) >= 1 )
        PROPAG = (TQ * dwBaudrate * 2 * (50+30+110)/1000000) - 1;
    else
        PROPAG = 0;
    t1t2 = TQ - 1 - (PROPAG + 1);

    if ( (t1t2 & 0x01) == 0x01 ) {
        PHASE1 = ((t1t2 - 1) / 2) - 1;
        PHASE2 = PHASE1 + 1;
    }
    else {
        PHASE1 = ((t1t2) / 2) - 1;
        PHASE2 = PHASE1;
    }

    if ( 1 > (4/(PHASE1 + 1)) ) SJW = 3;
    else                        SJW = PHASE1;

    if ( (PROPAG + PHASE1 + PHASE2) != (TQ - 4) ) {
        return 0;
    }

    pCan_Init->CAN_SJW = SJW;
    pCan_Init->CAN_BS1 = PHASE1;
    pCan_Init->CAN_BS2 = PHASE2;
    pCan_Init->CAN_Prescaler = BRP;
    return 1;
}
#endif

void CAND_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void CAND_PIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* GPIO clock enable */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN, ENABLE);
  ioInit();

  /* Configure CAN pin: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_TX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
  
  //GPIO_PinRemapConfig(GPIO_Remapping_CAN , ENABLE);
  
  /* CANx Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
}


void CAND_Init(CAN_InitTypeDef* InitS, CAN_FilterInitTypeDef* FilterInit, sRxFIFO* Rxbuf)
{
    uint8_t i;
    /* CAN register init */
    CAN_DeInit(CANx);
    //CAN_StructInit(InitS);

    InitStruct = InitS;
    FilterInitStruct = FilterInit;
    RxFIFOBuf = Rxbuf;

    RxFIFOBuf->alldlc = 0;
    RxFIFOBuf->usednum = 0;
    for (i = 0; i < RxFIFOBuf->fifonum; i++) {
        RxFIFOBuf->rxMsg[i].DLC = 0;
    }
}


void CAND_Activate(void)
{
    /* Init Can & Waiting Initialization Acknowledge*/
    InitSucess = CAN_Init(CANx, InitStruct);

    if (InitSucess) {
        CanExceptVal = 0;//reset except flag

        CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
        CAN_ITConfig(CANx, CAN_ERR_INT, ENABLE);

        FilterInitStruct->CAN_FilterNumber = 0;
        FilterInitStruct->CAN_FilterFIFOAssignment = 0;
        CAN_FilterInit(FilterInitStruct);

        //FilterInitStruct->CAN_FilterNumber = 1;
        //FilterInitStruct->CAN_FilterFIFOAssignment = 1;
        //CAN_FilterInit(FilterInitStruct);
   }
}

bool CAND_Wakeup(void)
{
    return (bool)CAN_WakeUp(CANx);
}

bool CAND_Sleep(void)
{
    return (bool)CAN_Sleep(CANx);
}

void CAND_Off(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, DISABLE);
}


bool CAND_IsReady(void)
{
    return InitSucess;
}


uint8_t CAND_Transfer(CanTxMsg* TxMessage)
{
    uint8_t mailboxid = 0;
    mailboxid = CAN_Transmit(CANx, TxMessage);

    if (mailboxid > 2) {
        return 0xff;
    }
    else {
        return mailboxid;
    }
}


bool CAND_IsTransferDone(uint8_t TransmitMailboxId)
{
    uint8_t transstatus = 0;
    transstatus = CAN_TransmitStatus(CANx, TransmitMailboxId);

    if (transstatus == 1) {
        return true;
    }

    return false;
}


void CAND_RequestComplete(uint8_t TransmitMailboxId)
{
    uint32_t rqcp = 0;

    switch (TransmitMailboxId) {
    case 0 :
        rqcp = CAN_FLAG_RQCP0;
        break;
        
    case 1 :
        rqcp = CAN_FLAG_RQCP1;
        break;
        
    case 2 :
        rqcp = CAN_FLAG_RQCP2;
        break;

    default :
        return;
    }
    
    CAN_ClearFlag(CANx, rqcp);
}


uint8_t CAND_IsExcept(void)
{
    return CanExceptVal;
}


void CANx_RxHandler(void)
{
    //FlagStatus fmp = RESET;

    while(CAN_GetFlagStatus(CANx, CAN_FLAG_FMP0) == SET){
        if (RxFIFOBuf->usednum < RxFIFOBuf->fifonum) {
            RxFIFOBuf->usednum++;
        }

        CAN_Receive(CAN1, CAN_FIFO0, &RxFIFOBuf->rxMsg[RxFIFOBuf->usednum - 1]);
        RxFIFOBuf->alldlc += RxFIFOBuf->rxMsg[RxFIFOBuf->usednum - 1].DLC;
    }

    while(CAN_GetFlagStatus(CANx, CAN_FLAG_FMP1) == SET){
        if (RxFIFOBuf->usednum < RxFIFOBuf->fifonum) {
            RxFIFOBuf->usednum++;
        }

        CAN_Receive(CAN1, CAN_FIFO1, &RxFIFOBuf->rxMsg[RxFIFOBuf->usednum - 1]);
        RxFIFOBuf->alldlc += RxFIFOBuf->rxMsg[RxFIFOBuf->usednum - 1].DLC;
    }
}


void CANx_SceHandler(void)
{
    uint8_t lec; //last error code
    uint8_t rec; //receive error counter 
    uint8_t tec; //transmit error counter

    /* 1.check sleep */

    /* 2.check wakeup */

    /* 3.check error */
    if (CAN_GetITStatus(CANx, CAN_IT_ERR)) {
        if (CAN_GetITStatus(CANx, CAN_IT_EWG)) {
            rec = CAN_GetReceiveErrorCounter(CANx);
            tec = CAN_GetLSBTransmitErrorCounter(CANx);
            printStr("-error warning, REC: %d, TEC: %d\n\r", rec, tec);

            CanExceptVal = 1;
            CAN_ClearITPendingBit(CANx, CAN_IT_EWG);
        }

        if (CAN_GetITStatus(CANx, CAN_IT_EPV)) {
            printStr("-error passive\n\r");

            CanExceptVal = 2;
            CAN_ClearITPendingBit(CANx, CAN_IT_EPV);
        }
            
        if (CAN_GetITStatus(CANx, CAN_IT_BOF)) {
            printStr("-error bus off\n\r");

            CanExceptVal = 3;
            CAN_ClearITPendingBit(CANx, CAN_IT_BOF);
        }
            
        if (CAN_GetITStatus(CANx, CAN_IT_LEC)) {
            lec = CAN_GetLastErrorCode(CANx);
            lec >>= 4;
            switch (lec) {
            case 0x00 :
                break;

            case 0x01 :
                printStr("--Stuff Error\n\r");
                break;

            case 0x02 :
                printStr("--Form Error\n\r");
                break;

            case 0x03 :
                printStr("--Ack Error\n\r");
                break;

            case 0x04 :
                printStr("--Bit recessive Error\n\r");
                break;

            case 0x05 :
                printStr("--Bit dominant Error\n\r");
                break;

            case 0x06 :
                printStr("--CRC Error \n\r");
                break;

            case 0x07 :
                printStr("--Set by software \n\r");
                break;

            default :
                printStr("--Unknow Error");
                break;
            }

            CanExceptVal = 4;
            CAN_ClearITPendingBit(CANx, CAN_IT_LEC);
        }

        CAN_ClearITPendingBit(CANx, CAN_IT_ERR);
    }
}

