/**
* @file ican_API.h
*
* ican user API funcs
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_API_H__
#define __ICAN_API_H__

#include "ican_type.h"

/* ican common api */
char* ican_error_parse(ierr_t err);
bool ican_can_wakeup(const uint8_t channel);
void ican_can_sleep(const uint8_t channel);
void ican_can_off(const uint8_t channel);

/* ican master core user api */
uint8_t ican_master_get_slave_num(void);
uint8_t ican_master_get_slave_macid(uint8_t index);
bool    ican_master_check_slave_status(uint8_t index);
ierr_t  ican_master_checkmac(uint8_t macid);
ierr_t  ican_master_connect(uint8_t slave_macid);
ierr_t  ican_master_disconnect(uint8_t slave_macid);
ierr_t  ican_master_read(uint8_t slave_macid, uint8_t src_id, uint8_t subsrc_id, uint8_t* buff, uint16_t len);
ierr_t  ican_master_write(uint8_t slave_macid, uint8_t src_id, uint8_t subsrc_id, uint8_t* buff, uint16_t len);
ierr_t  ican_master_poll(uint8_t* slave_macid, uint8_t* src_id, uint8_t* buff, uint8_t* len);
ierr_t  ican_master_reset(uint8_t slave_macid);
bool    ican_master_init(uint8_t channel);

/* ican master extended user api */
ierr_t im_read_slave_devinfo(uint8_t slave_macid, ican_dev_info* devinfo, bool newest);
ierr_t im_read_slave_cominfo(uint8_t slave_macid, ican_com_info* cominfo, bool newest);
ierr_t im_read_slave_ioparam(uint8_t slave_macid, ican_io_param* ioparam, bool newest);
ierr_t im_write_slave_cominfo(uint8_t slave_macid, ican_com_info* cominfo);

/* ican slave core user api */
bool ican_slave_init(uint8_t channel, uint8_t devMacId);
ierr_t	ican_slave_poll(uint8_t channel);

void ican_slave_init_fops(void);
void ican_slave_set_fops(is_file_operations_t *ifops);
uint8_t ican_slave_get_macid(uint8_t index);


/* ican slave extended user api */
void is_get_iosrc(ican_io_src* iosrc);
void is_get_devinfo(ican_dev_info* devinfo);
void is_get_cominfo(ican_com_info* cominfo);
void is_get_ioparm(ican_io_param* ioparam);
void is_set_iosrc(const ican_io_src* iosrc);
void is_set_devinfo(const ican_dev_info* devinfo);
void is_set_cominfo(const ican_com_info* cominfo);

ierr_t is_simulate_write_di(uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_read_do(uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_write_ai(uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_read_ao(uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_write_serail(uint8_t serial_num, uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_read_serail(uint8_t serial_num, uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_write_userdef(uint8_t src_id, uint8_t* buff, uint8_t len);
ierr_t is_simulate_read_userdef(uint8_t src_id, uint8_t* buff, uint8_t len);

#endif

