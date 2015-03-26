/**
* @file ican_slave.c
*
* ican slave
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <string.h>
#include <stdio.h>

#include "../include/ican_type.h"
#include "../lib/ican_timer.h"
#include "../lib/ican_stdio.h"
#include "ican.h"
#include "ican_slave.h"


static ican_slave_t islavedev;
static ican_frame ssendframes[ICAN_SPLIT_MAX_SEGS];
static ican_frame srecvframes[ICAN_SPLIT_MAX_SEGS];
static const uint8_t macid_table[ICAN_SLAVE_NUM] = ICAN_SLAVE_MACID;



static ierr_t __is_rd_di(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_DI_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_DI_START;

    if (pio_param->di_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_DI_NUM && length > pio_param->di_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        if ((fops == NULL) || (fops->is_rd_di == NULL)) {
            memcpy(buff, &pio_src->di_data[offset][0], length);
        }
        else {           
            fops->is_rd_di(src_id, length, buff);
        }
    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_ai(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_AI_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_AI_START;

    if (pio_param->ai_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_AI_NUM && length > pio_param->ai_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        if ((fops == NULL) || (fops->is_rd_ai == NULL)) {
            memcpy(buff, &pio_src->ai_data[offset][0], length);
        }
        else {           
            fops->is_rd_ai(src_id, length, buff);
        }
    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_do(uint8_t src_id, uint16_t length, uint8_t* buff)
{
#if (ICAN_DO_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_DO_START;

    if (pio_param->do_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_DO_NUM && length > pio_param->do_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        if ((fops == NULL) || (fops->is_wr_do == NULL)) {
            memcpy(buff, &pio_src->do_data[offset][0], length);
        }
        else {           
            fops->is_rd_do(src_id, length, buff);
        }
    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_ao(uint8_t src_id, uint16_t length, uint8_t* buff)
{
#if (ICAN_AO_LEN > 0)
     ican_io_src* pio_src;
     ican_io_param* pio_param;
     ican_slave_t* pisdev;
     is_file_operations_t* fops;
     uint8_t offset = 0;
     
     pisdev = &islavedev;
     pio_src = &pisdev->io_src;
     pio_param = &pisdev->io_param;
     fops = pisdev->ifops;
     offset = src_id - ICAN_SRC_AO_START;
     
     if (pio_param->ao_length == 0) {
         return ICAN_ERR_LEN_ZERO;
     }
     
     if (offset > ICAN_AO_NUM && length > pio_param->ao_length) {
         return ICAN_ERR_PARAM;
     }
     else {
         if ((fops == NULL) || (fops->is_wr_ao == NULL)) {
             memcpy(buff, &pio_src->ao_data[offset][0], length);
         }
         else {           
             fops->is_rd_ao(src_id, length, buff);
         }
     }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_serail0(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_SER0_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_SERIAL0_START;

    if (pio_param->serial0_length == 0)
        return ICAN_ERR_LEN_ZERO;

    if (offset > ICAN_SER0_NUM && length > pio_param->serial0_length)
        return ICAN_ERR_PARAM;
    else {
        if ((fops == NULL) || (fops->is_rd_serial0 == NULL)) {
            memcpy(buff, &pio_src->serial0_data[offset][0], length);
        }
        else {           
            fops->is_rd_serial0(src_id, length, buff);
        }

    }
    
    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_serail1(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_SER1_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t * pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_SERIAL1_START;

    if (pio_param->serial1_length == 0)
        return ICAN_ERR_LEN_ZERO;

    if (offset > ICAN_SER1_NUM && length >  pio_param->serial1_length)
        return ICAN_ERR_PARAM;
    else {
        if ((fops == NULL) || (fops->__is_rd_serail1== NULL)) {
            memcpy(buff, &pio_src->serial1_data[offset][0], length);
        }
        else {           
            fops->is_rd_serial1(src_id, length, buff);
        }

    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_userdef(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_USER_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_USERDEF_START;

    if (pio_param->userdef_length== 0)
        return ICAN_ERR_LEN_ZERO;

    if (offset > ICAN_USER_NUM && length > pio_param->userdef_length)
        return ICAN_ERR_PARAM;
    else {
        if ((fops == NULL) || (fops->is_rd_userdef == NULL)) {
            memcpy(buff, &pio_src->userdef_data[offset][0], length);
        }
        else {           
            fops->is_rd_userdef(src_id, length, buff);
        }

    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_rd_devinfo(uint8_t src_id, uint8_t length, uint8_t* buff)
{
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;
    uint8_t dev_info[sizeof(ican_dev_info)] = {0,};

    pisdev = &islavedev;
    fops = pisdev->ifops;
    offset = src_id - ICAN_DEVICE_INFO_START;

    if (offset > ICAN_DEVICE_INFO_SIZE
        || length > sizeof(ican_dev_info) - offset) {
        return ICAN_ERR_PARAM;
    }

    if (!((fops == NULL) || (fops->is_rd_devinfo == NULL))) {         
        fops->is_rd_devinfo(src_id, length, buff);
    }
    else {
        memcpy(dev_info, &pisdev->dev_info, sizeof(ican_dev_info));
        memcpy(buff, &dev_info[offset], length);
    }
    
    return ICAN_OK;
}


static ierr_t __is_rd_cominfo(uint8_t src_id, uint8_t length, uint8_t* buff)
{
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;
    uint8_t com_info[sizeof(ican_com_info)] = {0, };

    pisdev = &islavedev;
    fops = pisdev->ifops;
    offset = src_id - ICAN_COMMUNICATE_START;

    if (offset > ICAN_COMMUNICATE_SIZE
        || length > sizeof(ican_com_info) - offset) {
        return ICAN_ERR_PARAM;
    }


    if (!((fops == NULL) || (fops->is_rd_cominfo == NULL))) {         
        fops->is_rd_cominfo(src_id, length, buff);
    }
    else {
        memcpy(com_info, &pisdev->com_info, sizeof(ican_com_info));
        memcpy(buff, &com_info[offset], length);
    }

    return ICAN_OK;
}


static ierr_t __is_rd_ioparam(uint8_t src_id, uint8_t subsrc_id, uint8_t length, uint8_t* buff)
{
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;
    uint8_t io_param[sizeof(ican_io_param)] = {0, };

    pisdev = &islavedev;
    fops = pisdev->ifops;
    offset = src_id - ICAN_IO_PARAM_START;

    if (offset > ICAN_IO_PARAM_SIZE
        || length > sizeof(ican_io_param) - offset) {
        return ICAN_ERR_PARAM;
    }


    if (!((fops == NULL) || (fops->is_rd_ioparam == NULL))) {         
        fops->is_rd_ioparam(src_id, subsrc_id, length, buff);
    }
    else {
        memcpy(io_param, &pisdev->io_param, sizeof(ican_io_param));
        memcpy(buff, &io_param[offset], length);
    }
    
    return ICAN_OK;
}


static ierr_t __is_rd_ioconfig(uint8_t src_id, uint8_t subsrc_id, uint8_t length, uint8_t* buff)
{
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    fops = pisdev->ifops;
    offset = src_id - ICAN_IO_CONFIG_START;

    if (offset > ICAN_IO_CONFIG_SIZE) {
        return ICAN_ERR_PARAM;
    }

    if (!((fops == NULL) || (fops->is_rd_ioconfig == NULL))) {         
        fops->is_rd_ioconfig(src_id, subsrc_id, length, buff);
    }
    else {
        return ICAN_ERR_READ_SRC;
    }

    return ICAN_OK;
}


static ierr_t __is_wr_do(uint8_t src_id, uint16_t length, uint8_t* buff)
{
#if (ICAN_DO_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_DO_START;

    if (pio_param->do_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_DO_NUM && length > pio_param->do_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        if ((fops == NULL) || (fops->is_wr_do == NULL)) {
            memcpy(&pio_src->do_data[offset][0], buff, length);
        }
        else {           
            fops->is_wr_do(src_id, length, buff);
        }
    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_wr_ao(uint8_t src_id, uint16_t length, uint8_t* buff)
{
#if (ICAN_AO_LEN > 0)
     ican_io_src* pio_src;
     ican_io_param* pio_param;
     ican_slave_t* pisdev;
     is_file_operations_t* fops;
     uint8_t offset = 0;
     
     pisdev = &islavedev;
     pio_src = &pisdev->io_src;
     pio_param = &pisdev->io_param;
     fops = pisdev->ifops;
     offset = src_id - ICAN_SRC_AO_START;
     
     if (pio_param->ao_length == 0) {
         return ICAN_ERR_LEN_ZERO;
     }
     
     if (offset > ICAN_AO_NUM && length > pio_param->ao_length) {
         return ICAN_ERR_PARAM;
     }
     else {
         if ((fops == NULL) || (fops->is_wr_ao == NULL)) {
             memcpy(&pio_src->ao_data[offset][0], buff, length);
         }
         else {           
             fops->is_wr_ao(src_id, length, buff);
         }
     }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_wr_serial0(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_SER0_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_SERIAL0_START;

    if (pio_param->serial0_length == 0)
        return ICAN_ERR_LEN_ZERO;

    if (offset > ICAN_SER0_NUM && length > pio_param->serial0_length)
        return ICAN_ERR_PARAM;
    else {
        if ((fops == NULL) || (fops->is_wr_serial0 == NULL)) {
            memcpy(&pio_src->serial0_data[offset][0], buff, length);
        }
        else {           
            fops->is_wr_serial0(src_id, length, buff);
        }

    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_wr_serial1(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_SER1_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t * pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_SERIAL1_START;

    if (pio_param->serial1_length == 0)
        return ICAN_ERR_LEN_ZERO;

    if (offset > ICAN_SER1_NUM && length > pio_param->serial1_length)
        return ICAN_ERR_PARAM;
    else {
        if ((fops == NULL) || (fops->__is_wr_serial1== NULL)) {
            memcpy(&pio_src->serial1_data[offset][0], buff, length);
        }
        else {           
            fops->__is_wr_serial1(src_id, length, buff);
        }

    }
    
    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_wr_cominfo(uint8_t src_id, uint8_t length, uint8_t* buff)
{
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;
    uint8_t com_info[sizeof(ican_com_info)] = {0, };

    pisdev = &islavedev;
    fops = pisdev->ifops;
    offset = src_id - ICAN_COMMUNICATE_START;

    if (offset > ICAN_COMMUNICATE_SIZE
        || length > sizeof(ican_com_info) - offset) {
        return ICAN_ERR_PARAM;
    }


    if (!((fops == NULL) || (fops->is_rd_cominfo == NULL))) {         
        fops->is_rd_cominfo(src_id, length, buff);
    }
    else {
        memcpy(&com_info[offset], buff, length);
        memcpy(&pisdev->com_info, com_info, sizeof(ican_com_info));       
    }

    return ICAN_OK;
}


static ierr_t __is_wr_userdef(uint8_t src_id, uint8_t length, uint8_t* buff)
{
#if (ICAN_USER_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t * pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;
    uint8_t i;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    fops = pisdev->ifops;
    offset = src_id - ICAN_SRC_USERDEF_START;

    if (pio_param->userdef_length== 0)
        return ICAN_ERR_LEN_ZERO;

    if (offset > ICAN_USER_NUM && length > pio_param->userdef_length)
        return ICAN_ERR_PARAM;
    else {
        if ((fops == NULL) || (fops->is_wr_userdef == NULL)) {
            for (i = 0; i < length; i++) {
                pio_src->serial0_data[offset][i] = buff[i];
            }
        }
        else {           
            fops->is_wr_userdef(src_id, length, buff);
        }

    }

    return ICAN_OK;
#else
    return ICAN_ERR_LEN_ZERO;
#endif
}


static ierr_t __is_wr_ioconfig(uint8_t src_id, uint8_t subsrc_id, uint16_t length, uint8_t* buff)
{
    ican_slave_t* pisdev;
    is_file_operations_t* fops;
    uint8_t offset = 0;

    pisdev = &islavedev;
    fops = pisdev->ifops;
    offset = src_id - ICAN_IO_CONFIG_START;

    if (offset > ICAN_IO_CONFIG_SIZE) {
        return ICAN_ERR_PARAM;
    }

    if (!((fops == NULL) || (fops->is_wr_ioconfig == NULL))) {         
        fops->is_wr_ioconfig(src_id, subsrc_id, length, buff);
    }

    return ICAN_OK;
}


static ierr_t ican_slave_ackmac(uint8_t macid, uint32_t serial_num)
{
    ierr_t ret = ICAN_OK;
    uint32_t tmp = 0;
    ican_id canid;
    uint8_t cmdbuf[5] = {0, };
    uint8_t frames_num = 0;

    ican_frame sendframe[1];
    ican_slave_t* pisdev;

    pisdev = &islavedev;
    
    canid.src_mac_id = pisdev->dev_macid;
    canid.dest_mac_id = macid;
    canid.source_id = 0x00;
    canid.func_id = ICAN_FUC_MAC_CHECK;
    canid.ack = 1;

    tmp = serial_num;
    cmdbuf[0] = macid;
    cmdbuf[1] = tmp & 0xff;
    cmdbuf[2] = (tmp >> 8) & 0xff;
    cmdbuf[3] = (tmp >> 16) & 0xff;
    cmdbuf[4] = (tmp >> 24) & 0xff;
    
    frames_num = if_organize_msgs(sendframe, canid, cmdbuf, sizeof(cmdbuf));
    if (frames_num > ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    ret = ican_send_msg(pisdev->dev_channel, sendframe, frames_num, 100);

    return ret;
}


static ierr_t ican_slave_read(ican_frame* piframe)
{
    uint8_t src_id = 0;
    uint8_t subsrc_id = 0;
    uint16_t length = 0;
    ican_id canid;
    uint8_t buff[ICAN_SPLIT_MAX_BYTES] = {0, };
    ierr_t ret = ICAN_OK;
    uint8_t frames_num;
    ican_slave_t* pisdev;
    
    pisdev = &islavedev;

    src_id = if_get_source_id(piframe);    
    length = if_get_data_len(piframe);

    if (length > ICAN_SPLIT_MAX_BYTES) {
        return ICAN_ERR_PARAM;
    }

    memset(buff, 0, length);

    if (src_id >= ICAN_IO_CONFIG_START
        && src_id <= ICAN_IO_CONFIG_START + ICAN_IO_CONFIG_SIZE) {
        subsrc_id = if_get_subsrc_id(piframe);
        ret = __is_rd_ioconfig(src_id, subsrc_id, length, buff);
    }
    else if (src_id >= ICAN_IO_PARAM_START) {
        subsrc_id = if_get_subsrc_id(piframe);
        ret = __is_rd_ioparam(src_id, subsrc_id, length, buff);
    }
    else if (src_id >= ICAN_COMMUNICATE_START) {
        ret = __is_rd_cominfo(src_id, length, buff);
    }
    else if (src_id >= ICAN_DEVICE_INFO_START) {
        ret = __is_rd_devinfo(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_USERDEF_START) {
        ret = __is_rd_userdef(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_SERIAL1_START) {
        ret = __is_rd_serail1(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_SERIAL0_START) {
        ret = __is_rd_serail0(src_id, length, buff);
    }
    else if (src_id >=ICAN_SRC_AO_START) {
        ret = __is_rd_ao(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_AI_START) {
        ret = __is_rd_ai(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_DO_START) {
        ret = __is_rd_do(src_id, length, buff);
    }
    else if (src_id < ICAN_SRC_DO_START) {
        ret = __is_rd_di(src_id, length, buff);
    }
    else {
        ican_printf("the source %d can't be read\n\r", src_id);
        ret = ICAN_ERR_READ_SRC;
    }

    if (ret == ICAN_OK) {
        canid.dest_mac_id = piframe->id.src_mac_id;
        canid.src_mac_id = piframe->id.dest_mac_id;
        canid.func_id = piframe->id.func_id;
        canid.source_id = piframe->id.source_id;
        canid.ack = 1;
        frames_num = if_organize_msgs(ssendframes, canid, buff, length);
        if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
            return ICAN_ERR_SPLIT_FLOW;
        }
        
        ret = ican_send_msg(pisdev->dev_channel, ssendframes, frames_num, 200);
    }

    return ret;
}


static ierr_t ican_slave_write(ican_frame* piframe, const uint8_t frames_num)
{
    uint8_t src_id = 0;
    uint8_t subsrc_id = 0;
    uint16_t length = 0;
    ican_id canid;
    uint8_t buff[ICAN_SPLIT_MAX_BYTES] = {0, };
    ierr_t ret = ICAN_ERR_MAX_VALUE;
    uint8_t ifs_num = 0;

    ican_slave_t* pisdev;
    
    pisdev = &islavedev;   

    if (!if_check_multisegs(piframe, frames_num)) {
        return ICAN_ERR_SPLIT_TRANS;
    }    

    memset(buff, 0, length);
    src_id = if_get_source_id(piframe);
    subsrc_id = if_get_subsrc_id(piframe);
    length = if_copy_data(piframe, frames_num, buff);

    if (length > ICAN_SPLIT_MAX_BYTES) {
        return ICAN_ERR_PARAM;
    }    

    if (src_id >= ICAN_IO_CONFIG_START
        && src_id <= ICAN_IO_CONFIG_START + ICAN_IO_CONFIG_SIZE) {       
        ret = __is_wr_ioconfig(src_id, subsrc_id, length, buff);
    }
    else if (src_id >= ICAN_IO_PARAM_START) {       
        ret = ICAN_ERR_WRITE_SRC;
    }
    else if (src_id >= ICAN_COMMUNICATE_START) {
        ret = __is_wr_cominfo(src_id, length, buff);
    }
    else if (src_id >= ICAN_DEVICE_INFO_START) {
        ret = ICAN_ERR_WRITE_SRC;
    }
    else if (src_id >= ICAN_SRC_USERDEF_START) {
        ret = __is_wr_userdef(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_SERIAL1_START) {
        ret = __is_wr_serial1(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_SERIAL0_START) {
        ret = __is_wr_serial0(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_AO_START && src_id < ICAN_SRC_SERIAL0_START) {       
        ret = __is_wr_ao(src_id, length, buff);
    }
    else if (src_id >= ICAN_SRC_DO_START && src_id < ICAN_SRC_AI_START) {        
        ret = __is_wr_do(src_id, length, buff);
    }
    else {
        ican_printf("the source %d can't be write\n\r", src_id);
        ret = ICAN_ERR_WRITE_SRC;
    }

    if (ret == ICAN_OK) {
        canid.dest_mac_id = piframe->id.src_mac_id;
        canid.src_mac_id = piframe->id.dest_mac_id;
        canid.func_id = piframe->id.func_id;
        canid.source_id = piframe->id.source_id;
        canid.ack = 1;
        ifs_num = if_organize_msgs(ssendframes, canid, NULL, 0);
        if (ifs_num >= ICAN_ERR_SPLIT_FLOW) {
            return ICAN_ERR_SPLIT_FLOW;
        }
        
        ret = ican_send_msg(pisdev->dev_channel, ssendframes, ifs_num, SEND_TIME_OUT);
    }

    return ret;
}


static ierr_t ican_slave_connect(ican_frame* piframe)
{
    uint8_t src_id = 0;
    uint8_t old_master_id = 0;
    uint8_t new_master_id = 0;
    uint8_t cyclicmaster = 0;
    uint8_t frames_num;
    uint8_t databuf[4] = {0, };
    ican_id canid;
    ican_slave_t* pisdev;

    pisdev = &islavedev;
    old_master_id = pisdev->com_info.master_mac_id;
    src_id = piframe->id.source_id;
    new_master_id = piframe->frame_data[1];
    cyclicmaster = piframe->frame_data[2];

    if (src_id == 0xf7) {
        if ((old_master_id != 0xff)
            && (old_master_id != new_master_id)) {
            return ICAN_ERR_NODE_BUSY;
        }
        else {
            pisdev->com_info.master_mac_id = new_master_id;
            pisdev->com_info.cyclic_master = cyclicmaster;
            pisdev->dev_status = ICAN_IN_CONNECT;

            /* start timer checking */
            //...
            pisdev->linkertmr_init_val = ican_tmr_ms_get();
            pisdev->cycletmr_init_val = ican_tmr_ms_get();
            pisdev->eventtmr_init_val = ican_tmr_ms_get();

            /* send response */
            canid.dest_mac_id = piframe->id.src_mac_id;
            canid.src_mac_id = piframe->id.dest_mac_id;
            canid.func_id = piframe->id.func_id;
            canid.source_id = piframe->id.source_id;
            canid.ack = 1;

            databuf[0] = pisdev->io_param.ai_length;
            databuf[1] = pisdev->io_param.ao_length;
            databuf[2] = pisdev->io_param.di_length;
            databuf[3] = pisdev->io_param.do_length;
            
            frames_num = if_organize_msgs(ssendframes, canid, databuf, 4);
            if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
                return ICAN_ERR_SPLIT_FLOW;
            }
        
            return ican_send_msg(pisdev->dev_channel, ssendframes, frames_num, SEND_TIME_OUT);
        }
    }
    else {
        return ICAN_ERR_SOURCE_ID;
    }
}


