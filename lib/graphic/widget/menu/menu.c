#include "menu.h"

#include <stdint.h>

#include "base/point.h"
#include "base/size.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "menu_config.h"
#include "menu_indicator.h"
#include "menu_item.h"
#include "sys_clock/system_clock.h"
#include "widget/base_support/common/helpers.h"
#include "widget/menu/menu.h"
#include "widget/text.h"
#include "widget/text_config.h"

static inline const uint16_t get_menu_new_item_x(CFBD_Menu* pMenu)
{
    return pMenu->tl_point.x + pMenu->indicator.width + pMenu->indicator.x +
           CFBD_MENU_INDICATOR_GAP;
}

static const uint16_t get_menu_new_item_height(CFBD_MenuItem* item)
{
    CFBDGraphicSize sz = __fetch_font_size(item->text_obj.font_size);
    return (sz.height + 2 * CFBD_MENU_ITEM_Y_GAP);
}

static const uint16_t get_menu_new_item_y(CFBD_Menu* pMenu)
{
    uint16_t result = pMenu->tl_point.y;
    if (!pMenu || !pMenu->menu_items) {
        return result;
    }

    CFBD_MenuItemGroup* group = pMenu->menu_items;
    for (uint8_t i = 0; i < group->count; i++) {
        result += get_menu_new_item_height(&group->pItems[i]);
    }
    return result + CFBD_MENU_ITEM_Y_GAP;
}

static void CFBD_BindMenuItems(CFBD_Menu* pMenu, CFBD_MenuItemGroup* assigned_menu_items)
{
    pMenu->menu_items = assigned_menu_items;
    pMenu->selected = 0;
}

static void CFBD_AnimationSet(CFBD_Menu* pMenu, CFBD_BaseAnimation* pBaseAnimation)
{
    pMenu->animation = *pBaseAnimation;
}

static CFBD_Bool CFBD_MenuAddItems(CFBD_Menu* pMenu,
                                   const char* label,
                                   Ascii_Font_Size font_size,
                                   CFBD_MenuItemCallbackPack* callbacks)
{
    CFBD_MenuItemGroup* group = pMenu->menu_items;
    if (group->count >= group->capacity) {
        return CFBD_FALSE;
    }

    CFBD_MenuItem* it = &group->pItems[group->count];
    it->label = (char*) label;
    CFBDGraphic_Point p = {.x = get_menu_new_item_x(pMenu), .y = get_menu_new_item_y(pMenu)};
    CFBD_MenuItemInit(it, &p, callbacks);
    CFBDGraphicSize sz = {.width = pMenu->max_width,
                          .height = __fetch_font_size(font_size).height + 2 * CFBD_MENU_ITEM_Y_GAP};
    /* init text object */
    CFBDGraphic_InitText(&it->text_obj, it->tl, sz, font_size);
    CFBDGraphic_SetText(&it->text_obj, it->label);
    group->count++;

    return CFBD_TRUE;
}

static void CFBD_DrawMenu(CFBD_Menu* m)
{
    CFBD_GraphicDevice* dev = m->device;
    CFBD_MenuItemGroup* group = m->menu_items;
    for (size_t i = 0; i < group->count; ++i) {
        CFBD_MenuItem* it = &group->pItems[i];
        CFBDGraphic_DrawText(dev, &it->text_obj, CCGraphic_AsciiTextItem_RequestOldPoint);
    }

    /* draw indicator at selected */
    if (m->selected >= 0 && (size_t) m->selected < group->count) {
        CFBD_MenuItem* sel = &group->pItems[m->selected];
        uint16_t ix = m->tl_point.x + m->indicator.x;
        uint16_t iy = sel->tl.y;
        uint16_t ih = rect_height(&sel->text_obj.text_bounding_rect);
        CFBD_DrawMenuIndicator(m, ix, iy, m->indicator.width, ih);

        /* reversed the target */
        CFBDGraphic_Text* text = &sel->text_obj;
        m->device->ops->revert_area(m->device,
                                    text->tl_point.x,
                                    text->tl_point.y,
                                    rect_width(&text->text_bounding_rect),
                                    ih);
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(m->device)) {
        m->device->ops->update(m->device);
    }
}

