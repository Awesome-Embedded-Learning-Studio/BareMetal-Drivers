#pragma once

#include "point.h"
typedef struct
{
    CFBDGraphic_Point center;
    PointBaseType radius;
    int16_t start_degree;
    int16_t end_degree;
} CCGraphic_Arc;

void CFBDGraphic_DrawArc(CFBD_GraphicDevice* device, CCGraphic_Arc* circle);
void CFBDGraphic_DrawFilledArc(CFBD_GraphicDevice* device, CCGraphic_Arc* circle);
