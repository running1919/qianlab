/**
* @file ican_type.h
*
* ican all type define
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_TYPE_H__
#define __ICAN_TYPE_H__

#include <stdint.h>
#include <stdbool.h>

#include "ican_config.h"
#include "ican_error.h"

/*\Note All ican transfer over next macro */
#define ICAN_SPLIT_MAX_BYTES        (ICAN_SPLIT_MAX_SEGS * 7)

/**
* ican protocol source define
* 0x00 to 0xdf is used as ican io source
* 0xe0 to 0xff is used as ican config source
*
* ican io source include :
* di          do          ai
* ao          serial0    serial1
* user_def    reserve
*
* ican config source include :
* device identify       communicate parameters
* io parameters         io config
*/

/* ican io source define */
#define ICAN_SRC_DI_START           0x00
#define ICAN_SRC_DI_SIZE            0x20

#define ICAN_SRC_DO_START           0x20
#define ICAN_SRC_DO_SIZE            0x20

#define ICAN_SRC_AI_START           0x40
#define ICAN_SRC_AI_SIZE            0x20

#define ICAN_SRC_AO_START           0x60
#define ICAN_SRC_AO_SIZE            0x20

#define ICAN_SRC_SERIAL0_START      0x80
#define ICAN_SRC_SERIAL0_SIZE       0x20

#define ICAN_SRC_SERIAL1_START      0xA0
#define ICAN_SRC_SERIAL1_SIZE       0x20

#define ICAN_SRC_USERDEF_START      0xC0
#define ICAN_SRC_USERDEF_SIZE       0x20

/* ican config source define */
#define ICAN_DEVICE_INFO_START      0xE0
#define ICAN_DEVICE_INFO_SIZE       0x0E

#define ICAN_COMMUNICATE_START      0xEE
#define ICAN_COMMUNICATE_SIZE       0x0A

#define ICAN_IO_PARAM_START         0xF8
#define ICAN_IO_PARAM_SIZE          0x01

#define ICAN_IO_CONFIG_START        0xF9
#define ICAN_IO_CONFIG_SIZE         0x01


/* ican frame define */
typedef struct _ican_id {
    uint32_t
    source_id 	: 8,
    func_id     : 4,
    ack         : 1,    
    src_mac_id  : 8,
    dest_mac_id : 8,
    rev         : 3;
} ican_id;

typedef struct _ican_frame {
    ican_id	id;
    uint8_t	dlc;
    uint8_t	frame_data[8];
} ican_frame;


typedef struct _ican_io_src {
#if ICAN_DI_LEN
    uint8_t di_data[ICAN_DI_NUM][ICAN_DI_LEN];
#endif

#if ICAN_DO_LEN
    uint8_t do_data[ICAN_DO_NUM][ICAN_DO_LEN];
#endif

#if ICAN_AI_LEN
    uint8_t ai_data[ICAN_AI_NUM][ICAN_AI_LEN];
#endif

#if ICAN_AO_LEN
    uint8_t ao_data[ICAN_AO_NUM][ICAN_AO_LEN];
#endif

#if ICAN_SER0_LEN
    uint8_t serial0_data[ICAN_SER0_NUM][ICAN_SER0_LEN];
#endif

#if ICAN_SER1_LEN
    uint8_t serial1_data[ICAN_SER1_NUM][ICAN_SER1_LEN];
#endif

#if ICAN_USER_LEN
    uint8_t userdef_data[ICAN_USER_NUM][ICAN_USER_LEN];
#endif

    void * arg;
} ican_io_src;


#pragma pack(1)//According to the byte alignment
typedef struct _device_info {
    uint16_t vendor_id;
    uint16_t product_type;
    uint16_t product_code;
    uint16_t hardware_version;
    uint16_t firmware_version;
    uint32_t serial_number;
} ican_dev_info;

typedef struct _communicate_info {
    uint8_t  	dev_mac_id;
    uint8_t  	baud_rate;
    uint32_t    user_baud_rate;
    uint8_t  	cyclic_param;
    uint8_t  	cyclic_master;
    uint8_t  	cos_type;
    uint8_t  	master_mac_id;
} ican_com_info;

typedef struct _io_param {
    uint8_t di_length;
    uint8_t do_length;
    uint8_t ai_length;
    uint8_t ao_length;
    uint8_t serial0_length;
    uint8_t serial1_length;
    uint8_t userdef_length;
} ican_io_param;
#pragma pack()


typedef struct _is_file_operations {    
    ierr_t (*is_rd_di)(uint8_t src_id, uint8_t length, uint8_t* buff);    
    ierr_t (*is_rd_ai)(uint8_t src_id, uint8_t length, uint8_t* buff);    
    ierr_t (*is_rd_serial0)(uint8_t src_id, uint8_t length, uint8_t* buf);    
    ierr_t (*is_rd_serial1)(uint8_t src_id, uint8_t length, uint8_t* buf);  
    ierr_t (*is_rd_userdef)(uint8_t src_id, uint8_t length, uint8_t* buf); 
    ierr_t (*is_rd_devinfo)(uint8_t src_id, uint8_t length, uint8_t* buf);    
    ierr_t (*is_rd_cominfo)(uint8_t src_id, uint8_t length, uint8_t* buf);
    ierr_t (*is_rd_ioparam)(uint8_t src_id, uint8_t subsrc_id, uint8_t length, uint8_t* buff);    
    ierr_t (*is_rd_ioconfig)(uint8_t src_id, uint8_t subsrc_id, uint8_t length, uint8_t* buff);  
    ierr_t (*is_wr_do)(uint8_t src_id, uint16_t length, uint8_t* buf);
    ierr_t (*is_wr_ao)(uint8_t src_id, uint16_t length, uint8_t* buf);   
    ierr_t (*is_wr_serial0)(uint8_t src_id, uint8_t length, uint8_t* buf);   
    ierr_t (*is_wr_serial1)(uint8_t src_id, uint8_t length, uint8_t* buf);    
    ierr_t (*is_wr_userdef)(uint8_t src_id, uint8_t length, uint8_t* buf);   
    ierr_t (*is_wr_ioparam)(uint8_t src_id, uint8_t subsrc_id, uint16_t length, uint8_t* buf);    
    ierr_t (*is_wr_ioconfig)(uint8_t src_id, uint8_t subsrc_id, uint16_t length, uint8_t* buf);
    ierr_t (*is_cycle_timeout)(uint8_t* src_id, uint8_t* buff, uint8_t* length);
    ierr_t (*is_check_event)(uint8_t* src_id, uint8_t* buff, uint8_t* length);
} is_file_operations_t;


typedef struct _ican_slave {
    uint8_t dev_status;
    uint8_t dev_channel;
    uint8_t dev_macid;

    uint32_t cycletmr_init_val;  //
    uint32_t linkertmr_init_val; //
    uint32_t eventtmr_init_val;  //

    ican_io_src io_src;
    ican_dev_info dev_info;
    ican_com_info com_info;
    ican_io_param io_param;
    is_file_operations_t *ifops;
} ican_slave_t;


typedef struct _ican_slave_source {
    uint8_t slave_status;
    uint8_t dev_macid;

    uint32_t linkertmr_init_val; //
    uint32_t costmr_init_val;

    ican_dev_info dev_info;
    ican_com_info com_info;
    ican_io_param io_param;
} ican_slave_source;


typedef struct _ican_master {
    uint8_t dev_channel;
    uint8_t dev_macid;
    uint8_t dev_status;

    ican_dev_info dev_info;
    ican_com_info com_info;
    ican_slave_source slave_dev_list[ICAN_SLAVE_NUM];
} ican_master_t;

#endif

