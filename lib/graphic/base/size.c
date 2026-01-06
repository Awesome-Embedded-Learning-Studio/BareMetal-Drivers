#include "size.h"
#include "cfbd_graphic_define.h"
#include <stddef.h>

void CFBDGraphic_GetScreenSize(CFBD_GraphicDevice* device, CFBDGraphicSize* pSize){
    PointBaseType width, height;
    device->ops->self_consult(device, "width", NULL, &width);
    device->ops->self_consult(device, "height", NULL, &height);
    pSize->height = height;
    pSize->width = width;
}


