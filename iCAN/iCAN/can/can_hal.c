/**
* @file can_hal.c
*
* hardware abstraction layer for all platform
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/

#include "../include/can_type.h"
#include "../ican/ican.h"
#include "../lib/ican_timer.h"
#include "../lib/ican_stdio.h"
#include "can_hal.h"

#include <string.h>


bool can_hal_init(uint8_t channel, uint8_t recv_macid, uint16_t baudrate)
{
    can_configure(channel, baudrate);
    if (!can_active(channel, recv_macid)) {
        ican_printf("can%d can't active...\n\r", channel);
        return false;
    }

    ican_printf("can%d is actived^-^\n\r", channel);
    return true;
}


bool can_hal_wakeup(uint8_t channel)
{
    if (!can_wakeup(channel)) {
        ican_printf("can%d can't wakeup...\n\r", channel);
        return false;
    }

    ican_printf("can%d is wakeuped^-^\n\r", channel);
    return true;

}


void can_hal_sleep(uint8_t channel)
{
    ican_printf("can%d is sleeping^-^\n\r", channel);
    can_sleep(channel);
}


void can_hal_off(uint8_t channel)
{
    can_off(channel);
}


bool can_hal_is_except(uint8_t channel)
{
    return can_is_except(channel);
}


void can_hal_recv(const uint8_t channel, ican_frame * iframe)
{
    bool flag = 0;

    flag = can_check_inbox(channel);
    if (iframe == NULL || flag == false) {
        return;
    }

    can_read_mail(channel, iframe);
}


bool can_hal_send(const uint8_t channel, const ican_frame *iframe)
{
    uint8_t mailbox_id;
    uint8_t curtimerval;
    
    if (!can_mb_request(channel, &mailbox_id, ICAN_MAIL_BOX_TX)) {
        return false;
    }

    can_set_id(channel, mailbox_id, &iframe->id);
    can_write_mail(channel, mailbox_id, &iframe->frame_data[0], iframe->dlc);
    if (!can_transfer_start(channel, mailbox_id)) {
        ican_printf("can%d send msg failed\n\r", channel);
        can_mb_free(channel, mailbox_id);
        return false;
    }


    curtimerval= ican_tmr_ms_get();
    while (!can_check_outbox(channel, mailbox_id)) {
        if (ican_tmr_ms_delta(curtimerval) > 100) {
            can_mb_free(channel, mailbox_id);
            return false;
        }
    }

   can_mb_free(channel, mailbox_id);
   return true;
}

