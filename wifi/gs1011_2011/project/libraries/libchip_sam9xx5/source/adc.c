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
 * Implementation of Analog-to-Digital Converter (ADC).
 *
 */
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include "chip.h"

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** Current working clock */
static uint32_t dwAdcClock = 0;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
#if 0
/**
 * \brief Initialize the ADC controller
 *
 * \param pAdc Pointer to an Adc instance.
 * \param idAdc ADC Index
 * \param trgEn trigger mode, software or Hardware
 * \param trgSel hardware trigger selection
 * \param sleepMode sleep mode selection
 * \param resolution resolution selection 10 bits or 12 bits
 * \param mckClock value of MCK in Hz
 * \param adcClock value of the ADC clock in Hz
 * \param startup value of the start up time (in ADCClock) (see datasheet)
 * \param tracking Tracking Time (in ADCClock cycle)
 */
void ADC_Initialize( Adc* pAdc,
                     uint8_t idAdc,
                     uint8_t trgEn, uint8_t trgSel,
                     uint8_t sleepMode,
                     uint8_t resolution,
                     uint32_t mckClock, uint32_t adcClock,
                     uint32_t startup,
                     uint32_t tracking )
{
   
}
#endif
/**
 * Start conversion
 * \param pAdc Pointer to an Adc instance.
 */
void ADC_StartConversion( Adc *pAdc )
{
    pAdc->ADC_CR = ADC_CR_START;
}

/**
 * Reset
 * \param pAdc Pointer to an Adc instance.
 */
void ADC_Reset( Adc *pAdc )
{
    pAdc->ADC_CR = ADC_CR_SWRST;
}

/**
 * Set mode register.
 * \param pAdc Pointer to an Adc instance.
 * \param dwMode Mode register configuration.
 */
void ADC_ConfigureMode( Adc *pAdc, uint32_t dwMode )
{
    pAdc->ADC_MR = dwMode;
}

/**
 * Enable/Disable low resolution.
 * \param pAdc Pointer to an Adc instance.
 * \param bEnDis 8-bit/10-bit resolution select.
 */
void ADC_SetLowResolution( Adc *pAdc, uint8_t bEnDis )
{
#if !defined(sam9g25) && !defined(sam9x25)
    if (bEnDis) pAdc->ADC_MR |=  ADC_MR_LOWRES;
    else        pAdc->ADC_MR &= ~ADC_MR_LOWRES;
#else
    pAdc = pAdc; bEnDis = bEnDis;
#endif    
}

/**
 * Enable/Disable sleep mode.
 * \param pAdc Pointer to an Adc instance.
 * \param bEnDis Enable/Disable sleep mode.
 */
void ADC_SetSleepMode( Adc *pAdc, uint8_t bEnDis )
{
#if !defined(sam9g25) && !defined(sam9x25)
    if (bEnDis) pAdc->ADC_MR |=  ADC_MR_SLEEP;
    else        pAdc->ADC_MR &= ~ADC_MR_SLEEP;
#else
    pAdc = pAdc; bEnDis = bEnDis;
#endif    
}

/**
 * Enable/Disable fast wake up.
 * \param pAdc Pointer to an Adc instance.
 * \param bEnDis Enable/Disable fast wake up sleep mode.
 */
void ADC_SetFastWakeup( Adc *pAdc, uint8_t bOnOff)
{
#if !defined(sam9g25) && !defined(sam9x25)
    if (bOnOff) pAdc->ADC_MR |=  ADC_MR_FWUP;
    else        pAdc->ADC_MR &= ~ADC_MR_FWUP;
#else
    pAdc = pAdc; bOnOff = bOnOff;
#endif
}

/**
 * Set prescaler rate
 * \param pAdc  Pointer to an Adc instance.
 * \param dwClk Desired ADC clock frequency.
 * \param dwMck Current MCK.
 * \return 0 if not set. otherwise actual clock.
 */
uint32_t ADC_SetClock( Adc *pAdc, uint32_t dwClk, uint32_t dwMck )
{
    uint32_t dwPres, dwMr;
    /* Formula for PRESCAL is:
       PRESCAL = (MCK / (2 * ADCCLK)) + 1
       First, we do the division, multiplied by 10 to get higher precision
       If the last digit is not zero, we round up to avoid generating a higher
       than required frequency. */
    dwPres = (dwMck * 5) / dwClk;
    if (dwPres % 10) dwPres = dwPres / 10;
    else
    {
        if (dwPres == 0) return 0;
        dwPres = dwPres / 10 - 1;
    }

    dwMr = ADC_MR_PRESCAL(dwPres);
    if (dwMr == 0) return 0;

    dwMr |= (pAdc->ADC_MR & ~ADC_MR_PRESCAL_Msk);
    pAdc->ADC_MR = dwMr;

    dwAdcClock = dwMck / (dwPres + 1) / 2;
    //dwAdcClock = dwAdcClock / 1000 * 1000;
    return dwAdcClock;
}

