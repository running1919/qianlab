/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation
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
 
/**
  * \file
  *
  * Video structure, used for LCD and ISI.
  *
  */

#ifndef _VIDEO_H
#define _VIDEO_H

/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/
/** Type of video is YUV */
#define YUV 0
/** Type of video is RGB */
#define RGB 1

/** Video structure */
typedef struct _videoParam
{
    uint32_t  lcd_vsize;        /**< LCD Vertical Size */
    uint32_t  lcd_hsize;        /**< LCD Horizontal Size */
    uint32_t  lcd_nbpp;         /**< LCD Number of Bit Per Pixel */
    uint32_t  lcd_fb_addr;      /**< LCD Frame Buffer Address */
    uint32_t  Isi_fbd_base;     /**< Base address for the frame buffer descriptors list */
    uint32_t  Hblank;           /**< Start of Line Delay */
    uint32_t  Vblank;           /**< Start of frame Delay */
    uint32_t  codec_vsize;      /**< Vertical size of the Image sensor [0..2047] */
    uint32_t  codec_hsize;      /**< Horizontal size of the Image sensor [0..2047]*/
    uint32_t  codec_fb_addr;    /**< Base address for codec DMA */
    uint32_t  IsiPrevBuffIndex; /**<  Buffer index */
    uint8_t   rgb_or_yuv;       /**<  Type of video  */
    uint8_t   decimation;       /**<  Decimation ON/OFF */
}videoParam;

#endif

