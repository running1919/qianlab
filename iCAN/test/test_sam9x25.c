/**
* @file test_sam9x25.c
*
* ican example
* 
* Copyright (C) 2014-2018, Qian Runsheng<546515547@qq.com>
*/


#include <board.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../iCAN/lib/ican_timer.h"
#include "../iCAN/lib/ican_stdio.h"
#include "../iCAN/include/ican_API.h"
#include "../iCAN/include/ican_type.h"


#define CAN_AS_MASTER
#ifndef CAN_AS_MASTER
ierr_t isrd_di(uint8_t src_id, uint8_t length, uint8_t* buff)
{
    uint8_t data[30] = "i am di";
    ican_printf("read %d di\n\r", src_id);
    memcpy(buff, data, length);

    return ICAN_OK;
}

ierr_t isrd_ai(uint8_t src_id, uint8_t length, uint8_t* buff)
{
    uint8_t data[30] = "i am ai";
    ican_printf("read %d ai\n\r", src_id);
    memcpy(buff, data, length);

    return ICAN_OK;
}

ierr_t isrd_serial0(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint8_t data[30] = "i am serial0";
    ican_printf("read %d serial0\n\r", src_id);
    memcpy(buf, data, length);

    return ICAN_OK;
}

ierr_t isrd_serial1(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint8_t data[30] = "i am serial1";
    ican_printf("read %d serial1\n\r", src_id);
    memcpy(buf, data, length);

    return ICAN_OK;
}

ierr_t isrd_userdef(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint8_t data[30] = "i am userdef";
    ican_printf("read %d userdef\n\r", src_id);
    memcpy(buf, data, length);

    return ICAN_OK;
}

ierr_t isrd_device_info(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint8_t data[30] = "i am device info";
    ican_printf("read %d device info\n\r", src_id);
    memcpy(buf, data, length);

    return ICAN_OK;
}

ierr_t isrd_communicate(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint8_t data[30] = "i am communicate";
    ican_printf("read %d communicate\n\r", src_id);
    memcpy(buf, data, length);

    return ICAN_OK;
}

ierr_t isrd_io_param(uint8_t src_id, uint8_t subsrc_id, uint8_t length, uint8_t* buff)
{
    uint8_t data[30] = "i am io param";
    ican_printf("read %d %d io param\n\r", src_id, subsrc_id);
    memcpy(buff, data, length);

    return ICAN_OK;
}

ierr_t isrd_io_config(uint8_t src_id, uint8_t subsrc_id, uint8_t length, uint8_t* buff)
{
    uint8_t data[30] = "i am io config";
    ican_printf("read %d %d io config\n\r", src_id, subsrc_id);
    memcpy(buff, data, length);

    return ICAN_OK;
}

