/*
*   vf_log - v0.1
*   Header-only tiny logger library.
*
*   RECENT CHANGES:
*       0.11    (2024-06-19)    Added `extern C`;
*       0.1     (2024-06-19)    Creation;
*
*   LICENSE: MIT License
*       Copyright (c) 2024 Viktor Fejes
*
*       Permission is hereby granted, free of charge, to any person obtaining a copy
*       of this software and associated documentation files (the "Software"), to deal
*       in the Software without restriction, including without limitation the rights
*       to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*       copies of the Software, and to permit persons to whom the Software is
*       furnished to do so, subject to the following conditions:
*
*       The above copyright notice and this permission notice shall be included in all
*       copies or substantial portions of the Software.
*
*       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*       IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*       AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*       LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*       SOFTWARE.
*
*   TODO:
*       - [ ] Add platform specific code.
*       - [ ] Add async capability.
*       - [ ] Config handle needs to be sorted out.
*       - [ ] Add more configs...
*
*/
#ifndef VF_LOG_H
#define VF_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>

#define VF_LOG_BUFFER_SIZE 1024

// Levels
#define VF_LOG_LEVEL_DEBUG  0
#define VF_LOG_LEVEL_INFO   1
#define VF_LOG_LEVEL_WARN   2
#define VF_LOG_LEVEL_ERROR  3

// Levels in total for array or other uses...
#define VF_LOG_LEVELS 4

// Define the current log level
#ifndef VF_LOG_LEVEL
// If not in debug mode, only display `warn` and above
#ifdef NDEBUG
#define VF_LOG_LEVEL VF_LOG_LEVEL_WARN
#else
#define VF_LOG_LEVEL VF_LOG_LEVEL_DEBUG
#endif
#endif

// In case users want to use native platform logs
// Which will result in more includes for Windows, Linux, Mac...etc.
#ifdef VF_LOG_USE_NATIVE
#define VF_LOG_NATIVE 1
#else
#define VF_LOG_NATIVE 0
#endif

// Preprocessor for async mode, otherwise the logger is blocking
#ifndef VF_LOG_ASYNC
#define VF_LOG_ASYNC 0
#else
#define VF_LOG_ASYNC 1
#endif

typedef enum {
    VF_LOG_OUTPUT_CONSOLE = 1,
    VF_LOG_OUTPUT_FILE =    2,
    VF_LOG_OUTPUT_OTHER =   4,
} vf_log_output;

typedef struct {
    bool display_file[VF_LOG_LEVELS];
    bool display_line[VF_LOG_LEVELS];
    bool display_func[VF_LOG_LEVELS];

    int outputs[VF_LOG_LEVELS];
    const char* logfile_path;

    FILE* handle;
} vf_log_config;

// Init, shutdown
extern bool vf_log_init(vf_log_config config);
extern void vf_log_shutdown(void);

// Private logging functions. Use through provided macros!
extern void _vf_log_message(int level, const char* file, int line, const char* func, const char* fmt, ...);

#if VF_LOG_LEVEL <= VF_LOG_LEVEL_DEBUG
#define VF_LOG_DEBUG(...) _VF_LOG_DEBUG(__VA_ARGS__);
#define _VF_LOG_DEBUG(...) _vf_log_message(VF_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define VF_LOG_DEBUG(...)
#endif

#if VF_LOG_LEVEL <= VF_LOG_LEVEL_INFO
#define VF_LOG_INFO(...) _VF_LOG_INFO(__VA_ARGS__);
#define _VF_LOG_INFO(...) _vf_log_message(VF_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define VF_LOG_INFO(...)
#endif

#if VF_LOG_LEVEL <= VF_LOG_LEVEL_WARN
#define VF_LOG_WARN(...) _VF_LOG_WARN(__VA_ARGS__);
#define _VF_LOG_WARN(...) _vf_log_message(VF_LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define VF_LOG_WARN(...)
#endif

#if VF_LOG_LEVEL <= VF_LOG_LEVEL_ERROR
#define VF_LOG_ERROR(...) _VF_LOG_ERROR(__VA_ARGS__);
#define _VF_LOG_ERROR(...) _vf_log_message(VF_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define VF_LOG_ERROR(...)
#endif

#ifdef __cplusplus
}
#endif

// END OF HEADER. ------------------------------------------------------------------------

#ifdef VF_LOG_IMPLEMENTATION

#include <stdarg.h>

static vf_log_config _vf_log_internal_state;
static bool vf_log_initialized = false;

// String representation of levels
static const char* vf_log_level_str[VF_LOG_LEVELS] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

// ASCII terminal colors
static const char* vf_log_level_color[VF_LOG_LEVELS + 1] = {
    "\033[96m", // cyan
    "\033[92m", // green
    "\033[93m", // yellow
    "\033[91m", // red

    "\033[0m"   // reset
};

#define VF_LOG_TERM_RESET       "\033[0m"
#define VF_LOG_TERM_UNDERLINE   "\033[4m"

bool vf_log_init(vf_log_config config) {
    if (vf_log_initialized) {
        return false;
    }

    vf_log_initialized = true;

    for (int i = 0; i < VF_LOG_LEVELS; ++i) {
        _vf_log_internal_state.display_file[i] = config.display_file[i];
        _vf_log_internal_state.display_func[i] = config.display_func[i];
        _vf_log_internal_state.display_line[i] = config.display_line[i];
        _vf_log_internal_state.outputs[i] = config.outputs[i];
    }

    _vf_log_internal_state.logfile_path = config.logfile_path;

    _vf_log_internal_state.handle = config.handle;

    return true;
}

void vf_log_shutdown(void) {
    vf_log_initialized = false;
}

void _vf_log_message(int level, const char* file, int line, const char* func, const char* fmt, ...) {
    if (!vf_log_initialized) {
        return;
    }
    
    va_list args;
    va_start(args, fmt);
    fprintf(_vf_log_internal_state.handle, "%s[%s] ", vf_log_level_color[level], vf_log_level_str[level]);
    vfprintf(_vf_log_internal_state.handle, fmt, args);
    fprintf(_vf_log_internal_state.handle, " %s(%s:%d in function `%s`)", VF_LOG_TERM_UNDERLINE, file, line, func);
    fprintf(_vf_log_internal_state.handle, "%s\n", VF_LOG_TERM_RESET);
    va_end(args);
}

#endif // VF_LOG_IMPLEMENTATION
#endif // VF_LOG_H
