/**
 * @file systemview_serial_port.c
 * @brief stm32f407 systemview uart recorder 移植适配文件
 * @author JasonChan (x@chencc.cc)
 * @date 2021-05-19
 * @copyright Copyright (c) 2021
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"

#include "os_log.h"

/*** (systemview uart recorder sample) ----------------------------------------------------------*/
#define _SERVER_HELLO_SIZE (4)
#define _TARGET_HELLO_SIZE (4)

// "Hello" message expected by SysView: [ 'S', 'V', <PROTOCOL_MAJOR>, <PROTOCOL_MINOR> ]
static uint8_t _abHelloMsg[_TARGET_HELLO_SIZE] = \
    {'S', 'V', (SEGGER_SYSVIEW_VERSION / 10000), 
    (SEGGER_SYSVIEW_VERSION / 1000) % 10}; 

/**
 * @brief systemview信息
 * 
 */
static struct SVInfo_t
{
    uint8_t NumBytesHelloRcvd;  // 接收hello info计数
    uint8_t NumBytesHelloSent;  // 发送hello info计数
    int ChannelID;
} _SVInfo;
/**
 * @brief This function starts and initializes a SystemView session
 * 
 */
static void _StartSysView(void)
{
    int r;

    r = SEGGER_SYSVIEW_IsStarted();
    if (r == 0)
    {
        SEGGER_SYSVIEW_Start();
    }
}
/**
 * @brief This function is called when the UART receives data.
 * 
 * @param Data: 串口原始数据
 */
static void _cbOnRx(uint8_t Data)
{
    if (_SVInfo.NumBytesHelloRcvd < _SERVER_HELLO_SIZE)
    {
        if (_SVInfo.NumBytesHelloRcvd < 2)
        {   
            //  recv 'SV' to start
            if (Data != _abHelloMsg[_SVInfo.NumBytesHelloRcvd])
            {
                return;
            }
        }
        _SVInfo.NumBytesHelloRcvd++;

        if (_SVInfo.NumBytesHelloRcvd == _SERVER_HELLO_SIZE - 1)
        {
            _StartSysView();
        }
    }
    else 
    {
        // Write data into corresponding RTT buffer 
        // for application to read and handle accordingly
        SEGGER_RTT_WriteDownBuffer(_SVInfo.ChannelID, &Data, 1);
        if (SEGGER_SYSVIEW_IsStarted() == 0)
        {
            // when recv stop command clear NumBytesHelloRcvd
            _SVInfo.NumBytesHelloRcvd = 0;
        }
    }
}
/**
 * @brief This function is called when the UART should transmit data.
 * 
 * @param pChar: 得到需要发送的数据
 * @retval 0: 没有需要发送的数据
 * @retval > 0: 需要发送的数据数量
 */
static int _cbOnTx(uint8_t *pChar)
{
    int r;

    if (_SVInfo.NumBytesHelloSent < _TARGET_HELLO_SIZE)
    { // Not all bytes of <Hello> message sent to SysView yet?
        *pChar = _abHelloMsg[_SVInfo.NumBytesHelloSent];
        _SVInfo.NumBytesHelloSent++;
        r = 1;
        goto Done;
    }
    r = SEGGER_RTT_ReadUpBufferNoLock(_SVInfo.ChannelID, pChar, 1);
    if (r < 0)
    { // Failed to read from up buffer?
        r = 0;
    }
Done:
    return r;
}
/*** (uart hardware) ----------------------------------------------------------*/
#include "stm32f4xx_hal.h"
extern UART_HandleTypeDef huart1;

void uart_hw_enable_tx_empty_interrupt(void)
{
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
}
void uart_hw_disable_tx_empty_interrupt(void)
{
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
}
/**
 * @brief 串口中断服务函数，在中断里面调用
 * 
 * @param data: 串口接收数据
 * @retval : 0
 */
int systemview_uart_recorder_rx_isr(uint8_t data)
{
    _cbOnRx(data);
    return 0;
}
/**
 * @brief 串口发送空中断服务函数，在中断里面调用
 *  
 */
void systemview_uart_recorder_tx_empty_isr(void)
{
    uint8_t data;

    if (_cbOnTx(&data) == 1)
    {
        huart1.Instance->DR = data;
    }
    else
    {
        uart_hw_disable_tx_empty_interrupt();
    }
}
/**
 * @brief 串口硬件发送函数
 * 
 * @param pData: 发送数据指针
 * @param size: 数据大小
 */
void uart_hw_tx(uint8_t *pData, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        huart1.Instance->DR = *pData++;
        while ((huart1.Instance->SR & USART_SR_TXE) == 0)
        {
            ;
        }
    }
}
/**
 * @brief 串口发送函数
 * 
 * @param tx_size: 发送数据大小
 * @retval : 0
 */
int uart_tx_func(size_t tx_size)
{
    uart_hw_enable_tx_empty_interrupt();
    return 0;
}
/*** (时间戳) ----------------------------------------------------------*/
/**
 * @brief cpu 计数cycle初始化
 * 
 * @retval : 0
 */
int cpu_cycle_count_enable(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    CIAS_LOG_INFO("DWT->CTRL reg:%x", DWT->CTRL);

    return 0;
}
/*** (对外接口，初始化) ----------------------------------------------------------*/
/**
 * @brief systemview串口记录器初始化
 * 
 * @retval -1: error
 * @retval 0: successful
 */
int systemview_uart_recorder_init(void)
{
    SEGGER_SYSVIEW_Conf();

    _SVInfo.ChannelID = SEGGER_SYSVIEW_GetChannelID();
    _SVInfo.NumBytesHelloRcvd = 0;
    _SVInfo.NumBytesHelloSent = 0;
    //PORT 串口初始化
    // uart_init();
    cpu_cycle_count_enable();

    return 0;
}
/*** (end) ----------------------------------------------------------*/
