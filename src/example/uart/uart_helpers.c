#include "backend/stm-impl-uart.h"

static CFBD_ST_UART_AsyncHelper* g_helper = NULL;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (!g_helper) {
        g_helper = CFBD_GainUARTAsyncHelpers();
    }

    if (g_helper && g_helper->async_send_complete) {
        g_helper->async_send_complete(huart);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (!g_helper) {
        g_helper = CFBD_GainUARTAsyncHelpers();
    }
    if (g_helper && g_helper->async_receive_complete) {
        g_helper->async_receive_complete(huart);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    if (!g_helper) {
        g_helper = CFBD_GainUARTAsyncHelpers();
    }
    CFBD_ST_UARTPrivate* private_data =
            (CFBD_ST_UARTPrivate*) ((uint8_t*) huart - offsetof(CFBD_ST_UARTPrivate, hal_uart));

    CFBD_UARTHandle* handle = private_data->parent_handles;

    if (handle && handle->async_callbacks && handle->async_callbacks->on_error) {
        handle->async_callbacks->on_error(handle, huart->ErrorCode);
    }
}