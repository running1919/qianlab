/**
* @file readme.txt
*
* iCAN Project ReadMe
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/
---------------------------------------------------------------------
1.简介
iCAN 协议全称"Industry CAN-bus ApplicatI/On Protocol"，即工业CAN-bus
应用层协议，它向工业控制领域提供了一种易于构建的 CAN-bus 网络，为工业
现场设备(传感器、仪表等)与管理设备(工控机、PLC等)之间的连接提供了一种
低成本的通讯解决方案。

2.目录架构
--
|--can       can驱动层
|--doc       文档
|--ican      协议层
|--include   公共头文件
|--lib       公共lib


3. can驱动层简介
          --------
         |  ican  |
          --------
             ||
          --------
         |   HAL  |
          --------
             ||
          --------
         |can drv |
          --------
HAL层：向上提供了以can_hal_xxx()等函数完成各种平台的can驱动封装，向下
提供了can_xxx()等各种函数接口(这些接口函数需要根据自身硬件平台重新实
现)。

CAN DRV层：这层是根据系统硬件自身平台重新实现，相关函数放在can_xxx.c
文件里。关于CAN协议请查验相关资料。


4.ican协议层简介
首先，该层在can驱动层基础上实现了标准的ican收发函数、报文组包和拆包接
口以及相关数据结构。然后实现master/slave等相关工程，并提供高级用户API
，这些API可以在include目录下ican_API.h看到。
关于ican协议详细介绍请阅读doc/iCAN(1.0).pdf文档。


5.include目录下相关文件说明
*can_config.h
  该文件是对自身硬件CAN驱动的配置，与上层协议无关
*can_type.h
  该文件是对can驱动提供相关数据结构和宏定义
*ican_API.h
  该文件提供了ican协议用户接口API
*ican_config.h
  该文件提供了对ican协议的关键参数配置，里面每个定义均有说明，请使用协
  议之前认真阅读里面每个定义
*ican_type.h
  该文件提供了ican协议所有到所有数据结构和宏定义
*ican_error.h
  该文件定义了ican协议所有标准错误码，以及作者自身扩展的，关于错误码的
  解析请阅读char* ican_error_parse(uint8_t err)函数代码。
