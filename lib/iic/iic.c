#include "iic.h"
#include <stdint.h>

int CFBD_I2CRead(CFBD_I2CHandle* handle, CFBD_I2C_IORequestParams* r){
    if (!handle || !r->data || 
        (r->mem_addr_size!=1 && r->mem_addr_size!=2)){ 
        return I2C_ERR_INVAL;
    }
    
    uint8_t addr_buf[2];
    CFBD_I2C_Message msgs[2];

    if (r->mem_addr_size == 1) {
        addr_buf[0] = (uint8_t)(r->mem_addr & 0xFF);
        msgs[0].len = 1;
        msgs[0].buf = addr_buf;
    } else {
        addr_buf[0] = (uint8_t)((r->mem_addr >> 8) & 0xFF);
        addr_buf[1] = (uint8_t)(r->mem_addr & 0xFF);
        msgs[0].len = 2;
        msgs[0].buf = addr_buf;
    }
    msgs[0].addr = r->addr7;
    msgs[0].flags = 0;

    msgs[1].addr = r->addr7;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = r->len;
    msgs[1].buf = r->data;

    return CFBD_I2CTransfer(handle, msgs, 2, r->timeout_ms);
}

int CFBD_I2CWrite(CFBD_I2CHandle* handle, CFBD_I2C_IORequestParams* r){
    if (!handle || !r->data || 
        (r->mem_addr_size!=1 && r->mem_addr_size!=2)){ 
        return I2C_ERR_INVAL;
    }
    
    uint8_t addr_buf[2];
    CFBD_I2C_Message msgs[2];

    if (r->mem_addr_size == 1) {
        addr_buf[0] = (uint8_t)(r->mem_addr & 0xFF);
        msgs[0].len = 1;
        msgs[0].buf = addr_buf;
    } else {
        addr_buf[0] = (uint8_t)((r->mem_addr >> 8) & 0xFF);
        addr_buf[1] = (uint8_t)(r->mem_addr & 0xFF);
        msgs[0].len = 2;
        msgs[0].buf = addr_buf;
    }
    msgs[0].addr = r->addr7;
    msgs[0].flags = 0;

    msgs[1].addr = r->addr7;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = r->len;
    msgs[1].buf = r->data;

    return CFBD_I2CTransfer(handle, msgs, 2, r->timeout_ms);
}