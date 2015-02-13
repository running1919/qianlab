/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/** \file
 *
 * Implementation of Image Sensor Interface (ISI).
 *
 */
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include "chip.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 * \brief Enable ISI
 */
void ISI_Enable(void)
{
    /* Write one to this field to enable the module. */ 
    ISI->ISI_CR |= ISI_CR_ISI_EN;
    /* Software must poll ENABLE field in the ISI_STATUS register to verify that the command 
    has successfully completed.*/
    while( (ISI->ISI_SR & ISI_SR_ENABLE) != ISI_SR_ENABLE);
    /* Write one to this field to enable the preview DMA channel. */
    ISI->ISI_DMA_CHER |= ISI_DMA_CHER_P_CH_EN;
}

/**
 * \brief Disable ISI
 */
void ISI_Disable(void)
{
    /* Write one to this field to disable the module */
    ISI->ISI_CR |= ISI_CR_ISI_DIS;
    /* Software must poll DIS_DONE field in the ISI_STATUS register to verify that the command 
    has successfully completed.*/
    while( (ISI->ISI_SR & ISI_SR_DIS_DONE) != ISI_SR_DIS_DONE);
    /* Write one to this field to disable the DMA channel */
    ISI->ISI_DMA_CHDR |= ISI_DMA_CHDR_P_CH_DIS;
}
/**
 * \brief Enable ISI interrupt
 * \param  flag of interrupt to enable
 */
void ISI_EnableInterrupt(uint32_t flag)
{
    ISI->ISI_IER = flag;
}

/**
 * \brief Disable ISI interrupt
 * \param  flag of interrupt to disable
 */
void ISI_DisableInterrupt(uint32_t flag)
{
    ISI->ISI_IDR = flag;
}

/**
 * \brief Return ISI status register
 * \return Status of ISI register
 */
uint32_t ISI_StatusRegister(void)
{
    return(ISI->ISI_SR);
}

/**
 * \brief Enable Codec path for capture next frame
 */
void ISI_CodecPathFull(void)
{
    /* The codec path is enabled and the next frame is captured.
    Both codec and preview datapaths are working simultaneously */
    ISI->ISI_CR |= ISI_CR_ISI_CDC;
    /* A new request cannot be taken into account while CDC_PND bit is active in the ISI_STATUS register.*/
    while( (ISI->ISI_SR & ISI_SR_CDC_PND) != ISI_SR_CDC_PND);
    /* Both preview and codec DMA channels are operating simultaneously */
    ISI->ISI_CFG1 |= ISI_CFG1_FULL;
}

/**
 * \brief Set frame rate
 * \param  frate frame rate capture
 */
void ISI_SetFrame(uint32_t frate)
{
    if( frate > 7 ) {
        TRACE_ERROR("FRate too big\n\r");
        frate = 7;
    }
    ISI->ISI_CFG1 |= ISI_CFG1_FRATE(frate);
}

/**
 * \brief Get the number of byte per pixels
 * \param  bmpRgb BMP type can be YUV or RGB
 * \return Number of byte for one pixel
 */
uint8_t ISI_BytesForOnePixel(uint8_t bmpRgb)
{
    uint8_t nbByte_Pixel;

    if (bmpRgb == RGB) {
        if ((ISI->ISI_CFG2 & ISI_CFG2_RGB_MODE) == ISI_CFG2_RGB_MODE){
            // RGB: 5:6:5 16bits/pixels
            nbByte_Pixel = 2;
        } 
        else {
            // RGB: 8:8:8 24bits/pixels
            nbByte_Pixel = 3;
        }
    } 
    else {
        // YUV: 2 pixels for 4 bytes
        nbByte_Pixel = 2;
    }
    return nbByte_Pixel;
}

/**
 * \brief Reset ISI
 */
void ISI_Reset(void)
{
    volatile uint32_t timeout = 0;
    if (!(ISI->ISI_SR & ISI_SR_ENABLE))
        return;
    /* Resets the image sensor interface. Finish capturing the current frame and then shut down the module.*/
    ISI->ISI_CR = ISI_CR_ISI_SRST | ISI_CR_ISI_DIS;
    /* wait Software reset has completed successfully. */
    while( !(ISI->ISI_SR & ISI_SR_SRST) && (timeout < 0x500000) ){
        timeout++;
    }
    if( timeout == 0x500000 ) {
        TRACE_ERROR("ISI-Reset timeout\n\r");
    }
}

/**
 * \brief  ISI initialize
 * \param pVideo structure of video driver
 */
