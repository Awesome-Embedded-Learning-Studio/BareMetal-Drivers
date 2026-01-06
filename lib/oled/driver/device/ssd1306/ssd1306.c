#include "ssd1306.h"
#include <stdint.h>

static uint8_t ssd1306_inits_commands[] = {

};


static uint8_t* ssd1306_init_sessions(void){
    return ssd1306_inits_commands;
}

static CFBD_OLED_DeviceSpecific ssd1306_specific;

CFBD_OLED_DeviceSpecific* getSSD1306Specific(){
    ssd1306_specific.init_session_tables = ssd1306_init_sessions;
    return &ssd1306_specific;
}