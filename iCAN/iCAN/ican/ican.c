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
#include "../include/can_config.h"


#include "ican.h"

#define MSG_MAX_LIFE_TICK  (uint16_t)5000

typedef struct __IcanMsg {
    ican_frame ifs[RECV_MAX_MSG_NUM][ICAN_SPLIT_MAX_SEGS];
    bool msgValid[RECV_MAX_MSG_NUM]; //true is valid , false is invalid
    uint32_t msgLifeTick[RECV_MAX_MSG_NUM]; //    
    uint8_t msgFrameNum[RECV_MAX_MSG_NUM]; //msg has frame num    
    uint8_t maxMsgNum;
    uint16_t maxLifeTick;
    uint8_t wp;
} IcanMsg;


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
        if (ifs[0].dlc < 2 || ifs[0].dlc > 8) {
            return 0;
        }

        memcpy(buf, &(ifs[0].frame_data[2]), ifs[0].dlc - 1);
        len = ifs[0].dlc - 2;
        i = 1;
    }

    for (; i < ifs_num; i++) {
        if (ifs[i].dlc < 1 || ifs[i].dlc > 8) {
            return 0;
        }

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
#if 0
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
#else
    uint8_t i;
    uint8_t mid_ifs_num;

    if (ifs_num < 2 || ifs_num > ICAN_SPLIT_MAX_SEGS)
        return;

    /* fill first seg ,contain frame num */
    ifs[0].frame_data[0] = ICAN_SPLIT_SEG_FIRST | (ifs_num & 0x3f);

    /* fill mid segs */
    mid_ifs_num = ifs_num - 2;
    if (mid_ifs_num > 0) {
        for (i = 1; i <= mid_ifs_num; i++)
            ifs[i].frame_data[0] = (uint8_t)(ICAN_SPLIT_SEG_MID | (i & 0x3f));
    }

    /* fill last seg */
    ifs[ifs_num - 1].frame_data[0] = ICAN_SPLIT_SEG_LAST | ((ifs_num - 1) & 0x3f);
#endif
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
    uint32_t curTick;

    /* send msg */
    curTick = ican_tmr_ms_get();
    for (i = 0; i < ifs_num;) {     
        if (ican_tmr_ms_delta(curTick) > timeout) {
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
* \param src_macid receive specified source frames excepet 0xff can receive anyone source
* \param ifs_num return received ican frames num
*/
ierr_t ican_recv_msg(const uint8_t channel, uint8_t src_macid, ican_frame ifs[], uint8_t *ifs_num, const uint32_t timeout)
{
#if 0
    uint8_t i = 0, j = 0;
    uint8_t splitFlag = 0;
    uint8_t tryCnt = 0;
    uint32_t curTick;
    //ierr_t ret;
    ican_frame iframe;

    static bool initFlag = false;
    static IcanMsg icanmsg[NUM_CAN_IF];
    IcanMsg *curimsg;

    if (channel >= NUM_CAN_IF) {
        return ICAN_ERR_PARAM;
    }    

    if (initFlag != true) {
        for (j = 0; j < NUM_CAN_IF; j++) {
            for (i = 0; i < RECV_MAX_MSG_NUM; i++) {            
                icanmsg[j].msgLifeTick[i] = 0;
                icanmsg[j].msgValid[i] = false;
                icanmsg[j].msgFrameNum[i] = 0;
            }
            icanmsg[j].wp = 0;
            icanmsg[j].maxMsgNum = RECV_MAX_MSG_NUM;
            icanmsg[j].maxLifeTick = MSG_MAX_LIFE_TICK;
        }

        initFlag = true;
    }

    curimsg = &icanmsg[channel];

    /* discard too old msg */
    for (i = 0; i < RECV_MAX_MSG_NUM; i++) {
        if ((ican_tmr_ms_delta(curimsg->msgLifeTick[i])
            > curimsg->maxLifeTick)) {
            for (j = 0; j < curimsg->msgFrameNum[i]; j++) {
                memset(&curimsg->ifs[i][j], 0x00, sizeof(ican_frame));
            }
            curimsg->msgFrameNum[i] = 0;
            curimsg->msgValid[i] = false;
            curimsg->ifs[i][0].id.src_mac_id = 0xff;
        }
    }

    curTick = ican_tmr_ms_get();

    /* try recv all msg */
    do {
        /* recv anyone frame */
        iframe.dlc = 0;
        tryCnt = 0;
        while (1) {            
            can_hal_recv(channel, &iframe);
            if (iframe.dlc > 0) {
                break;
            }

            if (ican_tmr_ms_delta(curTick) >= timeout
                || tryCnt > 100) {
                break;
            }
        }

        /* process recved one frame */
        if (iframe.dlc > 0) {
            splitFlag = iframe.frame_data[0];
           
            if (splitFlag == ICAN_NO_SPLIT_SEG) {
                memcpy(&curimsg->ifs[curimsg->wp][0], &iframe, sizeof(ican_frame));
                curimsg->msgValid[curimsg->wp] = true;
                curimsg->msgFrameNum[curimsg->wp] = 1;
                curimsg->msgLifeTick[curimsg->wp] = ican_tmr_ms_get();
                curimsg->wp = (curimsg->wp + 1) % curimsg->maxMsgNum;

                iframe.dlc = 0;
                continue;
            }

#if 0
            if ((splitFlag & 0xc0) == ICAN_SPLIT_SEG_FIRST) {
                memcpy(&curimsg->ifs[curimsg->wp][0], &iframe, sizeof(ican_frame));
                curimsg->msgValid[curimsg->wp] = false;
                curimsg->msgFrameNum[curimsg->wp] = 1;
                curimsg->msgLifeTick[curimsg->wp] = ican_tmr_ms_get();
                curimsg->wp = (curimsg->wp + 1) % curimsg->maxMsgNum;

                iframe.dlc = 0;
                curTick = ican_tmr_ms_get();
                continue;
            }
#endif
            if ((splitFlag & 0x3f) > (ICAN_SPLIT_MAX_SEGS - 1)) {
                if (ican_tmr_ms_delta(curTick) >= timeout) {
                    break;
                }
                continue;
            }

            if ((splitFlag & 0xc0) == ICAN_SPLIT_SEG_FIRST
                ||(splitFlag & 0xc0) == ICAN_SPLIT_SEG_MID
                || (splitFlag & 0xc0) == ICAN_SPLIT_SEG_LAST) {
                for (i = 0; i < curimsg->maxMsgNum; i++) {//find one incomplete data 
                    if (curimsg->msgValid[i] == false) {
                        if (curimsg->ifs[i][0].id.src_mac_id == iframe.id.src_mac_id
                            && curimsg->ifs[i][0].id.func_id == iframe.id.func_id
                            && curimsg->ifs[i][0].id.source_id == iframe.id.source_id) {
                            if (curimsg->ifs[i][splitFlag & 0x3f].dlc > 0) {
                                curimsg->msgFrameNum[i] -= 1;
                                //ican_printf("[ican]:it has data\n\r");
                                //continue;
                            }
                            memcpy(&curimsg->ifs[i][splitFlag & 0x3f],
                                   &iframe, sizeof(ican_frame));
                            curimsg->msgValid[i] = false;
                            curimsg->msgFrameNum[i] += 1;
                            curimsg->msgLifeTick[i] = ican_tmr_ms_get();

                            if (curimsg->msgFrameNum[i] == (curimsg->ifs[i][0].frame_data[0] & 0x3f)) {
                                if ((curimsg->ifs[i][curimsg->msgFrameNum[i] - 1].frame_data[0] & 0xc0)
                                    == ICAN_SPLIT_SEG_LAST) {
                                    curimsg->msgValid[i] = true;
                                }
                                else {
                                    curimsg->msgFrameNum[i] = 0;
                                    curimsg->msgValid[i] = false;
                                    curimsg->ifs[i][0].id.src_mac_id = 0xff;
                                }
                            }

                            curTick = ican_tmr_ms_get();
                            iframe.dlc = 0;
                            break;
                        }
                        else {
                            continue;
                        }
                    }//if (curimsg->msgValid[i]
                    else {
                        continue;
                    }
                }//for
                if (i == curimsg->maxMsgNum && iframe.dlc > 0) {
                	memcpy(&curimsg->ifs[curimsg->wp][0], &iframe, sizeof(ican_frame));
                	curimsg->msgValid[curimsg->wp] = false;
                	curimsg->msgFrameNum[curimsg->wp] = 1;
                	curimsg->msgLifeTick[curimsg->wp] = ican_tmr_ms_get();
                	curimsg->wp = (curimsg->wp + 1) % curimsg->maxMsgNum;

                	iframe.dlc = 0;
				}
            }//if ((splitFlag & 0xc0)
        }//if (iframe.dlc > 0)

        if (ican_tmr_ms_delta(curTick) >= timeout) {
            break;
        }
    } while (1);

    /* pick up one msg */
    for (j = 0; j < curimsg->maxMsgNum; j++) {
        if (curimsg->msgValid[j] == false) {
            continue;
        }
        if (curimsg->ifs[j][0].id.src_mac_id == src_macid) {
            break;
        }
        if (src_macid == 0xff) {
            break;
        }
    }

    if (j == curimsg->maxMsgNum) {
        return ICAN_ERR_TIME_OUT;
    }

    for (i = 0; i < curimsg->msgFrameNum[j]; i++) {
        memcpy(&ifs[i], &curimsg->ifs[j][i], sizeof(ican_frame));
        memset(&curimsg->ifs[j][i], 0x00, sizeof(ican_frame));
    }
    *ifs_num = curimsg->msgFrameNum[j];

    curimsg->msgFrameNum[j] = 0;
    curimsg->msgValid[j] = false;
    curimsg->ifs[j][0].id.src_mac_id = 0xff;

    return ICAN_OK;
#endif

    uint8_t i = 0, j = 0;
    uint8_t splitFlag = 0;
    uint8_t splitHead = 0;
    uint8_t splitSegs = 0;
    uint8_t splitSegm = 0;
    uint8_t tryCnt = 0;
    uint32_t curTick = 0;
    //ierr_t ret;
    ican_frame iframe;
    IcanMsg *curimsg;

    static bool initFlag = false;
    static uint32_t lostCnt = 0;
    static IcanMsg icanmsg[NUM_CAN_IF];

    if (channel >= NUM_CAN_IF) {
        return ICAN_ERR_PARAM;
    }    

    if (initFlag != true) {
        for (j = 0; j < NUM_CAN_IF; j++) {
            for (i = 0; i < RECV_MAX_MSG_NUM; i++) {
                icanmsg[j].msgLifeTick[i] = 0;
                icanmsg[j].msgValid[i] = false;
                icanmsg[j].msgFrameNum[i] = 0;
            }
            icanmsg[j].wp = 0;
            icanmsg[j].maxMsgNum = RECV_MAX_MSG_NUM;
            icanmsg[j].maxLifeTick = MSG_MAX_LIFE_TICK;
        }

        initFlag = true;
    }

    curimsg = &icanmsg[channel];

    /* discard too old msg */
    for (i = 0; i < RECV_MAX_MSG_NUM; i++) {
        if (curimsg->msgFrameNum[i] == 0) {
            continue;
        }

        if ((ican_tmr_ms_delta(curimsg->msgLifeTick[i])
            > curimsg->maxLifeTick)) {
            for (j = 0; j < curimsg->msgFrameNum[i]; j++) {
                memset(&curimsg->ifs[i][j], 0x00, sizeof(ican_frame));
            }
            curimsg->msgFrameNum[i] = 0;
            curimsg->msgValid[i] = false;
            curimsg->ifs[i][0].id.src_mac_id = 0xff;
        }
    }

    curTick = ican_tmr_ms_get();

    /* try recv all msg */
    do {
        /* recv anyone frame */
        iframe.dlc = 0;
        tryCnt = 0;
        while (1) {
            can_hal_recv(channel, &iframe);
            if (iframe.dlc > 0) {
                break;
            }

            if (ican_tmr_ms_delta(curTick) >= timeout
                || tryCnt++ > 100) {
                break;
            }
        }

        /* process recved one frame */
        if (iframe.dlc > 0) {
            splitFlag = iframe.frame_data[0];

            if ((splitFlag == ICAN_NO_SPLIT_SEG)) {
                memcpy(&curimsg->ifs[curimsg->wp][0], &iframe, sizeof(ican_frame));
                curimsg->msgValid[curimsg->wp] = true;
                curimsg->msgFrameNum[curimsg->wp] = 1;
                curimsg->msgLifeTick[curimsg->wp] = ican_tmr_ms_get();
                curimsg->wp = (curimsg->wp + 1) % curimsg->maxMsgNum;

                iframe.dlc = 0;
                continue;
            }

            splitHead = splitFlag & 0xc0;
            splitSegm = splitFlag & 0x3f;

            if (splitHead > ICAN_SPLIT_SEG_LAST) {
                //printf("unkonwn frame1...\n\r");
                continue;
            }

            if (splitHead == ICAN_SPLIT_SEG_FIRST) {                
                if (splitSegm == 0 || splitSegm > ICAN_SPLIT_MAX_SEGS) {
                    //printf("unkonwn frame2...\n\r");
                    continue;
                }
                splitSegm = 0;
            }

            if (splitSegm >= ICAN_SPLIT_MAX_SEGS) {
                if (ican_tmr_ms_delta(curTick) >= timeout) {
                    //printf("frame too much split segs...\n\r");
                    break;
                }
                continue;
            }

            for (i = 0; i < curimsg->maxMsgNum; i++) {//find one incomplete data
                if (curimsg->msgValid[i] == false) {
                    if (curimsg->ifs[i][0].id.src_mac_id == iframe.id.src_mac_id
                        && curimsg->ifs[i][0].id.func_id == iframe.id.func_id
                        && curimsg->ifs[i][0].id.source_id == iframe.id.source_id) {
                        if (curimsg->ifs[i][splitSegm].dlc > 0) {
                            curimsg->msgFrameNum[i] -= 1;//it's not good process
                            lostCnt++;
                            if (lostCnt % 10 == 0) {
                                //printf("can lost %d frame\n\r", lostCnt);
                            }
                        }

                        memcpy(&curimsg->ifs[i][splitSegm],
                               &iframe, sizeof(ican_frame));
                        curimsg->msgValid[i] = false;
                        curimsg->msgFrameNum[i] += 1;
                        curimsg->msgLifeTick[i] = ican_tmr_ms_get();

                        splitSegs = curimsg->ifs[i][0].frame_data[0] & 0x3f;
                        if (curimsg->msgFrameNum[i] == splitSegs) {
                            if ((curimsg->ifs[i][splitSegs - 1].frame_data[0] & 0xc0)
                                == ICAN_SPLIT_SEG_LAST) {
                                curimsg->msgValid[i] = true;
                            }
                            else {
                                for (j = 0; j < splitSegs; j++) {
                                    memset(&curimsg->ifs[i][j], 0x00,
                                           sizeof(ican_frame));
                                }
                                curimsg->msgFrameNum[i] = 0;
                                curimsg->msgValid[i] = false;
                                curimsg->ifs[i][0].id.src_mac_id = 0xff;
                            }
                        }

                        curTick = ican_tmr_ms_get();
                        iframe.dlc = 0;
                        break;
                    }
                    else {
                        continue;
                    }
                }//if (curimsg->msgValid[i]
                else {
                    //printf("no palace\n\r");
                    continue;
                }
            }//for
            if (i == curimsg->maxMsgNum && iframe.dlc > 0) {
                memcpy(&curimsg->ifs[curimsg->wp][0], &iframe, sizeof(ican_frame));
                curimsg->msgValid[curimsg->wp] = false;
                curimsg->msgFrameNum[curimsg->wp] = 1;
                curimsg->msgLifeTick[curimsg->wp] = ican_tmr_ms_get();
                curimsg->wp = (curimsg->wp + 1) % curimsg->maxMsgNum;

                iframe.dlc = 0;
            }
        }//if (iframe.dlc > 0)

        if (ican_tmr_ms_delta(curTick) >= timeout) {            
            break;
        }
    } while (1);

    /* pick up one msg */
    for (j = 0; j < curimsg->maxMsgNum; j++) {
        if (curimsg->msgValid[j] == false) {
            continue;
        }
        if (curimsg->ifs[j][0].id.src_mac_id == src_macid) {
            break;
        }
        if (src_macid == 0xff) {
            break;
        }
    }

    if (j == curimsg->maxMsgNum) {
        return ICAN_ERR_TIME_OUT;
    }

    for (i = 0; i < curimsg->msgFrameNum[j]; i++) {
        memcpy(&ifs[i], &curimsg->ifs[j][i], sizeof(ican_frame));
        memset(&curimsg->ifs[j][i], 0x00, sizeof(ican_frame));      
    }
    *ifs_num = curimsg->msgFrameNum[j];

    curimsg->msgFrameNum[j] = 0;
    curimsg->msgValid[j] = false;
    curimsg->ifs[j][0].id.src_mac_id = 0xff;

    return ICAN_OK;
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

