#pragma once

#include "point.h"
typedef struct
{
    CFBDGraphic_Point p1;
    CFBDGraphic_Point p2;
    CFBDGraphic_Point p3;
} CFBDGraphic_Triangle;

void CCGraphic_DrawTriangle(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle);
void CCGraphic_DrawFilledTriangle(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle);