/**
* @file can_hal.h
*
* hardware abstraction layer for all platform
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/


#ifndef __CAN_HAL_H__
#define __CAN_HAL_H__


#include "../include/ican_type.h"

bool can_hal_init(uint8_t channel, uint8_t recv_macid, uint16_t baudrate);
bool can_hal_wakeup(uint8_t channel);
void can_hal_sleep(uint8_t channel);
void can_hal_off(uint8_t channel);
bool can_hal_is_except(uint8_t channel);
void can_hal_recv(const uint8_t channel, const uint8_t src_macid, ican_frame* iframe);
bool can_hal_send(const uint8_t channel, const ican_frame* iframe);


/**
* Next funcs are related to the hardware platform
*/
void can_configure(uint8_t channel, uint16_t baudrate);    
bool can_active(uint8_t channel, uint8_t recv_macid);

bool can_wakeup(uint8_t channel);
void can_sleep(uint8_t channel);
void can_off(uint8_t channel);
bool can_is_except(uint8_t channel);;

    
bool can_mb_request(uint8_t channel, uint8_t* mb_id, uint8_t mail_type);    
void can_mb_free(uint8_t channel, uint8_t mb_id); 

uint8_t can_check_inbox(uint8_t channel, uint8_t* mb_id);
bool can_check_outbox(uint8_t channel, uint8_t mb_id);

void can_clear_mailbox(uint8_t channel, uint8_t mailbox_id, bool all);

void can_get_id(uint8_t channel, uint8_t mb_id, ican_id* id);
void can_set_id(uint8_t channel, uint8_t mb_id, const ican_id* id);

void can_read_mail(uint8_t channel, uint8_t mb_id, uint8_t* buff, uint8_t size);
void can_write_mail(uint8_t channel, uint8_t mb_id, const uint8_t* buff, uint8_t size);

bool can_transfer_start(uint8_t channel, uint8_t mailbox_id);

#endif