/**
 * Sets the ADC startup time.
 * \param pAdc  Pointer to an Adc instance.
 * \param dwUs  Startup time in uS.
 */
void ADC_SetStartupTime( Adc *pAdc, uint32_t dwUs )
{
    uint32_t dwStart;
    uint32_t dwMr;

    if (dwAdcClock == 0) return;
    /* Formula for STARTUP is:
       STARTUP = (time x ADCCLK) / (1000000) - 1
       Division multiplied by 10 for higher precision */
    
    dwStart = (dwUs * dwAdcClock) / (100000);
    if (dwStart % 10) dwStart /= 10;
    else
    {
        dwStart /= 10;
        if (dwStart) dwStart --;
    }
    if      (dwStart >  896) dwMr = ADC_MR_STARTUP_SUT960;
    else if (dwStart >  832) dwMr = ADC_MR_STARTUP_SUT896;
    else if (dwStart >  768) dwMr = ADC_MR_STARTUP_SUT832;
    else if (dwStart >  704) dwMr = ADC_MR_STARTUP_SUT768;
    else if (dwStart >  640) dwMr = ADC_MR_STARTUP_SUT704;
    else if (dwStart >  576) dwMr = ADC_MR_STARTUP_SUT640;
    else if (dwStart >  512) dwMr = ADC_MR_STARTUP_SUT576;
    else if (dwStart >  112) dwMr = ADC_MR_STARTUP_SUT512;
    else if (dwStart >   96) dwMr = ADC_MR_STARTUP_SUT112;
    else if (dwStart >   80) dwMr = ADC_MR_STARTUP_SUT96;
    else if (dwStart >   64) dwMr = ADC_MR_STARTUP_SUT80;
    else if (dwStart >   24) dwMr = ADC_MR_STARTUP_SUT64;
    else if (dwStart >   16) dwMr = ADC_MR_STARTUP_SUT24;
    else if (dwStart >    8) dwMr = ADC_MR_STARTUP_SUT16;
    else if (dwStart >    0) dwMr = ADC_MR_STARTUP_SUT8;
    else                     dwMr = ADC_MR_STARTUP_SUT0;

    dwMr |= pAdc->ADC_MR & ~ADC_MR_STARTUP_Msk;
    pAdc->ADC_MR = dwMr;
}

/**
 * Set ADC tracking time
 * \param pAdc  Pointer to an Adc instance.
 * \param dwNs  Tracking time in nS.
 */
void ADC_SetTrackingTime( Adc *pAdc, uint32_t dwNs )
{
    uint32_t dwShtim;
    uint32_t dwMr;

    if (dwAdcClock == 0) return;
    /* Formula for SHTIM is:
       SHTIM = (time x ADCCLK) / (1000000000) - 1
       Since 1 billion is close to the maximum value for an integer, we first
       divide ADCCLK by 1000 to avoid an overflow */
    dwShtim = (dwNs * (dwAdcClock / 1000)) / 100000;
    if (dwShtim % 10) dwShtim /= 10;
    else
    {
        dwShtim /= 10;
        if (dwShtim) dwShtim --;
    }
    dwMr  = ADC_MR_TRACKTIM(dwShtim);
    dwMr |= pAdc->ADC_MR & ~ADC_MR_TRACKTIM_Msk;
    pAdc->ADC_MR = dwMr;
}

/**
 * Enable/Disable seqnence mode.
 * \param pAdc  Pointer to an Adc instance.
 */
void ADC_SetSequenceMode(Adc *pAdc, uint8_t bEnDis)
{
    if (bEnDis) pAdc->ADC_MR |=  ADC_MR_USEQ;
    else        pAdc->ADC_MR &= ~ADC_MR_USEQ;
}

/**
 * Set channel sequence
 * \param pAdc   Pointer to an Adc instance.
 * \param dwSEQ1 Sequence 1 ~ 8  channel number.
 * \param dwSEQ2 Sequence 9 ~ 16 channel number.
 */
