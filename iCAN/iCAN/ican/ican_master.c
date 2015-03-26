/**
* @file ican_master.c
*
* ican master 
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include <string.h>
#include <stdio.h>

#include "../include/ican_type.h"
#include "../lib/ican_timer.h"
#include "../lib/ican_stdio.h"
#include "ican.h"
#include "ican_master.h"

static ican_master_t imasterdev;
static ican_frame msendframes[ICAN_SPLIT_MAX_SEGS];
static ican_frame mrecvframes[ICAN_SPLIT_MAX_SEGS];
static const uint8_t slave_macid_table[ICAN_SLAVE_NUM] = ICAN_SLAVE_MACID;


static uint8_t smt_get_index(uint8_t slave_macid)
{
    uint8_t i = 0;
    for (;i < sizeof(slave_macid_table); i++) {
        if (slave_macid_table[i] == slave_macid) {
            return i;
        }
    }

    return ICAN_SLAVE_NUM;
}


static void __im_except_parse(const uint8_t src_id, const ierr_t err)
{
    char* errstr = NULL;
    errstr = ican_error_parse(err);
    
    if (src_id >= ICAN_IO_CONFIG_START) {
        ican_printf("operate io config error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_IO_PARAM_START) {
        ican_printf("operate io param error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_COMMUNICATE_START) {
        ican_printf("operate communicate param error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_DEVICE_INFO_START) {
        ican_printf("operate device info error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_SRC_USERDEF_START) {
        ican_printf("operate userdef error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_SRC_SERIAL1_START) {
        ican_printf("operate serial1 error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_SRC_SERIAL0_START) {
        ican_printf("operate serial0 error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_SRC_AO_START) {
        ican_printf("operate AO error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_SRC_AI_START) {
        ican_printf("operate AI error: %s\n\r", errstr);
    }
    else if (src_id >= ICAN_SRC_DO_START) {
        ican_printf("operate DO error: %s\n\r", errstr);
    }
    else {
        ican_printf("operate DI error: %s\n\r", errstr);
    }
}


static ierr_t __im_func_parse(ican_frame ifs[], uint8_t ifs_num, uint8_t* buff)
{
    ierr_t ret = ICAN_OK;
    uint8_t slave_macid = 0;
    uint8_t source_id = 0;
    uint8_t smt_index = 0xff;
    ican_master_t* pimdev;
    ican_slave_source* pisdev_src;
   
    slave_macid = ifs[0].id.src_mac_id;
    source_id = ifs[0].id.source_id;
    smt_index = smt_get_index(slave_macid);
    if (smt_index >= ICAN_SLAVE_NUM) {
        ican_printf("can't find slave in master's slave_macid_table\n\r");
        return ICAN_ERR_MAC_NONEXIST;
    }

    if (ifs[0].id.ack != 1) {
        ican_printf("%s\n\r", ican_error_parse(ICAN_ERR_NOTACK_FRAME));
        return ICAN_ERR_NOTACK_FRAME;
    }

    pimdev = &imasterdev;
    pisdev_src = &(pimdev->slave_dev_list[smt_index]);
    
    switch(ifs[0].id.func_id) {
    case ICAN_FUC_READ :
        if (buff != NULL && if_check_multisegs(ifs, ifs_num)) {
            if_copy_data(ifs, ifs_num, buff);
            ret = ICAN_OK;
        }
        else {
            ret = ICAN_ERR_SPLIT_TRANS;
        }
        break;
        
    case ICAN_FUC_WRITE :
        if (ifs[0].dlc == 1) {
            ret = ICAN_OK;
        }
        else {
            ret = ICAN_ERR_WRITE_SRC;
        }
        break;

    case ICAN_FUC_EST_CONNECT :
        if (ifs[0].dlc == 5 && ifs[0].id.source_id == 0xF7) {
            pisdev_src->slave_status = ICAN_IN_CONNECT;
            pisdev_src->io_param.ai_length = ifs[0].frame_data[1];
            pisdev_src->io_param.ao_length = ifs[0].frame_data[2];
            pisdev_src->io_param.di_length = ifs[0].frame_data[3];
            pisdev_src->io_param.do_length = ifs[0].frame_data[4];

            /* start linker timer */
            if (pisdev_src->com_info.cyclic_master > 0) {
                pisdev_src->linkertmr_init_val = ican_tmr_ms_get();
                pisdev_src->costmr_init_val = ican_tmr_ms_get();
            }
            
            ret = ICAN_OK;
        }
        else {
            ret = ICAN_ERR_CONNECT;
        }
        break;

    case ICAN_FUC_DEL_CONNECT :
        if (ifs[0].dlc == 1 && ifs[0].id.source_id == 0xF7) {
            pisdev_src->slave_status = ICAN_IN_DISCONNECT;
            ret = ICAN_OK;
        }
        else {
            ret = ICAN_ERR_DEL_CONNECT;
        }
        break;

    case ICAN_FUC_DEV_RESET:
        if (ifs[0].dlc == 1 && ifs[0].id.source_id == 0x0F) {
            ret = ICAN_OK;
        }
        else {
            ret = ICAN_ERR_RESET_DEV;
        }
        break;
 
    case ICAN_FUC_EVE_TRIGER :
        ican_printf("slave%d info event\n\r", slave_macid);
        ret = ICAN_OK;
        break;

    case ICAN_FUC_HEART_CHECK :
        ret = ICAN_OK;
        break;

    case ICAN_FUC_EXCEPT:
        ret = (ierr_t)(ifs[0].frame_data[1]); // error code
        __im_except_parse(source_id, ret);            
        break;

    default :
        ret = ICAN_ERR_FUNC_ID;
        ican_printf("Recv Except Response: %s\n\r", ican_error_parse(ret));
    }

    return ret;
}


