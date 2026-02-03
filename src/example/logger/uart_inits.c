#include "backend/stm-impl-uart.h"
#include "uart.h"

// 全局变量定义
CFBD_ST_UARTPrivate uart1_private;
CFBD_UARTHandle uart1_handle;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

static void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        // 可以添加LED闪烁等错误指示
    }
}

/**
 * @brief UART1 DMA初始化回调函数
 * @param private_data UART私有数据结构
 */
void uart1_init_callback(CFBD_ST_UARTPrivate* private_data)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    private_data->hal_uart.Instance = USART1;
    private_data->hal_uart.Init.BaudRate = 115200;
    private_data->hal_uart.Init.WordLength = UART_WORDLENGTH_8B;
    private_data->hal_uart.Init.StopBits = UART_STOPBITS_1;
    private_data->hal_uart.Init.Parity = UART_PARITY_NONE;
    private_data->hal_uart.Init.Mode = UART_MODE_TX_RX;
    private_data->hal_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    private_data->hal_uart.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&private_data->hal_uart);

    // TX DMA
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_usart1_tx);
    __HAL_LINKDMA(&private_data->hal_uart, hdmatx, hdma_usart1_tx);

    // RX DMA
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_usart1_rx);
    __HAL_LINKDMA(&private_data->hal_uart, hdmarx, hdma_usart1_rx);

    // 中断优先级
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    // 使能空闲中断
    __HAL_UART_ENABLE_IT(&private_data->hal_uart, UART_IT_IDLE);
}

void DMA1_Channel4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

/**
 * @brief DMA1 Channel5中断处理（USART1 RX）
 */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void USART1_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&uart1_private.hal_uart, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&uart1_private.hal_uart);

        CFBD_ST_UART_AsyncHelper* g_helper = CFBD_GainUARTAsyncHelpers();
        if (g_helper && g_helper->async_idle_line) {
            g_helper->async_idle_line(&uart1_private.hal_uart);
        }
    }

    HAL_UART_IRQHandler(&uart1_private.hal_uart);
}
