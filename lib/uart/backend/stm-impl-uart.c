#include "stm-impl-uart.h"

static inline UART_HandleTypeDef* native_uart_handle(CFBD_UARTHandle* handle)
{
    return (UART_HandleTypeDef*) handle->backends;
}

static void
uart_sync_send(CFBD_UARTHandle* handle, const uint8_t* buffer, const uint16_t buffer_size)
{
    HAL_UART_Transmit(native_uart_handle(handle), buffer, buffer_size, handle->accept_timeout);
}

static void uart_sync_receive(CFBD_UARTHandle* handle,
                              uint8_t* available_buffer,
                              const uint16_t available_buffer_size)
{
    HAL_UART_Receive(native_uart_handle(handle),
                     available_buffer,
                     available_buffer_size,
                     handle->accept_timeout);
}

static void
uart_async_send(CFBD_UARTHandle* handle, const uint8_t* buffer, const uint16_t buffer_size)
{
    UART_HandleTypeDef* huart = native_uart_handle(handle);

    if (HAL_UART_Transmit_DMA(huart, (uint8_t*) buffer, buffer_size) != HAL_OK) {
        HAL_UART_Transmit_IT(huart, (uint8_t*) buffer, buffer_size);
    }
}

static void uart_async_receive(CFBD_UARTHandle* handle,
                               uint8_t* available_buffer,
                               const uint16_t available_buffer_size)
{
    UART_HandleTypeDef* huart = native_uart_handle(handle);
    handle->rx_buffer_size = available_buffer_size;
    handle->rx_buffer = available_buffer;

    if (HAL_UART_Receive_DMA(huart, available_buffer, available_buffer_size) != HAL_OK) {
        HAL_UART_Receive_IT(huart, available_buffer, available_buffer_size);
    }
}
static CFBD_UARTOperations ops = {
        .sync_send = uart_sync_send,
        .sync_receive = uart_sync_receive,
        .async_send = uart_async_send,
        .async_receive = uart_async_receive,
};

void CFBD_Init_STUARTBackend(CFBD_UARTHandle* handle,
                             uint32_t timeouts,
                             CFBD_ST_UARTPrivate* st_packs,
                             void (*inits_callback)(CFBD_ST_UARTPrivate* init_callbacks))
{
    handle->ops = &ops;
    handle->backends = st_packs;

    handle->accept_timeout = timeouts;
    handle->rx_buffer = NULL;
    handle->rx_buffer_size = 0;

    st_packs->parent_handles = handle;
    /* Call backs for the inits */
    inits_callback(st_packs);
}

static void async_send_helper(void* native_data)
{
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*) native_data;

    CFBD_ST_UARTPrivate* private_data =
            (CFBD_ST_UARTPrivate*) ((uint8_t*) huart - offsetof(CFBD_ST_UARTPrivate, hal_uart));
    CFBD_UARTHandle* handle = private_data->parent_handles;

    if (handle && handle->async_callbacks && handle->async_callbacks->on_send_complete) {
        handle->async_callbacks->on_send_complete(handle);
    }
}

static void async_receive_helper(void* native_data)
{
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*) native_data;
    CFBD_ST_UARTPrivate* private_data =
            (CFBD_ST_UARTPrivate*) ((uint8_t*) huart - offsetof(CFBD_ST_UARTPrivate, hal_uart));

    CFBD_UARTHandle* handle = private_data->parent_handles;
    if (handle && handle->async_callbacks && handle->async_callbacks->on_receive_complete) {
        handle->async_callbacks->on_receive_complete(handle);
    }
}

static uint32_t CFBD_UART_GetDMARxRemaining(CFBD_UARTHandle* handle)
{
    UART_HandleTypeDef* huart = native_uart_handle(handle);

    if (huart->hdmarx != NULL) {
        return __HAL_DMA_GET_COUNTER(huart->hdmarx);
    }

    return 0;
}

static uint32_t CFBD_UART_GetDMARxReceivedCount(CFBD_UARTHandle* handle)
{
    uint32_t remaining = CFBD_UART_GetDMARxRemaining(handle);
    if (remaining == 0) {
        return handle->rx_buffer_size;
    }

    return handle->rx_buffer_size - remaining;
}

static void async_idle_line_helper(void* native_data)
{
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*) native_data;

    CFBD_ST_UARTPrivate* private_data =
            (CFBD_ST_UARTPrivate*) ((uint8_t*) huart - offsetof(CFBD_ST_UARTPrivate, hal_uart));

    CFBD_UARTHandle* handle = private_data->parent_handles;

    if (handle && handle->async_callbacks && handle->async_callbacks->on_idle_line) {
        uint32_t remaining = CFBD_UART_GetDMARxRemaining(handle);
        uint32_t received_len = handle->rx_buffer_size - remaining;

        HAL_UART_DMAStop(huart);

        handle->async_callbacks->on_idle_line(handle, received_len);
    }
}

static CFBD_ST_UART_AsyncHelper g_async_helper = {.async_send_complete = async_send_helper,
                                                  .async_receive_complete = async_receive_helper,
                                                  .async_idle_line = async_idle_line_helper};

CFBD_ST_UART_AsyncHelper* CFBD_GainUARTAsyncHelpers()
{
    return &g_async_helper;
}