/**
* @defgroup Master_Core_API ican_mater.c
* @{
*/

/**
* \brief Get system support slave num 
* \return slave num
*/
uint8_t ican_master_get_slave_num(void)
{
    return ICAN_SLAVE_NUM;    
}


/**
* \brief Get the first 'index' slave's MACID
* \return macid
*/
uint8_t ican_master_get_slave_macid(uint8_t index)
{
    if (index >= ICAN_SLAVE_NUM) {
        return 0xff;
    }
    return slave_macid_table[index];
}


/**
* \brief check the first 'index' slave status whether connected
* \return true is connected, false is disconnected
*/
bool ican_master_check_slave_status(uint8_t index)
{
    ican_master_t* pimdev;
    ican_slave_source* pisdev_src;

    if (index >= ICAN_SLAVE_NUM) {
        ican_printf("index over slave num\n\r");
        return ICAN_ERR_MAC_NONEXIST;
    }

    pimdev = &imasterdev;
    pisdev_src = &(pimdev->slave_dev_list[index]);

    if (pisdev_src->slave_status == ICAN_IN_CONNECT) {
        return true;
    }
    else {
        return false;
    }
}


/**
* \brief check slave's macid whether or not on bus line
* \return see ican error code explain in ican_error.h
*/
ierr_t ican_master_checkmac(uint8_t macid)
{
    ierr_t ret = ICAN_OK;
    uint32_t tmp = 0;
    ican_id canid;
    uint8_t cmdbuf[5] = {0, };
    uint8_t frames_num = 0;
    uint8_t recv_num = 0;

    ican_frame sendframe[1];
    ican_frame recvframe[1];
    ican_master_t* pimdev;

    pimdev = &imasterdev;
    
    canid.src_mac_id = pimdev->dev_macid;
    canid.dest_mac_id = macid;
    canid.source_id = 0x00;
    canid.func_id = ICAN_FUC_MAC_CHECK;
    canid.ack = 0;

    tmp = pimdev->dev_info.serial_number;
    cmdbuf[0] = macid;
    cmdbuf[1] = tmp & 0xff;
    cmdbuf[2] = (tmp >> 8) & 0xff;
    cmdbuf[3] = (tmp >> 16) & 0xff;
    cmdbuf[4] = (tmp >> 24) & 0xff;
    
    frames_num = if_organize_msgs(sendframe, canid, cmdbuf, sizeof(cmdbuf));
    if (frames_num > ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    ret = ican_send_msg(pimdev->dev_channel, sendframe, frames_num,
                        SEND_TIME_OUT);
    if (ret != ICAN_OK) {
        return ret;
    }
 
    ret = ican_recv_msg(pimdev->dev_channel, macid, recvframe, &recv_num,
                        CHECK_MACID_TIME_OUT);
    if (ret == ICAN_ERR_TIME_OUT) {
        return ICAN_OK;
    }
    else if (recv_num == 1) {
        return ICAN_ERR_MAC_EXIST;
    }
    else {
        return ICAN_ERR_MAC_MORE;//
    }
}


/**
* \brief check and maintenance heart beating with all slave device connect
* \return see ican error code explain in ican_error.h
*/
static void ican_master_checkheart(void)
{
    uint8_t i;
    uint8_t data[1];
    ierr_t ret = ICAN_OK;
    uint8_t state = 0;
    uint8_t recv_num = 0, frames_num;
    uint32_t link_start;
    uint32_t link_master;
    ican_id canid;

    ican_master_t* pimdev = &imasterdev;
    ican_slave_source* pis_src = NULL; 

    for (i = 0; i < ICAN_SLAVE_NUM; i++) {
        pis_src = &pimdev->slave_dev_list[i];
        link_start = pis_src->linkertmr_init_val;
        link_master = (uint32_t)pimdev->com_info.cyclic_master;
        
        if (pis_src->slave_status != ICAN_IN_CONNECT || link_master == 0) {
            continue;
        }
        if (ican_tmr_ms_delta(link_start)
            < (link_master - 1) * CYCLIC_UNIT_MS) {
            continue;
        }

        canid.src_mac_id = MASTER_MAC_ID;
        canid.dest_mac_id = pis_src->dev_macid;
        canid.source_id = 0xf4;
        canid.func_id = ICAN_FUC_HEART_CHECK;
        canid.ack = 0;   
        data[0] = pimdev->com_info.cyclic_master;                

        frames_num = if_organize_msgs(msendframes, canid, data, 1);
        if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
            return;
        }

        state = ICAN_SEND_STATUS;
        while (1) {
            switch (state) {
            case ICAN_SEND_STATUS :
                ret = ican_send_msg(pimdev->dev_channel, msendframes,
                                    frames_num, SEND_TIME_OUT);
                if (ret != ICAN_OK) {
                    break;
                }
                state = ICAN_RECV_STATUS;
                break;

            case ICAN_RECV_STATUS :
                ret = ican_recv_msg(pimdev->dev_channel, pis_src->dev_macid, mrecvframes,
                                    &recv_num, CHECK_HEART_TIME_OUT);
                if (ret != ICAN_OK) {
                    break;
                }
                state = ICAN_PARSE_STATUS;
                break;

            case ICAN_PARSE_STATUS:
                if (mrecvframes[0].id.func_id != ICAN_FUC_HEART_CHECK) {
                   ret = ICAN_ERR_INVALID_ACK;
                   break;
                }
                ret = __im_func_parse(mrecvframes, recv_num, NULL);
                if (ret != ICAN_OK) {                            
                    pis_src->slave_status = ICAN_IN_DISCONNECT;
                    break;
                }
                state = ICAN_IDLE_STATUS;
            }

            if (state == ICAN_IDLE_STATUS || ret != ICAN_OK) {
                if (ret != ICAN_OK) {
                    ican_printf("\n\rheartcheck slave:%x, faild: %s\n\r",
                                pis_src->dev_macid, ican_error_parse(ret));
                }
                break;
            }
        }
        pis_src->linkertmr_init_val = ican_tmr_ms_get();
    }
}