static ierr_t ican_slave_disconnect(ican_frame* piframe)
{
    uint8_t src_id = 0;
    uint8_t old_master_id = 0;
    uint8_t frames_num;
    ican_id canid;
    ican_slave_t* isdev;

    isdev = &islavedev;

    src_id = piframe->id.source_id;
    old_master_id = isdev->com_info.master_mac_id;

    if (src_id == 0xf7) {
        if(old_master_id != 0xff) {
            if (old_master_id != piframe->frame_data[1]) {
                return ICAN_ERR_DEL_CONNECT;
            }
            else {
                isdev->com_info.cyclic_master = 0;
                isdev->dev_status = ICAN_IN_DISCONNECT;

                /* send response */
                canid.dest_mac_id = piframe->id.src_mac_id;
                canid.src_mac_id = piframe->id.dest_mac_id;
                canid.func_id = piframe->id.func_id;
                canid.source_id = piframe->id.source_id;
                canid.ack = 1;
                frames_num = if_organize_msgs(ssendframes, canid, NULL, 0);
                if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
                    return ICAN_ERR_SPLIT_FLOW;
                }
        
                return ican_send_msg(isdev->dev_channel, ssendframes, frames_num, SEND_TIME_OUT);
             }
        }
        else {
            return ICAN_ERR_CONNECT;
        }
    }
    else {
        return ICAN_ERR_SOURCE_ID;
    }
}


