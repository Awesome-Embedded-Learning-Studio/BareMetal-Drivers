#include "animation.h"

#include "animation_config.h"

void CFBD_InitBaseAnimation(CFBD_BaseAnimation* pBaseAnimation)
{
    pBaseAnimation->anim_frame_delay_ms = CFBD_BASEANIMATION_DELAY_MS;
    pBaseAnimation->anim_frames = CFBD_BASEANIMATION_FRAME_CNT;
}
