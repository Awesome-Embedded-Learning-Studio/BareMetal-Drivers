#include "progressbar.h"

#include <stdint.h>
#include <string.h>

#include "cfbd_define.h"
#include "device/graphic_device.h"
#include "sys_clock/system_clock.h" /* for system_delay_ms */
#include "widget/animation/animation.h"
#include "widget/base_support/common/helpers.h"

/* Draw rectangle border using setPixel (1px thick) */
static void
draw_rect_border(CFBD_GraphicDevice* dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    CFBD_Bool (*SetPixelAction)(CFBD_GraphicDevice* dev, uint16_t x, uint16_t y) =
            dev->ops->setPixel;
    /* top/bottom */
    for (uint16_t ix = x; ix < x + w; ++ix) {
        SetPixelAction(dev, ix, y);
        SetPixelAction(dev, ix, y + h - 1);
    }
    /* left/right */
    for (uint16_t iy = y; iy < y + h; ++iy) {
        SetPixelAction(dev, x, iy);
        SetPixelAction(dev, x + w - 1, iy);
    }
}

/* Fill rectangular area with pixels (setPixel) */
static void fill_rect(CFBD_GraphicDevice* dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    CFBD_Bool (*SetPixelAction)(CFBD_GraphicDevice* dev, uint16_t x, uint16_t y) =
            dev->ops->setPixel;
    for (uint16_t iy = y; iy < y + h; ++iy) {
        for (uint16_t ix = x; ix < x + w; ++ix) {
            SetPixelAction(dev, ix, iy);
        }
    }
}

/* Clear area using device-provided clear_area (faster than per-pixel clear) */
static void clear_rect(CFBD_GraphicDevice* dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    dev->ops->clear_area(dev, x, y, w, h);
}

/* internal: compute inner box (where fill is drawn) */
static void compute_inner_box(CFBD_ProgressBar* pb,
                              uint16_t* out_x,
                              uint16_t* out_y,
                              uint16_t* out_w,
                              uint16_t* out_h)
{
    uint16_t x = pb->tl.x;
    uint16_t y = pb->tl.y;
    uint16_t w = pb->size.width;
    uint16_t h = pb->size.height;

    uint8_t pad = pb->padding;
    if (pb->border) {
        /* 1 px border assumed */
        if (w <= 2)
            w = 0;
        else {
            x += 1;
            w -= 2;
        }
        if (h <= 2)
            h = 0;
        else {
            y += 1;
            h -= 2;
        }
    }
    /* apply padding inside */
    if (w > 2 * pad) {
        x += pad;
        w -= (uint16_t) (2 * pad);
    }
    else
        w = 0;
    if (h > 2 * pad) {
        y += pad;
        h -= (uint16_t) (2 * pad);
    }
    else
        h = 0;

    *out_x = x;
    *out_y = y;
    *out_w = w;
    *out_h = h;
}

static float smoothstep(float t)
{
    /* classic smoothstep: 3t^2 - 2t^3 */
    return t * t * (3.0f - 2.0f * t);
}

static uint16_t compute_fill_width(CFBD_ProgressBar* pb, int32_t value)
{
    uint16_t ix, iy, iw, ih;
    compute_inner_box(pb, &ix, &iy, &iw, &ih);

    if (iw == 0 || pb->max <= pb->min)
        return 0;

    value = clamp_i32(value, pb->min, pb->max);
    float frac = (float) (value - pb->min) / (float) (pb->max - pb->min);

    if (frac < 0.0f)
        frac = 0.0f;
    if (frac > 1.0f)
        frac = 1.0f;

    return (uint16_t) (frac * (float) iw + 0.5f);
}

void CFBD_ProgressBar_Draw(CFBD_ProgressBar* pb)
{
    if (!pb || !pb->device)
        return;

    CFBD_GraphicDevice* dev = pb->device;

    uint16_t ix, iy, iw, ih;
    compute_inner_box(pb, &ix, &iy, &iw, &ih);

    /* clear inner area */
    clear_rect(dev, ix, iy, iw, ih);

    /* draw filled area */
    uint16_t fill_w = compute_fill_width(pb, pb->value);
    if (fill_w > 0)
        fill_rect(dev, ix, iy, fill_w, ih);

    /* border */
    if (pb->border)
        draw_rect_border(dev, pb->tl.x, pb->tl.y, pb->size.width, pb->size.height);

    dev->ops->update_area(dev, pb->tl.x, pb->tl.y, pb->size.width, pb->size.height);
}