static ierr_t ican_slave_reset(ican_frame* piframe)
{
    uint8_t frames_num;
    ican_id canid;
    ican_slave_t* pisdev;

    pisdev = &islavedev;

    /* send response */
    canid.dest_mac_id = piframe->id.src_mac_id;
    canid.src_mac_id = piframe->id.dest_mac_id;
    canid.func_id = piframe->id.func_id;
    canid.source_id = piframe->id.source_id;
    canid.ack = 1;
    frames_num = if_organize_msgs(ssendframes, canid, NULL, 0);
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }
    
    return ican_send_msg(pisdev->dev_channel, ssendframes, frames_num, SEND_TIME_OUT);
}


ierr_t ican_slave_heartcheck(ican_frame* piframe)
{
    ierr_t ret;
    uint8_t ifs_num;
    ican_id canid;
    ican_slave_t* pisdev = &islavedev;

    ican_printf("recv one heart frame\n\r");
       
    pisdev->com_info.cyclic_master = piframe->frame_data[1]; //linker timer thresh

    canid.dest_mac_id = piframe->id.src_mac_id;
    canid.src_mac_id =  piframe->id.dest_mac_id;
    canid.func_id = ICAN_FUC_HEART_CHECK;
    canid.source_id = 0xf4;
    canid.ack = 1;
    ifs_num = if_organize_msgs(ssendframes, canid, NULL, 0);
    if (ifs_num >= ICAN_ERR_SPLIT_FLOW) {
            return ICAN_ERR_SPLIT_FLOW;
    }
        
    ret = ican_send_msg(pisdev->dev_channel, ssendframes, ifs_num, SEND_TIME_OUT);
   
    return ret;
}


