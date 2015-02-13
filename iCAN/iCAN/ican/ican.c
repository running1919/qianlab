/**
* @file ican.c
*
* ican basic funcs
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/


#include <string.h>

#include "../lib/ican_timer.h"
#include "../can/can_hal.h"

#include "ican.h"


uint8_t if_get_source_id(ican_frame* iframe)
{
    return iframe->id.source_id;
}


uint8_t if_get_func_id(ican_frame* iframe)
{
    return iframe->id.func_id;
}


uint8_t if_get_dest_mac_id(ican_frame* iframe)
{
    return iframe->id.dest_mac_id;
}


uint8_t if_get_src_mac_id(ican_frame* iframe)
{
    return iframe->id.src_mac_id;
}


uint8_t if_get_length(ican_frame* iframe)
{
    return iframe->dlc;
}


uint8_t if_get_subsrc_id(ican_frame* iframe)
{
    if (iframe->id.source_id >= 0xF8) {
        return iframe->frame_data[1];
    }

    return 0xff;
}


uint8_t if_get_data_len(ican_frame* iframe)
{
    if (iframe->id.source_id < 0xF8) {
        return iframe->frame_data[1];
    }
    else {
        return iframe->frame_data[2];
    }
}


void if_set_source_id(ican_frame* iframe, uint8_t sid)
{
    iframe->id.source_id = sid;
}


void if_set_func_id(ican_frame* iframe, uint8_t fid)
{
    iframe->id.func_id = fid;
}


void if_set_dest_mac_id(ican_frame* iframe, uint8_t dmid)
{
    iframe->id.dest_mac_id = dmid;
}


void if_set_src_mac_id(ican_frame* iframe, uint8_t smid)
{
    iframe->id.src_mac_id = smid;
}


void if_set_length(ican_frame* iframe, uint8_t dlc)
{
    iframe->dlc = dlc;
}


void if_set_subsrc_id(ican_frame* iframe, uint8_t subsrc_id)
{
    if (iframe->id.source_id >= 0xF8) {
        iframe->frame_data[1] = subsrc_id;
    }
}


void if_set_data_len(ican_frame* iframe, uint8_t len)
{
    if (iframe->id.source_id < 0xF8) {
        iframe->frame_data[1] = len;
    }
    else {
        iframe->frame_data[2] = len;
    }
}


bool if_check_multisegs(const ican_frame ifs[], const uint8_t ifs_num)
{
    uint8_t i;

    if (ifs_num < 2) {
        return true;
    }

    if (ifs[0].frame_data[0] != ICAN_SPLIT_SEG_FIRST) {
        return false;
    }
    
 
    for (i = 1; i < ifs_num - 1; i++) {
        if (ifs[i].frame_data[0]
            != (uint8_t)(ICAN_SPLIT_SEG_MID | (i & 0x3f))) {
            return false;
        }
    }

    if (ifs[ifs_num - 1].frame_data[0] != ICAN_SPLIT_SEG_LAST) {
        return false;
    }

    return true;
}


uint16_t if_copy_data(const ican_frame ifs[], const uint8_t ifs_num, uint8_t* buf)
{
    uint8_t i = 0;
    uint16_t len = 0;

    if (ifs_num == 0 || buf == NULL) {
        return 0;
    }

    if (ifs[0].id.source_id >= 0xf8 && ifs[0].id.ack == 0) {
        memcpy(buf, &(ifs[0].frame_data[2]), ifs[0].dlc - 1);
        len = ifs[0].dlc - 2;
        i = 1;
    }

    for (; i < ifs_num; i++) {        
        memcpy(&buf[len], &(ifs[i].frame_data[1]), ifs[i].dlc - 1);
        len += ifs[i].dlc - 1;
    }

    return len;
}


void if_copy_canid(ican_id* dest_id, const ican_id* src_id)
{
     dest_id->source_id   = src_id->source_id;
     dest_id->func_id     = src_id->func_id;
     dest_id->ack         = src_id->ack;
     dest_id->dest_mac_id = src_id->dest_mac_id;
     dest_id->src_mac_id  = src_id->src_mac_id;
}


static void if_fill_msg(ican_frame* iframe, const ican_id canid,
                          const uint8_t *databuf, const uint8_t datalen)
{
    uint8_t i;
    
    if (datalen > 7) {
        return;
    }

    /* fill identifier*/
    iframe->id.src_mac_id = canid.src_mac_id;
    iframe->id.dest_mac_id = canid.dest_mac_id;
    iframe->id.source_id = canid.source_id;
    iframe->id.func_id = canid.func_id;
    iframe->id.ack = canid.ack;
    iframe->dlc = datalen + 1;//include segflag

    iframe->frame_data[0] = ICAN_NO_SPLIT_SEG;
    
    /* fill data */
    for (i = 1; i <= datalen; i++) {
        iframe->frame_data[i] = databuf[i - 1];
    }
}


