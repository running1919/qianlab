/**
 * \file
 *
 * Implementation for the SPI Flash with DMA driver.
 *
 */

 
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"
#include "GS_spidma_SAM9X25.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** DMA Link List size for spi transation*/
#define DMA_SPI_LLI     2

/*----------------------------------------------------------------------------
 *        Macros
 *----------------------------------------------------------------------------*/

 /*----------------------------------------------------------------------------
 *        Local Variables
 *----------------------------------------------------------------------------*/

/*  DMA driver instance */
static Spid spid;
static sDmad dmad;

static uint32_t spidTxChannel;
static uint32_t spidRxChannel;

/* Linked lists for multi transfer buffer chaining structure instance. */
static sDmaTransferDescriptor gsDamTxLinkList[DMA_SPI_LLI];
static sDmaTransferDescriptor gsDamRxLinkList[DMA_SPI_LLI];

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief SPI DMA Rx callback
 * Invoked on SPi DMA reception done.
 * \param dmaStatus DMA status.
 * \param pArg Pointer to callback argument - Pointer to Spid instance.   
 */ 
static void GS_SPID_Rx_Cb(uint32_t dmaStatus, Spid* pArg)
{
    SpidCmd *pSpidCmd = pArg->pCurrentCommand;
    //Spi *pSpiHw = pArg->pSpiHw;

    if (dmaStatus == DMAD_PARTIAL_DONE)
        return;
    
    /* Release the DMA channels */
    DMAD_FreeChannel(pArg->pDmad, spidRxChannel);
    DMAD_FreeChannel(pArg->pDmad, spidTxChannel);

    /* Release the dataflash semaphore */
    pArg->semaphore++;
        
    /* Invoke the callback associated with the current command */
    if (pSpidCmd && pSpidCmd->callback) {
    
        pSpidCmd->callback(0, pSpidCmd->pArgument);
    }
}

/**
 * \brief Configure the DMA Channels: 0 RX, 1 TX.
 * Channels are disabled after configure.
 * \returns 0 if the dma channel configuration successfully; otherwise returns
 * SPID_ERROR_XXX.
 */
static uint8_t __configureDmaChannels( Spid* pSpid )
{
    uint32_t dwCfg;
    uint8_t iController;

    /* Allocate a DMA channel for SPIx TX. */
    spidTxChannel = DMAD_AllocateChannel( pSpid->pDmad,
                                            DMAD_TRANSFER_MEMORY, pSpid->spiId);
    {
        if ( spidTxChannel == DMAD_ALLOC_FAILED ) 
        {
            return SPID_ERROR;
        }
    }
    /* Allocate a DMA channel for SPIx RX. */
    spidRxChannel = DMAD_AllocateChannel( pSpid->pDmad,
                                            pSpid->spiId, DMAD_TRANSFER_MEMORY);
    {
        if ( spidRxChannel == DMAD_ALLOC_FAILED ) 
        {
            return SPID_ERROR;
        }
    }
    iController = (spidRxChannel >> 8);
    /* Setup callbacks for SPIx RX */
    DMAD_SetCallback(pSpid->pDmad, spidRxChannel,
                     (DmadTransferCallback)GS_SPID_Rx_Cb, pSpid);
    //DMAD_SetCallback(pSpid->pDmad, spidRxChannel, NULL, NULL);

    /* Configure the allocated DMA channel for SPIx RX. */
    dwCfg = 0
           | DMAC_CFG_SRC_PER(
              DMAIF_Get_ChannelNumber( iController, pSpid->spiId, DMAD_TRANSFER_RX ))
           | DMAC_CFG_DST_PER(
              DMAIF_Get_ChannelNumber( iController, pSpid->spiId, DMAD_TRANSFER_RX ))
           | DMAC_CFG_SRC_H2SEL  // hardware handshaking
           | DMAC_CFG_SOD
           | DMAC_CFG_FIFOCFG_ALAP_CFG;

    if (DMAD_PrepareChannel( pSpid->pDmad, spidRxChannel, dwCfg ))
        return SPID_ERROR;

    iController = (spidTxChannel >> 8);
    /* Setup callbacks for SPIx TX (ignored) */
    DMAD_SetCallback(pSpid->pDmad, spidTxChannel, NULL, NULL);

    /* Configure the allocated DMA channel for SPIx TX. */
    dwCfg = 0
           | DMAC_CFG_SRC_PER(
              DMAIF_Get_ChannelNumber( iController, pSpid->spiId, DMAD_TRANSFER_TX ))
           | DMAC_CFG_DST_PER(
              DMAIF_Get_ChannelNumber( iController, pSpid->spiId, DMAD_TRANSFER_TX ))
           | DMAC_CFG_DST_H2SEL  // hardware handshaking
           | DMAC_CFG_SOD
           | DMAC_CFG_FIFOCFG_ALAP_CFG;

    if ( DMAD_PrepareChannel( pSpid->pDmad, spidTxChannel, dwCfg ))
        return SPID_ERROR;
    return 0;
}