static ierr_t ican_slave_eventcheck(ican_frame* piframe)
{
    static uint8_t first = 1;    
    ierr_t ret;
    ican_id canid;
    uint8_t ifs_num = 0;
    uint8_t src_id;
    uint8_t len = 0;
    uint8_t data[ICAN_SPLIT_MAX_BYTES] = {0,};
    uint32_t cyctmr_threshold;
    uint32_t evetmr_threshold;
    ican_slave_t* pisdev = &islavedev;

    cyctmr_threshold = (uint32_t)pisdev->com_info.cyclic_param * CYCLIC_UNIT_MS;
    evetmr_threshold = COS_TIME_OUT;//ms

    canid.dest_mac_id = pisdev->com_info.master_mac_id;
    canid.src_mac_id =  pisdev->com_info.dev_mac_id;
    canid.func_id = ICAN_FUC_EVE_TRIGER;
    canid.ack = 1;

    /* check event happens */
    if (first == 1 && pisdev->com_info.cos_type == 1
        && pisdev->ifops->is_check_event != NULL
        && ican_tmr_ms_delta(pisdev->eventtmr_init_val) > evetmr_threshold) {
        pisdev->eventtmr_init_val = ican_tmr_ms_get();// reset eve timer
        ret = pisdev->ifops->is_check_event(&src_id, data, &len);
        if (ret == ICAN_OK) {
            canid.source_id = src_id;            
            ifs_num = if_organize_msgs(ssendframes, canid, data, len);
            if (ifs_num >= ICAN_ERR_SPLIT_FLOW) {
               ican_printf("event has too much data...\n\r");
               ret = ICAN_ERR_SPLIT_FLOW;
            }
            else {
                ret = ican_send_msg(pisdev->dev_channel, ssendframes,
                                    ifs_num, SEND_TIME_OUT);
            }
        }

        /* event and cycle can't send in the same time */
        if (ret == ICAN_OK) {
            first = 2;
            ican_printf("send one event\n\r");
            return ICAN_OK;
        }
    }

    first = 1;
    /* check cycle timer out */
    if (cyctmr_threshold > 0
        && ican_tmr_ms_delta(pisdev->cycletmr_init_val) > cyctmr_threshold) {      
        pisdev->cycletmr_init_val = ican_tmr_ms_get();// reset cycle timer
        
        if (pisdev->ifops != NULL
            && pisdev->ifops->is_cycle_timeout != NULL) {
            ret = pisdev->ifops->is_cycle_timeout(&src_id, data, &len);
            if (ret != ICAN_OK ) {
                return ret;
            }
            canid.source_id = src_id;
            ifs_num = if_organize_msgs(ssendframes, canid, data, len);
            if (ifs_num >= ICAN_ERR_SPLIT_FLOW) {
                ican_printf("cycle has too much data...\n\r");
                return ICAN_ERR_SPLIT_FLOW;
            }
            else {
                ret = ican_send_msg(pisdev->dev_channel, ssendframes, ifs_num, SEND_TIME_OUT);
                if (ret == ICAN_OK) {
                    ican_printf("send one cycle\n\r");                    
                }
            }
            return ret;
        }        
    }

    canid.source_id = 0xf6;
    ifs_num = if_organize_msgs(ssendframes, canid, NULL, 0);
    if (ifs_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }
    else {
        ret = ican_send_msg(pisdev->dev_channel, ssendframes, ifs_num, SEND_TIME_OUT);
        if (ret == ICAN_OK) {
            ican_printf("send no event ack\n\r");
        }
    }

    return ICAN_OK;
}


