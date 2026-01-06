#pragma once
#include <stdint.h>

#include "cfbd_define.h"
#include "cfbd_graphic_define.h"

typedef struct _CFBD_Menu CFBD_Menu;

typedef struct __CFBD_MenuIndicator
{
    uint8_t width; // width of the sliding indicator(pixels), zero for none paints
    uint8_t x;     // x-position of indicator
} CFBD_MenuIndicator;

void CFBD_InitDefaultMenuIndicator(CFBD_MenuIndicator* menuIndicatorDefault);

CFBD_Bool CFBD_DrawMenuIndicator(CFBD_Menu* menu, uint16_t x, uint16_t y, uint16_t w, uint16_t h);