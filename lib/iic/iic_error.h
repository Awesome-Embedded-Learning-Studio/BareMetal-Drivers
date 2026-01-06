#pragma once

/* error codes (subset) */
#define I2C_OK 0
#define I2C_ERR_TIMEOUT -110 /* -ETIMEDOUT */
#define I2C_ERR_NACK -121    /* -EPROTO (nack) */
#define I2C_ERR_BUSY -16     /* -EBUSY */
#define I2C_ERR_IO -5        /* -EIO */
#define I2C_ERR_INVAL -22    /* -EINVAL */