static ierr_t ican_poll_parse(ican_frame* piframes, uint8_t recv_num)
{
    ierr_t ret = ICAN_OK;
    ican_slave_t* pisdev = &islavedev;

    if (recv_num == 1) {
        /* macid response */
        if (piframes[0].id.func_id == ICAN_FUC_MAC_CHECK) {
                return ican_slave_ackmac(piframes[0].id.src_mac_id, SERIAL_NUMBER);
        }

        /* build connect */        
        if (pisdev->dev_status != ICAN_IN_CONNECT) {
            if (piframes[0].id.func_id == ICAN_FUC_EST_CONNECT) {
                return ican_slave_connect(piframes);
            }
            else {
                return ICAN_ERR_CONNECT;
            }
        }
    }

    if (pisdev->dev_status != ICAN_IN_CONNECT) {
        return ICAN_ERR_CONNECT;
    }

    if (recv_num > 1) {
        switch(piframes[0].id.func_id) {
        case ICAN_FUC_WRITE :
            ret = ican_slave_write(piframes, recv_num);
            break;

        default :
            ret = ICAN_ERR_SPLIT_TRANS;
            break;
        }

        return ret;
    }
    
    switch(piframes[0].id.func_id) {
    case ICAN_FUC_WRITE :
        ret = ican_slave_write(piframes, recv_num);
        break;

    case ICAN_FUC_READ :
        ret = ican_slave_read(&piframes[0]);
        break;

    case ICAN_FUC_EST_CONNECT :
        ret = ican_slave_connect(&piframes[0]);
        break;

    case ICAN_FUC_DEL_CONNECT :
        ret = ican_slave_disconnect(&piframes[0]);
        break;

    case ICAN_FUC_DEV_RESET :
        ret = ican_slave_reset(&piframes[0]);
        break;

    case ICAN_FUC_HEART_CHECK :
        ret = ican_slave_heartcheck(&piframes[0]);
        break;

    case ICAN_FUC_EVENT_CHECK :
        ret = ican_slave_eventcheck(NULL);
        break;

    default :
        ret = ICAN_ERR_FUNC_ID;
        break;
    }

    return ret;
}


