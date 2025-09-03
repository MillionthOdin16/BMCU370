#pragma once

#include "main.h"
#include "config.h"

// Debug logging configuration
#define Debug_log_on
#define Debug_log_baudrate DEBUG_UART_BAUDRATE

#ifdef __cplusplus
extern "C"
{
#endif
    extern void Debug_log_init();
    extern uint64_t Debug_log_count64();
    extern void Debug_log_time();
    extern void Debug_log_write(const void *data);
    extern void Debug_log_write_num(const void *data, int num);
    extern void Debug_log_write_float(const void *data, float value, int precision);

#ifdef Debug_log_on
    #define DEBUG_init() Debug_log_init()
    #define DEBUG_MY(logs) Debug_log_write(logs)
    #define DEBUG_num(logs, num) Debug_log_write_num(logs, num)
    #define DEBUG_float(logs, precision) Debug_log_write_float("", logs, precision)
    #define DEBUG_time() Debug_log_time()
    #define DEBUG_get_time() Debug_log_count64()
    #define DEBUG_time_log() DEBUG_time()
#else
    #define DEBUG_init() do {} while(0)
    #define DEBUG_MY(logs) do {} while(0)
    #define DEBUG_num(logs, num) do {} while(0)
    #define DEBUG_float(logs, precision) do {} while(0)
    #define DEBUG_time() do {} while(0)
    #define DEBUG_get_time() (0ULL)
    #define DEBUG_time_log() do {} while(0)
#endif

#ifdef __cplusplus
}
#endif
