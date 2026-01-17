#include "ssd1327.h"

#include <stdint.h>

#include "../oled_ssd132x_privates.h"
#include "configs/iic_pack_type.h"

static uint8_t ssd1327_init_commands[] = {
        0xAE, // Display OFF

        0xA0, 0x51, // Remap (keep 0x51 for your module initial test)
        0xA1, 0x00, // Display Start Line = 0
        0xA2, 0x20, // Display Offset = 32 (128 - 96)

        0xA4,       // Normal display
        0xA8, 0x5F, // MUX = 96 - 1
        0xAB, 0x01, // Enable internal VDD
        0x81, 0x77, // Contrast
        0xB1, 0x31, 0xB3, 0xB1, 0xB5, 0x03, 0xB6, 0x0D, 0xBC, 0x07, 0xBE, 0x07, 0xD5, 0x02,
        0xAF // Display ON
};

#define CMD_TABLE_SZ ((sizeof(ssd1327_init_commands)) / sizeof(ssd1327_init_commands[0]))

static uint8_t* ssd1327_init_sessions(void)
{
    return ssd1327_init_commands;
}

static CFBD_OLED_DeviceSpecific ssd1327_specific;
static SSD132XPrivateDatas ssd1327_private_data = {.grey_scale = 0x05};

CFBD_OLED_DeviceSpecific* getSSD1327Specific()
{
    ssd1327_specific.init_session_tables = ssd1327_init_sessions;
    ssd1327_specific.init_session_tables_sz = CMD_TABLE_SZ;
    ssd1327_specific.cmd_prefix = 0x00;
    ssd1327_specific.data_prefix = 0x40;
    ssd1327_specific.logic_height = 96;
    ssd1327_specific.logic_width = 128;
    ssd1327_specific.iic_pack_type = SSD132X_REQUEST_IIC_PACK;
    ssd1327_specific.private_data = &ssd1327_private_data;
    return &ssd1327_specific;
}