static void ican_poll_errresp(uint8_t channel, uint8_t func_id, uint8_t src_id, uint8_t err)
{
    ierr_t ret = ICAN_OK;
    ican_id canid;
    uint8_t databuf[2] = {0, };
    uint8_t frames_num = 0;
    ican_slave_t* pisdev;    

    if (err == ICAN_OK) {
        return;
    }

    pisdev = &islavedev;
    canid.src_mac_id = pisdev->dev_macid;
    canid.dest_mac_id = pisdev->com_info.master_mac_id;
    canid.source_id = src_id;
    canid.func_id = 0x0f;
    canid.ack = 1;
    databuf[0] = err;
    
    frames_num = if_organize_msgs(srecvframes, canid, databuf, sizeof(databuf));
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return;
    }    
    
    switch(func_id) {
    case ICAN_FUC_READ:
    case ICAN_FUC_WRITE:
    case ICAN_FUC_EST_CONNECT:
    case ICAN_FUC_DEL_CONNECT:
    case ICAN_FUC_DEV_RESET:
        ret = ican_send_msg(channel, srecvframes, frames_num, SEND_TIME_OUT);
        break;

    case ICAN_FUC_EVE_TRIGER :
    case ICAN_FUC_HEART_CHECK :
    case ICAN_FUC_EVENT_CHECK :
        ret = ICAN_OK;
        break;

    default:
        ret = ican_send_msg(channel, srecvframes, frames_num, SEND_TIME_OUT);
    }

    if (ret != ICAN_OK) {
        ican_printf("Err response failed: %s\n\r", ican_error_parse(ret));
    }
}


/**
* @defgroup Slave_Core_API ican_slave.c
* @{
*/

/**
* \brief poll device event; check bus except and recover bus when exception
*        is happend; maintenance heart beating.
* \param channel can controler id
* \return see ican error code explain in ican_error.h
*/
ierr_t ican_slave_poll(uint8_t channel)
{
    ierr_t ret = ICAN_OK;
    uint8_t state = 0;
    uint8_t recv_num = 0;
    uint32_t linktmr_threshold;
    ican_slave_t* pisdev = &islavedev;

    /* check bus */
    if (ican_can_is_except(pisdev->dev_channel)) {
        ican_printf("can bus except happend, now restart it\n\r");
        pisdev->dev_status = ICAN_IN_DISCONNECT;
        ican_can_off(pisdev->dev_channel);
        ican_tmr_ms_delay(100);

        if (ican_can_init(pisdev->dev_channel, pisdev->dev_macid,
            pisdev->com_info.baud_rate)) {
            return ICAN_ERR_BUS_RESTART;
        }
        else {
            return ICAN_ERR_BUS_EXCEPT;
        }
    }
    
    linktmr_threshold = (uint32_t)pisdev->com_info.cyclic_master;
    
    state = ICAN_RECV_STATUS;
    while (1) {
        if (pisdev->dev_status == ICAN_IN_CONNECT) {                
            /* check linker timer out*/       
            if (linktmr_threshold > 0
                && (ican_tmr_ms_delta(pisdev->linkertmr_init_val)
                > (linktmr_threshold  + 1) * CYCLIC_UNIT_MS)) {
                pisdev->com_info.cyclic_master = 0;
                pisdev->dev_status = ICAN_IN_DISCONNECT;
                ican_printf("linker timer out...\n\r");
            }
        }
         
        switch (state) {
        case ICAN_RECV_STATUS :
            ret = ican_recv_msg(channel, islavedev.com_info.master_mac_id,
                                srecvframes, &recv_num, POLL_TIME_OUT);
            if (ret != ICAN_OK) { 
                return ret;
            }

            /* reset linker timer */
            if (pisdev->dev_status == ICAN_IN_CONNECT && linktmr_threshold > 0) {
                pisdev->linkertmr_init_val = ican_tmr_ms_get();
            }
            
            state = ICAN_PARSE_STATUS;
            break;

        case ICAN_PARSE_STATUS :
            ret = ican_poll_parse(srecvframes, recv_num);
            if (ret != ICAN_OK) {
                state = ICAN_ERROR_STATUS;
            }
            else {                
                return ICAN_OK;
            }
            break;

        case ICAN_ERROR_STATUS :
            ican_poll_errresp(channel, srecvframes[0].id.func_id,
                                 srecvframes[0].id.source_id, ret);
            return ICAN_OK;
        }
    }
}

