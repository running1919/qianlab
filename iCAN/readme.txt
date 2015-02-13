/**
* @file readme.txt
*
* ican test project readme
* 
* Copyright (C) 2014-2015, Qian Runsheng<546515547@qq.com>
*/
---------------------------------------------------------------------
1.软件工程目录架构
__
|--build        IAR工程开发环境
|--doc          技术文档
|--driver       芯片驱动高级封装,与回线单元平台无关
|--iCAN         iCAN通信协议
|--include      与业务无关头文件
|--init         系统核心启动代码
|--libs         芯片厂商提供标准库函数，不包含其他库
|--test         测试用例
|--readme.txt   工程说明文档


2.iCAN协议使用原则
*用户可以调用iCAN/include目录下ican_API.h、ican_error.h、ican_type.h
 文件所提供的函数接口和数据结构等

