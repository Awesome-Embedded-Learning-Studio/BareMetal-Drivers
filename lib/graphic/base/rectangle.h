#pragma once
#include <limits.h>
#include <stdint.h>

#include "base_helpers.h"
#include "cfbd_define.h"
#include "point.h"
#include "size.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    CFBDGraphic_Point tl;
    CFBDGraphic_Point br;
} CFBDGraphicRect;

void CFBDGraphic_DrawRect(CFBD_GraphicDevice* device, CFBDGraphicRect* rect);
void CFBDGraphic_FillRect(CFBD_GraphicDevice* device, CFBDGraphicRect* rect);

/**
 * @brief Followings are common utils for the rects
 */
static inline CFBDGraphicRect rect_normalize(CFBDGraphicRect r)
{
    PointBaseType x1 = r.tl.x, y1 = r.tl.y;
    PointBaseType x2 = r.br.x, y2 = r.br.y;
    int32_t lx = x1 < x2 ? x1 : x2;
    int32_t rx = x1 < x2 ? x2 : x1;
    int32_t ty = y1 < y2 ? y1 : y2;
    int32_t by = y1 < y2 ? y2 : y1;
    CFBDGraphicRect out = {{lx, ty}, {rx, by}};
    return out;
}

static inline uint32_t rect_width(const CFBDGraphicRect* r)
{
    CFBDGraphicRect n = rect_normalize(*r);
    return (uint32_t) asInt32_t(n.br.x) - (uint32_t) asInt32_t(n.tl.x);
}
static inline uint32_t rect_height(const CFBDGraphicRect* r)
{
    CFBDGraphicRect n = rect_normalize(*r);
    return (uint32_t) asInt32_t(n.br.y) - (uint32_t) asInt32_t(n.tl.y);
}

static inline CFBD_Bool rect_is_empty(const CFBDGraphicRect* r)
{
    return rect_width(r) == 0 || rect_height(r) == 0;
}

/* ---------- 转换：xywh <-> rect ---------- */
static inline CFBDGraphicRect rect_from_xywh(int32_t x, int32_t y, int32_t w, int32_t h)
{
    CFBDGraphicRect r;
    r.tl.x = clamp_u16_from_i32(x);
    r.tl.y = clamp_u16_from_i32(y);
    r.br.x = clamp_u16_from_i32(x + w);
    r.br.y = clamp_u16_from_i32(y + h);
    return rect_normalize(r);
}

static inline void rect_to_xywh(const CFBDGraphicRect* r,
                                int32_t* out_x,
                                int32_t* out_y,
                                int32_t* out_w,
                                int32_t* out_h)
{
    CFBDGraphicRect n = rect_normalize(*r);
    *out_x = asInt32_t(n.tl.x);
    *out_y = asInt32_t(n.tl.y);
    *out_w = asInt32_t(n.br.x) - asInt32_t(n.tl.x);
    *out_h = asInt32_t(n.br.y) - asInt32_t(n.tl.y);
}

/* ---------- 点 / 矩形包含 / 相交 ---------- */
static inline CFBD_Bool rect_contains_point(const CFBDGraphicRect* r, CFBDGraphic_Point p)
{
    CFBDGraphicRect n = rect_normalize(*r);
    return (p.x >= n.tl.x && p.x <= n.br.x && p.y >= n.tl.y && p.y <= n.br.y);
}

/* 判断两矩形是否相交（边缘算相交）*/
static inline CFBD_Bool rect_intersects(const CFBDGraphicRect* a, const CFBDGraphicRect* b)
{
    CFBDGraphicRect A = rect_normalize(*a);
    CFBDGraphicRect B = rect_normalize(*b);
    if (asInt32_t(A.br.x) < asInt32_t(B.tl.x) || asInt32_t(B.br.x) < asInt32_t(A.tl.x))
        return CFBD_FALSE;
    if (asInt32_t(A.br.y) < asInt32_t(B.tl.y) || asInt32_t(B.br.y) < asInt32_t(A.tl.y))
        return CFBD_FALSE;
    return CFBD_TRUE;
}

/* Anchor flags（水平 | 垂直） */
typedef enum
{
    RECT_ANCHOR_LEFT = 1 << 0,
    RECT_ANCHOR_HCENTER = 1 << 1,
    RECT_ANCHOR_RIGHT = 1 << 2,

    RECT_ANCHOR_TOP = 1 << 3,
    RECT_ANCHOR_VCENTER = 1 << 4,
    RECT_ANCHOR_BOTTOM = 1 << 5
} CCGraphicAnchor;

/* 交集 / 并集 */
CFBDGraphicRect rect_intersection(const CFBDGraphicRect* a, const CFBDGraphicRect* b);

CFBDGraphicRect rect_union(const CFBDGraphicRect* a, const CFBDGraphicRect* b);

/* 平移 / inset / 缩放 */
CFBDGraphicRect rect_offset(const CFBDGraphicRect* r, int32_t dx, int32_t dy);

CFBDGraphicRect
rect_inset(const CFBDGraphicRect* r, int32_t left, int32_t top, int32_t right, int32_t bottom);

CFBDGraphicRect rect_scale_about_center(const CFBDGraphicRect* r, float sx, float sy);

CFBDGraphicRect
rect_align_in_parent(const CFBDGraphicRect* parent, const CFBDGraphicRect* child, uint8_t anchor);

/* 点相关 */
CFBDGraphic_Point rect_closest_point(const CFBDGraphicRect* r, CFBDGraphic_Point p);

uint32_t rect_distance_to_point(const CFBDGraphicRect* r, CFBDGraphic_Point p);

CFBDGraphicRect rect_expand_to_include_point(const CFBDGraphicRect* r, CFBDGraphic_Point p);

/* 裁剪 */
CFBD_Bool rect_clip_line(CFBDGraphicRect* r, CFBDGraphic_Point* p0, CFBDGraphic_Point* p1);

CFBDGraphicRect
rect_clamp_to_screen(const CFBDGraphicRect* r, uint16_t screen_w, uint16_t screen_h);

static inline uint32_t rect_area(const CFBDGraphicRect* r)
{
    if (rect_is_empty(r))
        return 0;
    return rect_width(r) * rect_height(r);
}

static inline CFBDGraphicSize rect_size(const CFBDGraphicRect* r)
{
    CFBDGraphicSize sz = {0, 0};
    if (rect_is_empty(r))
        return sz;
    sz.height = rect_height(r);
    sz.width = rect_width(r);
    return sz;
}

static inline uint32_t rect_overlap_area(const CFBDGraphicRect* a, const CFBDGraphicRect* b)
{
    CFBDGraphicRect inter = rect_intersection(a, b);
    return rect_area(&inter);
}

#ifdef __cplusplus
}
#endif