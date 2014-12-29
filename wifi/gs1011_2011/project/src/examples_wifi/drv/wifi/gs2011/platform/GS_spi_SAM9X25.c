/**
* @file GS_spi_SAM9X25.c
*
* Method implmentation for sending data to module over SPI on the SAM9X25 platform
*
* Copyright (C) 2014-2016 Qian Runsheng
*/

#include <stdbool.h>
#include <string.h>
#include "../platform/GS_HAL.h"
#include "GS_spi_SAM9X25.h"


static const Pin pWifiIntPin = WIFI_INT_PIN; //'pWifiIntPin' must be static type

/*********************************
void WifiPowerOn(void)
{
    const Pin pin = WIFI_POWER_PIN;
    PIO_Set(&pin);
}

void WifiPowerOff(void)
{
    const Pin pin = WIFI_POWER_PIN;
    PIO_Clear(&pin);
}
**********************************/

void WifiReset(void)
{
    const Pin pin = WIFI_RST_PIN;

    PIO_Clear(&pin);
    MSTimerDelay(100);
    PIO_Set(&pin);
    MSTimerDelay(300);
}


void WifiCsOn(void)
{
#if WIFI_AUTOCS
    SPI_ChipSelect(WIFI_SPI_BASE, 1 << WIFI_SPI_PCS);
#else
    const Pin pin = WIFI_CS_PIN;
    PIO_Clear(&pin);
#endif
}


void WifiCsOff(void)
{
#if WIFI_AUTOCS
    SPI_ReleaseCS(WIFI_SPI_BASE);
    SPI_ChipSelect(WIFI_SPI_BASE, 0);
#else
    const Pin pin = WIFI_CS_PIN;
    while (!SPI_IsFinished(WIFI_SPI_BASE));
    PIO_Set(&pin);
#endif
}


bool WifiIsCsON(void)
{
    const Pin pin = WIFI_CS_PIN;
    return !PIO_Get(&pin);
}


uint8_t WifiGetExtInt(void)
{
    const Pin pin = WIFI_INT_PIN;
    return PIO_Get(&pin);
}


void WifiClearExtInt(void)
{
    const Pin pin = WIFI_INT_PIN;
    AIC->AIC_ICCR = 1 << pin.id;
}


void WifiDisableExtInt(void)
{
    PIO_DisableIt(&pWifiIntPin);
}


void WifiEnableExtInt(void)
{
    PIO_EnableIt(&pWifiIntPin);
}


void WifiWriteByte(uint8_t data)
{
    SPI_Write(WIFI_SPI_BASE, WIFI_SPI_PCS, data);
    /** Wait until SPI RX buffer is full */
    while (!(SPI_GetStatus(WIFI_SPI_BASE) & 0x1));
}

void WifiReadByte(uint8_t* buf)
{
    *buf = SPI_Read(WIFI_SPI_BASE);
}

void WifiIntHandler(const Pin* pin)
{
    GS_HAL_irq_proc();
}

void WifiIntInit()
{
    PIO_InitializeInterrupts(WIFI_INT_PRIORITY | AIC_SMR_SRCTYPE_EXT_POSITIVE_EDGE);
    PIO_ConfigureIt(&pWifiIntPin, WifiIntHandler);    
}


/**
* \brief Initialize Communication Interface
*/
void WifiBusInit()
{
    uint32_t dwConfig = 0;
    uint32_t dwNpcs = WIFI_SPI_PCS;
    Spi *spi = WIFI_SPI_BASE;
    const Pin WifiPins[] = WIFI_PINS;

    /* Inital I/O ports */
    PIO_Configure(WifiPins, PIO_LISTSIZE(WifiPins));

    /* Config SPI as Master mode, Fixed Peripheral Select */
    dwConfig = 0;
    dwConfig = SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_PCS(dwNpcs);
    SPI_Configure(spi, WIFI_SPI_ID, dwConfig);

    /* Config time series */
    dwConfig = 0;
    dwConfig |= (((WIFI_CPHA & 0x1) ^ 0x1) << 1) | WIFI_CPOL; //SPI mode
    dwConfig |= (WIFI_SPI_BITS - 8) << 4; //Bits per transfer
    dwConfig |= SPI_DLYBCT(WIFI_SPI_DLYBCT, BOARD_MCK); // 1 spck
    dwConfig |= SPI_DLYBS(WIFI_SPI_DLYBS, BOARD_MCK); // 1 spck
    dwConfig |= SPI_SCBR(WIFI_SPI_CLK, BOARD_MCK);
    SPI_ConfigureNPCS(spi, dwNpcs, dwConfig);

    /* Chip Select Active After Transfer */
    //SPI_ConfigureCSMode(spi, dwNpcs, 1);

    /* Enable the master SPI peripheral */
    SPI_Enable(spi);
}

