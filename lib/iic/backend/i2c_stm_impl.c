#include "i2c_stm_impl.h"

#include <memory.h>

#include "../iic.h"

void init_stm32_i2c_privates(CFBD_ST_I2CPrivate* priv,
                             I2C_HandleTypeDef* hi2c,
                             GPIO_TypeDef* scl_port,
                             uint16_t scl_pin,
                             GPIO_TypeDef* sda_port,
                             uint16_t sda_pin)
{
    priv->hi2c = hi2c;
    priv->scl_pin = scl_pin;
    priv->scl_port = scl_port;
    priv->sda_port = sda_port;
    priv->sda_pin = sda_pin;
}

/* forward declear */
void stm32_i2c_on_master_tx_cplt(I2C_HandleTypeDef* hi2c);
void stm32_i2c_on_master_rx_cplt(I2C_HandleTypeDef* hi2c);
void stm32_i2c_on_mem_tx_cplt(I2C_HandleTypeDef* hi2c);
void stm32_i2c_on_mem_rx_cplt(I2C_HandleTypeDef* hi2c);
void stm32_i2c_on_error(I2C_HandleTypeDef* hi2c);

/* forward ops */
static int stm32_init(CFBD_I2CHandle* bus);
static int stm32_deinit(CFBD_I2CHandle* bus);
static int
stm32_transfer(CFBD_I2CHandle* bus, CFBD_I2C_Message* msgs, int num, uint32_t timeout_ms);
static int
stm32_is_device_ready(CFBD_I2CHandle* bus, uint16_t addr, uint32_t trials, uint32_t timeout_ms);
static int stm32_recover_bus(CFBD_I2CHandle* bus);
static int stm32_get_error(CFBD_I2CHandle* bus);

static const CFBD_I2COperations stm32_i2c_ops = {
        .init = stm32_init,
        .deinit = stm32_deinit,
        .transfer = stm32_transfer,
        .is_device_ready = stm32_is_device_ready,
        .recover_bus = stm32_recover_bus,
        .get_error = stm32_get_error,
        .tx_dma_start = NULL,
        .rx_dma_start = NULL,
};

void stm32_i2c_bus_register(CFBD_I2CHandle* bus, CFBD_ST_I2CPrivate* priv)
{
    bus->ops = &stm32_i2c_ops;
    bus->private_handle = priv;
}

/* ---------- lifecycle ---------- */
static int stm32_init(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->private_handle)
        return I2C_ERR_INVAL;
    CFBD_ST_I2CPrivate* p = (CFBD_ST_I2CPrivate*) bus->private_handle;
    if (!p->hi2c)
        return I2C_ERR_INVAL;

    if (HAL_I2C_GetState(p->hi2c) == HAL_I2C_STATE_RESET) {
        if (HAL_I2C_Init(p->hi2c) != HAL_OK) {
            p->last_err = I2C_ERR_IO;
            return I2C_ERR_IO;
        }
    }
    p->last_err = I2C_OK;
    return I2C_OK;
}

static int stm32_deinit(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->private_handle)
        return I2C_ERR_INVAL;
    CFBD_ST_I2CPrivate* p = (CFBD_ST_I2CPrivate*) bus->private_handle;
    if (!p->hi2c)
        return I2C_ERR_INVAL;
    if (HAL_I2C_DeInit(p->hi2c) != HAL_OK) {
        p->last_err = I2C_ERR_IO;
        return I2C_ERR_IO;
    }
    p->last_err = I2C_OK;
    return I2C_OK;
}

