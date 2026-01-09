/**
 * @file graphic_device.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2026-01-05
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "cfbd_define.h"

typedef struct _CFBD_GraphicDevice CFBD_GraphicDevice;

/**
 * @brief GraphicAreaOperations means for the area operations
 *
 */
typedef CFBD_Bool (*GraphicAreaOperations)(CFBD_GraphicDevice* device,
                                           uint16_t x,
                                           uint16_t y,
                                           uint16_t width,
                                           uint16_t height);
/**
 * @brief GraphicFrameOperation indicates the frame related operations
 *
 */
typedef CFBD_Bool (*GraphicFrameOperation)(CFBD_GraphicDevice* device);
/**
 * @brief GraphicOLEDSelfOperation self indicates
 *
 */
typedef CFBD_Bool (*GraphicOLEDSelfOperation)(CFBD_GraphicDevice* device);

/**
 * @brief GraphicOLED_QueryOperation device query
 *
 */
typedef CFBD_Bool (*GraphicOLED_QueryOperation)(CFBD_GraphicDevice* device,
                                                const char* property, // property for query
                                                void* args,           // if property request args
                                                void* request_data);  // contains results

typedef struct
{
    /**
     * @brief init calls for the devices setup
     *
     */
    int (*init)(CFBD_GraphicDevice* device, void* init_args);

    // enable the pixel lighten or set brushed color
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y);

    // enable the area lightem or set brushed color
    CFBD_Bool (*setArea)(CFBD_GraphicDevice* device,
                         uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         uint8_t* source);

    /* Frame-level operations */
    GraphicFrameOperation update; // update the frame
    GraphicFrameOperation clear;  // clear the frame
    GraphicFrameOperation revert; // revert the frame

    /* Area-level operations */
    GraphicAreaOperations update_area; // update the area given
    GraphicAreaOperations clear_area;  // clear the area given
    GraphicAreaOperations revert_area; // revert the area given

    /* Open Close Devices */
    GraphicOLEDSelfOperation open;
    GraphicOLEDSelfOperation close;

    /*
     * Accept Queries:
     *   rgb    -> CFBD_Bool*
     *   width  -> uint16_t*
     *   height -> uint16_t*
     */
    GraphicOLED_QueryOperation self_consult;
} CFBD_GraphicDeviceOperation;

typedef enum
{
    OLED
} CFBDGraphic_DeviceType;

typedef void* CFBDGraphicDeviceHandle;

typedef struct _CFBD_GraphicDevice
{
    CFBD_GraphicDeviceOperation* ops;
    CFBDGraphic_DeviceType device_type;
    CFBDGraphicDeviceHandle internal_handle; // device handle internal
    CFBD_Bool immediate_draw;                // mannual / auto updates
} CFBD_GraphicDevice;

/**
 * @brief Check if each operations should update after the calls
 *
 * @param device
 * @return CFBD_Bool
 */
static inline CFBD_Bool CFBDGraphic_DeviceRequestUpdateAtOnce(CFBD_GraphicDevice* device)
{
    return device->immediate_draw;
}

/**
 * @brief Set if each operations should update after the calls
 *
 * @param device
 * @param requests
 */
static inline void CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(CFBD_GraphicDevice* device,
                                                              CFBD_Bool requests)
{
    device->immediate_draw = requests;
}

static inline void CFBDGraphic_DeviceClearImmediate(CFBD_GraphicDevice* device)
{
    device->ops->clear(device);
    device->ops->update(device);
}

void CFBDGraphic_BindDevice(CFBD_GraphicDevice* device,
                            CFBDGraphic_DeviceType device_type,
                            /*
                             * OLED -> CFBD_OLED*
                             */
                            CFBDGraphicDeviceHandle internal_handle);