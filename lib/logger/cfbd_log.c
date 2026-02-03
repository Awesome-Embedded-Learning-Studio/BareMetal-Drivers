/**
 * @file cfbd_log.c
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2026-02-01
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "cfbd_log.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char* level_tags[] = {
        "V", // VERBOSE
        "D", // DEBUG
        "I", // INFO
        "W", // WARN
        "E", // ERROR
        "F"  // FATAL
};

static const char* level_colors[] = {
        "\033[37m", /* VERBOSE - 白色 */
        "\033[36m", /* DEBUG - 青色 */
        "\033[32m", /* INFO - 绿色 */
        "\033[33m", /* WARN - 黄色 */
        "\033[31m", /* ERROR - 红色 */
        "\033[35m"  /* FATAL - 紫色 */
};

#define COLOR_RESET "\033[0m"

static const char* extract_filename(const char* path)
{
    if (path == NULL) {
        return "";
    }

    const char* filename = path;
    const char* p = path;

    while (*p != '\0') {
        if (*p == '/' || *p == '\\') {
            filename = p + 1;
        }
        p++;
    }

    return filename;
}

static int safe_vsnprintf(char* buffer, size_t size, const char* format, va_list args)
{
    int ret = vsnprintf(buffer, size, format, args);
    if (ret < 0) {
        return 0;
    }
    if ((size_t) ret >= size) {
        return size - 1;
    }
    return ret;
}

static void cfbd_log_message_impl(CFBD_Logger* log_handle,
                                  cfbd_log_level_t level,
                                  const char* tag,
                                  const char* file,
                                  int line,
                                  const char* fmt,
                                  ...)
{
    if (log_handle == NULL || fmt == NULL) {
        return;
    }

    if (level < log_handle->filter_off_level || level >= CFBD_LOG_LEVEL_NONE) {
        return;
    }

    if (log_handle->native_logs == NULL) {
        return;
    }

    /* 静态缓冲区（可以改为动态分配或使用logger自带的缓冲区） */
    char* buffer = (char*) log_handle->buffer_ptr;
    const uint16_t buffer_length = log_handle->buffer_sz;
    uint16_t offset = 0;
    int written;

    /* 添加颜色代码 */
    if (log_handle->enable_colorize && level < CFBD_LOG_LEVEL_NONE) {
        written = snprintf(buffer + offset, buffer_length - offset, "%s", level_colors[level]);
        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    /* 添加时间戳 */
    uint32_t (*timestamp_handle)(void* native_handle) = log_handle->native_logs->timestamp;
    if (timestamp_handle) {
        uint32_t timestamp = timestamp_handle(log_handle->native_handle);
        written = snprintf(buffer + offset,
                           buffer_length - offset,
                           "[%lu] ",
                           (unsigned long) timestamp);
        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    /* 添加日志级别标识 */
    if (level < CFBD_LOG_LEVEL_NONE) {
        written = snprintf(buffer + offset, buffer_length - offset, "[%s] ", level_tags[level]);
        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    /* 添加TAG */
    if (tag != NULL && tag[0] != '\0') {
        written = snprintf(buffer + offset, buffer_length - offset, "[%s] ", tag);
        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    if (file != NULL) {
        const char* filename = extract_filename(file);
        written = snprintf(buffer + offset, buffer_length - offset, "(%s:%d) ", filename, line);
        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    if (offset < buffer_length) {
        va_list args;
        va_start(args, fmt);
        written = safe_vsnprintf(buffer + offset, buffer_length - offset, fmt, args);
        va_end(args);

        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    /* 添加换行符 */
    if (offset < buffer_length - 1) {
        buffer[offset++] = '\r';
        buffer[offset++] = '\n';
    }

    /* 添加颜色重置 */
    if (log_handle->enable_colorize && offset < buffer_length - strlen(COLOR_RESET)) {
        written = snprintf(buffer + offset, buffer_length - offset, "%s", COLOR_RESET);
        if (written > 0 && offset + written < buffer_length) {
            offset += written;
        }
    }

    if (offset >= buffer_length) {
        offset = buffer_length - 1;
    }
    buffer[offset] = '\0';

    log_handle->native_logs->log_native(log_handle->native_handle, buffer, offset);
}

static CFBD_LoggerOperations default_logger_ops = {.log_message = cfbd_log_message_impl};

void CFBD_InitLogger(CFBD_Logger* logger,
                     CFBD_NativeLoggerOperations* native_send,
                     void* native_handle,
                     cfbd_log_level_t filter_level,
                     uint8_t* buffer_ptr,
                     uint16_t buffer_sz)
{
    logger->ops = &default_logger_ops;
    logger->native_logs = native_send;
    logger->filter_off_level = filter_level;

    logger->native_handle = native_handle;
    logger->buffer_ptr = buffer_ptr;
    logger->buffer_sz = buffer_sz;
}
