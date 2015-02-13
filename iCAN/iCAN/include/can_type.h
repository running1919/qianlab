/**
* @file can_type.h
*
* can all type define
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __CAN_TYPE_H__
#define __CAN_TYPE_H__


/* can baudrate type */
#define RATE_1000K                  0x00
#define RATE_800K                   0x01
#define RATE_500K                   0x02
#define RATE_250K                   0x03
#define RATE_125K                   0x04
#define RATE_100K                   0x05
#define RATE_50K                    0x06
#define RATE_25K                    0x07
#define RATE_10K                    0x08
#define RATE_NOT_EFFECT             0xff

/* can mail box type */
#define ICAN_MAIL_BOX_RX            1
#define ICAN_MAIL_BOX_RX_OVERWRITE  2
#define ICAN_MAIL_BOX_TX            3
#define ICAN_MAIL_BOX_CONSUMER      4
#define ICAN_MAIL_BOX_PRODUCER      5

#endif

