#pragma once
#include "cfbd_graphic_define.h"
#include "device/graphic_device.h"
typedef struct
{
    SizeBaseType width;
    SizeBaseType height;
} CFBDGraphicSize;

void CFBDGraphic_GetScreenSize(CFBD_GraphicDevice* device, CFBDGraphicSize* pSize);