static int stm32_transfer(CFBD_I2CHandle* bus, CFBD_I2C_Message* msgs, int num, uint32_t timeout_ms)
{
    if (!bus || !bus->private_handle || !msgs || num <= 0)
        return I2C_ERR_INVAL;
    CFBD_ST_I2CPrivate* p = (CFBD_ST_I2CPrivate*) bus->private_handle;
    if (!p->hi2c)
        return I2C_ERR_INVAL;

    for (int i = 0; i < num; ++i) {
        CFBD_I2C_Message* m = &msgs[i];
        uint16_t devAddr = ((m->addr & 0x7F) << 1);
        HAL_StatusTypeDef status;

        if ((m->flags & I2C_M_RD) == 0) {
            /* write */
            if (i + 1 < num) {
                CFBD_I2C_Message* next = &msgs[i + 1];

                if (!(next->flags & I2C_M_RD) && next->addr == m->addr && m->len == 1 &&
                    next->len == 1) {
                    uint16_t memAddr = m->buf[0];

                    if (p->hi2c->hdmatx != NULL) {
                        status = HAL_I2C_Mem_Write_DMA(p->hi2c,
                                                       devAddr,
                                                       memAddr,
                                                       I2C_MEMADD_SIZE_8BIT,
                                                       next->buf,
                                                       next->len);
                        if (status != HAL_OK) {
                            p->last_err = I2C_ERR_IO;
                            return I2C_ERR_IO;
                        }

                        uint32_t tickstart = HAL_GetTick();
                        while (HAL_I2C_GetState(p->hi2c) != HAL_I2C_STATE_READY) {
                            if ((HAL_GetTick() - tickstart) > timeout_ms) {
                                p->last_err = I2C_ERR_TIMEOUT;
                                return I2C_ERR_TIMEOUT;
                            }
                        }
                    }
                    else {
                        status = HAL_I2C_Mem_Write(p->hi2c,
                                                   devAddr,
                                                   memAddr,
                                                   I2C_MEMADD_SIZE_8BIT,
                                                   next->buf,
                                                   next->len,
                                                   timeout_ms);
                        if (status != HAL_OK) {
                            p->last_err = I2C_ERR_IO;
                            return I2C_ERR_IO;
                        }
                    }

                    i++; // 跳过下一个消息
                    continue;
                }
            }

            if (p->hi2c->hdmatx != NULL) {
                status = HAL_I2C_Master_Transmit_DMA(p->hi2c, devAddr, m->buf, m->len);
                if (status != HAL_OK) {
                    p->last_err = I2C_ERR_IO;
                    return I2C_ERR_IO;
                }

                uint32_t tickstart = HAL_GetTick();
                while (HAL_I2C_GetState(p->hi2c) != HAL_I2C_STATE_READY) {
                    if ((HAL_GetTick() - tickstart) > timeout_ms) {
                        p->last_err = I2C_ERR_TIMEOUT;
                        return I2C_ERR_TIMEOUT;
                    }
                }
            }
            else {
                status = HAL_I2C_Master_Transmit(p->hi2c, devAddr, m->buf, m->len, timeout_ms);
                if (status != HAL_OK) {
                    p->last_err = I2C_ERR_IO;
                    return I2C_ERR_IO;
                }
            }
        }
        else {
            /* read */
            if (i > 0) {
                CFBD_I2C_Message* prev = &msgs[i - 1];
                if (!(prev->flags & I2C_M_RD) && prev->addr == m->addr &&
                    (prev->len == 1 || prev->len == 2)) {
                    uint16_t mem = 0;
                    if (prev->len == 1)
                        mem = prev->buf[0];
                    else
                        mem = ((uint16_t) prev->buf[0] << 8) | prev->buf[1];
                    uint16_t memadd =
                            (prev->len == 1) ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT;

                    if (p->hi2c->hdmarx != NULL) {
                        status =
                                HAL_I2C_Mem_Read_DMA(p->hi2c, devAddr, mem, memadd, m->buf, m->len);
                        if (status != HAL_OK) {
                            p->last_err = I2C_ERR_IO;
                            return I2C_ERR_IO;
                        }

                        uint32_t tickstart = HAL_GetTick();
                        while (HAL_I2C_GetState(p->hi2c) != HAL_I2C_STATE_READY) {
                            if ((HAL_GetTick() - tickstart) > timeout_ms) {
                                p->last_err = I2C_ERR_TIMEOUT;
                                return I2C_ERR_TIMEOUT;
                            }
                        }
                    }
                    else {
                        status = HAL_I2C_Mem_Read(p->hi2c,
                                                  devAddr,
                                                  mem,
                                                  memadd,
                                                  m->buf,
                                                  m->len,
                                                  timeout_ms);
                        if (status != HAL_OK) {
                            p->last_err = I2C_ERR_IO;
                            return I2C_ERR_IO;
                        }
                    }
                    continue;
                }
            }

            // 普通读
            if (p->hi2c->hdmarx != NULL) {
                status = HAL_I2C_Master_Receive_DMA(p->hi2c, devAddr, m->buf, m->len);
                if (status != HAL_OK) {
                    p->last_err = I2C_ERR_IO;
                    return I2C_ERR_IO;
                }

                uint32_t tickstart = HAL_GetTick();
                while (HAL_I2C_GetState(p->hi2c) != HAL_I2C_STATE_READY) {
                    if ((HAL_GetTick() - tickstart) > timeout_ms) {
                        p->last_err = I2C_ERR_TIMEOUT;
                        return I2C_ERR_TIMEOUT;
                    }
                }
            }
            else {
                status = HAL_I2C_Master_Receive(p->hi2c, devAddr, m->buf, m->len, timeout_ms);
                if (status != HAL_OK) {
                    p->last_err = I2C_ERR_IO;
                    return I2C_ERR_IO;
                }
            }
        }
    }

    p->last_err = I2C_OK;
    return I2C_OK;
}

