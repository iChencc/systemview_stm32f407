# systemview移植使用说明

***

## 1. 移植文件目录说明

    systemview/
    ├── Config
    │   ├── Global.h  //Global type definitions for SEGGER code
    │   ├── SEGGER_RTT_Conf.h //SEGGER Real Time Transfer (RTT) configuration file
    │   └── SEGGER_SYSVIEW_Conf.h //SEGGER SYSTEMVIEW configuration file
    ├── FreeRTOSV10
    │   ├── Config
    │   │   └── SEGGER_SYSVIEW_Config_FreeRTOS.c //Initialization of SystemView for FreeRTOS
    │   ├── SEGGER_SYSVIEW_FreeRTOS.c //Interface between SYSTEMVIEW and FreeRTOS
    │   └── SEGGER_SYSVIEW_FreeRTOS.h //Interface header
    ├── port
    │   └── SYSVIEW_ci112x_serial.c  //移植适配层
    ├── SEGGER.h //Global header for SEGGER global types and general purpose utility functions.
    ├── SEGGER_RTT.c //SEGGER RTT module source.
    ├── SEGGER_RTT.h //SEGGER RTT module header.
    ├── SEGGER_RTT_printf.c //RTT打印模块，用jlink代替串口打印。
    ├── SEGGER_SYSVIEW.c //SEGGER SYSTEMVIEW module source.
    ├── SEGGER_SYSVIEW.h //SEGGER SYSTEMVIEW module header.
    ├── SEGGER_SYSVIEW_ConfDefaults.h //SEGGER SYSTEMVIEW configuration fallback(配置后备)
    └── SEGGER_SYSVIEW_Int.h //SEGGER SYSTEMVIEW internal header.

## 2. 移植说明

- 将systemview源码添加到工程中
- 编写配置SEGGER_SYSVIEW_Conf.h文件
- 编写配置SEGGER_SYSVIEW_Config_FreeRTOS.c文件
- FreeRTOSConfig.h 文件添加 #include "SEGGER_SYSVIEW_FreeRTOS.h"
- main函数调用SEGGER_SYSVIEW_Conf();初始化配置systemview。

## 3. 串口移植适配

### 3.1. 重要函数说明
- SEGGER_RTT_WriteDownBuffer函数: 将指定数量的字符存储在Down缓冲区的SEGGER RTT控制块中。
- SEGGER_RTT_ReadUpBufferNoLock函数: 用于执行与J-Link相同的操作，以通过其他通道（例如TCP / IP或UART）传输RTT数据。

### 3.2. 遇到的问题

使用DWT->CYCCNT得到的时间有问题，PC软件端无法计算时间（不清楚问题出在哪里）。
