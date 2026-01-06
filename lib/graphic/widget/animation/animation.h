#pragma once
#include <stdint.h>

typedef struct
{
    void (*delay_ms)(uint32_t ms); // animation delays
} CFBD_BaseAnimationOps;

typedef struct __CFBD_BaseAnimation
{
    uint8_t anim_frames;          /* frames for transition */
    uint32_t anim_frame_delay_ms; /* delay per frame in ms; if 0, no delay */
} CFBD_BaseAnimation;

/**
 * @brief create a base animation with default settings
 *
 * @param pBaseAnimation pointers to the inits
 */
void CFBD_InitBaseAnimation(CFBD_BaseAnimation* pBaseAnimation);