#include "image.h"

#include "device/graphic_device.h"


void CFBDGraphic_InitImage(CCGraphic_Image* image,
                           CFBDGraphic_Point* tl_point,
                           CFBDGraphicSize* image_size,
                           uint8_t* sources_register)
{
    image->image_size = *image_size;
    image->point = *tl_point;
    image->sources_register = sources_register;
}

void CFBDGraphic_DrawImage(CFBD_GraphicDevice* handler, CCGraphic_Image* image)
{
    if (!image->sources_register)
        return;
    handler->ops->setArea(
        handler, image->point.x, image->point.y,
        image->image_size.width, image->image_size.height, image->sources_register
    );
}