void ADC_SetSequence(Adc *pAdc, uint32_t dwSEQ1, uint32_t dwSEQ2)
{
    pAdc->ADC_SEQR1 = dwSEQ1;
    pAdc->ADC_SEQR2 = dwSEQ2;
}

/**
 * Set channel sequence
 * \param pAdc    Pointer to an Adc instance.
 * \param bChList Channel list.
 * \param bNumCh  Number of channels in list.
 */
void ADC_SetSequenceByList(Adc *pAdc, uint8_t bChList[], uint8_t bNumCh)
{
    uint8_t i;
    uint8_t shift;
    pAdc->ADC_SEQR1 = 0;
    for (i = 0, shift = 0; i < 8; i ++, shift += 4)
    {
        if (i >= bNumCh) return;
        pAdc->ADC_SEQR1 |= bChList[i] << shift;
        
    }
    pAdc->ADC_SEQR2 = 0;
    for (shift = 0; i < 16; i ++, shift += 4)
    {
        if (i >= bNumCh) return;
        pAdc->ADC_SEQR2 |= bChList[i] << shift;
    }
}

/**
 * Enable ADC channel
 * \param pAdc Pointer to an Adc instance.
 */
void ADC_EnableChannel( Adc *pAdc, uint32_t dwChannel )
{
    //pAdc->ADC_CHER = ADC_CHER_CH0 << dwChannel;
    pAdc->ADC_CHER = (ADC_CHER_CH5 >> 5) << dwChannel;
}

/**
 * Disable ADC channel
 * \param pAdc Pointer to an Adc instance.
 */
void ADC_DisableChannel( Adc *pAdc, uint32_t dwChannel )
{
    //pAdc->ADC_CHDR = ADC_CHDR_CH0 << dwChannel;
    pAdc->ADC_CHDR = (ADC_CHER_CH5 >> 5) << dwChannel;
}

/**
 * Return channel status
 * \param pAdc Pointer to an Adc instance.
 */
uint32_t ADC_GetChannelStatus( Adc *pAdc )
{
    return pAdc->ADC_CHSR;
}

/**
 * Return converted data of channel
 * \param pAdc Pointer to an Adc instance.
 * \param dwChannel ADC channel.
 */
uint32_t ADC_GetData( Adc *pAdc, uint32_t dwChannel )
{
  #if defined(REG_ADC_CDR0)
    volatile uint32_t *pCDR = (volatile uint32_t*)&pAdc->ADC_CDR0;
  #elif defined(REG_ADC_CDR)
    volatile uint32_t *pCDR = (volatile uint32_t*)(pAdc->ADC_CDR);
  #else
  #endif
    return pCDR[dwChannel];
}

/**
 * Return the last converted data
 * 
 * \param pAdc Pointer to an Adc instance.
 */
uint32_t ADC_GetLastData( Adc *pAdc )

{
    return pAdc->ADC_LCDR;
}

/**
 * Enable ADC interrupt
 * \param pAdc Pointer to an Adc instance.
 * \param dwSources Bitmap for interrupts.
 */
void ADC_EnableIt( Adc *pAdc, uint32_t dwSources )
{
    pAdc->ADC_IER = dwSources;
}

/**
 * Disable ADC interrupt
 * \param pAdc Pointer to an Adc instance.
 * \param dwSources Bitmap for interrupts.
 */
void ADC_DisableIt( Adc *pAdc, uint32_t dwSources )
{
    pAdc->ADC_IDR = dwSources;
}

/**
 * Return bit mask for enabled interrupts.
 * \param pAdc Pointer to an Adc instance.
 */
uint32_t ADC_GetItMask( Adc *pAdc )
{
    return pAdc->ADC_IMR;
}

/**
 * Return interrupt status
 * \param pAdc Pointer to an Adc instance.
 */
uint32_t ADC_GetItStatus( Adc *pAdc )
{
    return pAdc->ADC_ISR;
}

/**
 * Return overrun status for channels
 * \param pAdc Pointer to an Adc instance.
 */
uint32_t ADC_GetOverrunStatus( Adc *pAdc )
{
    return pAdc->ADC_OVER;
}

/**
 * Configure extended mode register
 * \param pAdc Pointer to an Adc instance.
 */
void ADC_ConfigureExtMode( Adc *pAdc, uint32_t dwMode )
{
    pAdc->ADC_EMR = dwMode;
}

/**
 * Set "TAG" mode, show channel number in last data or not.
 * \param pAdc   Pointer to an Adc instance.
 * \param bEnDis Enable/Disable TAG value.
 */