/**
* Core User API
*/

/**
* \brief init the first 'channel' can controler as slave
* \param channel can controler id
* \param devMacId device MACID
* \return true is OK, false is failed
* \note devMacId setted by calling ican_slave_get_macid()
*/
bool ican_slave_init(uint8_t channel, uint8_t devMacId)
{
    ican_slave_t* pisdev;

    if (!ican_can_init(channel, devMacId, BAUD_RATE)) {
        return false;
    }

    pisdev = &islavedev;    
    pisdev->dev_status                = ICAN_IN_DISCONNECT;
    pisdev->dev_macid                 = devMacId;
    pisdev->dev_channel               = channel;
    
    pisdev->dev_info.vendor_id        = VENDOR_ID;
    pisdev->dev_info.product_type     = PRODUCT_TYPE;
    pisdev->dev_info.product_code     = PRODUCT_CODE;
    pisdev->dev_info.hardware_version = HARDWARE_VERSION;
    pisdev->dev_info.firmware_version = FIRMWARE_VERSION;
    pisdev->dev_info.serial_number    = SERIAL_NUMBER;

    pisdev->com_info.dev_mac_id       = devMacId;
    pisdev->com_info.baud_rate        = BAUD_RATE;
    pisdev->com_info.user_baud_rate   = USER_BAUD_RATE;
    pisdev->com_info.cyclic_param     = CYCLIC_PARAM;
    pisdev->com_info.cyclic_master    = CYCLIC_MASTER;
    pisdev->com_info.cos_type         = COS_TYPE;
    pisdev->com_info.master_mac_id    = 0xFF;

    pisdev->io_param.di_length        = ICAN_DI_LEN;
    pisdev->io_param.do_length        = ICAN_DO_LEN;
    pisdev->io_param.ai_length        = ICAN_AI_LEN;
    pisdev->io_param.ao_length        = ICAN_AO_LEN;
    pisdev->io_param.serial0_length   = ICAN_SER0_LEN;
    pisdev->io_param.serial1_length   = ICAN_SER1_LEN;
    pisdev->io_param.userdef_length   = ICAN_USER_LEN;

    memset(&pisdev->io_src, 0, sizeof(ican_io_src));

    ican_slave_init_fops();

    return true;
}


/**
* \brief set slave source func operations
* \param ifops user define source func operations 
* \see ::is_file_operations_t in ican_type.h
*/
void ican_slave_set_fops(is_file_operations_t* ifops)
{
    ican_slave_t* pisdev = &islavedev;

    pisdev->ifops = ifops;
}


/**
* \brief init slave source func operations
* \see ::is_file_operations_t in ican_type.h
*/

void ican_slave_init_fops(void)
{
    ican_slave_t* pisdev = &islavedev;

    pisdev->ifops = NULL;
}

/**
* \biref get valid macid
* \note index must less than ICAN_SLAVE_NUM
*/
uint8_t ican_slave_get_macid(uint8_t index)
{
    if (index >= ICAN_SLAVE_NUM) {
        return 0xff;
    }

    return macid_table[index];
}
/** @}*/


/**
* @defgroup Slave_Extend_API ican_slave.c
* @{
*/

/**
* \brief get slave all io source data
* \param io_src all io source 
* \see ::ican_io_src in ican_type.h
*/
void is_get_iosrc(ican_io_src* iosrc)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(iosrc, &pisdev->io_src, sizeof(ican_io_src));
}


/**
* \brief get device information data
* \param devinfo device information
* \see ::ican_dev_info in ican_type.h
*/
void is_get_devinfo(ican_dev_info* devinfo)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(devinfo, &pisdev->dev_info, sizeof(ican_dev_info));
}


/**
* \brief get communication information data
* \param cominfo communication information
* \see ::ican_com_info in ican_type.h
*/
void is_get_cominfo(ican_com_info* cominfo)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(cominfo, &pisdev->com_info, sizeof(ican_com_info));
}


/**
* \brief get io parameters data
* \param ioparam communication information
* \see ::ican_com_info in ican_type.h
*/
void is_get_ioparm(ican_io_param* ioparam)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(ioparam, &pisdev->io_param, sizeof(ican_io_param));
}

/***
void is_set_iosrc(const ican_io_src* iosrc)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(&pisdev->io_src, iosrc, sizeof(ican_io_src));
}
***/


/**
* \brief set device information data
* \param devinfo device information
* \see ::ican_dev_info in ican_type.h
*/
void is_set_devinfo(const ican_dev_info* devinfo)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(&pisdev->dev_info, devinfo, sizeof(ican_dev_info));
}


/**
* \brief set communication information data
* \param cominfo communication information
* \see ::ican_com_info in ican_type.h
*/
void is_set_cominfo(const ican_com_info* cominfo)
{
    ican_slave_t* pisdev = &islavedev;

    memcpy(&pisdev->com_info, cominfo, sizeof(ican_com_info));
}


