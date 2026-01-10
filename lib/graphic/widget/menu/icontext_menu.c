/**
 * @file icontext_menu.c
 * @brief Icon-text menu widget implementation
 * @details Implementation of horizontal scrollable menu with icon+text items,
 *          smooth animations, and touch-based navigation.
 */

#include "icontext_menu.h"

#include <stdint.h>
#include <string.h>

#include "base/rectangle.h"
#include "cfbd_define.h"
#include "device/graphic_device.h"
#include "sys_clock/system_clock.h"
#include "widget/base_support/common/helpers.h"
#include "widget/text_config.h"

/**
 * @brief Calculate the X position for a menu item based on its index and scroll offset
 * @param pMenu - Menu structure
 * @param index - Item index
 * @return int16_t - Calculated X position (可以为负数)
 */
static int16_t _calculate_item_x(CFBD_IconTextMenu* pMenu, size_t index)
{
    int32_t x =
            pMenu->viewport.tl.x + (int32_t) (index * (pMenu->item_width + pMenu->item_spacing));
    x += pMenu->scroll_offset;
    return (int16_t) x;
}

/**
 * @brief Calculate the Y position for a menu item (center vertically in viewport)
 * @param pMenu - Menu structure
 * @return uint16_t - Calculated Y position
 */
static uint16_t _calculate_item_y(CFBD_IconTextMenu* pMenu)
{
    uint16_t viewport_height = pMenu->viewport.br.y - pMenu->viewport.tl.y;
    int16_t y_offset = (viewport_height - pMenu->item_height) / 2;
    return pMenu->viewport.tl.y + y_offset;
}

static void
_update_icon_widget(CFBD_IconTextMenu* pMenu, CFBD_IconTextMenuItem* item, int16_t item_x)
{
    int16_t icon_x = item_x + (int16_t) ((pMenu->item_width - item->icon_size.width) / 2);
    uint16_t icon_y = _calculate_item_y(pMenu) + CFBD_ICONTEXT_MENU_ICON_TEXT_GAP;

    item->icon_widget.point.x = (uint16_t) icon_x;
    item->icon_widget.point.y = icon_y;
}

/**
 * @brief Calculate text pixel width (需要根据实际字体系统实现)
 * @param text - Text string
 * @param font_size - Font size
 * @return uint16_t - Text width in pixels
 */
static uint16_t _calculate_text_width(const char* text, uint8_t font_size)
{
    // 简单估算：每个字符宽度 = font_size * 0.6 (需要根据实际字体调整)
    // 或者调用实际的字体宽度计算函数
    if (!text)
        return 0;

    uint16_t char_count = strlen(text);
    uint16_t fwidth = __fetch_font_size(font_size).width;
    return char_count * fwidth;
}

static void
_update_text_widget(CFBD_IconTextMenu* pMenu, CFBD_IconTextMenuItem* item, int16_t item_x)
{
    uint16_t text_y = _calculate_item_y(pMenu) + item->icon_size.height +
                      CFBD_ICONTEXT_MENU_ICON_TEXT_GAP * 2;

    // 计算文字实际宽度
    uint16_t text_width = _calculate_text_width(item->label, CFBD_ICONTEXT_MENU_TEXT_SIZE);

    uint16_t text_x;
    if (text_width < pMenu->item_width) {
        // 文字宽度小于item宽度：居中对齐
        text_x = item_x + (pMenu->item_width - text_width) / 2;
    }
    else {
        // 文字宽度大于等于item宽度：左对齐
        text_x = item_x;
    }

    item->text_widget.tl_point.x = text_x;
    item->text_widget.tl_point.y = text_y;
    item->text_widget.indexed_point = item->text_widget.tl_point;

    item->text_widget.TexthandleSize.width = pMenu->item_width;
    item->text_widget.TexthandleSize.height = 8;
}