void ADC_SetTagEnable( Adc *pAdc, uint8_t bEnDis )
{
    if (bEnDis) pAdc->ADC_EMR |=  ADC_EMR_TAG;
    else        pAdc->ADC_EMR &= ~ADC_EMR_TAG;
}

/**
 * Set compare channel
 * \param pAdc Pointer to an Adc instance.
 * \param dwChannel channel number to be set
 *                  0xFFFFFFFF for all channels
 */
extern void ADC_SetCompareChannel( Adc* pAdc, uint32_t dwChannel )
{
    uint32_t dwEmr;
    if (dwChannel == 0xFFFFFFFF) pAdc->ADC_EMR |= ADC_EMR_CMPALL;
    else
    {
        dwEmr  = pAdc->ADC_EMR & ~(ADC_EMR_CMPALL | ADC_EMR_CMPSEL_Msk);
        dwEmr |= ADC_EMR_CMPSEL(dwChannel);
        pAdc->ADC_EMR = dwEmr;
    }
}

/**
 * Set compare mode
 * \param pAdc Pointer to an Adc instance.
 * \param mode compare mode
 */
extern void ADC_SetCompareMode( Adc* pAdc, uint32_t dwMode )
{
    uint32_t dwEmr;
    dwEmr = pAdc->ADC_EMR & ~ADC_EMR_CMPMODE_Msk;
    pAdc->ADC_EMR = dwEmr | (dwMode & ADC_EMR_CMPMODE_Msk);
}

/**
 * Set comparsion window,one thereshold each time
 * \param pAdc Pointer to an Adc instance.
 * \param hi_lo Comparison Window
 */
extern void ADC_SetComparisonWindow( Adc* pAdc, uint32_t dwHi_Lo )
{
    pAdc->ADC_CWR = dwHi_Lo;
}


/**
 * Sets the trigger mode to following:
 * - \ref ADC_TRGR_TRGMOD_NO_TRIGGER
 * - \ref ADC_TRGR_TRGMOD_EX_TRIG_RISE
 * - \ref ADC_TRGR_TRGMOD_EX_TRIG_FALL
 * - \ref ADC_TRGR_TRGMOD_EX_TRIG_ANY
 * - \ref ADC_TRGR_TRGMOD_PEN_TIRG
 * - \ref ADC_TRGR_TRGMOD_PERID_TRIG
 * - \ref ADC_TRGR_TRGMOD_CONTINUOUS
 * \param pAdc   Pointer to an Adc instance.
 * \param dwMode Trigger mode.
 */
void ADC_SetTriggerMode(Adc *pAdc, uint32_t dwMode)
{
    uint32_t dwTrgr = pAdc->ADC_TRGR & ~ADC_TRGR_TRGMOD_Msk;
    pAdc->ADC_TRGR = dwTrgr | dwMode;
}

/**
 * Sets the trigger period.
 * \param pAdc   Pointer to an Adc instance.
 * \param dwPeriod Trigger period in nS.
 */
void ADC_SetTriggerPeriod(Adc *pAdc, uint32_t dwPeriod)
{
    uint32_t dwTrgper;
    uint32_t dwDiv = 100000000;
    uint32_t dwTrgr;
    if (dwAdcClock == 0) return;
    while (dwPeriod >= 10 && dwDiv >= 10)
    {
        dwPeriod /= 10; dwDiv /= 10;
    }
    dwTrgper = (dwPeriod * dwAdcClock) / dwDiv;
    if (dwTrgper % 10) dwTrgper /= 10;
    else
    {
        dwTrgper /= 10;
        if (dwTrgper) dwTrgper --;
    }
    dwTrgr = ADC_TRGR_TRGPER(dwTrgper);
    dwTrgr |= pAdc->ADC_TRGR & ~ADC_TRGR_TRGPER_Msk;
    pAdc->ADC_TRGR = dwTrgr;
}

#ifdef REG_ADC_TSMR

/**
 * Start screen calibration (VDD/GND measurement)
 * \param pAdc Pointer to an Adc instance.
 */
void ADC_TsCalibration( Adc *pAdc )
{
    pAdc->ADC_CR = ADC_CR_TSCALIB;
}

/**
 * Sets the operation mode of the touch screen ADC. The mode can be:
 * - \ref ADC_TSMR_TSMODE_NONE (TSADC off)
 * - \ref ADC_TSMR_TSMODE_4_WIRE_NO_PM
 * - \ref ADC_TSMR_TSMODE_4_WIRE (CH 0~3 used)
 * - \ref ADC_TSMR_TSMODE_5_WIRE (CH 0~4 used)
 * \param pADC   Pointer to an Adc instance.
 * \param dwMode Desired mode
 */