static void redraw_item(CFBD_Menu* m, int idx)
{
    if (idx < 0 || (size_t) idx >= m->menu_items->count)
        return;
    CFBD_MenuItem* it = &m->menu_items->pItems[idx];

    /* 使用驱动提供的 clear_area（clear_rect_area 已经会代理） */
    CFBDGraphicSize* size = &it->text_obj.TexthandleSize;
    m->device->ops->clear_area(m->device, it->tl.x, it->tl.y, size->width, size->height);
    CFBDGraphic_DrawText(m->device, &it->text_obj, CCGraphic_AsciiTextItem_RequestOldPoint);
}

static inline int32_t smoothstep_q8(int32_t f, int32_t frames)
{
    int32_t t = (f << 8) / frames; // Q8
    int32_t t2 = (t * t) >> 8;
    return (t2 * (3 * 256 - 2 * t)) >> 8;
}

void OLED_Menu_Select(CFBD_Menu* m, int new_index)
{
    if (new_index < 0)
        new_index = 0;
    CFBD_MenuItemGroup* group = m->menu_items;
    if (new_index >= (int) group->count)
        new_index = (int) group->count - 1;
    int old = m->selected;
    if (old == new_index)
        return;

    uint16_t start_y = (old >= 0 && (size_t) old < group->count) ? group->pItems[old].tl.y
                                                                 : group->pItems[0].tl.y;

    uint16_t end_y = group->pItems[new_index].tl.y;
    int frames = (m->animation.anim_frames > 1) ? m->animation.anim_frames : 1;

#define MAKE_RECT(rx, ry, rw, rh)                                                                  \
    (CFBDGraphicRect)                                                                              \
    {                                                                                              \
        {(uint16_t) (rx), (uint16_t) (ry)},                                                        \
        {                                                                                          \
            (uint16_t) ((rx) + (rw)), (uint16_t) ((ry) + (rh))                                     \
        }                                                                                          \
    }

    CFBDGraphicRect global_upd;
    CFBD_Bool have_global = CFBD_FALSE;

    /* restore old text highlight */
    uint16_t old_text_x = 0, old_text_w = 0, old_text_h = 0;
    if (old >= 0 && (size_t) old < group->count) {
        CFBDGraphic_Text* ot = &group->pItems[old].text_obj;
        old_text_x = ot->tl_point.x;
        old_text_w = rect_width(&ot->text_bounding_rect);
        old_text_h = rect_height(&ot->text_bounding_rect);
        m->device->ops->revert_area(m->device, old_text_x, ot->tl_point.y, old_text_w, old_text_h);
        CFBDGraphicRect r = MAKE_RECT(old_text_x, ot->tl_point.y, old_text_w, old_text_h);
        global_upd = r;
        have_global = CFBD_TRUE;
    }

    /* clear indicator column */
    uint16_t old_h = (old >= 0 && (size_t) old < group->count)
                             ? group->pItems[old].text_obj.TexthandleSize.height
                             : group->pItems[0].text_obj.TexthandleSize.height;
    uint16_t new_h = group->pItems[new_index].text_obj.TexthandleSize.height;
    uint16_t miny = MIN(start_y, end_y);
    uint16_t maxy = MAX(start_y, end_y) + MAX(old_h, new_h);
    uint16_t colx = m->tl_point.x + m->indicator.x, colw = m->indicator.width;
    m->device->ops->clear_area(m->device, colx, miny, colw, maxy - miny);
    {
        CFBDGraphicRect r = MAKE_RECT(colx, miny, colw, maxy - miny);
        if (!have_global) {
            global_upd = r;
            have_global = CFBD_TRUE;
        }
        else {
            global_upd = rect_union(&global_upd, &r);
        }
    }

    /* new text parameters */
    CFBDGraphic_Text* nt = &group->pItems[new_index].text_obj;
    uint16_t new_text_x = nt->tl_point.x;
    uint16_t new_text_w = rect_width(&nt->text_bounding_rect);
    volatile CFBDGraphicRect n = rect_normalize(nt->text_bounding_rect);
    uint16_t new_text_h = (uint32_t) asInt32_t(n.br.y) - (uint32_t) asInt32_t(n.tl.y);

    /* prepare interpolation bases (if old missing, base on new) */
    int base_x = (old >= 0 && (size_t) old < group->count) ? (int) old_text_x : (int) new_text_x;
    int base_w = (old >= 0 && (size_t) old < group->count) ? (int) old_text_w : (int) new_text_w;
    int base_h = (old >= 0 && (size_t) old < group->count) ? (int) old_text_h : (int) new_text_h;

    int last_rev_x = -1, last_rev_y = -1, last_rev_w = 0, last_rev_h = 0;

    for (int f = 0; f <= frames; ++f) {
        int32_t st = smoothstep_q8(f, frames);
#define LERP_Q8(a, b, s) ((a) + (((b) - (a)) * (s) >> 8))
        int cur_y = LERP_Q8(start_y, end_y, st);
        int cur_x = LERP_Q8(base_x, new_text_x, st);
        int cur_w = LERP_Q8(base_w, new_text_w, st);
        int cur_h = LERP_Q8(base_h, new_text_h, st);
#undef LERP_Q8

        if (cur_w < 1)
            cur_w = 1;
        if (cur_h < 1)
            cur_h = 1;

        CFBDGraphicRect frame_upd;
        CFBD_Bool have_frame = CFBD_FALSE;

        /* restore previous frame's revert (before drawing indicator) */
        if (last_rev_y >= 0) {
            m->device->ops->revert_area(m->device,
                                        (uint16_t) last_rev_x,
                                        (uint16_t) last_rev_y,
                                        (uint16_t) last_rev_w,
                                        (uint16_t) last_rev_h);
            CFBDGraphicRect r = MAKE_RECT(last_rev_x, last_rev_y, last_rev_w, last_rev_h);
            frame_upd = r;
            have_frame = CFBD_TRUE;
        }

        /* draw indicator */
        uint16_t ih = MAX(old_h, new_h);

        CFBD_DrawMenuIndicator(m, colx, cur_y, colw, ih);
        {
            CFBDGraphicRect r = MAKE_RECT(colx, cur_y, colw, ih);
            if (!have_frame) {
                frame_upd = r;
                have_frame = CFBD_TRUE;
            }
            else {
                frame_upd = rect_union(&frame_upd, &r);
            }
        }

        /* apply current-frame text revert (this produces the highlight) */
        m->device->ops->revert_area(m->device,
                                    (uint16_t) cur_x,
                                    (uint16_t) cur_y,
                                    (uint16_t) cur_w,
                                    (uint16_t) cur_h);
        {
            CFBDGraphicRect r = MAKE_RECT(cur_x, cur_y, cur_w, cur_h);
            if (!have_frame) {
                frame_upd = r;
                have_frame = CFBD_TRUE;
            }
            else {
                frame_upd = rect_union(&frame_upd, &r);
            }
        }

        last_rev_x = cur_x;
        last_rev_y = cur_y;
        last_rev_w = cur_w;
        last_rev_h = cur_h;

        if (have_frame) {
            uint16_t ux = frame_upd.tl.x;
            uint16_t uy = frame_upd.tl.y;
            uint16_t uw = (uint16_t) (frame_upd.br.x - frame_upd.tl.x);
            uint16_t uh = (uint16_t) (frame_upd.br.y - frame_upd.tl.y);
            m->device->ops->update_area(m->device, ux, uy, uw, uh);
            if (!have_global) {
                global_upd = frame_upd;
                have_global = CFBD_TRUE;
            }
            else
                global_upd = rect_union(&global_upd, &frame_upd);
        }

        system_delay_ms(m->animation.anim_frame_delay_ms);

        /* clear indicator (non-final frame) */
        if (f != frames) {
            m->device->ops->clear_area(m->device, colx, cur_y, colw, ih);
            CFBDGraphicRect r = MAKE_RECT(colx, cur_y, colw, ih);
            if (!have_global) {
                global_upd = r;
                have_global = CFBD_TRUE;
            }
            else {
                global_upd = rect_union(&global_upd, &r);
            }
        }
    }

    /* redraw old item and include into global update */
    if (old >= 0 && (size_t) old < group->count) {
        redraw_item(m, old);
        CFBDGraphic_Text* ot = &group->pItems[old].text_obj;
        CFBDGraphicRect r = MAKE_RECT(ot->tl_point.x,
                                      ot->tl_point.y,
                                      rect_width(&ot->text_bounding_rect),
                                      rect_height(&ot->text_bounding_rect));
        if (!have_global) {
            global_upd = r;
            have_global = CFBD_TRUE;
        }
        else
            global_upd = rect_union(&global_upd, &r);
    }

    /* final correction: if last_rev differs from exact end, restore and set exact final */
    if (last_rev_y >= 0 && (last_rev_y != (int) end_y || last_rev_w != (int) new_text_w ||
                            last_rev_x != (int) new_text_x || last_rev_h != (int) new_text_h)) {
        /* restore last */
        m->device->ops->revert_area(m->device,
                                    (uint16_t) last_rev_x,
                                    (uint16_t) last_rev_y,
                                    (uint16_t) last_rev_w,
                                    (uint16_t) last_rev_h);
        CFBDGraphicRect r1 = MAKE_RECT(last_rev_x, last_rev_y, last_rev_w, last_rev_h);
        if (!have_global) {
            global_upd = r1;
            have_global = CFBD_TRUE;
        }
        else {
            global_upd = rect_union(&global_upd, &r1);
        }

        /* final exact revert at end_y with new size */
        m->device->ops->revert_area(m->device, new_text_x, end_y, new_text_w, new_text_h);
        CFBDGraphicRect r2 = MAKE_RECT(new_text_x, end_y, new_text_w, new_text_h);
        if (!have_global) {
            global_upd = r2;
            have_global = CFBD_TRUE;
        }
        else {
            global_upd = rect_union(&global_upd, &r2);
        }

        /* flush final small region */
        uint16_t ux = global_upd.tl.x, uy = global_upd.tl.y;
        uint16_t uw = (uint16_t) (global_upd.br.x - global_upd.tl.x);
        uint16_t uh = (uint16_t) (global_upd.br.y - global_upd.tl.y);
        m->device->ops->update_area(m->device, ux, uy, uw, uh);
    }
    else {
        if (have_global) {
            uint16_t ux = global_upd.tl.x, uy = global_upd.tl.y;
            uint16_t uw = (uint16_t) (global_upd.br.x - global_upd.tl.x);
            uint16_t uh = (uint16_t) (global_upd.br.y - global_upd.tl.y);
            m->device->ops->update_area(m->device, ux, uy, uw, uh);
        }
    }

#undef MAKE_RECT
    m->selected = new_index;
}