void ISI_Init(videoParam * pVideoParam)
{
    ISI_Reset();

    // AT91C_ISI_HSYNC_POL    Horizontal synchronisation polarity
    // AT91C_ISI_VSYNC_POL    Vertical synchronisation polarity
    // AT91C_ISI_PIXCLK_POL   Pixel Clock Polarity

    /* SLD pixel clock periods to wait before the beginning of a line.
       SFD lines are skipped at the beginning of the frame.*/
    ISI->ISI_CFG1 |=  ISI_CFG1_SLD(pVideoParam->Hblank) + ISI_CFG1_SFD(pVideoParam->Vblank);
    TRACE_DEBUG("ISI_CFG1=0x%X\n\r", ISI->ISI_CFG1);

    /* IM_VSIZE: Vertical size of the Image sensor [0..2047]
       Vertical size = IM_VSIZE + 1
       IM_HSIZE: Horizontal size of the Image sensor [0..2047]
       Horizontal size = IM_HSIZE + 1
       YCC_SWAP : YCC image data */
    ISI->ISI_CFG2 = ISI_CFG2_IM_VSIZE(pVideoParam->codec_vsize - 1)
                  + ISI_CFG2_IM_HSIZE(pVideoParam->codec_hsize - 1)
                  + ISI_CFG2_YCC_SWAP(2);

    if (pVideoParam->rgb_or_yuv == RGB) {
        ISI->ISI_CFG2 |= ISI_CFG2_COL_SPACE | ISI_CFG2_RGB_MODE | ISI_CFG2_RGB_CFG(0);
    }
    else {
        ISI->ISI_CFG2 &=  ~ISI_CFG2_COL_SPACE;
    }
    TRACE_DEBUG("ISI_CFG2=0x%X\n\r", ISI->ISI_CFG2);

    /* Vertical Preview size = PREV_VSIZE + 1 (480 max only in RGB mode).
       Horizontal Preview size = PREV_HSIZE + 1 (640 max only in RGB mode).*/

    ISI->ISI_PSIZE = ISI_PSIZE_PREV_VSIZE(pVideoParam->lcd_vsize -1)
                    +ISI_PSIZE_PREV_HSIZE(pVideoParam->lcd_hsize -1);

    // DEC_FACTOR is 8-bit width, range is from 16 to 255. 
    // Values from 0 to 16 do not perform any decimation.
    if (pVideoParam->decimation)
    {
        uint32_t fh = (16 * pVideoParam->codec_hsize) / pVideoParam->lcd_hsize;
        uint32_t fv = (16 * pVideoParam->codec_vsize) / pVideoParam->lcd_vsize;
        uint32_t f = (fh > fv) ? fv : fh;
        if (f > 127 || f < 16) f = 16;
        ISI->ISI_PDECF = ISI_PDECF_DEC_FACTOR(f);
    }
    else
        ISI->ISI_PDECF = 16; // 

    TRACE_DEBUG("codec_hsize: %d\n\r", pVideoParam->codec_hsize);
    TRACE_DEBUG("lcd_hsize: %d\n\r", pVideoParam->lcd_hsize);
    TRACE_DEBUG("ISI_PDECF: %d\n\r", ISI->ISI_PDECF);
    if( ISI->ISI_PDECF <16) {
        TRACE_ERROR("ISI_PDECF, forbidden value: %d\n\r", ISI->ISI_PDECF);
        ISI->ISI_PDECF = 16;
    }

    ISI->ISI_DMA_P_DSCR = pVideoParam->Isi_fbd_base;
    ISI->ISI_DMA_P_CTRL = ISI_DMA_P_CTRL_P_FETCH;
    ISI->ISI_DMA_P_ADDR = pVideoParam->lcd_fb_addr;

    /* C0: Color Space Conversion Matrix Coefficient C0 */
    /* C1: Color Space Conversion Matrix Coefficient C1 */
    /* C2: Color Space Conversion Matrix Coefficient C2 */
    /* C3: Color Space Conversion Matrix Coefficient C3 */
    ISI->ISI_Y2R_SET0  = ISI_Y2R_SET0_C0(0x95)
                       + ISI_Y2R_SET0_C1(0xFF)
                       + ISI_Y2R_SET0_C2(0x68)
                       + ISI_Y2R_SET0_C3(0x32);

    /* C4: Color Space Conversion Matrix coefficient C4 */
    /* Yoff: Color Space Conversion Luminance 128 offset */
    /* Croff: Color Space Conversion Red Chrominance 16 offset */
    /* Cboff: Color Space Conversion Blue Chrominance 16 offset */
    ISI->ISI_Y2R_SET1  = ISI_Y2R_SET1_C4(0xCC) 
                       + ISI_Y2R_SET1_Yoff 
                       + ISI_Y2R_SET1_Croff 
                       + ISI_Y2R_SET1_Cboff;
}
