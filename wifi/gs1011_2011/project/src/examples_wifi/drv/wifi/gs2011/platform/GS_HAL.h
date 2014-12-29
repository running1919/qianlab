/**
* @file GS_HAL.h
*
* GS(1011 & 2011) header file
* 
* Copyright (C) 2014-2016 Qian Runsheng
*
* @note:if Wifi Chip is GS1011, FIRMWARE_IS_DMA must be set Zero.
*/


#ifndef __GS_HALH
#define __GS_HALH

#ifdef  __cplusplus
extern  "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "GS_mstimer.h"

#define FIRMWARE_IS_DMA        0

#define CS_TIMEOUT             10   //ms
#define BUFSIZE                4096 //byte

#define SPI_ESCAPE_CHARACTER   (0xFB)
#define SPI_ESCAPE_XOR_DATA    (0x20)
#define SPI_XON                (0xFD)
#define SPI_XOFF               (0xFA)
#define SPI_INACTIVE_LINK_LOW  (0x00)
#define SPI_IDLE               (0xF5)
#define SPI_INACTIVE_LINK_HIGH (0xFF)
#define SPI_LINK_READY         (0xF3)
#define SPI_DUMMY              (0xFF)

typedef enum { 
    Bit_RESET = 0,
    Bit_SET
} BitAction;

/**
* Firmware working in dma mode
*/
#if FIRMWARE_IS_DMA

#define GS_DMA_RECV_MAXLEN     2032

typedef enum {
    WRITE_REQUEST = 1,
    READ_REQUEST,
    DATA_FROM_MCU,
    RW_REQUEST
} SPI_DMA_REQUEST;

typedef enum {
    WRITE_RESPONSE_OK = 0x11,
    READ_RESPONSE_OK,
    WRITE_RESPONSE_NOK,
    READ_RESPONSE_NOK,
    DATA_TO_MCU,
    RW_RESPONSE_OK,
    RW_RESPONSE_NOK
} SPI_DMA_RESPONSE;

typedef enum {
    GS_DMA_IDLE = 0,
    GS_DMA_READ_REQUEST,
    GS_DMA_WRITE_REQUEST,
    GS_DMA_READ_RESPONSE,
    GS_DMA_WRITE_RESPONSE,
    GS_DMA_FILTER_HEADER,
    GS_DMA_DATA
} GS_DMA_STATE;

#endif //if FIRMWARE_IS_DMA


/**
* Next funcs which started with "Wifi" need to re-implement
* in different platform or bus interface.
*/
void WifiReset(void);
void WifiCsOn(void);
void WifiCsOff(void);
bool WifiIsCsON(void);
uint8_t WifiGetExtInt(void);
void WifiClearExtInt(void);
void WifiEnableExtInt(void);
void WifiDisableExtInt(void);
void WifiIntInit(void);
void WifiBusInit(void);
void WifiWriteByte(uint8_t data);
void WifiReadByte(uint8_t* buf);


/**
* @brief Initializes hardware
* Intializes the hardware. This method is the same for all platforms
*/
void GS_HAL_init(void);

/**
* @brief Sends data to module
*
* @param txBuf Data to transmit to the module
* @param numToWrite Number of bytes of data to transmit
 */
void GS_HAL_send(const uint8_t* txBuf, uint16_t numToWrite);

/**
* @brief Reads data from module   
*
* @param rxBuf Data buffer to hold incoming data
* @param numToRead Number of bytes to attempt to read
* @param block Set to true if read should wait until numToRead bytes have been read, 
* false to read only available bytes and return immediately
* @return Number of bytes received from module
 */
uint16_t GS_HAL_recv(uint8_t* rxBuf, uint16_t numToRead, bool block);

/**
* @brief Prints string
*/
void GS_HAL_print(char* strBuf);

/**
* @brief Get module's rxBuffer unprocessed data num
*/
uint16_t GS_HAL_unproc_bytes(void);

/**
* @brief reset rxBuffer
*/
void GS_HAL_reset_buf(void);

/**
* @brief process data from module
*/
void GS_HAL_irq_proc(void);

#ifdef  __cplusplus
}
#endif

#endif  /* __GS_HALH */



