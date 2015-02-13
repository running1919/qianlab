/**
* @file bitband.h
*
* cortex-M3 bitband header
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/


#ifndef __BITBAND_H__
#define __BITBAND_H__

#define RAM_BASE       0x20000000
#define RAM_BB_BASE    0x22000000
 
/* A mapping formula shows how to reference each word in the alias region to a 
   corresponding bit in the bit-band region. The mapping formula is:
   bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number x 4)

   where:
        - bit_word_addr : is the address of the word in the alias memory region that 
                    maps to the targeted bit.
        - bit_band_base : is the starting address of the alias region
        - byte_offset   : is the number of the byte in the bit-band region that contains 
                          the targeted bit
        - bit_number is : the bit position (0-7) of the targeted bit
*/

#define  Var_GetBitAddr_BB(VarAddr, BitNumber)  \
          ((__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)))

#define  Var_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 0)
   
#define Var_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define Var_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)))

#endif