/**
* \brief simulate write di to device io_src di buff
* \param src_id source id
* \param buff user data buff pointer
* \len user data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_write_di(uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_DI_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    offset = src_id - ICAN_SRC_DI_START;

    if (pio_param->di_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_DI_NUM && len > pio_param->di_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        memcpy(&pio_src->di_data[offset][0], buff, len);
    }
#endif

    return ICAN_OK;
}


/**
* \brief simulate read do data from device io_src do buff
* \param src_id source id
* \param buff user data buff pointer
* \len user want read data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_read_do(uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_DO_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    offset = src_id - ICAN_SRC_DO_START;

    if (pio_param->do_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_DO_NUM && len > pio_param->do_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        memcpy(buff, &pio_src->do_data[offset][0], len);
    }
#endif

    return ICAN_OK;
}


/**
* \brief simulate write ai to device io_src ai buff
* \param src_id source id
* \param buff user data buff pointer
* \len user data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_write_ai(uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_AI_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    offset = src_id - ICAN_SRC_AI_START;

    if (pio_param->ai_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_AI_NUM && len > pio_param->ai_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        memcpy(&pio_src->ai_data[offset][0], buff, len);
    }
#endif

    return ICAN_OK;
}


/**
* \brief simulate read ao data from device io_src ao buff
* \param src_id source id
* \param buff user data buff pointer
* \len user want read data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_read_ao(uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_AO_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    offset = src_id - ICAN_SRC_AO_START;

    if (pio_param->ao_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_AO_NUM && len > pio_param->ao_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        memcpy(buff, &pio_src->ao_data[offset][0], len);
    }
#endif

    return ICAN_OK;
}


/**
* \brief simulate write serial data to device io_src serial buff
* \param src_id source id
* \param serial_num the first 'serial_num' serial
* \param buff user data buff pointer
* \len user data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_write_serail(uint8_t serial_num, uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_SER0_LEN > 0 || ICAN_SER1_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    
    uint8_t offset = 0;
    uint8_t serial_src_start;
    uint8_t serial_src_num;
    uint8_t serial_src_len;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    
    if (serial_num == 0) {
        serial_src_start =  ICAN_SRC_SERIAL0_START;
        serial_src_num = ICAN_SER0_NUM;
        serial_src_len = pio_param->serial0_length;
    }
    else {
        serial_src_start =  ICAN_SRC_SERIAL1_START;
        serial_src_num = ICAN_SER1_NUM;
        serial_src_len = pio_param->serial1_length;
    }

    if (serial_src_len == 0) {
        return ICAN_ERR_LEN_ZERO;
    }
    
    offset = src_id - serial_src_start;
    if (offset > serial_src_num && len > serial_src_len) {
        return ICAN_ERR_PARAM;
    }
    else {
        if (serial_num == 0) {
#if ICAN_SER0_LEN
            memcpy(&pio_src->serial0_data[offset][0], &buff[0], len);
#endif
        }
        else {
#if ICAN_SER1_LEN
            memcpy(&pio_src->serial1_data[offset][0], &buff[0], len);
#endif
        }
    }
#endif

    return ICAN_OK;
}

/**
* \brief simulate read serial data from device io_src serial buff
* \param src_id source id
* \param serial_num the first 'serial_num' serial
* \param buff user data buff pointer
* \len user data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_read_serail(uint8_t serial_num, uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_SER0_LEN > 0 || ICAN_SER1_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    
    uint8_t offset = 0;
    uint8_t serial_src_start;
    uint8_t serial_src_num;
    uint8_t serial_src_len;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    
    if (serial_num == 0) {
        serial_src_start =  ICAN_SRC_SERIAL0_START;
        serial_src_num = ICAN_SER0_NUM;
        serial_src_len = pio_param->serial0_length;
    }
    else {
        serial_src_start =  ICAN_SRC_SERIAL1_START;
        serial_src_num = ICAN_SER1_NUM;
        serial_src_len = pio_param->serial1_length;
    }

    if (serial_src_len == 0) {
        return ICAN_ERR_LEN_ZERO;
    }
    
    offset = src_id - serial_src_start;
    if (offset > serial_src_num && len > serial_src_len) {
        return ICAN_ERR_PARAM;
    }
    else {
        if (serial_num == 0) {
#if ICAN_SER0_LEN
            memcpy(&buff[0], &pio_src->serial0_data[offset][0], len);
#endif
        }
        else {
#if ICAN_SER1_LEN
            memcpy(&buff[0], &pio_src->serial1_data[offset][0], len);
#endif
        }
    }
#endif

    return ICAN_OK;
}


/**
* \brief simulate write userdef data to device io_src userdef buff
* \param src_id source id
* \param buff user data buff pointer
* \len user data length
* \return ican error code
* \see ::ican_io_src in ican_type.h*/
ierr_t is_simulate_write_userdef(uint8_t src_id, uint8_t* buff, uint8_t len)
{
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    offset = src_id - ICAN_SRC_USERDEF_START;

    if (pio_param->userdef_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_USER_NUM && len > pio_param->userdef_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        memcpy(&pio_src->userdef_data[offset][0], buff, len);
    }

    return ICAN_OK;
}


/**
* \brief simulate read userdef data from device io_src userdef buff
* \param src_id source id
* \param buff user data buff pointer
* \len user want read data length
* \return ican error code
* \see ::ican_io_src in ican_type.h
*/
ierr_t is_simulate_read_userdef(uint8_t src_id, uint8_t* buff, uint8_t len)
{
#if (ICAN_USER_LEN > 0)
    ican_io_src* pio_src;
    ican_io_param* pio_param;
    ican_slave_t* pisdev;
    uint8_t offset = 0;

    pisdev = &islavedev;
    pio_src = &pisdev->io_src;
    pio_param = &pisdev->io_param;
    offset = src_id - ICAN_SRC_USERDEF_START;

    if (pio_param->userdef_length == 0) {
        return ICAN_ERR_LEN_ZERO;
    }

    if (offset > ICAN_USER_NUM && len > pio_param->userdef_length) {
        return ICAN_ERR_PARAM;
    }
    else {
        memcpy(buff, &pio_src->userdef_data[offset][0], len);
    }
#endif

    return ICAN_OK;
}
/** @}*/

