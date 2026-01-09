/**
 * @file oled.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief oled general handles
 * @version 0.1
 * @date 2026-01-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "cfbd_define.h"

/**
 * @brief
 *
 */
typedef enum
{
    CFBD_OLEDDriverType_IIC,
    CFBD_OLEDDriverType_SPI
} CFBD_OLEDDriverType;

typedef void* CFBD_OLEDHandle;

typedef struct _CFBD_OLED CFBD_OLED;
typedef CFBD_Bool (
        *AreaOperations)(CFBD_OLED* oled, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
typedef CFBD_Bool (*FrameOperation)(CFBD_OLED* oled);
typedef CFBD_Bool (*OLEDSelfOperation)(CFBD_OLED* oled);
typedef CFBD_Bool (*OLED_QueryOperation)(CFBD_OLED* oled,
                                         const char* property, // property for query
                                         void* args,           // if property request args
                                         void* request_data);  // contains results

typedef struct _CFBD_OLED_OPS
{
    int (*init)(CFBD_OLED* oled, void* init_args);
    CFBD_Bool (*setPixel)(CFBD_OLED* oled, uint16_t x, uint16_t y);
    CFBD_Bool (*setArea)(CFBD_OLED* device,
                         uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         uint8_t* source);
    FrameOperation update;
    FrameOperation clear;
    FrameOperation revert;

    AreaOperations update_area;
    AreaOperations clear_area;
    AreaOperations revert_area;

    OLEDSelfOperation open;
    OLEDSelfOperation close;
    /**
     * @brief   OLED_QueryOperation queries the sessions of oled
     *          currently
     * @note    device should at least supports settings:
     *      rgb: pass CFBD_Bool type, Always no currently
     *      width: pass uint16_t type, get the width
     *      height: pass uint16_t type, get the height
     */
    OLED_QueryOperation self_consult;
} CFBD_OLEDOperations;

typedef struct _CFBD_OLED
{
    const CFBD_OLEDOperations* ops;
    CFBD_OLEDDriverType driver_type;
    CFBD_OLEDHandle oled_internal_handle;
} CFBD_OLED;

typedef void* CFBDOLED_Params_Inits;
CFBD_Bool CFBD_GetOLEDHandle(CFBD_OLED* oled,
                             const CFBD_OLEDDriverType driver_type,
                             /**
                              *  iic -> CFBD_OLED_IICInitsParams
                              */
                             CFBDOLED_Params_Inits args,
                             CFBD_Bool request_immediate_init);
