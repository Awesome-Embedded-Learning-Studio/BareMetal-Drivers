
#include "text.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "base/base_helpers.h"
#include "base/point.h"
#include "base/size.h"
#include "base_support/image.h"
#include "cfbd_graphic_define.h"
#include "widget/text.h"

void CFBDGraphic_InitText(CFBDGraphic_Text* item,
                          CFBDGraphic_Point tl_point,
                          CFBDGraphicSize textHandleSize,
                          Ascii_Font_Size text_size)
{
    item->font_size = text_size;
    item->sources_borrowed = "";
    item->tl_point = tl_point;
    item->indexed_point = tl_point;
    item->TexthandleSize = textHandleSize;
}

static void __pvt_draw_char_each(CFBD_GraphicDevice* device_handle,
                                 CCGraphic_Image* borrowing_image,
                                 const char ch,
                                 Ascii_Font_Size size)
{
    borrowing_image->image_size = __fetch_font_size(size);
    uint8_t* ascii = __select_from_ascii_font_size(size, ch);
    borrowing_image->sources_register = ascii;
    CFBDGraphic_DrawImage(device_handle, borrowing_image);
}

static uint8_t inline __pvt_should_be_next_line(CFBD_GraphicDevice* device_handle,
                                                CFBDGraphic_Point* brpoint,
                                                CFBDGraphic_Point* cur_draw_p,
                                                Ascii_Font_Size s)
{
    return cur_draw_p->x + (int16_t) (1.5 * __fetch_font_size(s).width) >= brpoint->x;
}

static CFBDGraphic_Point inline __pvt_fetch_valid_final_point(CFBD_GraphicDevice* device_handle,
                                                              CFBDGraphicSize* size,
                                                              CFBDGraphic_Point* tl)
{
    CFBDGraphic_Point br;
    CFBDGraphicSize device_size;
    CFBDGraphic_GetScreenSize(device_handle, &device_size);

    br.x = tl->x + size->width;
    br.y = tl->y + size->height;
    if (device_size.width < br.x) {
        br.x = device_size.width;
    }
    if (device_size.height < br.y) {
        br.y = device_size.height;
    }
    return br;
}

CFBDGraphic_Point
CFBDGraphic_DrawText(CFBD_GraphicDevice* device_handle, CFBDGraphic_Text* item, AppendMethod method)
{
    if (!device_handle || !item || !device_handle->ops)
        return item->tl_point;

    if (strcmp(item->sources_borrowed, "") == 0)
        return item->tl_point;
    CFBDGraphic_Point old = item->tl_point;
    CCGraphic_Image handle_draw_image;
    CFBDGraphic_Point draw_tl_point = item->indexed_point;

    const Ascii_Font_Size font_size = item->font_size;
    const CFBDGraphicSize size = __fetch_font_size(font_size);
    const SizeBaseType font_width = size.width;
    const SizeBaseType font_height = size.height;

    CFBDGraphic_Point br = __pvt_fetch_valid_final_point(device_handle,
                                                         &(item->TexthandleSize),
                                                         &(item->tl_point));

    uint8_t offseterx = 0;
    uint8_t offsetery = 0;

    int32_t lx = INT32_MAX;
    int32_t ty = INT32_MAX;
    int32_t rx = INT32_MIN;
    int32_t by = INT32_MIN;

    for (uint8_t i = 0; item->sources_borrowed[i] != '\0'; i++) {
        draw_tl_point.x = item->indexed_point.x + offseterx * font_width;
        draw_tl_point.y = item->indexed_point.y + offsetery * font_height;

        handle_draw_image.point = draw_tl_point;

        __pvt_draw_char_each(device_handle,
                             &handle_draw_image,
                             item->sources_borrowed[i],
                             item->font_size);

        int32_t cx1 = asInt32_t(draw_tl_point.x);
        int32_t cy1 = asInt32_t(draw_tl_point.y);
        int32_t cx2 = cx1 + font_width;
        int32_t cy2 = cy1 + font_height;

        if (cx1 < lx)
            lx = cx1;
        if (cy1 < ty)
            ty = cy1;
        if (cx2 > rx)
            rx = cx2;
        if (cy2 > by)
            by = cy2;

        if (__pvt_should_be_next_line(device_handle, &br, &draw_tl_point, font_size)) {
            offseterx = 0;
            offsetery++;
            item->indexed_point.x = item->tl_point.x;
        }
        else {
            offseterx++;
        }
    }

    /* 缓存 TextBoundingRect */
    if (lx <= rx && ty <= by) {
        item->text_bounding_rect.tl.x = clamp_u16_from_i32(lx - CFBDGraphic_TEXT_PADDING_WIDTH);
        item->text_bounding_rect.tl.y = clamp_u16_from_i32(ty - CFBDGraphic_TEXT_PADDING_HEIGHT);
        item->text_bounding_rect.br.x = clamp_u16_from_i32(rx + CFBDGraphic_TEXT_PADDING_WIDTH);
        item->text_bounding_rect.br.y = clamp_u16_from_i32(by + CFBDGraphic_TEXT_PADDING_HEIGHT);
    }
    else {
        item->text_bounding_rect.tl = item->indexed_point;
        item->text_bounding_rect.br = item->indexed_point;
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(device_handle)) {
        device_handle->ops->update_area(device_handle,
                                        clamp_u16_from_i32(lx),
                                        clamp_u16_from_i32(ty),
                                        clamp_u16_from_i32(rx - lx + 1),
                                        clamp_u16_from_i32(by - ty + 1));
    }

    switch (method) {
        case CCGraphic_AsciiTextItem_AppendNextLine:
            item->indexed_point.x = item->tl_point.x;
            item->indexed_point.y += font_height;
            break;
        case CCGraphic_AsciiTextItem_AppendContinously:
            item->indexed_point.x += font_width;
            break;
        case CCGraphic_AsciiTextItem_RequestOldPoint:
            item->indexed_point = old;
            break;
        default:
            break;
    }

    return item->indexed_point;
}