/**
 * @brief Check if item's ICON is within visible viewport
 * @param pMenu - Menu structure
 * @param item - Menu item
 * @param item_x - Item's calculated X position
 * @return CFBD_Bool - CFBD_TRUE if icon is visible
 */
static CFBD_Bool
_is_item_visible(CFBD_IconTextMenu* pMenu, CFBD_IconTextMenuItem* item, int16_t item_x)
{
    // 计算图标的实际位置（居中在item中）
    int16_t icon_x = item_x + (int16_t) ((pMenu->item_width - item->icon_size.width) / 2);
    int16_t icon_right = icon_x + (int16_t) item->icon_size.width;

    int16_t viewport_left = pMenu->viewport.tl.x;
    int16_t viewport_right = pMenu->viewport.br.x;

    // 检查图标（而不是item）是否与视口有交集
    return (icon_right > viewport_left) && (icon_x < viewport_right);
}

/**
 * @brief Draw selection frame around the selected item's ICON only (not text)
 * @param pMenu - Menu structure
 * @param item - Current item
 * @param item_x - Item's X position
 */
static void
_draw_selection_frame(CFBD_IconTextMenu* pMenu, CFBD_IconTextMenuItem* item, int16_t item_x)
{
    if (!pMenu || !pMenu->device || !item) {
        return;
    }

    // Calculate icon position (same as _update_icon_widget)
    int16_t icon_x = item_x + (int16_t) ((pMenu->item_width - item->icon_size.width) / 2);
    uint16_t icon_y = _calculate_item_y(pMenu) + CFBD_ICONTEXT_MENU_ICON_TEXT_GAP;

    // Frame around icon only (not text)
    int16_t frame_x = icon_x - pMenu->selection_border_width;
    int16_t frame_y = icon_y - pMenu->selection_border_width;
    uint16_t frame_width = item->icon_size.width + 2 * pMenu->selection_border_width;
    uint16_t frame_height = item->icon_size.height + 2 * pMenu->selection_border_width;

    uint16_t border = pMenu->selection_border_width;

    // Draw four borders using draw_pixel
    // Top border
    for (uint16_t y = 0; y < border; y++) {
        for (uint16_t x = 0; x < frame_width; x++) {
            pMenu->device->ops->setPixel(pMenu->device, frame_x + x, frame_y + y);
        }
    }

    // Bottom border
    for (uint16_t y = 0; y < border; y++) {
        for (uint16_t x = 0; x < frame_width; x++) {
            pMenu->device->ops->setPixel(pMenu->device,
                                         frame_x + x,
                                         frame_y + frame_height - border + y);
        }
    }

    // Left border
    for (uint16_t y = border; y < frame_height - border; y++) {
        for (uint16_t x = 0; x < border; x++) {
            pMenu->device->ops->setPixel(pMenu->device, frame_x + x, frame_y + y);
        }
    }

    // Right border
    for (uint16_t y = border; y < frame_height - border; y++) {
        for (uint16_t x = 0; x < border; x++) {
            pMenu->device->ops->setPixel(pMenu->device,
                                         frame_x + frame_width - border + x,
                                         frame_y + y);
        }
    }
}

/**
 * @brief Update scroll offset to center the selected item
 * @param pMenu - Menu structure
 */
static void _center_selected_item(CFBD_IconTextMenu* pMenu)
{
    if (!pMenu) {
        return;
    }

    int16_t viewport_width = pMenu->viewport.br.x - pMenu->viewport.tl.x;
    int16_t center_offset = (viewport_width - pMenu->item_width) / 2;

    pMenu->scroll_offset =
            -pMenu->selected_index * (pMenu->item_width + pMenu->item_spacing) + center_offset;
    pMenu->target_scroll_offset = pMenu->scroll_offset;
    pMenu->prev_scroll_offset = pMenu->scroll_offset;
}

/* ========== Static Implementation Functions (prefixed with CFBD_) ========== */

