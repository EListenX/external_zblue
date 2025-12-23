/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <string.h>

static struct port_log_module_data *g_log_modules_list = NULL;
static bool g_log_output_enabled = false;

/* Default module data: used if a translation unit does not register its
 * own module.
 */
struct port_log_module_data __log_default_data = {
    .name = "default",
    .level = CONFIG_BT_DEBUG_LOG_LEVEL,
    .next = NULL,
};

void port_log_module_list_add(struct port_log_module_data *module)
{
    if (module) {
        module->next = g_log_modules_list;
        g_log_modules_list = module;
    }
}

void port_log_module_set_level(const char *module_name, int level)
{
    struct port_log_module_data *current = g_log_modules_list;
    while (current) {
        if (strcmp(current->name, module_name) == 0) {
            current->level = level;
            return;
        }
        current = current->next;
    }
}

int port_log_module_get_level(const char *module_name)
{
    struct port_log_module_data *current = g_log_modules_list;
    while (current) {
        if (strcmp(current->name, module_name) == 0) {
            return current->level;
        }
        current = current->next;
    }

    if (strcmp(__log_default_data.name, module_name) == 0) {
        return __log_default_data.level;
    }

    return -1; /* Not found */
}

void port_log_output_enable(void)
{
    g_log_output_enabled = true;
}

void port_log_output_disable(void)
{
    g_log_output_enabled = false;
}

bool port_log_output_is_enabled(void)
{
    return g_log_output_enabled;
}
