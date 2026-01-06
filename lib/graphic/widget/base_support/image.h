#pragma once
#include "base/point.h"
#include "base/size.h"
#include "cfbd_graphic_define.h"

typedef struct
{
    CFBDGraphic_Point point;
    CFBDGraphicSize image_size;
    uint8_t* sources_register;
} CCGraphic_Image;

void CFBDGraphic_InitImage(CCGraphic_Image* image,
                           CFBDGraphic_Point* tl_point,
                           CFBDGraphicSize* image_size,
                           uint8_t* sources_register);

void CFBDGraphic_DrawImage(CFBD_GraphicDevice* handler, CCGraphic_Image* image);
