/*-------------------------------------------------------------------------*
 * File:  mstimer.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Using a timer, provide a one millisecond accurate timer.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdbool.h>
#include "../platform/GS_mstimer.h"



/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* 32-bit counter of current number of milliseconds since timer started */
volatile uint32_t G_msTimer;


/*---------------------------------------------------------------------------*
 * Routine:  MSTimerInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize and start the one millisecond timer counter.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void MSTimerInit(void)
{
   G_msTimer = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  MSTimerGet
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the number of millisecond counters since started.  This value
 *      rolls over ever 4,294,967,296 ms or 49.7102696 days.
 * Inputs:
 *      void
 * Outputs:
 *      uint32_t -- Millisecond counter since timer started or last rollover.
 *---------------------------------------------------------------------------*/
uint32_t MSTimerGet(void)
{
    return G_msTimer;
}

/*---------------------------------------------------------------------------*
 * Routine:  MSTimerDelta
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the current number of milliseconds expired since a given
 *      start timer value.
 * Inputs:
 *      uint32_t start -- Timer value at start of delta.
 * Outputs:
 *      uint32_t -- Millisecond counter since given timer value.
 *---------------------------------------------------------------------------*/
uint32_t MSTimerDelta(uint32_t start)
{
    return MSTimerGet() - start;
}

/*---------------------------------------------------------------------------*
 * Routine:  MSTimerDelay
 *---------------------------------------------------------------------------*
 * Description:
 *      Routine to idle and delay a given number of milliseconds doing
 *      nothing.
 * Inputs:
 *      uint32_t ms -- Number of milliseconds to delay
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void MSTimerDelay(uint32_t ms)
{
    uint32_t start = MSTimerGet();

    while (MSTimerDelta(start) < ms) {
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  mstimer.c
 *-------------------------------------------------------------------------*/

