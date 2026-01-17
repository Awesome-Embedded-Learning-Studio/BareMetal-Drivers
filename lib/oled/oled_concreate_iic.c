#include <string.h>

#include "configs/external_impl_driver.h"
#include "configs/iic_pack_type.h"
#include "driver/backend/oled_iic_130x.h"
#include "driver/backend/oled_iic_132x.h"
#include "oled.h"

void CFBD_OLED_IICInit(CFBD_OLED* handle, CFBD_OLED_IICInitsParams* pvt_handle)
{
    if (strcmp(pvt_handle->device_specifics->iic_pack_type, SSD130X_REQUEST_IIC_PACK) == 0) {
        CFBD_OLED_IIC130XInit(handle, pvt_handle);
    }
    else if (strcmp(pvt_handle->device_specifics->iic_pack_type, SSD132X_REQUEST_IIC_PACK) == 0) {
        CFBD_OLED_IIC132XInit(handle, pvt_handle);
    }
}