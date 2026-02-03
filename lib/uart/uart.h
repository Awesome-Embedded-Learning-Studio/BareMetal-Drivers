#pragma once

#include <stdint.h>

#include "cfbd_define.h"

typedef struct _CFBD_UARTHandle CFBD_UARTHandle;

typedef enum
{
    SEND,
    RECEIVE
} CFBD_UARTAsyncOp;

typedef void (*CFBD_UARTAsyncCallback)(CFBD_UARTHandle* handle,
                                       uint8_t* buffer,
                                       const uint16_t buffer_size,
                                       CFBD_UARTAsyncOp what_async_done);

typedef struct _CFBD_UARTOperations
{
    void (*sync_send)(CFBD_UARTHandle* handle, const uint8_t* buffer, const uint16_t buffer_size);
    void (*sync_receive)(CFBD_UARTHandle* handle,
                         uint8_t* available_buffer,
                         const uint16_t available_buffer_size);
    void (*async_send)(CFBD_UARTHandle* handle, const uint8_t* buffer, const uint16_t buffer_size);
    void (*async_receive)(CFBD_UARTHandle* handle,
                          uint8_t* available_buffer,
                          const uint16_t available_buffer_size);
} CFBD_UARTOperations;

/**
 * @brief   As C owns no virtual, we need to filled the
 *          function processor by users
 * @note    Thinking Callbacks shits, so place NULL, if we dont want to do anything
 *
 */
typedef struct
{
    void (*on_send_complete)(CFBD_UARTHandle* handle);
    void (*on_receive_complete)(CFBD_UARTHandle* handle);
    void (*on_error)(CFBD_UARTHandle* handle, uint32_t error_code);
    void (*on_idle_line)(CFBD_UARTHandle* handle, uint32_t received_len);
} CFBD_UART_AsyncCallbacks;

typedef struct _CFBD_UARTHandle
{
    CFBD_UARTOperations* ops;
    CFBD_UARTAsyncCallback handle_async_callback;
    uint32_t accept_timeout;

    void* backends; // backends here
    CFBD_UART_AsyncCallbacks* async_callbacks;

    uint8_t* rx_buffer;
    uint16_t rx_buffer_size;
} CFBD_UARTHandle;

// OK, then we need to provide the async helpers
// to enable user level to control the details of everything

typedef struct
{
    void (*async_send_complete)(void* native_data);
    void (*async_receive_complete)(void* native_data);
    void (*async_idle_line)(void* native_data);
} CFBD_ST_UART_AsyncHelper;

/**
 * @brief   Gain the AsyncHelpers that can be used in callbacks
 *          at native frameworks
 *
 * @return CFBD_ST_UART_AsyncHelper*
 */
CFBD_ST_UART_AsyncHelper* CFBD_GainUARTAsyncHelpers();

static inline void CFBD_UARTBindRXProcessBuffer(CFBD_UARTHandle* handle,
                                                uint8_t* rx_buffer,
                                                const uint16_t rx_buffer_size)
{
    handle->rx_buffer_size = rx_buffer_size;
    handle->rx_buffer = rx_buffer;
}

#include "lib_settings.h"
#if defined(CFBD_IS_ST)
#include "backend/stm-impl-uart.h"
#else
#error "No supports chips"
#endif