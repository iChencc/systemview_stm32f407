# stm32f407 systemview uart recorder 移植使用实现

***

## 1. 准备工作

- [segger官网](https://www.segger.com/products/development-tools/systemview/)下载 [SystemView Target Sources](https://www.segger.com/downloads/systemview/systemview_target_src)用于提取文件移植进stm32f4工程编译。
- [segger官网](https://www.segger.com/products/development-tools/systemview/)下载 [SystemView for Windows, installer](https://www.segger.com/downloads/systemview/systemview_windows_installer) 安装电脑端可视化界面。
- stm32f407 systemview uart recorder 移植实现源码[git仓库地址](https://gitee.com/WTSO/stm32f407_systemview) 。

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

### 3.1. SystemView记录器缓冲区数据串口发送

```c
extern int uart_tx_func(size_t tx_size);
#define SEGGER_SYSVIEW_ON_EVENT_RECORDED(NumBytes)  uart_tx_func(NumBytes)
```

- SystemView构建数据包后通知记录器缓冲区中新事件的数据个数将调用 __SEGGER_SYSVIEW_ON_EVENT_RECORDED__ 功能宏。可以通过重新定义此宏串口发送记录器缓冲区中的数据。此功能宏定义的函数不能是阻塞的，并且运行一次此函数用时要非常小，降低SystemView对系统的影响。
- 可以一次读取完缓冲区里面的数据，通过串口DMA一次发送出去。
- 也可以在 __SEGGER_SYSVIEW_ON_EVENT_RECORDED__ 功能宏中开启串口发送空闲中断，通过中断每次1byte连续发送数据，当记录器缓冲区没有数据时关闭串口发送空闲中断。

### 3.2. 移植关键函数说明

- SEGGER_RTT_WriteDownBuffer函数: 将指定数量的字符存储在Down缓冲区的SEGGER RTT控制块中。
- SEGGER_RTT_ReadUpBufferNoLock函数: 用于执行与J-Link相同的操作，以通过其他通道（例如TCP / IP或UART）传输RTT数据。

### 3.3. Cortex-M4核DTW(Data Watchpoint and Trace)模块开启CPU Cycle Count功能

SystemView当定义

```c
#define SEGGER_SYSVIEW_CORE SEGGER_SYSVIEW_CORE_CM3
```

时，记录器获取时间戳的来源是DTW->CYCCNT(地址0xE0001004)寄存器。记录CPU运行的cycle，CPU正常运行的话其周期就是CPU的频率。

- 遇到问题：用jlink debug模式下使用DWT->CYCCNT可以获取到时间，当直接下载代码无法获取时间。
- 问题定位：在没有使用jlink debug的时候，没有使能DWT模块。
- 问题解决：需要设置CoreDebug->DEMCR寄存器CoreDebug_DEMCR_TRCENA_Msk位为1(DWT and ITM units enabled)。并且需要使用硬件复位按钮复位系统(不能使用jlink复位)，这是由于jlink复位按钮不会复位CoreDebug模块，并且通过jlink下载程序后，jlink对CoreDebug有控制权，CPU没有，程序写入其寄存器无效。

## 4. 移植步骤

- 将systemview源码添加到工程中
- 编写配置SEGGER_SYSVIEW_Conf.h文件
- 编写配置SEGGER_SYSVIEW_Config_FreeRTOS.c文件
- FreeRTOSConfig.h 文件添加 #include "SEGGER_SYSVIEW_FreeRTOS.h"
- main函数调用SEGGER_SYSVIEW_Conf();初始化配置systemview。
