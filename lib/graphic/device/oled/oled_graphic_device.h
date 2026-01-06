#pragma once
#include "../oled/oled.h"
#include "device/graphic_device.h"

/* binders */
void CFBDGraphic_BindOLEDAsDevice(CFBD_GraphicDevice* device, CFBD_OLED* oled);
