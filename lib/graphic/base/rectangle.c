#include "rectangle.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "device/graphic_device.h"

typedef enum
{
    CS_LEFT = 1 << 0,
    CS_RIGHT = 1 << 1,
    CS_BOTTOM = 1 << 2,
    CS_TOP = 1 << 3
} ____CSCode;

void CFBDGraphic_DrawRect(CFBD_GraphicDevice* device, CFBDGraphicRect* rect)
{
    if (device == NULL || rect == NULL)
        return;
    if (device->ops == NULL)
        return;

    CFBDGraphicRect n = rect_normalize(*rect);
    int32_t lx = asInt32_t(n.tl.x);
    int32_t ty = asInt32_t(n.tl.y);
    int32_t rx = asInt32_t(n.br.x);
    int32_t by = asInt32_t(n.br.y);

    if (rx < lx)
        rx = lx;
    if (by < ty)
        by = ty;

    uint16_t w = (uint16_t) (rx - lx + 1);
    uint16_t h = (uint16_t) (by - ty + 1);
    device->ops->clear_area(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);

    if (lx == rx && ty == by) {
        device->ops->setPixel(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty));
    }
    else {
        /* 顶边与底边（水平）: x = lx..rx */
        for (int32_t x = lx; x <= rx; ++x) {
            device->ops->setPixel(device, clamp_u16_from_i32(x), clamp_u16_from_i32(ty));
            if (by != ty) /* 如果高度>1 再画底边，防止重复画同一行 */
                device->ops->setPixel(device, clamp_u16_from_i32(x), clamp_u16_from_i32(by));
        }
        if (by - ty >= 2) {
            for (int32_t y = ty + 1; y <= by - 1; ++y) {
                device->ops->setPixel(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(y));
                if (rx != lx)
                    device->ops->setPixel(device, clamp_u16_from_i32(rx), clamp_u16_from_i32(y));
            }
        }
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(device)) {
        device->ops->update_area(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);
    }
}

void CFBDGraphic_FillRect(CFBD_GraphicDevice* device, CFBDGraphicRect* rect)
{
    CFBDGraphicRect n = rect_normalize(*rect);
    int32_t lx = asInt32_t(n.tl.x);
    int32_t ty = asInt32_t(n.tl.y);
    int32_t rx = asInt32_t(n.br.x);
    int32_t by = asInt32_t(n.br.y);

    uint16_t w = (uint16_t) (rx - lx + 1);
    uint16_t h = (uint16_t) (by - ty + 1);
    device->ops->clear_area(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);

    for (int32_t y = ty; y <= by; ++y) {
        for (int32_t x = lx; x <= rx; ++x) {
            device->ops->setPixel(device, clamp_u16_from_i32(x), clamp_u16_from_i32(y));
        }
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(device)) {
        device->ops->update_area(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);
    }
}

static int cs_compute_code(const CFBDGraphicRect* r, int32_t x, int32_t y)
{
    CFBDGraphicRect n = rect_normalize(*r);
    int32_t code = 0;

    if (x < asInt32_t(n.tl.x))
        code |= CS_LEFT;
    else if (x > asInt32_t(n.br.x))
        code |= CS_RIGHT;

    if (y < asInt32_t(n.tl.y))
        code |= CS_TOP;
    else if (y > asInt32_t(n.br.y))
        code |= CS_BOTTOM;

    return code;
}

CFBDGraphicRect rect_intersection(const CFBDGraphicRect* a, const CFBDGraphicRect* b)
{
    CFBDGraphicRect A = rect_normalize(*a);
    CFBDGraphicRect B = rect_normalize(*b);

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
    int32_t lx = MAX(asInt32_t(A.tl.x), asInt32_t(B.tl.x));
    int32_t ty = MAX(asInt32_t(A.tl.y), asInt32_t(B.tl.y));
    int32_t rx = MIN(asInt32_t(A.br.x), asInt32_t(B.br.x));
    int32_t by = MIN(asInt32_t(A.br.y), asInt32_t(B.br.y));
#undef MAX
#undef MIN

    if (rx < lx)
        rx = lx;
    if (by < ty)
        by = ty;

    return (CFBDGraphicRect) {{clamp_u16_from_i32(lx), clamp_u16_from_i32(ty)},
                              {clamp_u16_from_i32(rx), clamp_u16_from_i32(by)}};
}

CFBDGraphicRect rect_union(const CFBDGraphicRect* a, const CFBDGraphicRect* b)
{
    CFBDGraphicRect A = rect_normalize(*a);
    CFBDGraphicRect B = rect_normalize(*b);

    int32_t lx = asInt32_t(A.tl.x) < asInt32_t(B.tl.x) ? asInt32_t(A.tl.x) : asInt32_t(B.tl.x);
    int32_t ty = asInt32_t(A.tl.y) < asInt32_t(B.tl.y) ? asInt32_t(A.tl.y) : asInt32_t(B.tl.y);
    int32_t rx = asInt32_t(A.br.x) > asInt32_t(B.br.x) ? asInt32_t(A.br.x) : asInt32_t(B.br.x);
    int32_t by = asInt32_t(A.br.y) > asInt32_t(B.br.y) ? asInt32_t(A.br.y) : asInt32_t(B.br.y);

    return (CFBDGraphicRect) {{clamp_u16_from_i32(lx), clamp_u16_from_i32(ty)},
                              {clamp_u16_from_i32(rx), clamp_u16_from_i32(by)}};
}

