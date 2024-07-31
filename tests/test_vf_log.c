#define VF_LOG_IMPLEMENTATION
#include "../vf_log.h"

int main(void) {
    vf_log_config log_config = {
        .display_file = { 0, 0, 0, 0 },
        .display_line = { 0, 0, 0, 0 },
        .display_func = { 0, 0, 0, 0 },

        .outputs = { 0, 0, 0, 0 },
        .logfile_path = "log.txt",
        .handle = stderr
    };

    vf_log_init(&log_config);

    VF_LOG_DEBUG("This is a debug message!");
    VF_LOG_INFO("This is an info message!");
    VF_LOG_WARN("This is a warn message!");
    VF_LOG_ERROR("This is an error message!");

    vf_log_shutdown();

    return 0;
}