static CFBD_Bool CFBD_IconTextMenuAddItem(CFBD_IconTextMenu* pMenu,
                                          const char* label,
                                          uint8_t* icon_bitmap,
                                          CFBDGraphicSize* icon_size,
                                          CFBD_MenuItemCallbackPack* callback)
{
    if (!pMenu || pMenu->item_count >= pMenu->capacity || !label || !icon_bitmap) {
        return CFBD_FALSE;
    }

    CFBD_IconTextMenuItem* item = &pMenu->items[pMenu->item_count];

    item->label = label;
    item->icon_bitmap = icon_bitmap;
    item->icon_size = *icon_size;
    item->state = CFBD_ICONTEXT_ITEM_NORMAL;
    item->user_data = NULL;

    if (callback) {
        item->callback = *callback;
    }
    else {
        item->callback.callback = NULL;
        item->callback.user_data = NULL;
    }

    item->render_position.x = 0;
    item->render_position.y = 0;

    CFBDGraphic_InitImage(&item->icon_widget, &item->render_position, icon_size, icon_bitmap);

    CFBDGraphicSize text_area = {.width = pMenu->item_width, .height = 8};
    CFBDGraphic_InitText(&item->text_widget,
                         item->render_position,
                         text_area,
                         CFBD_ICONTEXT_MENU_TEXT_SIZE);
    item->text_widget.no_wrap = CFBD_TRUE;
    CFBDGraphic_SetText(&item->text_widget, (char*) label);

    pMenu->item_count++;
    return CFBD_TRUE;
}

static void CFBD_IconTextMenuScroll(CFBD_IconTextMenu* pMenu,
                                    CFBD_IconTextScrollDirection direction)
{
    if (!pMenu || pMenu->item_count == 0 || pMenu->is_animating) {
        return;
    }

    int new_index = pMenu->selected_index;

    if (direction == CFBD_ICONTEXT_SCROLL_RIGHT) {
        new_index++;
        if (new_index >= (int) pMenu->item_count) {
            if (pMenu->is_circular) {
                new_index = 0;
            }
            else {
                return;
            }
        }
    }
    else if (direction == CFBD_ICONTEXT_SCROLL_LEFT) {
        new_index--;
        if (new_index < 0) {
            if (pMenu->is_circular) {
                new_index = pMenu->item_count - 1;
            }
            else {
                return;
            }
        }
    }
    else {
        return;
    }

    pMenu->selected_index = new_index;
    pMenu->scroll_direction = direction;

    // Calculate target offset to center the new selected item
    int16_t viewport_width = pMenu->viewport.br.x - pMenu->viewport.tl.x;
    int16_t center_offset = (viewport_width - pMenu->item_width) / 2;
    pMenu->target_scroll_offset =
            -new_index * (pMenu->item_width + pMenu->item_spacing) + center_offset;

    pMenu->animation_start_offset = pMenu->scroll_offset;
    pMenu->animation_elapsed_ms = 0;
    pMenu->anim_current_frame = 0;
    pMenu->is_animating = CFBD_TRUE;
}

static CFBD_Bool CFBD_IconTextMenuSelectIndex(CFBD_IconTextMenu* pMenu, int index)
{
    if (!pMenu || index < 0 || index >= (int) pMenu->item_count) {
        return CFBD_FALSE;
    }

    pMenu->selected_index = index;

    // Center the selected item immediately
    _center_selected_item(pMenu);

    return CFBD_TRUE;
}

static void CFBD_IconTextMenuActivateCurrent(CFBD_IconTextMenu* pMenu)
{
    if (!pMenu || pMenu->selected_index < 0 || pMenu->selected_index >= (int) pMenu->item_count) {
        return;
    }

    CFBD_IconTextMenuItem* item = &pMenu->items[pMenu->selected_index];

    item->state = CFBD_ICONTEXT_ITEM_PRESSED;

    if (item->callback.callback) {
        item->callback.callback(item->callback.user_data);
    }

    item->state = CFBD_ICONTEXT_ITEM_FOCUSED;
}

