#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "os_log.h"
#include "stm32f4xx_hal.h"

#define UART_LOG_BUFF_SIZE  1024

extern UART_HandleTypeDef huart2;
/**
  * @功能:字符串打印
  * @参数: 1.str字符串的首地址
  * @返回值:打印的字符个数
  */
int _puts(char* str)
{
    int counter=0;
    if(!str)
    {
        return 0;
    }
    while(*str && (counter < UART_LOG_BUFF_SIZE))
    {
        while ((huart2.Instance->SR & USART_SR_TXE) == 0)
        {
            ;
        }
        huart2.Instance->DR = *str++;
        counter++;
    }
    return counter;
}
/**
 * @brief 打印函数
 * 
 * @param format: 
 * @param ...: 
 * @retval ***: 
 */
int __printf(char* format,...)
{
    static char printk_string[UART_LOG_BUFF_SIZE] = {0};

    va_list args;
    int plen;

    va_start(args, format);
    plen = vsnprintf(printk_string, sizeof(printk_string)/sizeof(char) - 1, format, args);
    _puts(printk_string);
    va_end(args);
    return plen;
}

