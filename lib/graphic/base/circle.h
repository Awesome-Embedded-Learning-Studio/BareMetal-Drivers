#pragma once
#include "cfbd_graphic_define.h"
#include "point.h"

typedef struct
{
    SizeBaseType radius;
    CFBDGraphic_Point center;
} CFBDGraphicCircle;

void CFBDGraphic_DrawCircle(CFBD_GraphicDevice* device, CFBDGraphicCircle* circle);
void CFBDGraphic_DrawFilledCircle(CFBD_GraphicDevice* device, CFBDGraphicCircle* circle);
