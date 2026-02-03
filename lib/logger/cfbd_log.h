#pragma once
#include <stdint.h>

#include "cfbd_define.h"

/*
 * Loggers are Loggers, because they are loggers
 */

typedef enum
{
    CFBD_LOG_LEVEL_LOWEST = -1,
    CFBD_LOG_LEVEL_VERBOSE = 0, /* 详细信息 */
    CFBD_LOG_LEVEL_DEBUG,       /* 调试信息 */
    CFBD_LOG_LEVEL_INFO,        /* 一般信息 */
    CFBD_LOG_LEVEL_WARN,        /* 警告信息 */
    CFBD_LOG_LEVEL_ERROR,       /* 错误信息 */
    CFBD_LOG_LEVEL_FATAL,
    CFBD_LOG_LEVEL_NONE
} cfbd_log_level_t;

typedef struct __CFBD_Logger CFBD_Logger;

typedef struct
{
    void (*log_message)(CFBD_Logger* log_handle,
                        cfbd_log_level_t level,
                        const char* tag,
                        const char* file,
                        int line,
                        const char* fmt,
                        ...);
} CFBD_LoggerOperations;

typedef struct
{
    void (*log_native)(void* native_handle, const char* buffer, const uint16_t buffer_size);
    uint32_t (*timestamp)(void* native_handle); // If, native handle can access
} CFBD_NativeLoggerOperations;

typedef struct __CFBD_Logger
{
    CFBD_LoggerOperations* ops;
    CFBD_NativeLoggerOperations* native_logs; // As expected, dont call these direct

    cfbd_log_level_t filter_off_level;

    uint8_t* buffer_ptr;
    uint16_t buffer_sz;
    void* native_handle;

    CFBD_Bool enable_colorize;
} CFBD_Logger;

void CFBD_InitLogger(CFBD_Logger* logger,
                     CFBD_NativeLoggerOperations* native_send,
                     void* native_handle,
                     cfbd_log_level_t filter_level,
                     uint8_t* buffer_ptr,
                     uint16_t buffer_sz);

static inline void CFBD_SetFilteredOffLevel(CFBD_Logger* logger, cfbd_log_level_t filter_off_level)
{
    logger->filter_off_level = filter_off_level;
}

#define CFBD_LOG_V(logger_handle, tag, fmt, ...)                                                   \
    logger_handle->ops->log_message(logger_handle,                                                 \
                                    CFBD_LOG_LEVEL_VERBOSE,                                        \
                                    tag,                                                           \
                                    __FILE__,                                                      \
                                    __LINE__,                                                      \
                                    fmt,                                                           \
                                    ##__VA_ARGS__)

#define CFBD_LOG_D(logger_handle, tag, fmt, ...)                                                   \
    logger_handle->ops->log_message(logger_handle,                                                 \
                                    CFBD_LOG_LEVEL_DEBUG,                                          \
                                    tag,                                                           \
                                    __FILE__,                                                      \
                                    __LINE__,                                                      \
                                    fmt,                                                           \
                                    ##__VA_ARGS__)

#define CFBD_LOG_I(logger_handle, tag, fmt, ...)                                                   \
    logger_handle->ops->log_message(logger_handle,                                                 \
                                    CFBD_LOG_LEVEL_INFO,                                           \
                                    tag,                                                           \
                                    __FILE__,                                                      \
                                    __LINE__,                                                      \
                                    fmt,                                                           \
                                    ##__VA_ARGS__)

#define CFBD_LOG_W(logger_handle, tag, fmt, ...)                                                   \
    logger_handle->ops->log_message(logger_handle,                                                 \
                                    CFBD_LOG_LEVEL_WARN,                                           \
                                    tag,                                                           \
                                    __FILE__,                                                      \
                                    __LINE__,                                                      \
                                    fmt,                                                           \
                                    ##__VA_ARGS__)

#define CFBD_LOG_E(logger_handle, tag, fmt, ...)                                                   \
    logger_handle->ops->log_message(logger_handle,                                                 \
                                    CFBD_LOG_LEVEL_ERROR,                                          \
                                    tag,                                                           \
                                    __FILE__,                                                      \
                                    __LINE__,                                                      \
                                    fmt,                                                           \
                                    ##__VA_ARGS__)

#define CFBD_LOG_F(logger_handle, tag, fmt, ...)                                                   \
    logger_handle->ops->log_message(logger_handle,                                                 \
                                    CFBD_LOG_LEVEL_FATAL,                                          \
                                    tag,                                                           \
                                    __FILE__,                                                      \
                                    __LINE__,                                                      \
                                    fmt,                                                           \
                                    ##__VA_ARGS__)

#define CFBD_LOGV(handle, fmt, ...) CFBD_LOG_V(handle, NULL, fmt, ##__VA_ARGS__)
#define CFBD_LOGD(handle, fmt, ...) CFBD_LOG_D(handle, NULL, fmt, ##__VA_ARGS__)
#define CFBD_LOGI(handle, fmt, ...) CFBD_LOG_I(handle, NULL, fmt, ##__VA_ARGS__)
#define CFBD_LOGW(handle, fmt, ...) CFBD_LOG_W(handle, NULL, fmt, ##__VA_ARGS__)
#define CFBD_LOGE(handle, fmt, ...) CFBD_LOG_E(handle, NULL, fmt, ##__VA_ARGS__)
#define CFBD_LOGF(handle, fmt, ...) CFBD_LOG_F(handle, NULL, fmt, ##__VA_ARGS__)