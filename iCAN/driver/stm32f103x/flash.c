/**
* @file flash.c
*
* stm32f10x internal flash interface
*
* \note user can store at most 4KB data
*
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include "stm32f10x.h"
#include "flash.h"

#define FLASH_PAGE_SIZE        1024

#define USER_FLASH_START_ADDR  ((uint32_t)0x0800F000)
#define USER_FLASH_END_ADDR    ((uint32_t)0x0800FFFF)

#define USER_FLASH_PAGE_START  (USER_FLASH_START_ADDR / FLASH_PAGE_SIZE)
#define USER_FLASH_SIZE        (USER_FLASH_END_ADDR - USER_FLASH_START_ADDR + 1)


static FLASH_Status flashPageWrite(uint16_t pageIndex, uint16_t offset, uint8_t* buff, uint32_t len)
{
    uint16_t i;
    uint32_t data[FLASH_PAGE_SIZE / 4]; //1024 byte
    uint8_t* pdata;
    uint32_t pageAddr;
    FLASH_Status fs;

    if (offset > 1023 || offset + len > 1024) {
        return FLASH_ERROR_PG;
    }

    pdata = (uint8_t *)data;
    pageAddr = (uint32_t)(USER_FLASH_PAGE_START + pageIndex) * FLASH_PAGE_SIZE;

    /* get page data in flash */
    for (i = 0; i < 256; i++) {
        data[i] = *((uint32_t *)(pageAddr + i * 4)); 
    }

    /* copy user data to buffer */
    for (i = 0; i < len; i++) {
        pdata[offset + i] = buff[i];
    }

    FLASH_UnlockBank1();

    /* erase flash page */
    fs = FLASH_ErasePage(pageAddr);
    if (fs != FLASH_COMPLETE) {
        FLASH_LockBank1();
        return fs;
    }

    /* program flash page */
    for (i = 0; i < 256; i++) {
        fs = FLASH_ProgramWord(pageAddr + i * 4, data[i]);
        if (fs != FLASH_COMPLETE) {
            FLASH_LockBank1();
            return fs;
        }
    }

    FLASH_LockBank1();

    return FLASH_COMPLETE;
}


void flashInit(void)
{
#if 0
    uint16_t userData;
    FLASH_Status fs;
    
    userData = (FLASH_GetUserOptionByte() >> 8) & 0xffff;
    if ((userData & 0xfff0 != 0x6560) && (userData & 0x000f < 4)) {
        userData = 0x6560;
        sectorId = 0;

        FLASH_UnlockBank1();

        fs = FLASH_EraseOptionBytes();
        if (fs != FLASH_COMPLETE) {
            while (1);
        }        
        fs = FLASH_ProgramOptionByteData(0x1FFFF804, userData & 0x00ff);
        if (fs != FLASH_COMPLETE) {
            while (1);
        }        
        fs = FLASH_ProgramOptionByteData(0x1FFFF806, userData >> 8);
        if (fs != FLASH_COMPLETE) {
            while (1);
        }
        
        FLASH_LockBank1();        
    }
    else {
        sectorId = userData & 0x000f;
    }
#endif
}


/**
* \brief flash read
* \param offset offset to user data flash start
* \param buff user data buffer pointer
* \param len user wish read data length
* \return real read data length
*/
uint32_t flashRead(uint32_t offset, uint8_t* buff, uint32_t len)
{
    uint32_t i;
    uint32_t size;
    uint32_t addr;
    uint8_t* start;

    addr = USER_FLASH_START_ADDR + offset;
    start = (uint8_t *)addr;

    if (addr > USER_FLASH_END_ADDR) {
        return 0;
    }

    size = len;
    if (addr + len > USER_FLASH_END_ADDR) {
        size = USER_FLASH_END_ADDR - addr + 1;
    }

    for (i = 0; i < size; i++) {
        buff[i] = *(start + i);
    }

    return size;
}


/**
* \brief flash write
* \param offset offset to user data flash start
* \param buff user data buffer pointer
* \param len user wish write data length
* \return real write data length
*/
uint32_t flashWrite(uint32_t offset, uint8_t* buff, uint32_t len)
{
    uint8_t i;
    uint32_t actualWriteSize;
    uint32_t startAddr;
    uint16_t firstWriteLen, midPageNum, lastWriteLen;    
    uint16_t firstPageIndex, firstPageOffset;
    FLASH_Status fs;

    startAddr = USER_FLASH_START_ADDR + offset;
    if (startAddr > USER_FLASH_END_ADDR || len == 0) {
        return 0;
    }

    /* get the actual writing data length */    
    actualWriteSize = len;
    if (startAddr + len > USER_FLASH_END_ADDR) {
        actualWriteSize = USER_FLASH_END_ADDR - startAddr + 1;
    }

    /* write first page */
    firstPageIndex = startAddr / FLASH_PAGE_SIZE - USER_FLASH_PAGE_START;
    firstPageOffset = startAddr % FLASH_PAGE_SIZE;
    firstWriteLen = FLASH_PAGE_SIZE - firstPageOffset;
    if (firstWriteLen > len) {        
        firstWriteLen = actualWriteSize;
    }

    fs = flashPageWrite(firstPageIndex,
                        firstPageOffset, buff, firstWriteLen);
    if (fs != FLASH_COMPLETE) {
        return 0;
    }

    /* write middle data */
    midPageNum = (actualWriteSize - firstWriteLen) / FLASH_PAGE_SIZE;
    for (i = 1; i <= midPageNum; i++) {//i must set 1
        fs = flashPageWrite(i, 0, &buff[firstWriteLen
                            + (i - 1) * FLASH_PAGE_SIZE], 1024);
        if (fs != FLASH_COMPLETE) {
            return 0;
        }
    }

    /* write last data */
    lastWriteLen = (actualWriteSize - firstWriteLen) % FLASH_PAGE_SIZE;
    if (lastWriteLen > 0) {
        fs = flashPageWrite(midPageNum + 1, 0, &buff[firstWriteLen
                            + midPageNum * FLASH_PAGE_SIZE], lastWriteLen);
        if (fs != FLASH_COMPLETE) {
            return 0;
        }
    }

    return actualWriteSize;
}

