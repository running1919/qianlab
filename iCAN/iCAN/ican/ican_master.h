/**
* @file ican_master.h
*
* ican master
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#ifndef __ICAN_MASTER_H__
#define __ICAN_MASTER_H__


/* master core user api */
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

/* master extend user api */
ierr_t im_read_slave_devinfo(uint8_t slave_macid, ican_dev_info* devinfo, bool newest);
ierr_t im_read_slave_cominfo(uint8_t slave_macid, ican_com_info* cominfo, bool newest);
ierr_t im_read_slave_ioparam(uint8_t slave_macid, ican_io_param* ioparam, bool newest);
ierr_t im_write_slave_cominfo(uint8_t slave_macid, ican_com_info* cominfo);

#endif

