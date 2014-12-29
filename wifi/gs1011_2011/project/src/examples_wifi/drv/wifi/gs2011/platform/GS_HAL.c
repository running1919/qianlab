/**
* @file GS_HAL.c
*
* Copyright (C) 2014-2016 Qian Runsheng
*
* Method implementations for the Hardware Abstraction Layer that are platform independent 
*/

#include <string.h>
#include <stdio.h>
#include "GS_HAL.h"


/** 
* Private Variables
*/

static volatile uint8_t rxBuffer[BUFSIZE];
static volatile uint16_t rxHead;
static uint16_t rxTail;
static bool bEscRx;
static bool bWriting;
static bool bReading;
static bool bXOFF;

#if FIRMWARE_IS_DMA
static GS_DMA_STATE GsState = GS_DMA_IDLE;//state machine
static uint8_t rxRepBuf[8];
static uint8_t rxRepHead = 0;
#endif


/**
* Platform Independent Private Funcs
*/

static void gs_hal_init(void)
{
    WifiBusInit();
    WifiIntInit();
    
    //WIFI_POWER_ON();
    WifiReset();    
   
    WifiEnableExtInt();
    WifiCsOff();
}


static void gs_hal_write_byte(uint8_t value, bool escapeValue)
{
    uint32_t csTimeout;
    if (escapeValue) {
        gs_hal_write_byte(SPI_ESCAPE_CHARACTER, false);
        value ^= SPI_ESCAPE_XOR_DATA;
    }

    if (WifiIsCsON()) {
        csTimeout = MSTimerGet();
        /** two consecutive transfers need delay */
        while (WifiIsCsON()) {
            if (MSTimerDelta(csTimeout) > CS_TIMEOUT) {
                WifiCsOff();
            }
        }
    }

    WifiCsOn();
    WifiWriteByte(value);    
    WifiCsOff();
}


/**
* \brief Reads a byte from the SPI peripherial
* This method reads a byte from the SPI register and 
* handles the Gainspan S2W escape sequences
*/
static void gs_hal_read_byte(void)
{
    uint8_t value;

    WifiReadByte(&value);
     
    if (bEscRx){// Check to see if the previous value was escaped, if so save it
        if (((rxHead + 1) % BUFSIZE) != rxTail) {
            rxBuffer[rxHead] = value ^ SPI_ESCAPE_XOR_DATA;
            rxHead = (rxHead + 1) % BUFSIZE;
        }
        bEscRx = false;
    }
#if FIRMWARE_IS_DMA
    else if (GsState != GS_DMA_DATA && GsState != GS_DMA_IDLE) {
        if (rxRepHead >= sizeof(rxRepBuf)) {
            rxRepHead = 0;
        }
        rxRepBuf[rxRepHead++] = value;
    }
#endif
    else {
        switch(value) {
            case SPI_IDLE:
                break;

            case SPI_ESCAPE_CHARACTER:
                bEscRx = true;
                break;
      
            case SPI_XON:
                bXOFF = false;
                break;
      
            case SPI_XOFF:
                bXOFF = true;
                break;
      
            case SPI_LINK_READY:
                break;
      
            case SPI_INACTIVE_LINK_HIGH:
                break;
      
            case SPI_INACTIVE_LINK_LOW:
                break;
      
            default:
                if (((rxHead + 1) % BUFSIZE) != rxTail) {
                    rxBuffer[rxHead] = value;                    
                    rxHead = (rxHead + 1) % BUFSIZE;
                }
                break;
        }
    }
}


/**
* \brief  Activates Gainspan chip select, writes and read a byte
*
* \param value Byte to write out SPI port
* \param escapeValue If true escapes the value before sending.
*/
static void gs_hal_transfer_byte(uint8_t value, bool escapeValue)
{
    gs_hal_write_byte(value, escapeValue);
    gs_hal_read_byte();
}


/**
* \brief Reads bytes from RX buffer
* \param rxBuf Location to store data read from Rx buffer
* \param numToRead Number of bytes to read from Rx buffer
* \return number of bytes actually read from Rx buffer
*/
static uint16_t gs_hal_read_rxbuffer(uint8_t* rxBuf, uint16_t numToRead)
{
    uint16_t bytesRead = 0;
  
    while (rxTail != rxHead && bytesRead < numToRead) {
        *rxBuf++ = rxBuffer[rxTail];
        rxTail = (rxTail + 1) % BUFSIZE;
        bytesRead++;
    }	
  
    return bytesRead;
}


#if FIRMWARE_IS_DMA

