#pragma once
/**
 * @file demo_manager.h
 * @author CharlieChen
 * @brief Generic demo manager for CFBD-based embedded GUI demonstrations
 *
 * @details
 * This module provides a unified mechanism to:
 *  - Register independent demo modules
 *  - Run a single specified demo
 *  - Run demos sequentially (queue mode)
 *  - Run demos in circular rotation (loop mode)
 *
 * Design goals:
 *  - No dynamic memory allocation
 *  - Pure C, OOP-style abstraction
 *  - Easy extensibility (add a demo = add one file)
 */

#include <stdint.h>

#include "cfbd_graphic_define.h"


typedef void (*CFBD_DemoEntry)(CFBD_GraphicDevice* dev);

/**
 * @brief Demo descriptor structure
 */
typedef struct
{
    const char* name;     /**< Human-readable demo name */
    CFBD_DemoEntry entry; /**< Demo entry function */
    uint32_t duration;    /**< Duration in milliseconds (0 = infinite) */
} CFBD_DemoDescriptor;

/**
 * @brief Demo run mode
 */
typedef enum
{
    CFBD_DEMO_RUN_SINGLE, /**< Run only one demo */
    CFBD_DEMO_RUN_QUEUE,  /**< Run demos sequentially once */
    CFBD_DEMO_RUN_LOOP    /**< Run demos continuously in loop */
} CFBD_DemoRunMode;

/**
 * @brief Run demos according to selected mode
 *
 * @param dev        Graphic device
 * @param demos      Demo descriptor array
 * @param demo_count Number of demos
 * @param mode       Run mode
 * @param index      Index used only in SINGLE mode
 */
void CFBD_DemoManager_Run(CFBD_GraphicDevice* dev,
                          const CFBD_DemoDescriptor* demos,
                          uint32_t demo_count,
                          CFBD_DemoRunMode mode,
                          uint32_t index);