void CFBD_ProgressBar_SetValue(CFBD_ProgressBar* pb, int32_t new_value)
{
    if (!pb || !pb->device)
        return;

    new_value = clamp_i32(new_value, pb->min, pb->max);

    int32_t old_value = pb->value;
    if (pb->animation.anim_frames <= 1) {
        pb->value = new_value;
        CFBD_ProgressBar_Draw(pb);
        return;
    }

    uint16_t ix, iy, iw, ih;
    compute_inner_box(pb, &ix, &iy, &iw, &ih);
    if (iw == 0 || ih == 0) {
        pb->value = new_value;
        return;
    }

    int frames = pb->animation.anim_frames;
    int delay = (pb->animation.anim_frame_delay_ms > 0) ? pb->animation.anim_frame_delay_ms : 16;

    uint16_t last_fill = compute_fill_width(pb, old_value);

    for (int f = 1; f <= frames; ++f) {
        float t = (float) f / (float) frames;
        float st = smoothstep(t);

        int32_t cur_value = old_value + (int32_t) ((float) (new_value - old_value) * st);

        uint16_t cur_fill = compute_fill_width(pb, cur_value);

        /* incremental update only */
        if (cur_fill > last_fill) {
            /* grow */
            fill_rect(pb->device, ix + last_fill, iy, cur_fill - last_fill, ih);
        }
        else if (cur_fill < last_fill) {
            /* shrink */
            clear_rect(pb->device, ix + cur_fill, iy, last_fill - cur_fill, ih);
        }

        if (pb->border)
            draw_rect_border(pb->device, pb->tl.x, pb->tl.y, pb->size.width, pb->size.height);

        pb->device->ops->update_area(pb->device,
                                     pb->tl.x,
                                     pb->tl.y,
                                     pb->size.width,
                                     pb->size.height);

        last_fill = cur_fill;
        pb->value = cur_value;

        system_delay_ms(delay);
    }

    pb->value = new_value;
}

/*
 *          -> border, uint8_t as boarders
 *          -> padding, uint8_t as padding
 *          -> animation, CFBD_BaseAnimation* passed
 */
CFBD_Bool
CFBD_ProgressBar_SetProperty(CFBD_ProgressBar* pb, const char* property, const void* value)
{
    if (strcmp(property, "border") == 0) {
        uint8_t border = *(uint8_t*) (value);
        pb->border = border;
        return CFBD_TRUE;
    }
    else if (strcmp(property, "padding") == 0) {
        uint8_t padding = *(uint8_t*) (value);
        pb->padding = padding;
        return CFBD_TRUE;
    }
    else if (strcmp(property, "animation") == 0) {
        CFBD_BaseAnimation* animation = (CFBD_BaseAnimation*) (value);
        pb->animation = *animation;
        return CFBD_TRUE;
    }
    return CFBD_FALSE;
}

CFBD_ProgressBarOps ops = {.immediate_draw = CFBD_ProgressBar_Draw,
                           .set_property = CFBD_ProgressBar_SetProperty,
                           .set_value = CFBD_ProgressBar_SetValue};

/* simple setters */
void CFBD_ProgressBar_Init(CFBD_ProgressBar* pb,
                           CFBD_GraphicDevice* device,
                           CFBDGraphic_Point* tl,
                           CFBDGraphicSize* sz,
                           int32_t min,
                           int32_t max)
{
    if (!pb)
        return;
    memset(pb, 0, sizeof(*pb));
    pb->tl = *tl;
    pb->size = *sz;
    pb->min = min;
    pb->max = max;
    pb->value = min;
    pb->device = device;
    pb->border = 1;
    pb->padding = 1;
    pb->ops = &ops;
}