static void __flush_rep_buf(void)
{
    rxRepHead = 0;
    memset(rxRepBuf, 0, sizeof(rxRepBuf));
}


static void __gen_header (uint8_t headStr[], const SPI_DMA_REQUEST request, const uint16_t datalen)
{
    headStr[0] = 0xA5;
    headStr[1] = request;
    headStr[2] = 0;
    headStr[3] = 0;
    headStr[4] = 0;
    /* data len */
    headStr[5] = (uint8_t)datalen;
    headStr[6] = (uint8_t)((datalen >> 8) & 0x00FF);
    
    /* compute checksum */
    uint8_t i;
    uint32_t checksum = 0;
    headStr[7] = 0;
    for (i = 1; i < 7; i++) {
        checksum += headStr[i];
    }
    headStr[7] = (uint8_t)(checksum ^ 0xFFFFFFFF);
}


/**
* \brief start one dma requset, and recieve response
* \param requeset Request type
* \param onlyreq It just send header if pass 0
*/
static uint16_t __dma_sync(const uint16_t datalen, const SPI_DMA_REQUEST request, bool onlyreq)
{
    uint8_t headStr[8] = {0,};
    uint8_t i, extIntState;
    uint16_t replen = 0;
    uint32_t Timeout;

    /* 1.Make Header */
    __gen_header(headStr, request, datalen);

    __flush_rep_buf();
    extIntState = 0;

    /* 2.Check cur ext pin's state whether high */
    if (WifiGetExtInt() == Bit_SET) {
        extIntState = 1;
    }
    
    /* 3.Send request */
    for (i = 0; i < 8; i++) {
        gs_hal_transfer_byte(headStr[i], false);
        if (WifiGetExtInt() == Bit_RESET) {
            extIntState = 0;
        }
    }

    /* 4.Wating ext pin be pulled down if it was high before sending requeset */
    Timeout = MSTimerGet();    
    if (extIntState == 1 && (WifiGetExtInt() == Bit_SET)) {
        //MSTimerDelay(1);            
        while (extIntState) {
            if (MSTimerDelta(Timeout) > CS_TIMEOUT) {
                extIntState = 0;
            }

            if (WifiGetExtInt() == Bit_RESET) {
                extIntState = 0;
            }
        }
    }
    
    if (onlyreq) {//Only Send Requset
        return 0;
    }
    
    if (request != DATA_FROM_MCU) {//Recv reponse if request is not DATA_FROM_MCU
        if (request == READ_REQUEST) {
            GsState = GS_DMA_READ_RESPONSE;
        }
        else {
            GsState = GS_DMA_WRITE_RESPONSE;
        }                

    /* 5.Wating ext pin be pulled up */
        Timeout = MSTimerGet();
        while (WifiGetExtInt() != Bit_SET) {
            if (MSTimerDelta(Timeout) > CS_TIMEOUT) {
                printf("\n\rRecving Response Failed\n\r");
                return 0;
            }
        }
        
        __flush_rep_buf();

    /* 6.Read response */
        for (i = 0; i < 8; i++) {
            gs_hal_transfer_byte(SPI_IDLE, false);
        }

    /* 7.Compute response length */
        replen = rxRepBuf[5];
        replen += (uint16_t)rxRepBuf[6] << 8;

        return replen;
    }

     return 0;
}