ierr_t iswr_do(uint8_t src_id, uint16_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d do, %d bytes data\n\r", src_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t iswr_ao(uint8_t src_id, uint16_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d ao, %d bytes data\n\r", src_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t iswr_serial0(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d serial0, %d bytes data\n\r", src_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t iswr_serial1(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d serial1, %d bytes data\n\r", src_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t iswr_userdef(uint8_t src_id, uint8_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d userdef, %d bytes data\n\r", src_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t iswr_io_param(uint8_t src_id, uint8_t subsrc_id, uint16_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d %d io param, %d bytes data\n\r", src_id, subsrc_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t iswr_io_config(uint8_t src_id, uint8_t subsrc_id, uint16_t length, uint8_t* buf)
{
    uint16_t i;
    ican_printf("write %d %d io config, %d bytes data\n\r", src_id, subsrc_id, length);
    for (i = 0; i < length; i++) {
        ican_printf("%c", buf[i]);
    }
    ican_printf("\n\r");

    return ICAN_OK;
}

ierr_t is_cycle_timeout(uint8_t* src_id, uint8_t* buff, uint8_t* length)
{
    uint8_t data[] = "cycle timeout";

    *src_id = 0x22;
    memcpy(buff, data, sizeof(data));
    *length = sizeof(data);

    return ICAN_OK;
}

ierr_t is_check_event(uint8_t* src_id, uint8_t* buff, uint8_t* size)
{
    uint8_t data[ICAN_SPLIT_MAX_BYTES] = {0, };

    *src_id = 0x11;    
    memset(data, 0x0, sizeof(data));
    sprintf((char*)data, "current data 0x%08x", ican_tmr_ms_get());
    memcpy(buff, data, strlen((char*)data));
    *size = strlen((char*)data);


    return ICAN_OK;
}
#endif


void sysint_handler(void)
{
    /* Clear interrupt */
    PIT_GetPIVR();
    AIC->AIC_ICCR = 1 << ID_SYS;

    TimeTick_Increment(1);

    ican_tmr_irq_proc();
}


void main(void)
{
    /*Disable watchdog */
    WDT_Disable(WDT);
    IRQ_DisableIT(ID_FIQ);
    
    /* Uses USART0 instead of DBGU */
    DBGU_ConsoleUseUSART0();

    /* Open sys interrupt and pit interrupt */
    IRQ_ConfigureIT(ID_SYS, 5, sysint_handler);
    IRQ_EnableIT(ID_SYS);
    TimeTick_Configure(BOARD_MCK);    

    ican_printf("\n\n\r**************** Test iCAN *******************\n\n\r");

#ifdef CAN_AS_MASTER
    uint8_t slave_macid;
    ierr_t ret;
    uint8_t i, j;
    const uint8_t slave_num = ican_master_get_slave_num();

    /**** 1. init can as master ****/
    while (!ican_master_init(ICAN_MASTER_CHANNEL)){
        ican_printf("restart init\n\r");
    }

    /**** 2. build all slave with master connecting ****/
    for (i = 0; i < slave_num; i++) {
        slave_macid = ican_master_get_slave_macid(i);

        /* 2.1 macid check for verify the device is online */
        ret = ican_master_checkmac(slave_macid);
        if (ret != ICAN_ERR_MAC_EXIST) {//if equal, node is exit
            ican_printf("%x slave doesn't on line\n\r", slave_macid);
            continue;
        }
        
        /* 2.2 build connect */
        j = 0;
        while (j++ < 3) {
            ican_printf("%d times connect with %x slave\n\r", j, slave_macid);
            ret = ican_master_connect(slave_macid);
        
            if (ret != ICAN_OK) {            
                ican_tmr_ms_delay((CYCLIC_MASTER + 1) * CYCLIC_UNIT_MS);
                continue;
            }
            break;
        }

#if 1
        /* 2.3 read slave device info and write comunicate info */
        bool newest = true;
        ican_dev_info devinfo;
        ican_com_info cominfo;
        ican_io_param ioparam;


        ret = im_read_slave_devinfo(slave_macid, &devinfo, newest);
        if (ret == ICAN_OK) {
            ican_printf("\n\r vd: %x\n\r pt: %x\n\r pc: %x\n\r hv: %x \
                        \n\r fv: %x\n\r sn: %x\n\r",
                        devinfo.vendor_id, devinfo.product_type, devinfo.product_code,
                        devinfo.hardware_version, devinfo.firmware_version, devinfo.serial_number);
        }
        else {
            ican_printf("read dev info failed: %s\n\r", ican_error_parse(ret));
        }

        ret = im_read_slave_cominfo(slave_macid, &cominfo, newest);
        if (ret == ICAN_OK) {
            ican_printf("\n\r dm: %x\n\r br: %x\n\r ub: %x\n\r cp: %x \
                        \n\r cm: %x\n\r ct: %x\n\r mm: %x\n\r",
                        cominfo.dev_mac_id, cominfo.baud_rate, cominfo.user_baud_rate, cominfo.cyclic_param,
                        cominfo.cyclic_master, cominfo.cos_type, cominfo.master_mac_id);
        }
        else {
            ican_printf("read com info failed\n\r: %s\n\r", ican_error_parse(ret));
        }

        cominfo.user_baud_rate = 0x20150119;
        ret = im_write_slave_cominfo(slave_macid, &cominfo);
        if (ret == ICAN_OK) {
            ican_printf("\n\r dm: %x\n\r br: %x\n\r ub: %x\n\r cp: %x \
                        \n\r cm: %x\n\r ct: %x\n\r mm: %x\n\r",
                        cominfo.dev_mac_id, cominfo.baud_rate, cominfo.user_baud_rate, cominfo.cyclic_param,
                        cominfo.cyclic_master, cominfo.cos_type, cominfo.master_mac_id);
        }
        else {
            ican_printf("write com info failed\n\r: %s\n\r", ican_error_parse(ret));
        }

        ret = im_read_slave_ioparam(slave_macid, &ioparam, newest);
        if (ret == ICAN_OK) {
            ican_printf("\n\r di: %x\n\r do: %x\n\r ai: %x\n\r ao: %x \
                        \n\r s0: %x\n\r s1: %x\n\r ud: %x\n\r",
                        ioparam.di_length, ioparam.do_length, ioparam.ai_length, ioparam.ao_length,
                        ioparam.serial0_length, ioparam.serial1_length, ioparam.userdef_length);
        }
        else {
            ican_printf("read io param failed\n\r: %s\n\r", ican_error_parse(ret));
        }
#endif
    }

#if 1
    /**** 3. poll all device event ****/
    uint8_t eventBuff[ICAN_SPLIT_MAX_BYTES] = {0, };
    uint8_t ioBuf[ICAN_SPLIT_MAX_BYTES] = {0,};
    uint8_t eventSize = 0;
    uint8_t pool_macid = 0xff;//0xff means poll all slave
    uint8_t source_id = 0;
    uint32_t delta1_start = ican_tmr_ms_get();
    uint32_t delta2_start = ican_tmr_ms_get();
    uint8_t k = 0;
    ican_printf("\n\r********** Test Event Func **********\n\r");
    while (1) {
        /* 3.1check event */
        ret = ican_master_poll(&pool_macid, &source_id, eventBuff, &eventSize);
        if (ret == ICAN_OK) {
            ican_printf("\n\rslave:0x%x source:0x%x, size: %d \n\r",
                        pool_macid, source_id, eventSize);
            for (i = 0; i < eventSize; i++) {
                ican_printf("%c", eventBuff[i]);
            }
            ican_printf("\n\r");            
            //break;
        }

        if (ret == ICAN_ERR_BUS_EXCEPT || ret == ICAN_ERR_BUS_RESTART) {
            break;
        }
        pool_macid = 0xff;

        /*3.2 read write io src */        
        if (ican_tmr_ms_delta(delta1_start) > 500) {
            for (j = 0; j < slave_num; j++) {
                if (ican_master_check_slave_status(j)) {
                    slave_macid = ican_master_get_slave_macid(j);  
                    ican_printf("\n\rslave macid %x: \n\r", slave_macid);
                    ican_master_read(slave_macid, ICAN_SRC_DI_START, 0, ioBuf, ICAN_DI_LEN);
                    ican_printf("\n\rdi:      ");
                    for (i = 0; i < ICAN_DI_LEN; i++) {
                        ican_printf("%x ", ioBuf[i]);
                    }
                    ican_printf("\n\r");

                    ican_master_read(slave_macid, ICAN_SRC_AI_START, 0, ioBuf, ICAN_AI_LEN);
                    ican_printf("ai:      ");
                    for (i = 0; i < ICAN_AI_LEN; i++) {
                        ican_printf("%x ", ioBuf[i]);
                    }
                    ican_printf("\n\r");

                    ican_master_read(slave_macid, ICAN_SRC_SERIAL0_START, 0, ioBuf, ICAN_SER0_LEN);
                    ican_printf("serial:  ");
                    for (i = 0; i < ICAN_SER0_LEN; i++) {
                        ican_printf("%x ", ioBuf[i]);
                    }
                    ican_printf("\n\r");

                    memset(ioBuf, 0, ICAN_USER_LEN);
                    ican_master_read(slave_macid, ICAN_SRC_USERDEF_START, 0, ioBuf, ICAN_USER_LEN);
                    ican_printf("userdef:  ");
                    for (i = 0; i < ICAN_USER_LEN; i++) {
                        ican_printf("%x ", ioBuf[i]);
                    }
                    ican_printf("\n\r");

                    delta1_start = ican_tmr_ms_get();
                }
            }
        }

        if (ican_tmr_ms_delta(delta2_start) > 1000) {
            for (j = 0; j < slave_num; j++) {
                if (ican_master_check_slave_status(j)) {
                    slave_macid = ican_master_get_slave_macid(j);
                    ++k;
                    memset(ioBuf, k, ICAN_DO_LEN);
                    ican_master_write(slave_macid, ICAN_SRC_DO_START, 0, ioBuf, ICAN_DO_LEN);
                    memset(ioBuf, k + 1, ICAN_DO_LEN);
                    ican_master_write(slave_macid, ICAN_SRC_AO_START, 0, ioBuf, ICAN_AO_LEN);

                    delta2_start = ican_tmr_ms_get();
                }
            }
        }
    }
#endif

    /**** 4.delete all connect ****/
    for (i = 0; i < slave_num; i++) {
        slave_macid = ican_master_get_slave_macid(i);

        /* 4.1 check slave status */
        if (!ican_master_check_slave_status(i)) {
            continue;
        }

        /* 4.2 delete connect */
        j = 0;
        while (j++ < 3) {
            ret = ican_master_disconnect(slave_macid);            
            if (ret != ICAN_OK) {                   
                continue;
            }

            break;
        }
    }
#else

    uint8_t dibuf[ICAN_DI_LEN];
    uint8_t dobuf[ICAN_DO_LEN];
    uint8_t aibuf[ICAN_AI_LEN];
    uint8_t aobuf[ICAN_AO_LEN];
    uint8_t serialbuf[ICAN_SER0_LEN];
    uint8_t userdefbuf[ICAN_USER_LEN];
    uint8_t i, j;
    is_file_operations_t ifops;

#if 0
    ifops.is_rd_di = isrd_di;
    ifops.is_rd_ai = isrd_ai;
    ifops.is_rd_serial0 = isrd_serial0;
    ifops.is_rd_serial1 = isrd_serial1;
    ifops.is_rd_userdef = isrd_userdef;
    //ifops.is_rd_devinfo = isrd_device_info;
    //ifops.is_rd_cominfo = isrd_communicate;
    //ifops.is_rd_ioparam = isrd_io_param;
    //ifops.is_rd_ioconfig = isrd_io_config;    
    
    ifops.is_wr_do = iswr_do;
    ifops.is_wr_ao = iswr_ao;
    ifops.is_wr_serial0 = iswr_serial0;
    ifops.is_wr_serial1 = iswr_serial1;
    ifops.is_wr_userdef = iswr_userdef;
    ifops.is_wr_ioparam = iswr_io_param;
    ifops.is_wr_ioconfig = iswr_io_config;
#endif

    ifops.is_cycle_timeout = is_cycle_timeout;
    ifops.is_check_event = is_check_event;
    
    ican_slave_init(ICAN_SLAVE_CHANNEL, ican_slave_get_macid(0));
    ican_slave_set_fops(&ifops);

    uint32_t delta1_start = ican_tmr_ms_get();
    uint32_t delta2_start = ican_tmr_ms_get();
    while (1) {
        ican_slave_poll(ICAN_SLAVE_CHANNEL);        

        if (ican_tmr_ms_delta(delta1_start) > 100){
            i++;
            memset(dibuf, i, sizeof(dibuf));            
            is_simulate_write_di(ICAN_SRC_DI_START, dibuf, sizeof(dibuf));

            memset(aibuf, i + 1, sizeof(aibuf));
            is_simulate_write_ai(ICAN_SRC_AI_START, aibuf, sizeof(aibuf));

            memset(serialbuf, i + 2, sizeof(serialbuf));
            is_simulate_write_serail(0, ICAN_SRC_SERIAL0_START, serialbuf,
                                     sizeof(serialbuf));

            memset(userdefbuf, i + 3, sizeof(userdefbuf));
            is_simulate_write_userdef(ICAN_SRC_USERDEF_START, serialbuf,
                                      sizeof(serialbuf));

            delta1_start = ican_tmr_ms_get();
        }

        if (ican_tmr_ms_delta(delta2_start) > 800) {
            is_simulate_read_do(ICAN_SRC_DO_START, dobuf, sizeof(dobuf));
            ican_printf("\n\rdo: ");
            for (j = 0; j < ICAN_DO_LEN; j++) {
                ican_printf("%x ", dobuf[j]);
            }
            ican_printf("\n\r");
            
            is_simulate_read_ao(ICAN_SRC_AO_START, aobuf, sizeof(aobuf));
            ican_printf("\n\rao: ");
            for (j = 0; j < ICAN_AO_LEN; j++) {
                ican_printf("%x ", aobuf[j]);
            }
            ican_printf("\n\r");

            delta2_start = ican_tmr_ms_get();
        }
    }
#endif
}

