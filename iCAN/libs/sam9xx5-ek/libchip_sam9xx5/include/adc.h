/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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

/** \file */

/** \addtogroup adc_module
 *
 *  It converts the analog input to digital format. The converted result could be
 *  8bit or 10bit. The ADC supports up to 12 analog lines.
 *
 *  \section Usage
 *
 *  To Enable a ADC conversion,the user has to follow these few steps:
 * <ul>
 * <li> Select an appropriate reference voltage on ADVREF   </li>
 * <li> Configure the ADC according to its requirements and special needs,which
 *      could be  broken down into several parts:
 *   -#   Select the resolution by setting or clearing ADC_MR_LOWRES bit in
 *        ADC_MR (Mode Register)
 *   -#   Set ADC clock by setting ADC_MR_PRESCAL bits in ADC_MR, the clock is
 *        calculated with ADCClock = MCK / ( (PRESCAL+1) * 2 )
 *   -#   Set Startup Time,Tracking Clock cycles and Transfer Clock respectively
 *        in ADC_MR.
 *  </li>
 * <li> Start conversion by setting ADC_CR_START in ADC_CR. </li>
 * </ul>
 *
 *  To use the module for ADC operations:
 *  -# Initialize the ADC with ADC_Initialize().
 *  -# Select the active channel using ADC_EnableChannel()
 *  -# Start the conversion with ADC_StartConversion()
 *  -# Wait the end of the conversion by polling status with ADC_GetStatus()
 *  -# Finally, get the converted data using ADC_GetConvertedData()
 *
 *  \note PINs are switched to ADC function automatically if the ADC channel
 *        is used.
 *
 *  To support Touchscreen monitoring:
 *  -# Configure ADC as described above.
 *  -# Enable touchscreen mode with ADC_SetTsMode().
 *  -# Enable touchscreen pen detection by ADC_SetTsPenDetect(), setup its
 *     debounce time with ADC_SetTsDebounce().
 *  -# Configure touchscreen trigger with ADC_SetTriggerMode().
 *  -# When pen detected, the ADC samples are reported, X,Y,Pressure values
 *     can be read by ADC_GetTsXPosition(), ADC_GetTsYPosition(),
 *     ADC_GetTsPressure().
 *  -# Touchscreen interrupts and status are also managed by ADC functions as
 *     ADC_EnableIt(), ADC_DisableIt(), ADC_GetStatus().
 *
 * For more accurate information, please look at the ADC section of the
 * Datasheet.
 *
 * \sa \ref tsd_module
 *
 * Related files :\n
 * \ref adc.c\n
 * \ref adc.h\n
 */
#ifndef _ADC_
#define _ADC_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"

#include <assert.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/
#define ADC_CHANNEL_0  0
#define ADC_CHANNEL_1  1
#define ADC_CHANNEL_2  2
#define ADC_CHANNEL_3  3
#define ADC_CHANNEL_4  4
#define ADC_CHANNEL_5  5
#define ADC_CHANNEL_6  6
#define ADC_CHANNEL_7  7
#define ADC_CHANNEL_8  8
#define ADC_CHANNEL_9  9
#define ADC_CHANNEL_10 10
#define ADC_CHANNEL_11 11
#define ADC_CHANNEL_12 12

#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------------------------------------------------
 *         Macros function of register access
 *------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/
extern void ADC_Initialize( Adc* pAdc, uint8_t idAdc, uint8_t trgEn, uint8_t trgSel, uint8_t sleepMode, uint8_t resolution,
                            uint32_t mckClock, uint32_t adcClock, uint32_t startupTime, uint32_t sampleAndHoldTime ) ;


extern void ADC_StartConversion(Adc * pAdc);
extern void ADC_Reset(Adc * pAdc);
extern void ADC_TsCalibration(Adc * pAdc);

extern void ADC_ConfigureMode(Adc * pAdc,uint32_t dwMode);
extern void ADC_SetLowResolution(Adc * pAdc,uint8_t bEnDis);
extern void ADC_SetSleepMode(Adc * pAdc,uint8_t bEnDis);
extern void ADC_SetFastWakeup(Adc * pAdc,uint8_t bOnOff);
extern uint32_t ADC_SetClock(Adc * pAdc,uint32_t dwClk,uint32_t dwMck);
extern void ADC_SetStartupTime(Adc * pAdc,uint32_t dwUs);
extern void ADC_SetTrackingTime(Adc * pAdc,uint32_t dwNs);

extern void ADC_SetSequenceMode(Adc * pAdc,uint8_t bEnDis);
extern void ADC_SetSequence(Adc * pAdc,uint32_t dwSEQ1,uint32_t dwSEQ2);
extern void ADC_SetSequenceByList(Adc * pAdc,uint8_t bChList [ ],uint8_t bNumCh);

extern void ADC_EnableChannel(Adc * pAdc,uint32_t dwChannel);
extern void ADC_DisableChannel(Adc * pAdc,uint32_t dwChannel);
extern uint32_t ADC_GetChannelStatus(Adc * pAdc);

extern uint32_t ADC_GetData( Adc* pAdc, uint32_t dwChannel );
extern uint32_t ADC_GetLastData(Adc * pAdc);

extern void ADC_EnableIt(Adc * pAdc,uint32_t dwSources);
extern void ADC_DisableIt(Adc * pAdc,uint32_t dwSources);
extern uint32_t ADC_GetItMask(Adc * pAdc);
extern uint32_t ADC_GetItStatus(Adc * pAdc);
extern uint32_t ADC_GetOverrunStatus(Adc * pAdc);

extern void ADC_ConfigureExtMode(Adc * pAdc,uint32_t dwMode);
extern void ADC_SetTagEnable(Adc * pAdc,uint8_t bEnDis);
extern void ADC_SetCompareChannel( Adc* pAdc, uint32_t dwChannel );
extern void ADC_SetCompareMode( Adc* pAdc, uint32_t dwMode );
extern void ADC_SetComparisonWindow( Adc* pAdc, uint32_t dwHi_Lo );

extern void ADC_SetTsMode(Adc * pADC,uint32_t dwMode);
extern uint32_t ADC_GetTsMode(Adc * pAdc);
extern void ADC_SetTsDebounce(Adc * pADC,uint32_t dwTime);
extern void ADC_SetTsPenDetect(Adc * pADC,uint8_t bEnDis);
extern void ADC_SetTsAverage(Adc * pADC,uint32_t dwAvg2Conv);

extern uint32_t ADC_GetTsXPosition(Adc * pADC);
extern uint32_t ADC_GetTsYPosition(Adc * pADC);
extern uint32_t ADC_GetTsPressure(Adc * pADC);

extern void ADC_SetTriggerMode(Adc * pAdc,uint32_t dwMode);
extern void ADC_SetTriggerPeriod(Adc * pAdc,uint32_t dwPeriod);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ADC_ */
