#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "cfbd_define.h"
#include "uart.h"

#define RX_BUFFER_SIZE 256
uint8_t uart1_rx_buffer[RX_BUFFER_SIZE];

void uart1_idle_handler(CFBD_UARTHandle* handle, uint32_t received_len)
{
    if (received_len > 0) {
        handle->ops->async_send(handle, handle->rx_buffer, received_len);
    }

    handle->ops->async_receive(handle, handle->rx_buffer, handle->rx_buffer_size);
}

void uart1_send_complete(CFBD_UARTHandle* handle)
{
    /* if we send our session finished, then done, clean this */
    memset(handle->rx_buffer, 0, handle->rx_buffer_size);
}

void uart1_receive_complete(CFBD_UARTHandle* handle)
{
    const char* finished_session = "Receive Done!\r\n";
    handle->ops->async_send(handle, (uint8_t*) finished_session, strlen(finished_session));
}

void uart1_error_handler(CFBD_UARTHandle* handle, uint32_t error_code)
{
    // We do nothing
}

static CFBD_UART_AsyncCallbacks uart1_callbacks = {.on_send_complete = uart1_send_complete,
                                                   .on_receive_complete = uart1_receive_complete,
                                                   .on_error = uart1_error_handler,
                                                   .on_idle_line = uart1_idle_handler};

extern CFBD_UARTHandle uart1_handle;

int main()
{
    /* SomeTimes, we just launch the apps... */
    // CFBDApplication* app =
    getApp(CFBD_TRUE);

    /* We can tell the callbacks */
    uart1_handle.async_callbacks = &uart1_callbacks;

    // 发送欢迎消息
    uint8_t welcome[] = "UART DMA Test Ready!\r\nPlease send data...\r\n";
    uart1_handle.ops->async_send(&uart1_handle, welcome, strlen((char*) welcome));

    // 等待发送完成
    HAL_Delay(100);
    uart1_handle.ops->async_receive(&uart1_handle, uart1_rx_buffer, RX_BUFFER_SIZE);

    uint16_t counter = 0;

    while (1) {
        // 主循环
        HAL_Delay(10000);

        // 周期性发送心跳消息
        char heartbeat[64];
        sprintf(heartbeat, "Heartbeat: %hu\r\n", counter++);
        uart1_handle.ops->async_send(&uart1_handle, (uint8_t*) heartbeat, strlen(heartbeat));
    }
}