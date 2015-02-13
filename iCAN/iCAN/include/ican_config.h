/**
* @file ican_config.h
*
* ican func config
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_CONFIG_H__
#define __ICAN_CONFIG_H__

#include "can_type.h"

#define ICAN_MASTER_CHANNEL     0
#define ICAN_SLAVE_CHANNEL      0

#define ICAN_SLAVE_MACID        {0x11, 0x12, 0x13, 0x14}
#define ICAN_SLAVE_NUM          4    //value must equal macid's num
#define DEV_MAC_ID              0x12 //<0xff, ican as slave device id
#define MASTER_MAC_ID           0x78 //<0xff, ican as master device id


//ican io source
#define ICAN_DI_NUM             4   //DI SRC NUM, <=0x20
#if (ICAN_DI_NUM > 0)
#define ICAN_DI_LEN             4   //BYTE
#else
#define ICAN_DI_LEN             0
#endif

#define ICAN_DO_NUM             4   //DO SRC NUM, <=0x20
#if (ICAN_DO_NUM > 0)
#define ICAN_DO_LEN             4   //BYTE
#else
#define ICAN_DO_LEN             0
#endif

#define ICAN_AI_NUM             2   //AI SRC NUM, <=0x20
#if (ICAN_AI_NUM > 0)
#define ICAN_AI_LEN             4   //BYTE
#else
#define ICAN_AI_LEN             0
#endif

#define ICAN_AO_NUM             2   //AO SRC NUM, <=0x20
#if (ICAN_AO_NUM > 0)
#define ICAN_AO_LEN             4   //BYTE
#else
#define ICAN_AO_LEN             0
#endif

#define ICAN_SER0_NUM           4   //SERIAL0 SRC NUM, <=0x20
#if (ICAN_SER0_NUM > 0)
#define ICAN_SER0_LEN           4   //BYTE
#else
#define ICAN_SER0_LEN           0
#endif

#define ICAN_SER1_NUM           0   //SERIAL1 SRC NUM, <=0x20
#if (ICAN_SER1_NUM > 0)
#define ICAN_SER1_LEN           0   //BYTE
#else
#define ICAN_SER1_LEN           0
#endif

#define ICAN_USER_NUM           2   //USERDEF SRC NUM, <=0x20
#if (ICAN_USER_NUM > 0)
#define ICAN_USER_LEN           4   //BYTE
#else
#define ICAN_USER_LEN           0
#endif


//ican device identification
#define VENDOR_ID               0x4347      //<=0xffff
#define PRODUCT_TYPE            0x00        //<=0xffff
#define PRODUCT_CODE            0x00        //<=0xffff
#define HARDWARE_VERSION        0x01        //<=0xffff
#define FIRMWARE_VERSION        0x01        //<=0xffff
#define SERIAL_NUMBER           0xa55aa55a  //<=0xffffffff

//timer basic unit 
#define CYCLIC_UNIT_MS          50      //ms

//ican device communication param
#define BAUD_RATE               RATE_1000K
#define USER_BAUD_RATE          0       //<0xffff HZ
#define CYCLIC_PARAM            200     //<0xff CYCLIC_UNIT_MS, cycle time out limit
#define CYCLIC_MASTER           100     //<0xff CYCLIC_UNIT_MS, linker time out limit
#define COS_TYPE                1       //1 enable, 0 disable


//ican time out config
#define SEND_TIME_OUT           100     /* ms, > 10ms, if CYCLIC_MASTER > 0, the value must less
                                         * than ((CYCLIC_MASTER * CYCLIC_UNIT_MS) / ICAN_SLAVE_NUM)
                                         */
#define RESPONE_TIME_OUT        1000    /* ms, > 100ms, if CYCLIC_MASTER > 0, the value must less
                                         * than ((CYCLIC_MASTER * CYCLIC_UNIT_MS) / ICAN_SLAVE_NUM)
                                         */
#define POLL_TIME_OUT           20      /* ms, > 1ms, if CYCLIC_MASTER > 0, the value must less
                                         * than ((CYCLIC_MASTER * CYCLIC_UNIT_MS) / ICAN_SLAVE_NUM)
                                         */
#define CHECK_EVENT_TIME_OUT    1000    /* ms, > 100ms, if CYCLIC_MASTER > 0, the value must less
                                         * than ((CYCLIC_MASTER * CYCLIC_UNIT_MS) / ICAN_SLAVE_NUM)
                                         */
#define COS_TIME_OUT            1000    /* ms, > 100ms, if CYCLIC_MASTER > 0, the value must less
                                         * than ((CYCLIC_MASTER * CYCLIC_UNIT_MS) / ICAN_SLAVE_NUM)
                                         */

/* don't changed its defined!!! */
#define CHECK_MACID_TIME_OUT    ((CYCLIC_MASTER * CYCLIC_UNIT_MS) / ICAN_SLAVE_NUM)
#define CHECK_HEART_TIME_OUT    CHECK_MACID_TIME_OUT

//max segments by one transfer 
#define ICAN_SPLIT_MAX_SEGS     6       // <=64

//ican debug mode control
#define DEBUG_ICAN              1       //1 enable, 0 disable

#endif
