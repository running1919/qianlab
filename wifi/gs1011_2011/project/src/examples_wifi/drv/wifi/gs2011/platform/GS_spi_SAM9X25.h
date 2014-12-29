/**
* @file GS_spi_SAM9X25.h
*
* GS(1011 & 2011) over SPI on the SAM9X25 platform header file
* 
* Copyright (C) 2014-2016 Qian Runsheng
*/


#ifndef __GS_SPI_SAM9X25H
#define __GS_SPI_SAM9X25H

#ifdef  __cplusplus
extern  "C" {
#endif


#include <board.h>
#include "GS_spidma_SAM9X25.h"

/**
* SPI mode Define
*/
#if FIRMWARE_IS_DMA
#define WIFI_SPI_CLK        10000000   //10MHZ
#define WIFI_SPI_DLYBCT     (WIFI_SPI_CLK / 100000)//100ns
#define WIFI_SPI_DLYBS      (WIFI_SPI_CLK / 100000)//100ns
#else
#define WIFI_SPI_CLK        1000000    //1MHZ
#define WIFI_SPI_DLYBCT     (WIFI_SPI_CLK / 1000)//1us
#define WIFI_SPI_DLYBS      (WIFI_SPI_CLK / 1000)//1us
#endif
#define WIFI_SPI_BASE       SPI0      //SPI Master Base
#define WIFI_SPI_ID         ID_SPI0   //SPI Id
#define WIFI_SPI_PCS        0         //CS Pin
#define WIFI_CPHA           0         //Clock Phas
#define WIFI_CPOL           0         //Clock Polarity
#define WIFI_SPI_BITS       8         //Transfer bits
#define WIFI_INT_PRIORITY   4         //Interrupt Priority

#define WIFI_AUTOCS         0         //Auto select chip
#define WIFI_DMA_ID         ID_DMAC1

/**
* Pins Define
*/

#define WIFI_SPI_PINS       PINS_SPI0

#if WIFI_AUTOCS
#define WIFI_CS_PIN         PIN_SPI0_NPCS0
#else
#define WIFI_CS_PIN         {PIO_PA18, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#endif

#ifdef WIFI_POLLING
#define WIFI_INT_PIN        {PIO_PA19, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#else
#define WIFI_INT_PIN        {PIO_PA19, PIOA, ID_PIOA, PIO_INPUT, PIO_DEBOUNCE | PIO_IT_RISE_EDGE}
#endif

#define WIFI_POWER_PIN      {PIO_PA16, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define WIFI_RST_PIN        {PIO_PA17, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}

/* ALL Using Pins */
#define WIFI_PINS           {WIFI_SPI_PINS, WIFI_CS_PIN, WIFI_POWER_PIN, WIFI_RST_PIN, WIFI_INT_PIN}


#ifdef  __cplusplus
}
#endif

#endif  /* __GS_SPI_SAM9X25H */


