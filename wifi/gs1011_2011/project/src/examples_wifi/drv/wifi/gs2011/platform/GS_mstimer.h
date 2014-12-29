#include <stdint.h>
#include <stdbool.h>

/**
   @brief Initializes millisecond timer
   
   This method is platform specific. 
 */ 
void MSTimerInit(void);

/**
   @brief Gets current millisecond timer count

   This method is platform specific.
   @return Milliseconds since the timer was initialized
 */
uint32_t MSTimerGet(void);

/**
   @brief Calculates the number of milliseconds between current time and value
   
   This method is platform specific.
   @param start Millisecond value to subtract from current time
   @return Milliseconds elapsed between start value and current time
 */
uint32_t MSTimerDelta(uint32_t start);

/**
   @brief Blocks for specified number of milliseconds
   
   This method is platform specific.
   @param ms Number of milliseconds to wait before returning
 */
void MSTimerDelay(uint32_t ms);