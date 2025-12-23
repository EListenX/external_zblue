/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_H_

#include <nuttx/config.h>
#include <syslog.h>
#include <stdio.h>
#include <stdbool.h>

/* Basic helper macros */
#define _LOG_CONCAT(x, y) x ## y
#define LOG_CONCAT(x, y)  _LOG_CONCAT(x, y)

#define _GET_ARG_N(_1, _2, _3, _4, _5, N, ...)  N
#define _NUM_ARGS(...) _GET_ARG_N(__VA_ARGS__, 5, 4, 3, 2, 1, 0)

/* Log level mapping (syslog-compatible). */
#define PORT_LOG_EMERG   0
#define PORT_LOG_ALERT   1
#define PORT_LOG_CRIT    2
#define PORT_LOG_ERR     3
#define PORT_LOG_WARNING 4
#define PORT_LOG_NOTICE  5
#define PORT_LOG_INFO    6
#define PORT_LOG_DEBUG   7

/* Zephyr-like aliases (optional) */
#define LOG_LEVEL_NONE PORT_LOG_EMERG
#define LOG_LEVEL_ERR  PORT_LOG_ERR
#define LOG_LEVEL_WRN  PORT_LOG_WARNING
#define LOG_LEVEL_INF  PORT_LOG_INFO
#define LOG_LEVEL_DBG  PORT_LOG_DEBUG

#ifndef CONFIG_BT_DEBUG_LOG_LEVEL
#define CONFIG_BT_DEBUG_LOG_LEVEL LOG_LEVEL_INF
#endif

#ifndef CONFIG_BT_LOG_LEVEL
#define CONFIG_BT_LOG_LEVEL CONFIG_BT_DEBUG_LOG_LEVEL
#endif

#ifndef CONFIG_NET_BUF_LOG_LEVEL
#define CONFIG_NET_BUF_LOG_LEVEL CONFIG_BT_DEBUG_LOG_LEVEL
#endif

#ifndef CONFIG_BT_L2CAP_LOG_LEVEL
#define CONFIG_BT_L2CAP_LOG_LEVEL CONFIG_BT_DEBUG_LOG_LEVEL
#endif

struct port_log_module_data {
    const char *name;
    int level;
    struct port_log_module_data *next;
};

void port_log_default_module_init(void);
void port_log_module_list_add(struct port_log_module_data *module);
void port_log_module_set_level(const char *module_name, int level);
int port_log_module_get_level(const char *module_name);

void port_log_output_enable(void);
void port_log_output_disable(void);
bool port_log_output_is_enabled(void);

/* Default log module data (implemented in log.c). */
extern struct port_log_module_data __log_default_data;

/* Each translation unit that includes this header and does not call
 * LOG_MODULE_REGISTER() will use this default module data.
 */
static struct port_log_module_data *__log_module __attribute__((used)) = &__log_default_data;

/* Registration macros: set up per-module log data and update __log_module
 * at runtime using constructor functions.
 */
#define _LOG_MODULE_REGISTER_1(module_name) \
    static struct port_log_module_data __log_data_##module_name = { \
        .name = #module_name, \
        .level = CONFIG_BT_DEBUG_LOG_LEVEL, \
        .next = NULL, \
    }; \
    static void __attribute__((constructor)) __log_init_##module_name(void) { \
        __log_module = &__log_data_##module_name; \
        port_log_module_list_add(&__log_data_##module_name); \
    }

#define _LOG_MODULE_REGISTER_2(module_name, _level) \
    static struct port_log_module_data __log_data_##module_name = { \
        .name = #module_name, \
        .level = _level, \
        .next = NULL, \
    }; \
    static void __attribute__((constructor)) __log_init_##module_name(void) { \
        __log_module = &__log_data_##module_name; \
        port_log_module_list_add(&__log_data_##module_name); \
    }

#define _LOG_MODULE_REGISTER_N(N, ...) LOG_CONCAT(_LOG_MODULE_REGISTER_, N)(__VA_ARGS__)

#define LOG_MODULE_REGISTER(...) _LOG_MODULE_REGISTER_N(_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define LOG_MODULE_DECLARE(...) LOG_MODULE_REGISTER(...)

/* Core logging implementation: forward messages to syslog() when enabled
 * by both the global Bluetooth log level and the per-module log level.
 */
#define PORT_LOG(_level, lvl_str, fmt, ...) do { \
    if (port_log_output_is_enabled() && (_level <= CONFIG_BT_LOG_LEVEL)) { \
        if (_level <= __log_module->level) { \
            syslog(_level, "[%s] %s " fmt "\n", \
                   __log_module->name, \
                   lvl_str, \
                   ##__VA_ARGS__); \
        } \
    } \
} while(0)

/* Public logging macros. Undefine potential definitions from syslog.h
 * before providing the Zephyr-style APIs.
 */
#undef LOG_DBG
#undef LOG_INF
#undef LOG_WRN
#undef LOG_ERR

#define LOG_ERR(fmt, ...) PORT_LOG(LOG_LEVEL_ERR, "<err>", fmt, ##__VA_ARGS__)
#define LOG_WRN(fmt, ...) PORT_LOG(LOG_LEVEL_WRN, "<wrn>", fmt, ##__VA_ARGS__)
#define LOG_INF(fmt, ...) PORT_LOG(LOG_LEVEL_INF, "<inf>", fmt, ##__VA_ARGS__)
#define LOG_DBG(fmt, ...) PORT_LOG(LOG_LEVEL_DBG, "<dbg>", fmt, ##__VA_ARGS__)

/* Hexdump logging helpers: currently implemented as no-ops. */
#define LOG_HEXDUMP_INF(_data, _length, _str)
#define LOG_HEXDUMP_DBG(_data, _length, _str)

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_H_ */