static void if_split_msg(ican_frame ifs[], const uint8_t ifs_num)
{
    uint8_t i;
    uint8_t mid_ifs_num;

    if (ifs_num < 2 || ifs_num > ICAN_SPLIT_MAX_SEGS)
        return;

    /* fill first seg */
    ifs[0].frame_data[0] = ICAN_SPLIT_SEG_FIRST;

    /* fill mid segs */
    mid_ifs_num = ifs_num - 2;
    if (mid_ifs_num > 0) {
        for (i = 1; i <= mid_ifs_num; i++)
            ifs[i].frame_data[0] = (uint8_t)(ICAN_SPLIT_SEG_MID | (i & 0x3f));
    }

    /* fill last seg */
    ifs[ifs_num - 1].frame_data[0] = ICAN_SPLIT_SEG_LAST;
}


uint8_t if_organize_msgs(ican_frame ifs[], const ican_id canid
                         , const uint8_t *databuf, const uint16_t datalen)
{
    uint8_t i;
    uint8_t ifs_num = 0;

    if (datalen > ICAN_SPLIT_MAX_SEGS * 7) {
        return (uint8_t)ICAN_ERR_SPLIT_FLOW;
    }

    if (datalen == 0) {
        if_fill_msg(&ifs[0], canid, NULL, 0);
        return 1;
    }

    /* fill msg */
    ifs_num = datalen / 7;
    for (i = 0; i < ifs_num; i++) {
        if_fill_msg(&ifs[i], canid, &databuf[i * 7], 7);
    }
    if (datalen % 7 != 0) {
        if_fill_msg(&ifs[ifs_num], canid
                      , &databuf[ifs_num * 7], datalen % 7);
        ++ifs_num;
    }

    /* split msgs */
    if_split_msg(ifs, ifs_num);

    return ifs_num;
}


/**
* \brief send ican frames
* \param channel choice can bus(if has two or more than two can controlers) 
* \param timeout max sending time
*/
ierr_t ican_send_msg(const uint8_t channel, const ican_frame ifs[], const uint8_t ifs_num, uint32_t timeout)
{
    uint8_t i;
    uint32_t curtimerval;

    /* send msg */
    curtimerval = ican_tmr_ms_get();
    for (i = 0; i < ifs_num;) {     
        if (ican_tmr_ms_delta(curtimerval) > timeout) {
            return ICAN_ERR_TIME_OUT;
        }
        if (!can_hal_send(channel, &ifs[i])) {
            continue;
        }
        i++;
    }    

    return ICAN_OK;
}


/**
* \brief receive ican frames
* \param ifs_num receive ican frames num
*/
ierr_t ican_recv_msg(const uint8_t channel, uint8_t src_macid, ican_frame ifs[], uint8_t *ifs_num, const uint32_t timeout)
{
    uint8_t recvf_num = 0;
    ierr_t ret;
    uint32_t curtimerval;

    curtimerval = ican_tmr_ms_get();

    /* recv msg */
    do {
        /* recv one msg */ 
        while (1) {
            if (ican_tmr_ms_delta(curtimerval) > timeout) {
                 return ICAN_ERR_TIME_OUT;
            }
            ifs[0].dlc = 0;
            can_hal_recv(channel, src_macid, &ifs[0]);

            if (ifs[0].dlc > 0) {
                break;
            }
        }

        if ((ifs[0].frame_data[0] == ICAN_NO_SPLIT_SEG)) {
            recvf_num = 1;
            if (ifs_num != NULL) {
                *ifs_num = 1;
            }
            return ICAN_OK;
        }
        
        if (ifs[0].frame_data[0] == ICAN_SPLIT_SEG_FIRST) {
            recvf_num = 1;
            break;
        }
        else {            
            if (ican_tmr_ms_delta(curtimerval) > timeout) {
                return ICAN_ERR_TIME_OUT;
            }
        }
    } while (1);

    curtimerval = ican_tmr_ms_get();
    /* recv mid & last seg msg */
    if (recvf_num == 1) {
        do {
            if (ican_tmr_ms_delta(curtimerval) > timeout) {
                return ICAN_ERR_TIME_OUT;
            }
            ifs[recvf_num].dlc = 0;
            can_hal_recv(channel, src_macid, &ifs[recvf_num]);
            if (ifs[recvf_num].dlc == 0) {
                continue;
            }
            
            /* excepet happen */
            if (ifs[recvf_num].id.source_id != ifs[0].id.source_id
                && ifs[recvf_num].frame_data[0] & 0xc0 != ICAN_SPLIT_SEG_MID
                && ifs[recvf_num].frame_data[0] & 0x3f != recvf_num) {             
                ret = ICAN_ERR_SPLIT_TRANS;
                break;
            }

            if (ifs[recvf_num].frame_data[0] == ICAN_SPLIT_SEG_LAST) {
                ++recvf_num;
                ret = ICAN_OK;
                break;
            }
            ++recvf_num;
            curtimerval = ican_tmr_ms_get();
        } while (1);
    }

    if (ifs_num != NULL) {
        *ifs_num = recvf_num;
    }

    return ret;
}


