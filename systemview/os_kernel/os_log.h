/**
 * @file cias_log.h
 * @brief 打印日志
 * @author JasonChan (x@chencc.cc)
 * @date 2021-04-18
 * @copyright Copyright (c) 2021
 */
#ifndef _CIAS_LOG_H_
#define _CIAS_LOG_H_

#define __LOG_DISABLE       0
#define __LOG_ERROR         1
#define __LOG_WARN          2
#define __LOG_HIGHLIGHT     3
#define __LOG_INFO          4
#define __LOG_DEBUG         5
/**
 * 定义断言打印等级
 * 0：关闭断言
 * 1：打开断言，关闭断言打印
 * 2：简单打印，可以节约代码RO段大小
 * 3：断言打印，将打印断言出错的文件和行数
*/
#ifndef CIAS_LOG_ASSERT_EN  
#define CIAS_LOG_ASSERT_EN  3
#endif
/**
 * @brief 定义打印等级
 * 
 */
#ifndef CIAS_LOG_LEVEL
#define CIAS_LOG_LEVEL      __LOG_DEBUG
#endif     
/**
 * @brief 定义打印模块
 * 
 */
#ifndef CIAS_LOG_MODULE
#define CIAS_LOG_MODULE     "[LOG]"
#endif
/**
 * @brief 配置适配接口
 * 
 */
extern int __printf(char* format,...);
#define CIAS_LOG_AGENT                      __printf
/**
 * @brief 欢迎语
 * 
 */
#define CIAS_WELCOME                        CIAS_LOG_AGENT("\r\n  \\ | /\r\n- CI AS - "__DATE__", "__TIME__"\r\n  / | \\\r\n")
/**
 * @brief 打印定义
 * 
 */
#if (CIAS_LOG_LEVEL >= __LOG_HIGHLIGHT)
    #define CIAS_LOG_HL(fmt, ...)           CIAS_LOG_AGENT(CIAS_LOG_MODULE" \033[1;32m"fmt"\033[0;39m\r\n", ##__VA_ARGS__)
#else 
    #define CIAS_LOG_HL(fmt, ...)
#endif
#if (CIAS_LOG_LEVEL >= __LOG_ERROR)
    #define CIAS_LOG_ERR(fmt, ...)          CIAS_LOG_AGENT(CIAS_LOG_MODULE" \033[31m"fmt"\033[0m\r\n", ##__VA_ARGS__)
#else
    #define CIAS_LOG_ERR(fmt, ...)
#endif
#if (CIAS_LOG_LEVEL >= __LOG_WARN)
    #define CIAS_LOG_WARN(fmt, ...)         CIAS_LOG_AGENT(CIAS_LOG_MODULE" \033[33m"fmt"\033[0m\r\n", ##__VA_ARGS__)
#else 
    #define CIAS_LOG_WARN(fmt, ...)
#endif
#if (CIAS_LOG_LEVEL >= __LOG_INFO)
    #define CIAS_LOG_INFO(fmt, ...)         CIAS_LOG_AGENT(CIAS_LOG_MODULE" "fmt"\r\n", ##__VA_ARGS__)
#else 
    #define CIAS_LOG_INFO(fmt, ...)
#endif
#if (CIAS_LOG_LEVEL >= __LOG_DEBUG)
    #define CIAS_LOG_DEBUG(fmt, ...)        CIAS_LOG_AGENT(CIAS_LOG_MODULE" \033[36m"fmt"\033[0m\r\n", ##__VA_ARGS__)
#else 
    #define CIAS_LOG_DEBUG(fmt, ...)
#endif
/**
 * @brief 断言定义
 * 
 */
#if (CIAS_LOG_ASSERT_EN == 2)
#define __LOG_ASSERT(EX)                    CIAS_LOG_AGENT("\r\n\n"CIAS_LOG_MODULE" \033[31massert failed!!!\033[0m \r\n\n")
#elif (CIAS_LOG_ASSERT_EN == 3)
#define __LOG_ASSERT(EX)                    CIAS_LOG_AGENT(CIAS_LOG_MODULE" \033[31m["__FILE__":%d] ("#EX")failed!!!\033[0m\r\n", __LINE__)
#else
#define __LOG_ASSERT(EX)
#endif

#if (CIAS_LOG_ASSERT_EN > 0)
#define CIAS_ASSERT(EX)                     do{\
                                                if (!(EX)){\
                                                    __LOG_ASSERT(EX);\
                                                }\
                                            }while(0)
#define CIAS_ASSERT_GOFAIL(EX)              do{\
                                                if (!(EX)){\
                                                    __LOG_ASSERT(EX);\
                                                    goto __fail;\
                                                }\
                                            }while(0)       
#define CIAS_ASSERT_RETURN(EX)              do{\
                                                if (!(EX)){\
                                                    __LOG_ASSERT(EX);\
                                                    return -1;\
                                                }\
                                            }while(0)       

#define CIAS_ASSERT_RETURN_NULL(EX)         do{\
                                                if (!(EX)){\
                                                    __LOG_ASSERT(EX);\
                                                    return NULL;\
                                                }\
                                            }while(0)       
#else
#define CIAS_ASSERT(EX)
#define CIAS_ASSERT_GOFAIL(EX)
#define CIAS_ASSERT_RETURN(EX)
#define CIAS_ASSERT_RETURN_NULL(EX)
#endif
/**
 * @brief 编译过程中打印#define变量 例如#pragma message(CIAS_PRINT_DEFINE(sendto))
 * 
 */
#define __PRINT_DEFINE(x)       #x  
#define CIAS_PRINT_DEFINE(x)    #x"="__PRINT_DEFINE(x)


#endif