static void gs_hal_recv(void)
{
    /* 1.Disable ext interrupt,avoid nesting */
    WifiDisableExtInt();
    bReading = true;

    if (WifiGetExtInt() == Bit_SET) {
        uint16_t replen = 0;
        uint16_t headlen = 0;
        uint16_t reqtimes = 0;
        uint32_t Timeout;

    /* 2.Send read request & Recv response */
        do {
            GsState = GS_DMA_READ_REQUEST;
            replen = __dma_sync(GS_DMA_RECV_MAXLEN, READ_REQUEST, false);
        
            if (replen > GS_DMA_RECV_MAXLEN || rxRepBuf[0] != 0xA5
                || rxRepBuf[1] == READ_RESPONSE_NOK) {
                if (WifiGetExtInt() == Bit_SET) {
                    if (reqtimes > 0) {
                        printf("\n\rreplen %d, reponse %x\n\r", replen, rxRepBuf[1]);
                        MSTimerDelay(reqtimes);
                    }
                    
                    if (reqtimes++ > 10) {
                        printf("\n\rSend Read Req Failed\n\r");
                        GsState = GS_DMA_IDLE;
                        bReading = false;
                        WifiEnableExtInt();

                        return;
                    }
                    continue;
                }
                replen = 0;
            }
            break;
        } while (1);

        if (replen == 0) {
            GsState = GS_DMA_IDLE;
            bReading = false;
            WifiEnableExtInt();
            return;
        }

    /* 3.Wait external interrupt pin pull down */
        Timeout = MSTimerGet();
        while (WifiGetExtInt() == Bit_RESET) {
            if (MSTimerDelta(Timeout) > CS_TIMEOUT) {
                printf("\n\r Reading Data Failed\n\r");
                GsState = GS_DMA_IDLE;
                bReading = false;
                WifiEnableExtInt();

                return;
            }
        }

    /* 4.Filter data header */
        GsState = GS_DMA_FILTER_HEADER;
        headlen = 8;
        __flush_rep_buf();
        while (headlen--) {
            gs_hal_transfer_byte(SPI_IDLE, false);
        }

    /* 5.Receive data */
        GsState = GS_DMA_DATA;
        while (replen--) {           
            gs_hal_transfer_byte(SPI_IDLE, false);
        }        
    }

    /* 6.Complete once data receiving */
    GsState = GS_DMA_IDLE;
    bReading = false;
    WifiEnableExtInt();
}


static void gs_hal_send(const uint8_t* txBuf, uint16_t numToWrite)
{   
    if (txBuf == NULL || numToWrite == 0 || numToWrite > 2000) {
        return;
    }

    /* 1.Wating recv over & disable ext interrupt */
    while (bReading || GsState != GS_DMA_IDLE);
    WifiDisableExtInt();    
    bWriting = true;

    uint16_t replen = 0;
    uint16_t reqtimes = 0;
    uint32_t Timeout;
    bool bByteStuffData = false;

    /* 2.Send Write Requeset & Check Response */
    while (1) {
        GsState = GS_DMA_WRITE_REQUEST;
        replen = __dma_sync(numToWrite, WRITE_REQUEST, false);
        if (replen != numToWrite || rxRepBuf[1] == WRITE_RESPONSE_NOK) {
            printf("\n\rreplen %d, numToWrite %d, reponse %x\n\r"
                   , replen, numToWrite, rxRepBuf[1]);

            if (reqtimes++ > 3) {
                printf("\n\rSend Write Req Failed\n\r");
                bWriting = false;
                GsState = GS_DMA_IDLE;
                WifiEnableExtInt();

                return;
            }
            MSTimerDelay(10);
            continue;
        }
        
        break;
    }

    /* 3.Wait external interrupt pin pull down */
    Timeout = MSTimerGet();
    while (WifiGetExtInt() == Bit_RESET) {
        if (MSTimerDelta(Timeout) > CS_TIMEOUT) {
            printf("\n\r Writing Data Failed\n\r");
            GsState = GS_DMA_IDLE;
            bReading = false;
            WifiEnableExtInt();

            return;
        }
    }

    /* 4.Send Data Header */
    GsState = GS_DMA_DATA;
    __dma_sync(numToWrite, DATA_FROM_MCU, true);
    
    /* 5.Send Data */
    while (numToWrite--){//Send Data
        while (bXOFF) {
            printf("\n\rdata over flow......\n\r");
            gs_hal_transfer_byte(SPI_IDLE, false); // Read data until Flow Control ends
        }

        // Decide if we should bytestuff data
        bByteStuffData = (*txBuf == SPI_ESCAPE_CHARACTER) || 
            (*txBuf == SPI_XON) || 
                (*txBuf == SPI_XOFF) || 
                    (*txBuf == SPI_IDLE) ||
                        (*txBuf == SPI_LINK_READY) ||
                            (*txBuf == SPI_INACTIVE_LINK_LOW) ||
                                (*txBuf == SPI_INACTIVE_LINK_HIGH);    

        // gs_hal_transfer_byte(*txBuf, bByteStuffData);
        gs_hal_write_byte(*txBuf, bByteStuffData);
        txBuf++;
    }

    /* 6.Complete once Data sending */
    bWriting = false;
    GsState = GS_DMA_IDLE;
    WifiEnableExtInt();
}

#else

static void gs_hal_recv(void)
{
    /* 1.Disable ext interrupt,avoid nesting */
    WifiDisableExtInt();
    bReading = true;

    uint32_t Timeout;
    Timeout = MSTimerGet();
    /* 2.Receive Data */
    while (WifiGetExtInt() == Bit_SET) {       
        gs_hal_transfer_byte(SPI_IDLE, false);

        if (MSTimerDelta(Timeout) > 1000) {
            printf("\n\rWifi may be halted...\n\r");
            break;
        }
    }

    /* 3.Complete once data receiving */
    bReading = false;
    WifiEnableExtInt();
}


