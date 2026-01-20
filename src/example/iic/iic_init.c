#include "iic_init.h"

#include "ah_no.h"
#include "iic_init.h"

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx; // 新增
DMA_HandleTypeDef hdma_i2c1_rx; // 新增

/* I2C1 init function */
void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        CFBD_AH_NO();
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (i2cHandle->Instance == I2C1) {
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /**I2C1 GPIO Configuration */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* I2C1 clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();

        /* ========== 新增：DMA 配置 ========== */
        /* DMA controller clock enable */
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* I2C1 TX DMA Init (DMA1 Channel 6) */
        hdma_i2c1_tx.Instance = DMA1_Channel6;
        hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
        hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK) {
            CFBD_AH_NO();
        }
        __HAL_LINKDMA(i2cHandle, hdmatx, hdma_i2c1_tx);

        /* I2C1 RX DMA Init (DMA1 Channel 7) */
        hdma_i2c1_rx.Instance = DMA1_Channel7;
        hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
        hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK) {
            CFBD_AH_NO();
        }
        __HAL_LINKDMA(i2cHandle, hdmarx, hdma_i2c1_rx);

        /* DMA interrupt init */
        HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
        HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
        /* ====================================== */

        /* I2C1 interrupt Init */
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0); // 新增错误中断
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
    if (i2cHandle->Instance == I2C1) {
        __HAL_RCC_I2C1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);

        /* DMA DeInit */
        HAL_DMA_DeInit(i2cHandle->hdmatx);
        HAL_DMA_DeInit(i2cHandle->hdmarx);

        /* Peripheral interrupt DeInit */
        HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);
        HAL_NVIC_DisableIRQ(DMA1_Channel7_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    }
}

static CFBD_ST_I2CPrivate iic_params;

CFBD_ST_I2CPrivate* getIICPrivateInits()
{
    init_stm32_i2c_privates(&iic_params, &hi2c1, GPIOB, GPIO_PIN_6, GPIOB, GPIO_PIN_7);
    MX_I2C1_Init();
    return &iic_params;
}

/**
 * @brief This function handles DMA1 channel6 global interrupt (I2C1 TX).
 */
void DMA1_Channel6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}

/**
 * @brief This function handles DMA1 channel7 global interrupt (I2C1 RX).
 */
void DMA1_Channel7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

/**
 * @brief This function handles I2C1 error interrupt.
 */
void I2C1_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&hi2c1);
}