CFBDGraphicRect rect_offset(const CFBDGraphicRect* r, int32_t dx, int32_t dy)
{
    CFBDGraphicRect n = rect_normalize(*r);

    return (CFBDGraphicRect) {{clamp_u16_from_i32(asInt32_t(n.tl.x) + dx),
                               clamp_u16_from_i32(asInt32_t(n.tl.y) + dy)},
                              {clamp_u16_from_i32(asInt32_t(n.br.x) + dx),
                               clamp_u16_from_i32(asInt32_t(n.br.y) + dy)}};
}

CFBDGraphicRect
rect_inset(const CFBDGraphicRect* r, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    CFBDGraphicRect n = rect_normalize(*r);

    int32_t lx = asInt32_t(n.tl.x) + left;
    int32_t ty = asInt32_t(n.tl.y) + top;
    int32_t rx = asInt32_t(n.br.x) - right;
    int32_t by = asInt32_t(n.br.y) - bottom;

    if (rx < lx)
        rx = lx;
    if (by < ty)
        by = ty;

    return (CFBDGraphicRect) {{clamp_u16_from_i32(lx), clamp_u16_from_i32(ty)},
                              {clamp_u16_from_i32(rx), clamp_u16_from_i32(by)}};
}

CFBDGraphicRect rect_scale_about_center(const CFBDGraphicRect* r, float sx, float sy)
{
    CFBDGraphicRect n = rect_normalize(*r);

    int32_t cx = (asInt32_t(n.tl.x) + asInt32_t(n.br.x)) / 2;
    int32_t cy = (asInt32_t(n.tl.y) + asInt32_t(n.br.y)) / 2;

    int32_t hw = asInt32_t(n.br.x) - asInt32_t(n.tl.x);
    int32_t hh = asInt32_t(n.br.y) - asInt32_t(n.tl.y);

    int32_t new_hw = (int32_t) floorf(hw * sx / 2.0f + 0.5f);
    int32_t new_hh = (int32_t) floorf(hh * sy / 2.0f + 0.5f);

    return (CFBDGraphicRect) {{clamp_u16_from_i32(cx - new_hw), clamp_u16_from_i32(cy - new_hh)},
                              {clamp_u16_from_i32(cx + new_hw), clamp_u16_from_i32(cy + new_hh)}};
}

CFBDGraphicRect
rect_align_in_parent(const CFBDGraphicRect* parent, const CFBDGraphicRect* child, uint8_t anchor)
{
    CFBDGraphicRect P = rect_normalize(*parent);
    CFBDGraphicRect C = rect_normalize(*child);
    int32_t pw = asInt32_t(P.br.x) - asInt32_t(P.tl.x);
    int32_t ph = asInt32_t(P.br.y) - asInt32_t(P.tl.y);
    int32_t cw = asInt32_t(C.br.x) - asInt32_t(C.tl.x);
    int32_t ch = asInt32_t(C.br.y) - asInt32_t(C.tl.y);
    int32_t nx = asInt32_t(P.tl.x);
    int32_t ny = asInt32_t(P.tl.y); /* 水平 */
    if (anchor & RECT_ANCHOR_LEFT) {
        nx = asInt32_t(P.tl.x);
    }
    else if (anchor & RECT_ANCHOR_RIGHT) {
        nx = asInt32_t(P.br.x) - cw;
    }
    else { /* center by default */
        nx = asInt32_t(P.tl.x) + (pw - cw) / 2;
    } /* 垂直 */
    if (anchor & RECT_ANCHOR_TOP) {
        ny = asInt32_t(P.tl.y);
    }
    else if (anchor & RECT_ANCHOR_BOTTOM) {
        ny = asInt32_t(P.br.y) - ch;
    }
    else {
        ny = asInt32_t(P.tl.y) + (ph - ch) / 2;
    }
    return rect_from_xywh(nx, ny, cw, ch);
}

/* ---------- 最近点 / 点到矩形距离 ---------- */

/* 返回矩形内到点最近的点（如果点在矩形内，则返回点本身，结果为 uint16_t） */
CFBDGraphic_Point rect_closest_point(const CFBDGraphicRect* r, CFBDGraphic_Point p)
{
    CFBDGraphicRect n = rect_normalize(*r);
    int32_t px = asInt32_t(p.x), py = asInt32_t(p.y);
    int32_t lx = asInt32_t(n.tl.x), ty = asInt32_t(n.tl.y);
    int32_t rx = asInt32_t(n.br.x), by = asInt32_t(n.br.y);
    int32_t cx = px < lx ? lx : (px > rx ? rx : px);
    int32_t cy = py < ty ? ty : (py > by ? by : py);
    CFBDGraphic_Point out = {clamp_u16_from_i32(cx), clamp_u16_from_i32(cy)};
    return out;
}

