/**
* @file ican.h
*
* ican basic funcs
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_H__
#define __ICAN_H__

#include "../include/ican_type.h"

/* ican function code define */
#define ICAN_FUC_Reserve            0x00
#define ICAN_FUC_WRITE	            0x01
#define ICAN_FUC_READ               0x02
#define ICAN_FUC_EVE_TRIGER         0x03
#define ICAN_FUC_EST_CONNECT        0x04
#define ICAN_FUC_DEL_CONNECT        0x05
#define ICAN_FUC_DEV_RESET          0x06
#define ICAN_FUC_MAC_CHECK          0x07

#define ICAN_FUC_HEART_CHECK        0x08
#define ICAN_FUC_EVENT_CHECK        0x09
#define ICAN_FUC_EXCEPT             0x0f

/* ican connect flag */
#define ICAN_IN_CONNECT             0x01
#define ICAN_IN_DISCONNECT          0x02

/* ican transfer statemachine */
#define ICAN_IDLE_STATUS            0x00
#define ICAN_CHECK_STATUS           0x01
#define ICAN_RECV_STATUS            0x02
#define ICAN_SEND_STATUS            0x03
#define ICAN_PARSE_STATUS           0x04
#define ICAN_ERROR_STATUS           0x05

/* ican split flag */
#define ICAN_NO_SPLIT_SEG           (0x00 << 6)
#define ICAN_SPLIT_SEG_FIRST	    (0x01 << 6)
#define ICAN_SPLIT_SEG_MID          (0x02 << 6)
#define ICAN_SPLIT_SEG_LAST         (0x03 << 6)


uint8_t if_get_source_id(ican_frame* iframe);
uint8_t if_get_func_id(ican_frame* iframe);
uint8_t if_get_dest_mac_id(ican_frame* iframe);
uint8_t if_get_src_mac_id(ican_frame* iframe);
uint8_t if_get_length(ican_frame* iframe);
uint8_t if_get_subsrc_id(ican_frame* iframe);
uint8_t if_get_data_len(ican_frame* iframe);

void if_set_source_id(ican_frame* iframe, uint8_t sid);
void if_set_func_id(ican_frame* iframe, uint8_t fid);
void if_set_dest_mac_id(ican_frame* iframe, uint8_t dmid);
void if_set_src_mac_id(ican_frame* iframe, uint8_t smid);
void if_set_length(ican_frame* iframe, uint8_t dlc);
void if_set_subsrc_id(ican_frame* iframe, uint8_t subsrc_id);
void if_set_data_len(ican_frame* iframe, uint8_t len);

bool if_check_multisegs(const ican_frame ifs[], const uint8_t ifs_num);
void if_copy_canid(ican_id* dest_id, const ican_id* src_id);
uint16_t if_copy_data(const ican_frame ifs[], const uint8_t ifs_num, uint8_t* buf);
uint8_t if_organize_msgs(ican_frame ifs[], const ican_id canid, const uint8_t *databuf, const uint16_t datalen);

ierr_t ican_send_msg(const uint8_t channel, const ican_frame ifs[], const uint8_t ifs_num, uint32_t timeout);
ierr_t ican_recv_msg(const uint8_t channel, uint8_t src_macid, ican_frame ifs[], uint8_t *ifs_num, const uint32_t timeout);
char* ican_error_parse(uint8_t err);

bool ican_can_init(const uint8_t channel, const uint8_t recv_macid, const uint8_t baudrate);
bool ican_can_wakeup(const uint8_t channel);
void ican_can_sleep(const uint8_t channel);
void ican_can_off(const uint8_t channel);
bool ican_can_is_except(const uint8_t channel);

#endif

