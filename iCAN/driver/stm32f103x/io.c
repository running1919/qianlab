/**
* @file io.c
*
* stm32f10x io interface
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include "io.h"


void ioInit(void)
{
    static bool initEnable = false;
    /* GPIO clock enable */
    if (initEnable == 0) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

        initEnable = true;
    }
}


void ioConfigInput(uint8_t pinArray, uint8_t pinNum)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = (uint16_t)1 << pinNum;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 
    switch (pinArray) {
        case 'A' :
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            break;

        case 'B' :
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            break;
    }
}


void ioConfigOutput(uint8_t pinArray, uint8_t pinNum)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = (uint16_t)1 << pinNum;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 
    switch (pinArray) {
        case 'A' :
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            break;

        case 'B' :
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            break;

        case 'C' :
        case 'D' :
        default  :
            break;
    }
}


/**
* \brief read input mode io pin
* \return io pin level
*/
uint8_t ioReadI(uint8_t pinArray, uint8_t pinNum)
{
    uint16_t GPIO_Pin = (uint16_t)1 << pinNum;
 
    switch (pinArray) {
        case 'A' :
            return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin);

        case 'B' :
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin);

        case 'C' :
        case 'D' :
        default  :
            return 0xFF;
    }
}

/**
* \brief read output mode io pin
* \return io pin level
*/
uint8_t ioReadO(uint8_t pinArray, uint8_t pinNum)
{
    uint16_t GPIO_Pin = (uint16_t)1 << pinNum;
 
    switch (pinArray) {
        case 'A' :
            return GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin);

        case 'B' :
            return GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin);

        case 'C' :
        case 'D' :
        default  :
            return 0xFF;
    }
}


void ioWrite(uint8_t pinArray, uint8_t pinNum, uint8_t level)
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;

    switch (pinArray) {
        case 'A' :
            GPIOx = GPIOA;
            break;

        case 'B' :
            GPIOx = GPIOB;
            break;

        case 'C' :
        case 'D' :
        default  :
            return;
    }

    GPIO_Pin = (uint16_t)1 << pinNum;
    if (level > 0) {
        GPIO_SetBits(GPIOx, GPIO_Pin);
    }
    else {
        GPIO_ResetBits(GPIOx, GPIO_Pin);
    }
}