void ADC_SetTsMode(Adc* pADC, uint32_t dwMode)
{
    pADC->ADC_TSMR = (pADC->ADC_TSMR & ~ADC_TSMR_TSMODE_Msk) | dwMode;
}

/**
 * Return touchscreen mode
 * \param pAdc Pointer to an Adc instance.
 */
uint32_t ADC_GetTsMode(Adc *pAdc)
{
    return pAdc->ADC_TSMR;
}

/**
 * Sets the touchscreen pan debounce time.
 * \param pADC   Pointer to an Adc instance.
 * \param dwTime Debounce time in nS.
 */
void ADC_SetTsDebounce(Adc *pADC, uint32_t dwTime)
{
    uint32_t dwDiv = 1000000000;
    uint32_t dwClk = dwAdcClock;
    uint32_t dwPenbc = 0;
    uint32_t dwTarget, dwCurrent;
    uint32_t dwTsmr;
    if (dwTime == 0 || dwAdcClock == 0) return;
    /* Divide time & ADCCLK to avoid overflows */
    while ((dwDiv > 1) && ((dwTime % 10) == 0))
    {
        dwTime /= 10; dwDiv /= 10;
    }
    while ((dwDiv > 1) && ((dwClk & 10) == 0))
    {
        dwClk /= 10; dwDiv /= 10;
    }
    /* Compute PENDBC */
    dwTarget = dwTime * dwClk / dwDiv;
    dwCurrent = 1;
    while (dwCurrent < dwTarget)
    {
        dwPenbc ++; dwCurrent *= 2;
    }
    dwTsmr = ADC_TSMR_PENDBC(dwPenbc);
    if (dwTsmr == 0) return;
    dwTsmr |= pADC->ADC_TSMR & ~ADC_TSMR_PENDBC_Msk;
    pADC->ADC_TSMR = dwTsmr;
}

/**
 * Enable/Disable touch screen pen detection.
 * \param pADC   Pointer to an Adc instance.
 * \param bEnDis If true, pen detection is enabled;
 *               in normal mode otherwise.
 */
void ADC_SetTsPenDetect(Adc* pADC, uint8_t bEnDis)
{
    if (bEnDis)
        pADC->ADC_TSMR |=  ADC_TSMR_PENDET;
    else
        pADC->ADC_TSMR &= ~ADC_TSMR_PENDET;
}

/**
 * Sets the average of the touch screen ADC. The mode can be:
 * - \ref ADC_TSMR_TSAV_NO_FILTER (No filtering)
 * - \ref ADC_TSMR_TSAV_AVG2CONV (Average 2 conversions)
 * - \ref ADC_TSMR_TSAV_AVG4CONV (Average 4 conversions)
 * - \ref ADC_TSMR_TSAV_AVG8CONV (Average 8 conversions)
 * \param pADC   Pointer to an Adc instance.
 * \param dwMode Desired mode
 */
void ADC_SetTsAverage(Adc* pADC, uint32_t dwAvg2Conv)
{
    uint32_t dwMr = pADC->ADC_TSMR & ~ADC_TSMR_TSAV_Msk;
    uint32_t dwTSAV = dwAvg2Conv >> ADC_TSMR_TSAV_Pos;
    uint32_t dwTSFREQ = (dwMr & ADC_TSMR_TSFREQ_Msk) >> ADC_TSMR_TSFREQ_Pos;
    if (dwTSAV)
    {
        if (dwTSAV > dwTSFREQ)
        {
            dwMr &= ~ADC_TSMR_TSFREQ_Msk;
            dwMr |=  ADC_TSMR_TSFREQ(dwTSAV);
        }
    }
    pADC->ADC_TSMR = dwMr | dwAvg2Conv;
}

/**
 * Return X measurement position value.
 * \param pADC   Pointer to an Adc instance.
 */
uint32_t ADC_GetTsXPosition(Adc *pADC)
{
    return pADC->ADC_XPOSR;
}

/**
 * Return Y measurement position value.
 * \param pADC   Pointer to an Adc instance.
 */
uint32_t ADC_GetTsYPosition(Adc *pADC)
{
    return pADC->ADC_YPOSR;
}

/**
 * Return Z measurement position value.
 * \param pADC   Pointer to an Adc instance.
 */
uint32_t ADC_GetTsPressure(Adc *pADC)
{
    return pADC->ADC_PRESSR;
}

#endif /* #ifdef REG_ADC_TSMR */

