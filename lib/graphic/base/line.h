#pragma once

#include "point.h"

typedef struct
{
    CFBDGraphic_Point p_left;
    CFBDGraphic_Point p_right;
} CFBDGraphic_Line;

void CFBDGraphic_DrawLine(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line);