char* ican_error_parse(uint8_t err)
{
    char *errstr = NULL;
    
    switch (err) {
    case ICAN_OK :
        errstr = "OK";
        break;
        
    case ICAN_ERR_FUNC_ID :
        errstr = "#iCAN# Func ID Not Exit";
        break;

    case ICAN_ERR_SOURCE_ID :
        errstr = "#iCAN# Source Not Exit";
        break;
        
    case ICAN_ERR_COMMAND :
        errstr = "#iCAN# UnSpport This Command";
        break;
        
    case ICAN_ERR_PARAM :
        errstr = "#iCAN# Invalid Param";
        break;
        
    case ICAN_ERR_CONNECT :
        errstr = "#iCAN# Connect Not Exit";
        break;
        
    case ICAN_ERR_OPERATE :
        errstr = "#iCAN# Operate Invalid";
        break;
        
    case ICAN_ERR_SPLIT_TRANS :
        errstr = "#iCAN# SPLIT Trans Failed";
        break;

    //Selfdefine error code
    case ICAN_ERR_MAC_NONEXIST:
        errstr = "#iCAN# MacID NonExist";
        break;
        
    case ICAN_ERR_TIME_OUT :
        errstr = "#iCAN# Operate Time Out";
        break;

    case ICAN_ERR_TIME_NOOUT :
        errstr = "#iCAN# Time No Out";
        break;
        
    case ICAN_ERR_BUFF_OVERFLOW :
        errstr = "#iCAN# Buff Overflow";
        break;

    case ICAN_ERR_LEN_ZERO :
        errstr = "#iCAN# Souce Len Zero";
        break;
        
    case ICAN_ERR_NODE_BUSY :
        errstr = "#iCAN# Node Busy";
        break;
        
    case ICAN_ERR_DEL_CONNECT :
        errstr = "#iCAN# Del Connect Failed";
        break;
        
    case ICAN_ERR_MAC_EXIST :
        errstr = "#iCAN# MacID Exist";
        break;
        
    case ICAN_ERR_RESET_DEV :
        errstr = "#iCAN# Reset Device Failed";
        break;
        
    case ICAN_ERR_READ_SRC :
        errstr = "#iCAN# Read Source Failed";
        break;
            
    case ICAN_ERR_WRITE_SRC :
        errstr = "#iCAN# Write Source Failed";
        break;
        
    case ICAN_ERR_SPLIT_FLOW :
        errstr = "#iCAN# Split Msg Flow";
        break;
        
    case ICAN_ERR_BUS_EXCEPT :
        errstr = "#iCAN# CAN Bus Except";
        break;
        
    case ICAN_ERR_BUS_RESTART :
        errstr = "#iCAN# CAN Bus Restart Failed";
        break;

    case ICAN_ERR_NOTACK_FRAME :
        errstr = "#iCAN# Frame Is Not Ack";
        break;
        
    case ICAN_ERR_INVALID_POINTER :
        errstr = "#iCAN# Invalid Pointer";
        break;

    case ICAN_ERR_NO_EVENT :
        errstr = "#iCAN# No Event Happen";
        break;

    case ICAN_ERR_INVALID_ACK :
        errstr = "#iCAN# Invalid Ack";
        break;

    case  ICAN_ERR_MAC_MORE :
        errstr = "#iCAN# More Than 2 Dev Has the Same ID";
        break;

    default :
        errstr = "#iCAN# Unknown Error";
    }

    return errstr;
}


bool ican_can_init(const uint8_t channel, const uint8_t recv_macid, const uint8_t baudrate)
{
    return can_hal_init(channel, recv_macid, baudrate);
}


bool ican_can_wakeup(const uint8_t channel)
{
    return can_hal_wakeup(channel);
}


void ican_can_sleep(const uint8_t channel)
{
    can_hal_sleep(channel);
}


void ican_can_off(const uint8_t channel)
{
    can_hal_off(channel);
}


bool ican_can_is_except(const uint8_t channel)
{
    return can_hal_is_except(channel);
    //return false;
}

