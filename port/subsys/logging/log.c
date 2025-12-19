#include <zephyr/logging/log.h>

/* Default log module used when a translation unit does not register its
 * own module via LOG_MODULE_REGISTER().
 */
const struct port_log_module_data __log_default_data = {
    .name = "sys",
    .level = LOG_LEVEL_INF
};
