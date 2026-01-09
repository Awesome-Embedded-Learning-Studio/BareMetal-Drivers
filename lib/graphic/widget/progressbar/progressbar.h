#pragma once
#include <stdint.h>

#include "base/point.h"
#include "base/size.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "widget/animation/animation.h"

typedef struct __CFBD_ProgressBar CFBD_ProgressBar;

typedef struct
{
    void (*immediate_draw)(CFBD_ProgressBar*);
    void (*set_value)(CFBD_ProgressBar*, int32_t new_value);
    /**
     * @brief   Set Property values, like
     *          -> border, uint8_t as boarders
     *          -> padding, uint8_t as padding
     *          -> animation, CFBD_BaseAnimation* passed
     *
     */
    CFBD_Bool (*set_property)(CFBD_ProgressBar*, const char* property, const void* args);
} CFBD_ProgressBarOps;

typedef struct __CFBD_ProgressBar
{
    CFBD_ProgressBarOps* ops;
    CFBDGraphic_Point tl; /* top-left */
    CFBDGraphicSize size;
    int32_t min;   /* min value */
    int32_t max;   /* max value */
    int32_t value; /* current value */
    CFBD_GraphicDevice* device;

    /* visual config */
    uint8_t border;  /* 0 = no border, 1 = border */
    uint8_t padding; /* inner padding px between border and fill */

    /* animation config */
    CFBD_BaseAnimation animation;
} CFBD_ProgressBar;

/* Initialize progressbar (no dynamic allocation). device must be valid. */
void CFBD_ProgressBar_Init(CFBD_ProgressBar* pb,
                           CFBD_GraphicDevice* device,
                           CFBDGraphic_Point* tl,
                           CFBDGraphicSize* sz,
                           int32_t min,
                           int32_t max);