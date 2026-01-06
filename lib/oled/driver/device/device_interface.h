#pragma once
#include <stdint.h>

typedef struct
{
    uint8_t* (*init_session_tables)(void);
    uint8_t** grams;
    uint16_t init_session_tables_sz;
    uint8_t data_prefix;
    uint8_t cmd_prefix;
    uint16_t logic_width;
    uint16_t logic_height;
} CFBD_OLED_DeviceSpecific;
