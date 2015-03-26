/**
* @file ican_slave.h
*
* ican slave
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_SLAVE_H__
#define __ICAN_SLAVE_H__

/* Core User API */
bool ican_slave_init(uint8_t channel, uint8_t devMacId);
ierr_t	ican_slave_poll(uint8_t channel);

void ican_slave_init_fops(void);
void ican_slave_set_fops(is_file_operations_t *ifops);
uint8_t ican_slave_get_macid(uint8_t index);


/* Extended User API */
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

