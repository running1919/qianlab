/**
 * \file
 *
 * Implementation of SPI driver, transfer data through DMA.
 *
 */

#ifndef _SPI_DMA_
#define _SPI_DMA_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** An unspecified error has occured.*/
#define SPID_ERROR          1

/** SPI driver is currently in use.*/
#define SPID_ERROR_LOCK     2


#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/** SPI transfer complete callback. */
typedef void (*SpidCallback)( uint8_t, void* ) ;

/** \brief Spi Transfer Request prepared by the application upper layer.
 *
 * This structure is sent to the SPI_SendCommand function to start the transfer.
 * At the end of the transfer, the callback is invoked by the interrupt handler.
 */
typedef struct _SpidCmd
{
    /** Pointer to the command data. */
    const uint8_t *pCmd;
    /** Command size in bytes. */
    uint8_t cmdSize;
    /** Pointer to the data to be sent. */
    const uint8_t *pTxData;
    /** Pointer to the data to be recv. */
    uint8_t *pRxData;
    /** Data size in bytes. */
    uint16_t dataSize;
    /** SPI chip select. */
    uint8_t spiCs;
    /** Callback function invoked at the end of transfer. */
    SpidCallback callback;
    /** Callback arguments. */
    void *pArgument;
} SpidCmd ;

/** Constant structure associated with SPI port. This structure prevents
    client applications to have access in the same time. */
typedef struct _Spid
{
    /** Pointer to SPI Hardware registers */
    Spi* pSpiHw ;
    /** Current SpiCommand being processed */
    SpidCmd *pCurrentCommand ;
    /** Pointer to DMA driver */
    sDmad* pDmad;
    /** SPI Id as defined in the product datasheet */
    uint8_t spiId ;
    /** Mutual exclusion semaphore. */
    volatile int8_t semaphore ;
} Spid ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern uint32_t GS_SPID_Configure(Spid* pSpid,
                                Spi* pSpiHw,
                                uint8_t spiId,
                                sDmad* pDmad);

extern uint32_t GS_SPID_SendCommand(SpidCmd* pCommand);

extern void GS_SPID_DmaHandler(void);

extern uint32_t GS_SPID_IsBusy(void) ;

extern void GS_SpiDma_Init(Spi *pSpiHw,
                        uint8_t spiId,
                        uint8_t dmaId);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _SPI_DMA_ */
