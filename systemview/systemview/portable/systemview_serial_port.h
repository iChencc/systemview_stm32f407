/**
 * @file systemview_serial_port.h
 * @brief systemview串口记录器适配头文件
 * @author JasonChan (x@chencc.cc)
 * @date 2021-05-02
 * @copyright Copyright (c) 2021
 */
#ifndef SYSTEMVIEW_SERIAL_PORT_H
#define SYSTEMVIEW_SERIAL_PORT_H

/**
 * @brief 串口中断服务函数，在中断里面调用
 * 
 * @param data: 串口接收数据
 * @retval : 0
 */
extern int systemview_uart_recorder_rx_isr(uint8_t data);
/**
 * @brief 串口发送空中断服务函数，在中断里面调用
 *  
 */
extern void systemview_uart_recorder_tx_empty_isr(void);
/**
 * @brief systemview串口记录器初始化
 * 
 * @retval -1: error
 * @retval 0: successful
 */
extern int systemview_uart_recorder_init(void);

#endif

