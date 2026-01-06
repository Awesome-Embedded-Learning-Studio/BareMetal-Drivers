#include "oled_graphic_device.h"

#include <stddef.h>

#include "cfbd_define.h"


static inline CFBD_OLED* _get_oled(CFBD_GraphicDevice* device)
{
    return (CFBD_OLED*) device->internal_handle;
}

/* ---------- init ---------- */

static int graphic_oled_init(CFBD_GraphicDevice* device, void* init_args)
{
    CFBD_OLED* oled = _get_oled(device);
    return oled->ops->init(oled, init_args);
}

/* ---------- pixel ---------- */

static CFBD_Bool graphic_oled_setPixel(CFBD_GraphicDevice* device, uint16_t x, uint16_t y)
{
    CFBD_OLED* oled = _get_oled(device);
    return oled->ops->setPixel(oled, x, y);
}

static CFBD_Bool graphic_oled_drawArea(CFBD_GraphicDevice* device,
                                       uint16_t x,
                                       uint16_t y,
                                       uint16_t width,
                                       uint16_t height,
                                       uint8_t* source)
{
    CFBD_OLED* oled = _get_oled(device);
    return oled->ops->setArea(oled, x, y, width, height, source);
}
/* ---------- frame ---------- */

static CFBD_Bool graphic_oled_update(CFBD_GraphicDevice* device)
{
    return _get_oled(device)->ops->update(_get_oled(device));
}

static CFBD_Bool graphic_oled_clear(CFBD_GraphicDevice* device)
{
    return _get_oled(device)->ops->clear(_get_oled(device));
}

static CFBD_Bool graphic_oled_revert(CFBD_GraphicDevice* device)
{
    return _get_oled(device)->ops->revert(_get_oled(device));
}

/* ---------- area ---------- */

static CFBD_Bool
graphic_oled_update_area(CFBD_GraphicDevice* device, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    return _get_oled(device)->ops->update_area(_get_oled(device), x, y, w, h);
}

static CFBD_Bool
graphic_oled_clear_area(CFBD_GraphicDevice* device, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    return _get_oled(device)->ops->clear_area(_get_oled(device), x, y, w, h);
}

static CFBD_Bool
graphic_oled_revert_area(CFBD_GraphicDevice* device, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    return _get_oled(device)->ops->revert_area(_get_oled(device), x, y, w, h);
}

/* ---------- lifecycle ---------- */

static CFBD_Bool graphic_oled_open(CFBD_GraphicDevice* device)
{
    return _get_oled(device)->ops->open(_get_oled(device));
}

static CFBD_Bool graphic_oled_close(CFBD_GraphicDevice* device)
{
    return _get_oled(device)->ops->close(_get_oled(device));
}

/* ---------- query ---------- */

static CFBD_Bool graphic_oled_self_consult(CFBD_GraphicDevice* device,
                                           const char* property,
                                           void* args,
                                           void* request_data)
{
    return _get_oled(device)->ops->self_consult(_get_oled(device), property, args, request_data);
}

static CFBD_GraphicDeviceOperation graphic_oled_ops = {
        .init = graphic_oled_init,
        .setPixel = graphic_oled_setPixel,
        .setArea = graphic_oled_drawArea,

        .update = graphic_oled_update,
        .clear = graphic_oled_clear,
        .revert = graphic_oled_revert,

        .update_area = graphic_oled_update_area,
        .clear_area = graphic_oled_clear_area,
        .revert_area = graphic_oled_revert_area,

        .open = graphic_oled_open,
        .close = graphic_oled_close,

        .self_consult = graphic_oled_self_consult,
};

void CFBDGraphic_BindOLEDAsDevice(CFBD_GraphicDevice* device, CFBD_OLED* oled)
{
    if (device == NULL || oled == NULL || oled->ops == NULL)
        return;

    device->ops = &graphic_oled_ops;
    device->device_type = OLED;
    device->internal_handle = (CFBDGraphicDeviceHandle) oled;
}