/**
* @file usart.c
*
* USART1 configure as console
*
* 115200 baud, 8 Bits, 1 Stop Bit, No parity, No hardware flow
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include "io.h"
#include "usart.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>


extern void usartConsoleInit(void)
{
    USART_InitTypeDef USART_InitType;
    GPIO_InitTypeDef GPIO_InitType;

    /* Enable IO clocks */
    ioInit();

    /* Enable USART2 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /* Enable the USART1 Pins Software Remapping */
    //GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitType.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitType.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitType.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitType);

    /* Configure USART1 Rx as input floating */
    GPIO_InitType.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitType.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitType);

    /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control diabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitType.USART_BaudRate = 115200;
    USART_InitType.USART_WordLength = USART_WordLength_8b;
    USART_InitType.USART_StopBits = USART_StopBits_1;
    USART_InitType.USART_Parity = USART_Parity_No ;
    USART_InitType.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitType.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitType);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}


extern void usartPutchar(int c)
{
    USART_SendData(USART1, (uint16_t)c);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  
}


extern int usartGetchar(void)
{
    uint16_t result = 0;
    uint16_t ch = 0;

    while ((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET));
    ch = USART_ReceiveData(USART1);
    result = ch; //save & return the first char
    USART_SendData(USART1, ch); //echo

     while (ch != '\r') { 
        if ((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)) {
            ch = USART_ReceiveData(USART1);
            USART_SendData(USART1, ch);//echo
        }
    }

    return (int)result;
}


void printStr(const char *format, ...)
{
#ifdef DEBUG_ENABLE
    uint8_t buffer[512];
    va_list args;

    // Start processing the arguments 
    va_start(args, format);

    // Output the parameters into a string. Note:printf max 512 bytes
    vsnprintf((char *)buffer, sizeof(buffer) - 1, format, args);

    // Output the string to the console 
    printf((char *)buffer);     

    // End processing of the arguments 
    va_end(args);
#endif
}

#ifndef DBUG_SEMIHOSTED
#if defined __ICCARM__ /* IAR Ewarm 5.41+ */
#define WEAK __weak

/**
 * \brief Outputs a character on the usart.
 * \param c  Character to output.
 * \return The character that was output.
 */
extern WEAK signed int putchar(signed int c)
{
    usartPutchar(c) ;

    return c ;
}


/**
 * \brief Inputs a character on the usart.
 * \return The character that was input.
 */
extern WEAK signed int getchar(void)
{
    return usartGetchar();
}
#endif // defined __ICCARM__
#endif