/* 点到矩形最短距离（像素距离，若在矩形内则为0）*/
uint32_t rect_distance_to_point(const CFBDGraphicRect* r, CFBDGraphic_Point p)
{
    CFBDGraphic_Point cp = rect_closest_point(r, p);
    int32_t dx = asInt32_t(cp.x) - asInt32_t(p.x);
    int32_t dy = asInt32_t(cp.y) - asInt32_t(p.y);
    return (uint32_t) (sqrt((double) (dx * dx + dy * dy)));
}

/* ---------- 扩展 / 包含点 ---------- */
CFBDGraphicRect rect_expand_to_include_point(const CFBDGraphicRect* r, CFBDGraphic_Point p)
{
    CFBDGraphicRect n = rect_normalize(*r);
    int32_t lx = asInt32_t(n.tl.x) < asInt32_t(p.x) ? asInt32_t(n.tl.x) : asInt32_t(p.x);
    int32_t ty = asInt32_t(n.tl.y) < asInt32_t(p.y) ? asInt32_t(n.tl.y) : asInt32_t(p.y);
    int32_t rx = asInt32_t(n.br.x) > asInt32_t(p.x) ? asInt32_t(n.br.x) : asInt32_t(p.x);
    int32_t by = asInt32_t(n.br.y) > asInt32_t(p.y) ? asInt32_t(n.br.y) : asInt32_t(p.y);
    CFBDGraphicRect out = {{clamp_u16_from_i32(lx), clamp_u16_from_i32(ty)},
                           {clamp_u16_from_i32(rx), clamp_u16_from_i32(by)}};
    return out;
}

CFBD_Bool rect_clip_line(CFBDGraphicRect* r, CFBDGraphic_Point* p0, CFBDGraphic_Point* p1)
{
    CFBDGraphicRect n = rect_normalize(*r);
    int32_t x0 = asInt32_t(p0->x), y0 = asInt32_t(p0->y);
    int32_t x1 = asInt32_t(p1->x), y1 = asInt32_t(p1->y);
    int code0 = cs_compute_code(&n, x0, y0);
    int code1 = cs_compute_code(&n, x1, y1);
    CFBD_Bool accept = CFBD_FALSE;
    while (1) {
        if ((code0 | code1) == 0) {
            accept = CFBD_TRUE;
            break;
        }
        else if (code0 & code1) {
            accept = CFBD_FALSE;
            break;
        }
        else {
            int outcode = code0 ? code0 : code1;
            int32_t nx = 0, ny = 0;
            if (outcode & CS_TOP) {
                nx = x0 +
                     (int32_t) ((double) (x1 - x0) * (asInt32_t(n.tl.y) - y0) / (double) (y1 - y0));
                ny = asInt32_t(n.tl.y);
            }
            else if (outcode & CS_BOTTOM) {
                nx = x0 +
                     (int32_t) ((double) (x1 - x0) * (asInt32_t(n.br.y) - y0) / (double) (y1 - y0));
                ny = asInt32_t(n.br.y);
            }
            else if (outcode & CS_RIGHT) {
                ny = y0 +
                     (int32_t) ((double) (y1 - y0) * (asInt32_t(n.br.x) - x0) / (double) (x1 - x0));
                nx = asInt32_t(n.br.x);
            }
            else if (outcode & CS_LEFT) {
                ny = y0 +
                     (int32_t) ((double) (y1 - y0) * (asInt32_t(n.tl.x) - x0) / (double) (x1 - x0));
                nx = asInt32_t(n.tl.x);
            }
            if (outcode == code0) {
                x0 = nx;
                y0 = ny;
                code0 = cs_compute_code(&n, x0, y0);
            }
            else {
                x1 = nx;
                y1 = ny;
                code1 = cs_compute_code(&n, x1, y1);
            }
        }
    }
    if (accept) {
        p0->x = clamp_u16_from_i32(x0);
        p0->y = clamp_u16_from_i32(y0);
        p1->x = clamp_u16_from_i32(x1);
        p1->y = clamp_u16_from_i32(y1);
    }
    return accept;
}

/* ---------- 多用途：裁剪到屏幕边界 ---------- */
CFBDGraphicRect rect_clamp_to_screen(const CFBDGraphicRect* r, uint16_t screen_w, uint16_t screen_h)
{
    CFBDGraphicRect n = rect_normalize(*r);
    int32_t lx = asInt32_t(n.tl.x) < 0 ? 0 : asInt32_t(n.tl.x);
    int32_t ty = asInt32_t(n.tl.y) < 0 ? 0 : asInt32_t(n.tl.y);
    int32_t rx = asInt32_t(n.br.x) > screen_w ? screen_w : asInt32_t(n.br.x);
    int32_t by = asInt32_t(n.br.y) > screen_h ? screen_h : asInt32_t(n.br.y);
    if (rx < lx)
        rx = lx;
    if (by < ty)
        by = ty;
    CFBDGraphicRect out = {{clamp_u16_from_i32(lx), clamp_u16_from_i32(ty)},
                           {clamp_u16_from_i32(rx), clamp_u16_from_i32(by)}};
    return out;
}