static void CFBD_IconTextMenuDraw(CFBD_IconTextMenu* pMenu)
{
    if (!pMenu || !pMenu->device || pMenu->item_count == 0) {
        return;
    }

    // 【修复1】扩大清除区域，确保边框也被清除
    int16_t clear_x = pMenu->viewport.tl.x;
    int16_t clear_y = pMenu->viewport.tl.y;
    uint16_t clear_w = rect_width(&pMenu->viewport);
    uint16_t clear_h = rect_height(&pMenu->viewport);

    // 向外扩展边框宽度的距离
    if (clear_x >= pMenu->selection_border_width) {
        clear_x -= pMenu->selection_border_width;
        clear_w += pMenu->selection_border_width * 2;
    }
    if (clear_y >= pMenu->selection_border_width) {
        clear_y -= pMenu->selection_border_width;
        clear_h += pMenu->selection_border_width * 2;
    }

    pMenu->device->ops->clear_area(pMenu->device, clear_x, clear_y, clear_w, clear_h);

    for (size_t i = 0; i < pMenu->item_count; i++) {
        CFBD_IconTextMenuItem* item = &pMenu->items[i];
        int16_t item_x = _calculate_item_x(pMenu, i);

        if (!_is_item_visible(pMenu, item, item_x)) {
            continue;
        }

        _update_icon_widget(pMenu, item, item_x);
        _update_text_widget(pMenu, item, item_x);

        if ((int) i == pMenu->selected_index) {
            _draw_selection_frame(pMenu, item, item_x);
        }

        if (item->icon_bitmap) {
            CFBDGraphic_DrawImageClipped(pMenu->device, &item->icon_widget, &pMenu->viewport);
        }

        CFBDGraphic_DrawText(pMenu->device,
                             &item->text_widget,
                             CCGraphic_AsciiTextItem_RequestOldPoint);
    }

    pMenu->device->ops->update_area(pMenu->device,
                                    pMenu->viewport.tl.x,
                                    pMenu->viewport.tl.y,
                                    rect_width(&pMenu->viewport),
                                    rect_height(&pMenu->viewport));

    pMenu->prev_scroll_offset = pMenu->scroll_offset;
}

static CFBD_Bool CFBD_IconTextMenuUpdate(CFBD_IconTextMenu* pMenu, uint32_t delta_ms)
{
    (void) delta_ms;

    if (!pMenu || !pMenu->is_animating)
        return CFBD_FALSE;

    uint8_t max_frames = pMenu->scroll_animation.anim_frames;

    if (pMenu->anim_current_frame < max_frames)
        pMenu->anim_current_frame++;

    int32_t start = pMenu->animation_start_offset;
    int32_t delta = (int32_t) pMenu->target_scroll_offset - start;

    pMenu->scroll_offset = (int16_t) (start + (delta * pMenu->anim_current_frame) / max_frames);

    if (pMenu->anim_current_frame >= max_frames) {
        pMenu->scroll_offset = pMenu->target_scroll_offset;
        pMenu->is_animating = CFBD_FALSE;
        pMenu->anim_current_frame = 0;
        return CFBD_FALSE;
    }

    return CFBD_TRUE;
}

static void CFBD_IconTextMenuSetDimensions(CFBD_IconTextMenu* pMenu,
                                           SizeBaseType width,
                                           SizeBaseType height,
                                           uint16_t spacing)
{
    pMenu->item_width = width;
    pMenu->item_height = height;
    pMenu->item_spacing = spacing;
}

static CFBD_IconTextMenuItem* CFBD_IconTextMenuGetSelectedItem(CFBD_IconTextMenu* pMenu)
{
    if (!pMenu || pMenu->selected_index < 0 || pMenu->selected_index >= (int) pMenu->item_count) {
        return NULL;
    }
    return &pMenu->items[pMenu->selected_index];
}

static CFBD_IconTextMenuItem* CFBD_IconTextMenuGetItem(CFBD_IconTextMenu* pMenu, int index)
{
    if (!pMenu || index < 0 || index >= (int) pMenu->item_count) {
        return NULL;
    }
    return &pMenu->items[index];
}

