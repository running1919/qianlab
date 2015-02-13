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

/*------------------------------------------------------------------------------ */
/*         Headers                                                               */
/*------------------------------------------------------------------------------ */
#include <chip.h>

/*------------------------------------------------------------------------------ */
/*         Exported functions */
/*------------------------------------------------------------------------------ */

/**
 * \brief Initializes MMU.
 * \param pTB  Address of the translation table.
 */
void MMU_Initialize(unsigned int *pTB)
{
    unsigned int index;
    unsigned int addr;
    unsigned int domain_access = 0;

    // Reset table entries
    for (index = 0; index < 4096; index++)
        pTB[index] = 0;

    // section ROM (code + data)
    // ROM address (after remap) 0x0000_0000
    pTB[0x000] = (0x000 << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 1 <<  3)| // C bit : cachable => YES
                    ( 1 <<  2)| // B bit : write-back => YES
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section ROM (code + data)
    // ROM address (after remap) 0x0010_0000
    pTB[0x001] = (0x001 << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 1 <<  3)| // C bit : cachable => YES
                    ( 1 <<  2)| // B bit : write-back => YES
                    ( 2 <<  0); // Set as 1 Mbyte section


    // section RAM 0
    // SRAM address (after remap) 0x0030_0000
    pTB[0x003] = (0x003 << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PERIPH
    // periph address 0xF000_0000
    pTB[0xF00] = (0xF00ul << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PERIPH
    // periph address 0xF800_0000
    pTB[0xF80] = (0xF80ul << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PERIPH
    // periph address 0xFFF0_0000
    pTB[0xFFF] = (0xFFFul << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PSRAM CS0
    // periph address 0x1000_0000
    for(addr = 0x100; addr < 0x110; addr++)
        pTB[addr] = (addr << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section


    // section PSRAM CS2
    // periph address 0x3000_0000
    for(addr = 0x300; addr < 0x310; addr++)
        pTB[addr] = (addr << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PSRAM CS3
    // periph address 0x4000_0000
    for(addr = 0x400; addr < 0x410; addr++)
        pTB[addr] = (addr << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PSRAM CS4
    // periph address 0x5000_0000
    for(addr = 0x500; addr < 0x510; addr++)
        pTB[addr] = (addr << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section

    // section PSRAM CS5
    // periph address 0x6000_0000
    for(addr = 0x600; addr < 0x610; addr++)
        pTB[addr] = (addr << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 0 <<  3)| // C bit : cachable => NO
                    ( 0 <<  2)| // B bit : write-back => NO
                    ( 2 <<  0); // Set as 1 Mbyte section


    // section SDRAM/DDRAM
    // periph address 0x2000_0000
    for(addr = 0x200; addr < 0x280; addr++)
        pTB[addr] = (addr << 20)| // Physical Address
                    ( 3 << 10)| // Access in supervisor mode (AP)
                    ( 0 <<  5)| // Domain 0
                    ( 1 <<  4)| // (SBO)
                    ( 1 <<  3)| // C bit : cachable => YES
                    ( 0 <<  2)| // B bit : write-back => YES
                    ( 2 <<  0); // Set as 1 Mbyte section


    // Program the domain access register
    domain_access =
                    ( CP15_DOMAIN_NO_ACCESS     << (15*2) ) | // Domain 15
                    ( CP15_DOMAIN_NO_ACCESS     << (14*2) ) | // Domain 14
                    ( CP15_DOMAIN_NO_ACCESS     << (13*2) ) | // Domain 13
                    ( CP15_DOMAIN_NO_ACCESS     << (12*2) ) | // Domain 12
                    ( CP15_DOMAIN_NO_ACCESS     << (11*2) ) | // Domain 11
                    ( CP15_DOMAIN_NO_ACCESS     << (10*2) ) | // Domain 10
                    ( CP15_DOMAIN_NO_ACCESS     << ( 9*2) ) | // Domain 9
                    ( CP15_DOMAIN_NO_ACCESS     << ( 8*2) ) | // Domain 8
                    ( CP15_DOMAIN_NO_ACCESS     << ( 7*2) ) | // Domain 7
                    ( CP15_DOMAIN_NO_ACCESS     << ( 6*2) ) | // Domain 6
                    ( CP15_DOMAIN_NO_ACCESS     << ( 5*2) ) | // Domain 5
                    ( CP15_DOMAIN_NO_ACCESS     << ( 4*2) ) | // Domain 4
                    ( CP15_DOMAIN_NO_ACCESS     << ( 3*2) ) | // Domain 3
                    ( CP15_DOMAIN_NO_ACCESS     << ( 2*2) ) | // Domain 2
                    ( CP15_DOMAIN_NO_ACCESS     << ( 1*2) ) | // Domain 1
                    ( CP15_DOMAIN_CLIENT_ACCESS << ( 0*2) );  // Domain 0


    CP15_WriteTTB((unsigned int)pTB);
    CP15_InvalidateTLB();
    CP15_WriteDomainAccessControl(domain_access); // only domain 0: access are not checked
    CP15_EnableMMU();
}