/**
 * \brief Configure the DMA source and destination with Linker List mode.
 *
 * \param pCommand Pointer to command
  * \returns 0 if the dma multibuffer configuration successfully; otherwise returns
 * SPID_ERROR_XXX.
 */
static uint8_t __configureLinkList(Spi *pSpiHw, sDmad *pDmad, SpidCmd *pCommand)
{
    gsDamRxLinkList[0].dwSrcAddr = (uint32_t)&pSpiHw->SPI_RDR;
    gsDamRxLinkList[0].dwDstAddr = (uint32_t)pCommand->pRxData;
    gsDamRxLinkList[0].dwCtrlA   = pCommand->dataSize | DMAC_CTRLA_SRC_WIDTH_BYTE | DMAC_CTRLA_DST_WIDTH_BYTE;
    gsDamRxLinkList[0].dwCtrlB   = 0 | DMAC_CTRLB_SRC_DSCR | DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_PER2MEM_DMA_FC
                              | DMAC_CTRLB_SRC_INCR_FIXED | DMAC_CTRLB_DST_INCR_INCREMENTING;
    
    gsDamRxLinkList[0].dwCtrlB &=  ~DMAC_CTRLB_AUTO;
    gsDamRxLinkList[0].dwDscAddr = 0;

    gsDamTxLinkList[0].dwSrcAddr = (uint32_t)pCommand->pTxData;
    gsDamTxLinkList[0].dwDstAddr = (uint32_t)&pSpiHw->SPI_TDR;
    gsDamTxLinkList[0].dwCtrlA   = pCommand->dataSize | DMAC_CTRLA_SRC_WIDTH_BYTE | DMAC_CTRLA_DST_WIDTH_BYTE;
    gsDamTxLinkList[0].dwCtrlB   = 0 | DMAC_CTRLB_SRC_DSCR | DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_MEM2PER_DMA_FC
                             | DMAC_CTRLB_SRC_INCR_INCREMENTING | DMAC_CTRLB_DST_INCR_FIXED;
    //gsDamTxLinkList[0].dwCtrlB   = 0 | DMAC_CTRLB_SIF_AHB_IF1 | DMAC_CTRLB_DIF_AHB_IF0
    //                          | DMAC_CTRLB_SRC_DSCR | DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_MEM2PER_DMA_FC
    //                          | DMAC_CTRLB_SRC_INCR_INCREMENTING | DMAC_CTRLB_DST_INCR_FIXED;
    gsDamTxLinkList[0].dwCtrlB &=  ~DMAC_CTRLB_AUTO;
    gsDamTxLinkList[0].dwDscAddr = 0;

    if (DMAD_PrepareSingleTransfer( pDmad, spidRxChannel, &gsDamRxLinkList[0]))
        return SPID_ERROR;
    if (DMAD_PrepareSingleTransfer( pDmad, spidTxChannel, &gsDamTxLinkList[0]))
        return SPID_ERROR;
    return 0;   
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 * \brief Initializes the Spid structure and the corresponding SPI & DMA hardware.
 * select value.
 * The driver will uses DMA channel 0 for RX and DMA channel 1 for TX.
 * The DMA channels are freed automatically when no SPI command processing.
 *
 * \param pSpid  Pointer to a Spid instance.
 * \param pSpiHw Associated SPI peripheral.
 * \param spiId  SPI peripheral identifier.
 * \param pDmad  Pointer to a Dmad instance. 
 */
uint32_t GS_SPID_Configure( Spid *pSpid ,
                         Spi *pSpiHw , 
                         uint8_t spiId,
                         sDmad *pDmad )
{
    /* Initialize the SPI structure */
    pSpid->pSpiHw = pSpiHw;
    pSpid->spiId  = spiId;
    pSpid->semaphore = 1;
    pSpid->pCurrentCommand = 0;
    pSpid->pDmad = pDmad;

    return 0;
}


/**
 * \brief Starts a SPI master transfer. This is a non blocking function. It will
 *  return as soon as the transfer is started.
 *
 * \param pSpid  Pointer to a Spid instance.
 * \param pCommand Pointer to the SPI command to execute.
 * \returns 0 if the transfer has been started successfully; otherwise returns
 * SPID_ERROR_LOCK is the driver is in use, or SPID_ERROR if the command is not
 * valid.
 */
uint32_t GS_SPID_SendCommand(SpidCmd *pCommand)
{
    Spid *pSpid = &spid;
    Spi *pSpiHw = pSpid->pSpiHw;
         
    /* Try to get semaphore */
     if (pSpid->semaphore == 0) {
    
         return SPID_ERROR_LOCK;
     }
     pSpid->semaphore--;


     /* Initialize DMA controller using channel 0 for RX, 1 for TX. */
     if (__configureDmaChannels(pSpid))
         return SPID_ERROR_LOCK;
     if (__configureLinkList(pSpiHw, pSpid->pDmad, pCommand))
         return SPID_ERROR_LOCK;

     // Initialize the callback
     pSpid->pCurrentCommand = pCommand;

     /* Start DMAx 2(RX) && 1(TX) */
     //if (DMAD_StartTransfer( pSpid->pDmad, spiDmaRxChannel )) 
     //    return SPID_ERROR_LOCK;
     //if (DMAD_StartTransfer( pSpid->pDmad, spiDmaTxChannel )) 
     //    return SPID_ERROR_LOCK;

     if (DMAD_StartTransfer(pSpid->pDmad, spidRxChannel)) 
         return SPID_ERROR_LOCK;
     if (DMAD_StartTransfer(pSpid->pDmad, spidTxChannel)) 
         return SPID_ERROR_LOCK;

     return 0;    
}


/**
 * \brief DMA transfer ISR for SPI driver.
 */
void GS_SPID_DmaHandler()
{
    static uint8_t times = 0;
    if (++times == 2) {
        times = 0;
    }
    DMAD_Handler(&dmad);
}

/**
 * \brief Check if the SPI driver is busy.
 *
 * \param pSpid  Pointer to a Spid instance.
 * \returns 1 if the SPI driver is currently busy executing a command; otherwise
 */
uint32_t GS_SPID_IsBusy(void)
{
    const Spid *pSpid = &spid;

    if (pSpid->semaphore == 0) {

        return 1;
    }
    else {

        return 0;
    }
}

void GS_SpiDma_Init(Spi *pSpiHw, uint8_t spiId, uint8_t dmaId)
{
    /* Initialize DMA driver instance with interrupt mode */
    DMAD_Initialize(&dmad, 0);

    /* Enable interrupts */
    IRQ_ConfigureIT(dmaId, 0, GS_SPID_DmaHandler);
    IRQ_EnableIT(dmaId);

    GS_SPID_Configure(&spid, pSpiHw, spiId, &dmad);
}