static void CFBD_IconTextMenuSetCircular(CFBD_IconTextMenu* pMenu, CFBD_Bool is_circular)
{
    if (!pMenu) {
        return;
    }
    pMenu->is_circular = is_circular;
}

static void CFBD_IconTextMenuSetSelectionBorder(CFBD_IconTextMenu* pMenu, uint16_t border_width)
{
    if (!pMenu) {
        return;
    }
    pMenu->selection_border_width = border_width;
}

static void CFBD_IconTextMenuAnimateScroll(CFBD_IconTextMenu* pMenu,
                                           CFBD_IconTextScrollDirection direction)
{
    if (!pMenu || pMenu->item_count == 0) {
        return;
    }

    pMenu->operations->scroll(pMenu, direction);

    if (!pMenu->is_animating) {
        return;
    }

    uint32_t frame_delay = pMenu->scroll_animation.anim_frame_delay_ms;

    while (pMenu->is_animating) {
        pMenu->operations->update(pMenu, 0);
        pMenu->operations->immediate_draw(pMenu);

        if (frame_delay > 0) {
            system_delay_ms(frame_delay);
        }
    }
}

/* ========== Operation Table ========== */

static CFBD_IconTextMenuOps _cfbd_icontext_menu_ops = {
        .add_item = CFBD_IconTextMenuAddItem,
        .scroll = CFBD_IconTextMenuScroll,
        .select_index = CFBD_IconTextMenuSelectIndex,
        .activate_current = CFBD_IconTextMenuActivateCurrent,
        .immediate_draw = CFBD_IconTextMenuDraw,
        .update = CFBD_IconTextMenuUpdate,
        .set_dimensions = CFBD_IconTextMenuSetDimensions,
        .get_selected_item = CFBD_IconTextMenuGetSelectedItem,
        .get_item = CFBD_IconTextMenuGetItem,
        .set_circular = CFBD_IconTextMenuSetCircular,
        .set_selection_border = CFBD_IconTextMenuSetSelectionBorder,
        .animate_scroll = CFBD_IconTextMenuAnimateScroll};

/* ========== Public API ========== */

void CFBD_InitIconTextMenu(CFBD_IconTextMenu* pMenu,
                           CFBD_GraphicDevice* device,
                           CFBDGraphicRect* viewport,
                           CFBD_IconTextMenuItem* items_buffer,
                           size_t capacity)
{
    pMenu->operations = &_cfbd_icontext_menu_ops;
    pMenu->items = items_buffer;
    memset(pMenu->items, 0, capacity * sizeof(CFBD_IconTextMenuItem));

    pMenu->device = device;
    pMenu->viewport = *viewport;
    pMenu->capacity = capacity;
    pMenu->item_count = 0;

    pMenu->selected_index = 0;
    pMenu->first_visible_index = 0;
    pMenu->scroll_offset = 0;
    pMenu->prev_scroll_offset = 0;
    pMenu->target_scroll_offset = 0;
    pMenu->scroll_direction = CFBD_ICONTEXT_SCROLL_NONE;
    pMenu->item_width = CFBD_ICONTEXT_MENU_DEFAULT_ITEM_WIDTH;
    pMenu->item_height = CFBD_ICONTEXT_MENU_DEFAULT_ITEM_HEIGHT;
    pMenu->item_spacing = 5;
    pMenu->is_animating = CFBD_FALSE;
    pMenu->is_circular = CFBD_FALSE;

    pMenu->selection_border_width = 1;

    pMenu->animation_start_offset = 0;
    pMenu->animation_elapsed_ms = 0;
    pMenu->anim_current_frame = 0;

    CFBD_InitBaseAnimation(&pMenu->scroll_animation);
    pMenu->scroll_animation.anim_frames = 10;
    pMenu->scroll_animation.anim_frame_delay_ms = 30;
}