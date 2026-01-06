#include "oled.h"

#include "cfbd_define.h"
#include "driver/backend/oled_iic.h"

static inline void CFBD_OLED_ImmediateInit(CFBD_OLED* oled)
{
    const CFBD_OLEDOperations* ops = oled->ops;
    ops->init(oled, NULL);
    ops->clear(oled);
    ops->update(oled);
}

CFBD_Bool CFBD_GetOLEDHandle(
    CFBD_OLED* oled, const CFBD_OLEDDriverType driver_type, 
    CFBDOLED_Params_Inits args, CFBD_Bool request_immediate_init)
{
    switch(driver_type){
        case CFBD_OLEDDriverType_IIC:
            CFBD_OLED_IICInit(oled, args);
        break;
        default:
            return CFBD_FALSE;
        break;
    }

    if(request_immediate_init){
        CFBD_OLED_ImmediateInit(oled);
    }

    return CFBD_TRUE;
}