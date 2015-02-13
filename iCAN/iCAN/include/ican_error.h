/**
* @file ican_error.h
*
* ican error code 
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_ERROR_H__
#define __ICAN_ERROR_H__


/*ican error code type */
typedef enum _ierr_t {
//ican define error code
    ICAN_OK	                    = 0x00,
    ICAN_ERR_FUNC_ID            = 0x01,
    ICAN_ERR_SOURCE_ID          = 0x02,
    ICAN_ERR_COMMAND            = 0x03,
    ICAN_ERR_PARAM              = 0x04,
    ICAN_ERR_CONNECT            = 0x05,
    ICAN_ERR_OPERATE            = 0x06,
    ICAN_ERR_SPLIT_TRANS        = 0x07,

//selfdef error code
    ICAN_ERR_MAC_NONEXIST       = 0x08,
    ICAN_ERR_TIME_OUT           = 0x09,
    ICAN_ERR_TIME_NOOUT         = 0x0a,
    ICAN_ERR_BUFF_OVERFLOW      = 0x0b,
    ICAN_ERR_LEN_ZERO           = 0x0c,
    ICAN_ERR_NODE_BUSY          = 0x0d,
    ICAN_ERR_DEL_CONNECT        = 0x0e,
    ICAN_ERR_MAC_EXIST          = 0x0f,
    ICAN_ERR_RESET_DEV          = 0x10,
    ICAN_ERR_READ_SRC           = 0x11,
    ICAN_ERR_WRITE_SRC          = 0x12,
    ICAN_ERR_BUS_EXCEPT         = 0x13,
    ICAN_ERR_BUS_RESTART        = 0x14,
    ICAN_ERR_NOTACK_FRAME       = 0x15,
    ICAN_ERR_INVALID_POINTER    = 0x16,
    ICAN_ERR_NO_EVENT           = 0x17,
    ICAN_ERR_INVALID_ACK        = 0x18,
    ICAN_ERR_MAC_MORE           = 0X19,

    ICAN_ERR_SPLIT_FLOW         = 0xee,

    ICAN_ERR_MAX_VALUE          = 0xff
} ierr_t;

#endif