static void reset_tl_points(CFBD_Menu* m, CFBDGraphic_Point* p, CFBD_Bool request_updates)
{
    m->device->ops->clear_area(m->device,
                               m->tl_point.x,
                               m->tl_point.y,
                               m->max_width,
                               get_menu_new_item_y(m) - m->tl_point.y);
    m->tl_point = *p;
    PointBaseType y_height = CFBD_MENU_ITEM_Y_GAP + m->tl_point.y;
    for (int i = 0; i < m->menu_items->count; i++) {
        CFBD_MenuItem* item = &m->menu_items->pItems[i];
        item->tl.x = get_menu_new_item_x(m);
        item->tl.y = y_height;
        CFBDGraphic_SetTextTLPointPoint(&item->text_obj, &item->tl);
        y_height += get_menu_new_item_height(item) - CFBD_MENU_ITEM_Y_GAP;
    }
    if (request_updates) {
        m->device->ops->update_area(m->device,
                                    m->tl_point.x,
                                    m->tl_point.y,
                                    m->max_width,
                                    get_menu_new_item_y(m) - m->tl_point.y);
        CFBD_DrawMenu(m);
    }
}

static void OLED_Menu_Activate(CFBD_Menu* m)
{
    if (m->selected < 0 || (size_t) m->selected >= m->menu_items->count)
        return;
    CFBD_MenuItem* it = &m->menu_items->pItems[m->selected];
    CFBD_MenuItemCallbackPack* pack = &it->on_select;
    if (pack->callback) {
        pack->callback(pack->user_data);
    }
}

static CFBD_MenuOps _ops = {.bind_item_groups_contains = CFBD_BindMenuItems,
                            .set_animation = CFBD_AnimationSet,
                            .add_item = CFBD_MenuAddItems,
                            .immediate_draw = CFBD_DrawMenu,
                            .activate_current = OLED_Menu_Activate,
                            .select_index = OLED_Menu_Select,
                            .reset_tl_points = reset_tl_points};

void CFBD_InitMenu(CFBD_Menu* pMenu,
                   CFBD_GraphicDevice* devices,
                   CFBD_MenuItemGroup* assigned_menu_items,
                   SizeBaseType max_width)
{
    pMenu->operations = &_ops;
    pMenu->max_width = max_width;
    pMenu->device = devices;
    pMenu->tl_point.x = 0;
    pMenu->tl_point.y = 0;
    CFBD_InitBaseAnimation(&pMenu->animation);
    CFBD_InitDefaultMenuIndicator(&pMenu->indicator);
    pMenu->menu_items = assigned_menu_items;
    pMenu->selected = 0;
}