static int
stm32_is_device_ready(CFBD_I2CHandle* bus, uint16_t addr, uint32_t trials, uint32_t timeout_ms)
{
    if (!bus || !bus->private_handle)
        return I2C_ERR_INVAL;
    CFBD_ST_I2CPrivate* p = (CFBD_ST_I2CPrivate*) bus->private_handle;
    if (!p->hi2c)
        return I2C_ERR_INVAL;
    uint16_t devAddr = (addr & 0x7F) << 1;
    if (HAL_I2C_IsDeviceReady(p->hi2c, devAddr, trials, timeout_ms) == HAL_OK) {
        p->last_err = I2C_OK;
        return I2C_OK;
    }
    p->last_err = I2C_ERR_IO;
    return I2C_ERR_IO;
}

static int stm32_recover_bus(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->private_handle)
        return I2C_ERR_INVAL;
    CFBD_ST_I2CPrivate* p = (CFBD_ST_I2CPrivate*) bus->private_handle;
    if (!p->scl_port || !p->sda_port)
        return I2C_ERR_INVAL;

    if (p->hi2c)
        HAL_I2C_DeInit(p->hi2c);

    GPIO_InitTypeDef gpio = {0};

    gpio.Pin = p->scl_pin;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p->scl_port, &gpio);

    gpio.Pin = p->sda_pin;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p->sda_port, &gpio);

    HAL_GPIO_WritePin(p->scl_port, p->scl_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(p->sda_port, p->sda_pin, GPIO_PIN_SET);
    HAL_Delay(1);

    for (int i = 0; i < 9; ++i) {
        if (HAL_GPIO_ReadPin(p->sda_port, p->sda_pin) == GPIO_PIN_SET)
            break;
        HAL_GPIO_WritePin(p->scl_port, p->scl_pin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(p->scl_port, p->scl_pin, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    HAL_GPIO_WritePin(p->sda_port, p->sda_pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(p->scl_port, p->scl_pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(p->sda_port, p->sda_pin, GPIO_PIN_SET);
    HAL_Delay(1);

    gpio.Pin = p->scl_pin;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p->scl_port, &gpio);

    gpio.Pin = p->sda_pin;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p->sda_port, &gpio);

    if (p->hi2c) {
        if (HAL_I2C_Init(p->hi2c) != HAL_OK) {
            p->last_err = I2C_ERR_IO;
            return I2C_ERR_IO;
        }
    }

    p->last_err = I2C_OK;
    return I2C_OK;
}

static int stm32_get_error(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->private_handle)
        return I2C_ERR_INVAL;
    CFBD_ST_I2CPrivate* p = (CFBD_ST_I2CPrivate*) bus->private_handle;
    return p->last_err;
}