/**
* \brief check all slave device event
* \param slave_macid slave macid
* \param buff user data buffer variable pointer
* \param len user data length variable pointer
* \return see ican error code explain in ican_error.h
* \note user buff size must greater than ICAN_SPLIT_MAX_BYTES(ican_type.h)
*/
static ierr_t ican_master_checkevent(uint8_t slave_macid, uint8_t* buff, uint8_t* len)
{
    uint8_t i;
    ierr_t ret = ICAN_OK;
    uint8_t state = 0;
    uint8_t recv_num = 0, frames_num;
    uint32_t tmr_thresh;
    ican_id canid;

    ican_master_t* pimdev = &imasterdev;
    ican_slave_source* pis_src = NULL;

    i = smt_get_index(slave_macid);
    pis_src = &pimdev->slave_dev_list[i];

    if (pis_src->slave_status != ICAN_IN_CONNECT) {
        return ICAN_ERR_CONNECT;
    }

    tmr_thresh = (uint32_t)pis_src->com_info.cyclic_param * CYCLIC_UNIT_MS;
    if (tmr_thresh > COS_TIME_OUT) {
        tmr_thresh = COS_TIME_OUT;
    }        
    if (ican_tmr_ms_delta(pis_src->costmr_init_val) < tmr_thresh) {
        return ICAN_ERR_TIME_NOOUT;
    }
    
    pis_src->costmr_init_val = ican_tmr_ms_get();

    canid.src_mac_id = MASTER_MAC_ID; 
    canid.dest_mac_id = slave_macid;
    canid.source_id = 0xf6;
    canid.func_id = ICAN_FUC_EVENT_CHECK;
    canid.ack = 0;

    frames_num = if_organize_msgs(msendframes, canid, NULL, 0);
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    state = ICAN_SEND_STATUS;
    while (1) {
        switch (state) {
        case ICAN_SEND_STATUS :
            ret = ican_send_msg(pimdev->dev_channel, msendframes, frames_num, SEND_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            state = ICAN_RECV_STATUS;
            break;

        case ICAN_RECV_STATUS :
            ret = ican_recv_msg(pimdev->dev_channel, pis_src->dev_macid, mrecvframes,
                                &recv_num, CHECK_EVENT_TIME_OUT);
            if (ret != ICAN_OK) {
               return ret;
            }
            state = ICAN_PARSE_STATUS;
            break;

        case ICAN_PARSE_STATUS:
            if (mrecvframes[0].id.func_id != ICAN_FUC_EVE_TRIGER) {
                return ICAN_ERR_INVALID_ACK;
            }
            if (mrecvframes[0].dlc < 2) {
                return ICAN_ERR_NO_EVENT;
            }            
            ret = __im_func_parse(mrecvframes, recv_num, NULL);
            if (ret != ICAN_OK) {
                return ret;
            }

            if (buff == NULL) {
                return ICAN_ERR_INVALID_POINTER;
            }

            if (if_check_multisegs(mrecvframes, recv_num)) {                        
                *len = if_copy_data(mrecvframes, recv_num, buff);
                return ICAN_OK;
            }
            else {
                return ICAN_ERR_SPLIT_TRANS;
            }                
            break;
        }
    }    
}


/**
* \brief read slave source data
* \param slave_macid slave macid
* \param src_id slave source id
* \param subsrc_id slave subsource id
* \param buff user data buffer variable pointer
* \param len user want read data len
* \return see ican error code explain in ican_error.h
* \note len must less than or equal ICAN_SPLIT_MAX_BYTES(ican_type.h)
*/
ierr_t ican_master_read(uint8_t slave_macid, uint8_t src_id, uint8_t subsrc_id,
                        uint8_t* buff, uint16_t len)
{
    uint8_t state = 0;
    ierr_t ret = ICAN_OK;
    ican_id canid;
    uint8_t cmdbuf[8] = {0, };
    uint8_t frames_num = 0;
    uint8_t recv_num = 0;
    uint8_t datalen = 0;
    ican_master_t* pimdev;

    if (len > ICAN_SPLIT_MAX_BYTES) {
        return ICAN_ERR_PARAM;
    }

    pimdev = &imasterdev;
    
    canid.src_mac_id = MASTER_MAC_ID;
    canid.dest_mac_id = slave_macid;
    canid.source_id = src_id;
    canid.func_id = ICAN_FUC_READ;
    canid.ack = 0;    
    
    if (src_id < 0xF8) {
        cmdbuf[0] = len;
        datalen = 1;
    }
    else {
        cmdbuf[0] = subsrc_id;
        cmdbuf[1] = len;
        datalen = 2;
    }
    
    frames_num = if_organize_msgs(msendframes, canid, cmdbuf, datalen);
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }
    
    state = ICAN_SEND_STATUS;
    while (1) {
        switch(state)
        {
        case ICAN_SEND_STATUS :
            ret = ican_send_msg(pimdev->dev_channel, msendframes, frames_num, SEND_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            state = ICAN_RECV_STATUS;
            break;
        case ICAN_RECV_STATUS :
            ret = ican_recv_msg(pimdev->dev_channel, slave_macid, mrecvframes, &recv_num, RESPONE_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            if (mrecvframes[0].id.func_id != ICAN_FUC_READ) {
                return ICAN_ERR_INVALID_ACK;
            }
            state = ICAN_PARSE_STATUS;
            break;
        case ICAN_PARSE_STATUS:
            ret = __im_func_parse(mrecvframes, recv_num, buff);
            if (ret != ICAN_OK) {
                ican_printf("ican read failed: %s\n\r", ican_error_parse(ret));
                return ret;
            }
            return ICAN_OK;
        }
    }
}


/**
* \brief write data to slave source
* \param slave_macid slave macid
* \param src_id slave source id
* \param subsrc_id slave subsource id
* \param buff user data buffer variable pointer
* \param len user want write data len
* \return see ican error code explain in ican_error.h
* \note len must less than or equal ICAN_SPLIT_MAX_BYTES(ican_type.h)
*/
ierr_t ican_master_write(uint8_t slave_macid, uint8_t src_id, uint8_t subsrc_id, 
                          uint8_t* buff, uint16_t len)
{
    uint8_t state = 0;
    ierr_t ret = ICAN_OK;
    ican_id canid;
    uint8_t cmdbuf[ICAN_SPLIT_MAX_BYTES] = {0, };
    uint8_t frames_num = 0;
    uint8_t recv_num = 0;
    uint16_t datalen = 0;
    ican_master_t* pimdev = &imasterdev;
  
    if (len > sizeof(cmdbuf) - 2) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    canid.src_mac_id = MASTER_MAC_ID;
    canid.dest_mac_id = slave_macid;
    canid.source_id = src_id;
    canid.func_id = ICAN_FUC_WRITE;
    canid.ack = 0;
    
    if (src_id < 0xF8) {
        datalen = 0;
    }
    else {
        cmdbuf[0] = subsrc_id;
        datalen = 1;
    }

    memcpy(&cmdbuf[datalen], buff, len);
    datalen += len;

    frames_num = if_organize_msgs(msendframes, canid, cmdbuf, datalen);
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    state = ICAN_SEND_STATUS;
    while (1) {
        switch(state) {
        case ICAN_SEND_STATUS :
            ret = ican_send_msg(pimdev->dev_channel, msendframes, frames_num, SEND_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            state = ICAN_RECV_STATUS;
            break;

        case ICAN_RECV_STATUS :  
            ret = ican_recv_msg(pimdev->dev_channel, slave_macid, mrecvframes,
                                &recv_num, RESPONE_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            if (mrecvframes[0].id.func_id != ICAN_FUC_WRITE) {
                return ICAN_ERR_INVALID_ACK;
            }
            state = ICAN_PARSE_STATUS;
            break;

        case ICAN_PARSE_STATUS:
            ret = __im_func_parse(mrecvframes, recv_num, buff);
            if (ret != ICAN_OK) {
                ican_printf("ican write failed: %s\n\r", ican_error_parse(ret));
                return ret;
            }           
            
            return ret;
        }
    }

}


/**
* \brief connect with slave device which macid is slave_macid
* \param slave_macid slave devcie's macid
* \note slave macid must get from ican_master_get_slave_macid()
*/
ierr_t ican_master_connect(uint8_t slave_macid)
{
    uint8_t state = 0;
    ierr_t ret = ICAN_OK;
    ican_id canid;
    uint8_t cmdbuf[2] = {0, };
    uint8_t frames_num = 0;
    uint8_t recv_num = 0;
    ican_master_t* pimdev = &imasterdev;

#if 0
    /* check slave device whether or on line*/
    ret = ican_master_checkmac(slave_macid);
    if (ret != ICAN_ERR_MAC_EXIST) {//if not equal, node is not exit
        return ICAN_ERR_MAC_NONEXIST;
    }
#endif

    canid.src_mac_id = pimdev->dev_macid;
    canid.dest_mac_id = slave_macid;
    canid.source_id = 0xf7;
    canid.func_id = ICAN_FUC_EST_CONNECT;
    canid.ack = 0;
    cmdbuf[0] = pimdev->dev_macid;
    cmdbuf[1] = pimdev->com_info.cyclic_master;

    frames_num = if_organize_msgs(msendframes, canid, cmdbuf, sizeof(cmdbuf));
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    state = ICAN_SEND_STATUS;
    while (1) {
        switch(state) {
        case ICAN_SEND_STATUS :
            ret = ican_send_msg(pimdev->dev_channel, msendframes, frames_num, SEND_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }

            state = ICAN_RECV_STATUS;
            break;

        case ICAN_RECV_STATUS :
            ret = ican_recv_msg(pimdev->dev_channel, slave_macid, mrecvframes,
                                &recv_num, RESPONE_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            if (mrecvframes[0].id.func_id != ICAN_FUC_EST_CONNECT) {
                return ICAN_ERR_INVALID_ACK;
            }
            state = ICAN_PARSE_STATUS;
            break;

        case ICAN_PARSE_STATUS:
            ret = __im_func_parse(mrecvframes, frames_num, NULL);
            if (ret != ICAN_OK) {
                return ret;
            }
            ican_printf("connect with slave: 0x%x sucessed!\n\r", slave_macid);
            return ret;
        }
    }
}


/**
* \brief disconnect with slave device which macid is slave_macid
* \param slave_macid slave devcie's macid
* \note slave macid must get from ican_master_get_slave_macid()
*/
ierr_t ican_master_disconnect(uint8_t slave_macid)
{
    uint8_t state = 0;
    ierr_t ret = ICAN_OK;
    ican_id canid;
    uint8_t cmdbuf[1] = {0, };
    uint8_t frames_num = 0;
    uint8_t recv_num = 0;
    ican_master_t* pimdev;

    pimdev = &imasterdev;

    canid.src_mac_id = pimdev->dev_macid;
    canid.dest_mac_id = slave_macid;
    canid.source_id = 0xf7;
    canid.func_id = ICAN_FUC_DEL_CONNECT;
    canid.ack = 0;
    cmdbuf[0] = pimdev->dev_macid;

    frames_num = if_organize_msgs(msendframes, canid, cmdbuf, sizeof(cmdbuf));
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }

    state = ICAN_SEND_STATUS;
    while (1) {
        switch(state) {
        case ICAN_SEND_STATUS :
            ret = ican_send_msg(pimdev->dev_channel, msendframes,
                                frames_num, SEND_TIME_OUT);
            if (ret != ICAN_OK) {               
                return ret;
            }

            state = ICAN_RECV_STATUS;
            break;

        case ICAN_RECV_STATUS :
            ret = ican_recv_msg(pimdev->dev_channel, slave_macid, mrecvframes,
                                &recv_num, RESPONE_TIME_OUT);
            if (ret != ICAN_OK) {               
                return ret;
            }
            if (mrecvframes[0].id.func_id != ICAN_FUC_DEL_CONNECT) {
                return ICAN_ERR_INVALID_ACK;
            }
            state = ICAN_PARSE_STATUS;
            break;

        case ICAN_PARSE_STATUS:
            ret = __im_func_parse(mrecvframes, recv_num, NULL);
            if (ret != ICAN_OK) {
                return ret;
            }
            ican_printf("disconnect slave: 0x%x sucessed!\n\r", slave_macid);
            return ret;
         }        
    }
}


/**
* \brief reset slave device which macid is slave_macid
* \param slave_macid slave devcie's macid
* \note slave macid must get from ican_master_get_slave_macid()
*/
ierr_t ican_master_reset(uint8_t slave_macid)
{
    uint8_t state = 0;
    ierr_t ret = ICAN_OK;
    ican_id canid;
    uint8_t cmdbuf[1] = {0, };
    uint8_t frames_num = 0;
    uint8_t recv_num = 0;
    ican_master_t* pimdev;

    pimdev = &imasterdev;
    
    canid.src_mac_id = MASTER_MAC_ID;
    canid.dest_mac_id = slave_macid;
    canid.source_id = 0xff;
    canid.func_id = ICAN_FUC_DEV_RESET;
    canid.ack = 0;
    cmdbuf[0] = slave_macid;
    
    frames_num = if_organize_msgs(msendframes, canid, cmdbuf, 1);
    if (frames_num >= ICAN_ERR_SPLIT_FLOW) {
        return ICAN_ERR_SPLIT_FLOW;
    }
    
    state = ICAN_SEND_STATUS;
    while (1) {
        switch(state) {
        case ICAN_SEND_STATUS :
            ret = ican_send_msg(pimdev->dev_channel, msendframes,
                                frames_num, SEND_TIME_OUT);
            if (ret != ICAN_OK) {
                return ret;
            }
            state = ICAN_RECV_STATUS;
            break;

        case ICAN_RECV_STATUS :
            ret = ican_recv_msg(pimdev->dev_channel, slave_macid, mrecvframes,
                                &recv_num, RESPONE_TIME_OUT);
            if (ret != frames_num) {
                return ret;
            }
            if (mrecvframes[0].id.func_id != ICAN_FUC_DEV_RESET) {
                return ICAN_ERR_INVALID_ACK;
            }
            state = ICAN_PARSE_STATUS;
            break;

        case ICAN_PARSE_STATUS:
            ret = __im_func_parse(mrecvframes, recv_num, NULL);
            if (ret != ICAN_OK) {
                return ret;
            }
    
            return ICAN_OK;
        }
    }
}


/**
* \brief poll all slave device event; check bus except and recover bus when exception
*        is happend; maintenance all slave heart beating.
* \param slave_macid user slave_macid variable pointer, it set slave macid when one slave
         has event reporting to master
* \param src_id it set slave source id when one slave has event reporting to master
* \param buff user buffer pointer which put event data
* \param len user len poninter which put event data length
* \return see ican error code explain in ican_error.h
* \note if return ICAN_OK means slave has data reported
*/
ierr_t ican_master_poll(uint8_t* slave_macid, uint8_t* src_id, uint8_t* buff, uint8_t* len)
{
    ierr_t ret;
    uint8_t i;
    bool init;
    ican_master_t* pimdev;

    if (slave_macid == NULL || src_id == NULL
        || buff == NULL || len == NULL) {
        ican_printf("poll: param error\n\r");
        return ICAN_ERR_PARAM;
    }

    pimdev = &imasterdev;

    /* check bus state */
    if (ican_can_is_except(pimdev->dev_channel)) {
        ican_printf("\n\rcan bus except happend, now restart\n\r");
        ican_can_off(pimdev->dev_channel);
        ican_tmr_ms_delay(100);

        for (i = 0; i < ICAN_SLAVE_NUM; i++) {
            pimdev->slave_dev_list[i].slave_status = ICAN_IN_DISCONNECT;
        }

        i = 0;
        init = false;
        while (i++ < 5 && !init) {
            init = ican_can_init(pimdev->dev_channel, pimdev->dev_macid,
                                 pimdev->com_info.baud_rate);
        }        
        
        if (init) {
            return ICAN_ERR_BUS_RESTART;
        }
        else {
            return ICAN_ERR_BUS_EXCEPT;
        }
    }

    /* check heart */
    ican_master_checkheart();

    /* check event */
    if (*slave_macid != 0xff) {//check one special slave
        ret = ican_master_checkevent(*slave_macid, buff, len);
        if (ret == ICAN_OK) {
            if (*len > 0) {
                return ICAN_OK;
            }
            return ICAN_ERR_NO_EVENT;
        }
        return ret;
    }
    else {
        for (i = 0; i < ICAN_SLAVE_NUM; i++) {
            ret = ican_master_checkevent(slave_macid_table[i], buff, len);            
            if (ret == ICAN_OK) {
                *slave_macid = slave_macid_table[i];
                if (*len > 0) {
                    return ICAN_OK;
                }
            }
        }
        *len = 0;
        *slave_macid = 0xff;
        return ret;
    }    
}


/**
* \brief init the first 'channel' can controler as master
* \param channel can controler id
* \return true is OK, false is failed
*/
bool ican_master_init(uint8_t channel)
{
    uint8_t i = 0;
    ican_master_t * pimdev;

    /* initialize the can controller */
    if (!ican_can_init(channel, MASTER_MAC_ID, BAUD_RATE)) {
        return false;
    }

    pimdev = &imasterdev;
    pimdev->dev_channel = channel;
    pimdev->dev_macid                   = MASTER_MAC_ID;
    pimdev->dev_status                  = ICAN_IDLE_STATUS;
    pimdev->dev_info.vendor_id          = VENDOR_ID;
    pimdev->dev_info.product_type       = PRODUCT_TYPE;
    pimdev->dev_info.product_code       = PRODUCT_CODE;
    pimdev->dev_info.hardware_version   = HARDWARE_VERSION;
    pimdev->dev_info.firmware_version   = FIRMWARE_VERSION;
    pimdev->dev_info.serial_number      = SERIAL_NUMBER;

    pimdev->com_info.dev_mac_id         = MASTER_MAC_ID;
    pimdev->com_info.baud_rate          = BAUD_RATE;
    pimdev->com_info.user_baud_rate     = USER_BAUD_RATE;
    pimdev->com_info.cyclic_param       = CYCLIC_PARAM;
    pimdev->com_info.cyclic_master      = CYCLIC_MASTER;
    pimdev->com_info.cos_type           = COS_TYPE;
    pimdev->com_info.master_mac_id      = MASTER_MAC_ID;

    for (i = 0; i < ICAN_SLAVE_NUM; i++) {
        pimdev->slave_dev_list[i].slave_status = ICAN_IN_DISCONNECT;
        pimdev->slave_dev_list[i].dev_macid = slave_macid_table[i];
        
        memset((void *)&(pimdev->slave_dev_list[i].dev_info),
               0, sizeof(ican_dev_info));
        memset((void *)&(pimdev->slave_dev_list[i].com_info),
               0, sizeof(ican_com_info));
    }

    return true;
}
/** @}*/


/**
* @defgroup Master_Extend_API ican_master.c
* @{
*/

/**
* \brief read slave device information
* \param slave_macid slave device macid
* \param devinfo device information struct
* \newest if true get newest from slave device and update local info
* \return ican error code 
*/
ierr_t im_read_slave_devinfo(uint8_t slave_macid, ican_dev_info* devinfo, bool newest)
{
    ierr_t ret;
    uint8_t smt_index = 0xff;
    ican_master_t* pimdev;
    ican_slave_source* pisdev_src;    
    
    smt_index = smt_get_index(slave_macid);
    if (smt_index >= ICAN_SLAVE_NUM) {
        ican_printf("can't find slave id in master's slave_macid_table\n\r");
        return ICAN_ERR_MAC_NONEXIST;
    }
    
    pimdev = &imasterdev;
    pisdev_src = &(pimdev->slave_dev_list[smt_index]);

    if (!newest) {
        memcpy(devinfo, &pisdev_src->dev_info, sizeof(ican_dev_info));
    }
    else {
        if (pisdev_src->slave_status == ICAN_IN_DISCONNECT) {
            ret = ican_master_connect(slave_macid);
            if (ret != ICAN_OK) {
                return ret;
            }            
        }

        ret = ican_master_read(slave_macid, ICAN_DEVICE_INFO_START, 0,
                               (uint8_t*)devinfo, sizeof(ican_dev_info));
        if (ret != ICAN_OK) {
            return ret;
        }

        /* refresh device info */
        memcpy(&pisdev_src->dev_info, devinfo, sizeof(ican_dev_info));
    }

    return ICAN_OK;
}


/**
* \brief read slave communication information
* \param slave_macid slave device macid
* \param cominfo communication information struct
* \newest if true get newest from slave device and update local info
* \return ican error code 
*/
ierr_t im_read_slave_cominfo(uint8_t slave_macid, ican_com_info* cominfo, bool newest)
{
    ierr_t ret;
    uint8_t smt_index = 0xff;
    ican_master_t* pimdev;
    ican_slave_source* pisdev_src;    
    
    smt_index = smt_get_index(slave_macid);
    if (smt_index >= ICAN_SLAVE_NUM) {
        ican_printf("can't find slave id in master's slave_macid_table\n\r");
        return ICAN_ERR_MAC_NONEXIST;
    }
    
    pimdev = &imasterdev;
    pisdev_src = &(pimdev->slave_dev_list[smt_index]);

    if (!newest) {
        memcpy(cominfo, &pisdev_src->com_info, sizeof(ican_com_info));
    }
    else {
        if (pisdev_src->slave_status == ICAN_IN_DISCONNECT) {
            ret = ican_master_connect(slave_macid);
            if (ret != ICAN_OK) {
                return ret;
            }            
        }

        ret = ican_master_read(slave_macid, ICAN_COMMUNICATE_START, 0,
                               (uint8_t*)cominfo, sizeof(ican_com_info));
        if (ret != ICAN_OK) {
            return ret;
        }

        /* refresh device info */
        memcpy(&pisdev_src->com_info, cominfo, sizeof(ican_com_info));
    }


    return ICAN_OK;
}


/**
* \brief read slave io parameters
* \param slave_macid slave device macid
* \param ioparam io parameters struct
* \newest if true get newest from slave device and update local info
* \return ican error code 
*/
ierr_t im_read_slave_ioparam(uint8_t slave_macid, ican_io_param* ioparam, bool newest)
{
    ierr_t ret;
    uint8_t smt_index = 0xff;
    ican_master_t* pimdev;
    ican_slave_source* pisdev_src;    
    
    smt_index = smt_get_index(slave_macid);
    if (smt_index >= ICAN_SLAVE_NUM) {
        ican_printf("can't find slave id in master's slave_macid_table\n\r");
        return ICAN_ERR_MAC_NONEXIST;
    }
    
    pimdev = &imasterdev;
    pisdev_src = &(pimdev->slave_dev_list[smt_index]);

    if (!newest) {
        memcpy(ioparam, &pisdev_src->io_param, sizeof(ican_io_param));
    }
    else {
        if (pisdev_src->slave_status == ICAN_IN_DISCONNECT) {
            ret = ican_master_connect(slave_macid);
            if (ret != ICAN_OK) {
                return ret;
            }            
        }

        ret = ican_master_read(slave_macid, ICAN_IO_PARAM_START, 0,
                               (uint8_t*)ioparam, sizeof(ican_io_param));
        if (ret != ICAN_OK) {
            return ret;
        }

        /* refresh device info */
        memcpy(&pisdev_src->io_param, ioparam, sizeof(ican_io_param));
    }


    return ICAN_OK;
}


/**
* \brief write slave communication information
* \param slave_macid slave device macid
* \param cominfo communication information struct
* \return ican error code 
*/
ierr_t im_write_slave_cominfo(uint8_t slave_macid, ican_com_info* cominfo)
{
    ierr_t ret;
    uint8_t smt_index = 0xff;
    ican_master_t* pimdev;
    ican_slave_source* pisdev_src;    
    
    smt_index = smt_get_index(slave_macid);
    if (smt_index >= ICAN_SLAVE_NUM) {
        ican_printf("can't find slave id in master's slave_macid_table\n\r");
        return ICAN_ERR_MAC_NONEXIST;
    }
    
    pimdev = &imasterdev;
    pisdev_src = &(pimdev->slave_dev_list[smt_index]);

    if (pisdev_src->slave_status == ICAN_IN_DISCONNECT) {
        ret = ican_master_connect(slave_macid);
        if (ret != ICAN_OK) {
            return ret;
         }            
    }

    ret = ican_master_write(slave_macid, ICAN_COMMUNICATE_START, 0,
                           (uint8_t*)cominfo, sizeof(ican_com_info));
    if (ret != ICAN_OK) {
        return ret;
    }

    /*update local com info */
    memcpy(&pisdev_src->com_info, cominfo, sizeof(ican_com_info));

    return ICAN_OK;
}

/** @}*/

