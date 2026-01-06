#pragma once
#include "cfbd_graphic_define.h"
#include "point.h"

typedef struct
{
    CFBDGraphic_Point center;
    PointBaseType X_Radius;
    PointBaseType Y_Radius;
} CCGraphic_Ellipse;

void CFBDGraphic_DrawEllipse(CFBD_GraphicDevice* device, CCGraphic_Ellipse* ellipse);
void CFBDGraphic_DrawFilledEllipse(CFBD_GraphicDevice* device, CCGraphic_Ellipse* ellipse);
