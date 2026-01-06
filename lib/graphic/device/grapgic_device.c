#include "graphic_device.h"
#include "oled/oled_graphic_device.h"

void CFBDGraphic_BindDevice(CFBD_GraphicDevice* device,
                            CFBDGraphic_DeviceType device_type,
                            /*
                             * OLED -> CFBD_OLED*
                             */
                            CFBDGraphicDeviceHandle internal_handle)
{
    device->internal_handle = internal_handle;
    device->device_type = device_type;
    switch(device_type){
        case OLED:
            CFBDGraphic_BindOLEDAsDevice(device, internal_handle);
            return;
        default:
            return;
        break;
    }
}