static void gs_hal_send(const uint8_t* txBuf, uint16_t numToWrite)
{
    if (txBuf == NULL || numToWrite == 0 || numToWrite > 2000) {
        return;
    }     
 
    /* 1.Wating recv over & disable ext interrupt */
    while (bReading);
    WifiDisableExtInt();    
    bWriting = true; 

    bool bByteStuffData = false;

    /* 2.Send Data */
    while (numToWrite--){//Send Data
        while (bXOFF) {
            printf("\n\rdata over flow......\n\r");
            gs_hal_transfer_byte(SPI_IDLE, false); // Read data until Flow Control ends
        }
    
        // Decide if we should bytestuff data
        bByteStuffData = (*txBuf == SPI_ESCAPE_CHARACTER) || 
            (*txBuf == SPI_XON) || 
                (*txBuf == SPI_XOFF) || 
                    (*txBuf == SPI_IDLE) ||
                        (*txBuf == SPI_LINK_READY) ||
                            (*txBuf == SPI_INACTIVE_LINK_LOW) ||
                                (*txBuf == SPI_INACTIVE_LINK_HIGH);    
    
        gs_hal_transfer_byte(*txBuf, bByteStuffData);
        //gs_hal_write_byte(*txBuf, bByteStuffData);
        txBuf++;
    }
    
    /* 3.Complete once Data sending */
    bWriting = false;
    WifiEnableExtInt();
}
#endif


/**
* Platform Independent Public Funcs
*/

/**
* \brief print char on terminal
*/
void GS_HAL_print(char* strBuf)
{
    printf("%s\n\r", strBuf);
}


/**
* \brief interrupt process
*/
extern void GS_HAL_irq_proc(void)
{
    /* Clear the EXT */
    WifiClearExtInt();

    if (WifiGetExtInt() == Bit_SET) {
#if FIRMWARE_IS_DMA
        /* Kickoff receive */
        if (!bWriting && GsState == GS_DMA_IDLE) {
            gs_hal_recv();
        }
#else
        if (!bWriting) {
            gs_hal_recv();
        }
#endif
        }
    else {// Spurious Interrupt
        GS_HAL_print("\n\rSpurious PIOA Interrupt\n\r");
    }

}


/**
* \brief clear rx buf
*/
extern void GS_HAL_reset_buf(void)
{
    rxHead = 0;
    rxTail = 0;

    memset((void *)rxBuffer, 0x0, sizeof(rxBuffer));
}


/**
* \brief gs has unprocessed bytes
*/
extern uint16_t GS_HAL_unproc_bytes(void)
{
    return (BUFSIZE + rxHead - rxTail) % BUFSIZE;
}


/**
* \brief gs send data interface
*/
extern void GS_HAL_send(const uint8_t* txBuf, uint16_t numToWrite) 
{    
    gs_hal_send(txBuf, numToWrite);

    uint32_t Timeout;
    Timeout = MSTimerGet();
    // Check to see if we should kick off another read
    while (WifiGetExtInt() == Bit_SET) {
        gs_hal_recv();
        if (MSTimerDelta(Timeout) > 5000) {
            GS_HAL_print("\n\rWifi may be halted, Please restart wifi and control external sending speed\n\r");
            break;
        }
    }
}


/**
* \brief gs recv data interface
*/
extern uint16_t GS_HAL_recv(uint8_t* rxBuf, uint16_t numToRead, bool block)
{
    if (rxBuf == NULL || numToRead == 0) {
        return 0;
    }

    if (block) {
        // Blocking mode, so don't return until we read all the bytes requested
        uint16_t bytesRead;
        // Keep getting data if we have a number of bytes to fetch 
        while (numToRead) {
            bytesRead = gs_hal_read_rxbuffer(rxBuf, numToRead);
            if (bytesRead) {
                rxBuf += bytesRead;
                numToRead -= bytesRead;
            }
        }
        return bytesRead;
    } else {
        // Non-blocking mode, just read what is available in buffer 
        return gs_hal_read_rxbuffer(rxBuf, numToRead);
    }
}


/**
* \brief gs initalize communication interface
*/
extern void GS_HAL_init(void)
{
    rxHead = 0;
    rxTail = 0;

    MSTimerInit();
    gs_hal_init();
}

