# stm32f407 systemview uart recorder 移植使用实现

***

## 1. 准备工作

- [segger官网](https://www.segger.com/products/development-tools/systemview/)下载 [SystemView Target Sources](https://www.segger.com/downloads/systemview/systemview_target_src)用于提取文件移植进stm32f4工程编译。
- [segger官网](https://www.segger.com/products/development-tools/systemview/)下载 [SystemView for Windows, installer](https://www.segger.com/downloads/systemview/systemview_windows_installer) 安装电脑端可视化界面。

## 2. 移植文件目录说明

从 __SystemView Target Sources__ 中提取关键文件如下：

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
    ├── portable
    │   └── systemview_serial_port.c  //uart recorder移植适配文件，需自己实现。
    ├── SEGGER.h //Global header for SEGGER global types and general purpose utility functions.
    ├── SEGGER_RTT.c //SEGGER RTT module source.
    ├── SEGGER_RTT.h //SEGGER RTT module header.
    ├── SEGGER_SYSVIEW.c //SEGGER SYSTEMVIEW module source.
    ├── SEGGER_SYSVIEW.h //SEGGER SYSTEMVIEW module header.
    ├── SEGGER_SYSVIEW_ConfDefaults.h //SEGGER SYSTEMVIEW configuration fallback(配置后备)
    └── SEGGER_SYSVIEW_Int.h //SEGGER SYSTEMVIEW internal header.

## 3. systemview_serial_port.c 移植适配文件说明



## 3. 移植说明

- 将systemview源码添加到工程中
- 编写配置SEGGER_SYSVIEW_Conf.h文件
- 编写配置SEGGER_SYSVIEW_Config_FreeRTOS.c文件
- FreeRTOSConfig.h 文件添加 #include "SEGGER_SYSVIEW_FreeRTOS.h"
- main函数调用SEGGER_SYSVIEW_Conf();初始化配置systemview。

## 4. 串口移植适配

### 4.1. 重要函数说明
- SEGGER_RTT_WriteDownBuffer函数: 将指定数量的字符存储在Down缓冲区的SEGGER RTT控制块中。
- SEGGER_RTT_ReadUpBufferNoLock函数: 用于执行与J-Link相同的操作，以通过其他通道（例如TCP / IP或UART）传输RTT数据。

### 4.2. 遇到的问题

使用DWT->CYCCNT得到的时间有问题，PC软件端无法计算时间。
- 问题定位：用jlink debug模式下使用DWT->CYCCNT可以获取到时间，直接下载代码无法获取时间。
- 问题解决：在没有使用jlink debug的时候，没有使能DWT模块。需要设置CoreDebug->DEMCR寄存器CoreDebug_DEMCR_TRCENA_Msk位为1:DWT and ITM units enabled. 然后使用硬件复位按钮复位系统(不能使用jlink复位)。