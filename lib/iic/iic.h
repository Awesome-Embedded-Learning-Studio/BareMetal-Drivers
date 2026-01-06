#pragma once
#include <stddef.h>
#include <stdint.h>

#include "iic_error.h"

/* i2c_msg flags (like Linux) */
#define I2C_M_RD 0x0001      /* read data, from slave to master */
#define I2C_M_NOSTART 0x4000 /* no start between messages (rare) */

typedef struct _CFBD_I2C_Message
{
    uint16_t addr;  /* 7-bit address (0..0x7F) */
    uint16_t flags; /* I2C Message Flags */
    uint16_t len;   /* length of buffer */
    uint8_t* buf;   /* pointer to data */
} CFBD_I2C_Message;

/*
 * Async Callbacks
 */
typedef void CFBD_I2C_AsyncCallback(int status, void* arg);

typedef struct _CFBD_I2CHandle CFBD_I2CHandle;

typedef struct
{
    /*
     * init the i2c
     */
    int (*init)(CFBD_I2CHandle* handle);
    /*
     * deinit the i2c
     */
    int (*deinit)(CFBD_I2CHandle* handle);

    int (*transfer)(CFBD_I2CHandle* bus, CFBD_I2C_Message* msgs, int num, uint32_t timeout_ms);

    int (*is_device_ready)(CFBD_I2CHandle* bus,
                           uint16_t addr,
                           uint32_t trials,
                           uint32_t timeout_ms);
    int (*recover_bus)(CFBD_I2CHandle* bus);

    int (*get_error)(CFBD_I2CHandle* bus);

    /* optional DMA hooks */
    int (*tx_dma_start)(CFBD_I2CHandle* bus, const uint8_t* buf, size_t len);
    int (*rx_dma_start)(CFBD_I2CHandle* bus, uint8_t* buf, size_t len);
} CFBD_I2COperations;

typedef void* CFBD_I2CPrivateHandle;

typedef struct _CFBD_I2CHandle
{
    const CFBD_I2COperations* ops;
    CFBD_I2CPrivateHandle private_handle;
} CFBD_I2CHandle;

/* --------- inline wrappers  ---------- */
static inline int CFBD_I2CInit(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->ops || !bus->ops->init)
        return I2C_ERR_INVAL;
    return bus->ops->init(bus);
}

static inline int CFBD_I2CDeInit(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->ops || !bus->ops->deinit)
        return I2C_ERR_INVAL;
    return bus->ops->deinit(bus);
}

static inline int
CFBD_I2CTransfer(CFBD_I2CHandle* bus, CFBD_I2C_Message* msgs, int num, uint32_t timeout_ms)
{
    if (!bus || !bus->ops || !bus->ops->transfer)
        return I2C_ERR_INVAL;
    return bus->ops->transfer(bus, msgs, num, timeout_ms);
}

static inline int CFBD_I2CTransferIsDeviceReady(CFBD_I2CHandle* bus,
                                                uint16_t addr,
                                                uint32_t trials,
                                                uint32_t timeout_ms)
{
    if (!bus || !bus->ops || !bus->ops->is_device_ready)
        return I2C_ERR_INVAL;
    return bus->ops->is_device_ready(bus, addr, trials, timeout_ms);
}

static inline int CFBD_I2CRecoverBus(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->ops || !bus->ops->recover_bus)
        return I2C_ERR_INVAL;
    return bus->ops->recover_bus(bus);
}

typedef struct
{
    uint16_t addr7;
    uint32_t mem_addr;
    uint8_t mem_addr_size;
    uint8_t* data;
    uint16_t len;
    uint32_t timeout_ms;
} CFBD_I2C_IORequestParams;

int CFBD_I2CRead(CFBD_I2CHandle* handle, CFBD_I2C_IORequestParams* r);
int CFBD_I2CWrite(CFBD_I2CHandle* handle, CFBD_I2C_IORequestParams* r);

#include "lib_settings.h"
#if defined(CFBD_IS_ST)
#include "backend/i2c_stm_impl.h"
#else
#error "No supports chips"
#endif