#include "ssd1309.h"

#include <stdint.h>


static uint8_t ssd1309_inits_commands[] = {
        0xAE,       // Turn off OLED panel
        0xFD, 0x12, // Set display clock divide ratio/oscillator frequency
        0xD5,       // Set display clock divide ratio
        0xA0,       // Set multiplex ratio
        0xA8,       // Set multiplex ratio (1 to 64)
        0x3F,       // 1/64 duty
        0xD3,       // Set display offset
        0x00,       // No offset
        0x40,       // Set start line address
        0xA1,       // Set SEG/Column mapping (0xA0 for reverse, 0xA1 for normal)
        0xC8,       // Set COM/Row scan direction (0xC0 for reverse, 0xC8 for normal)
        0xDA,       // Set COM pins hardware configuration
        0x12,       // COM pins configuration
        0x81,       // Set contrast control register
        0xBF,       // Set SEG output current brightness
        0xD9,       // Set pre-charge period
        0x25,       // Set pre-charge as 15 clocks & discharge as 1 clock
        0xDB,       // Set VCOMH
        0x34,       // Set VCOM deselect level
        0xA4,       // Disable entire display on
        0xA6,       // Disable inverse display on
        0xAF        // Turn on the display
};

#define CMD_TABLE_SZ ((sizeof(ssd1309_inits_commands)) / sizeof(ssd1309_inits_commands[0]))

static uint8_t* ssd1309_init_sessions(void)
{
    return ssd1309_inits_commands;
}

static CFBD_OLED_DeviceSpecific ssd1309_specific;

CFBD_OLED_DeviceSpecific* getSSD1309Specific()
{
    ssd1309_specific.init_session_tables = ssd1309_init_sessions;
    ssd1309_specific.init_session_tables_sz = CMD_TABLE_SZ;
    ssd1309_specific.cmd_prefix = 0x00;
    ssd1309_specific.data_prefix = 0x40;
    ssd1309_specific.logic_height = 64;
    ssd1309_specific.logic_width = 128;

    return &ssd1309_specific;
}