#pragma once
#include <stdint.h>

#include "base/point.h"
#include "base/rectangle.h"
#include "base/size.h"
#include "cfbd_graphic_define.h"
#include "text_config.h"

#define CFBDGraphic_TEXT_PADDING_WIDTH (1)
#define CFBDGraphic_TEXT_PADDING_HEIGHT (0)

typedef struct
{
    char* sources_borrowed;
    CFBDGraphic_Point tl_point;
    CFBDGraphic_Point indexed_point;
    CFBDGraphicSize TexthandleSize;
    Ascii_Font_Size font_size;
    CFBDGraphicRect text_bounding_rect;
} CFBDGraphic_Text;

typedef enum
{
    CCGraphic_AsciiTextItem_AppendContinously,
    CCGraphic_AsciiTextItem_AppendNextLine,
    CCGraphic_AsciiTextItem_RequestOldPoint
} AppendMethod;

void CFBDGraphic_InitText(CFBDGraphic_Text* item,
                          CFBDGraphic_Point tl_point,
                          CFBDGraphicSize textHandleSize,
                          Ascii_Font_Size text_size);

static inline void CFBDGraphic_SetText(CFBDGraphic_Text* item, char* text)
{
    item->sources_borrowed = text;
}

static inline void CFBDGraphic_SetTextTLPointPoint(CFBDGraphic_Text* item, CFBDGraphic_Point* p)
{
    item->tl_point = *p;
    item->indexed_point = *p;
}

static inline void CFBDGraphic_SetTextIndexedPoint(CFBDGraphic_Text* item, CFBDGraphic_Point* p)
{
    item->indexed_point = *p;
}

static inline void CCGraphicWidget_AsciiTextItem_relocate(CFBDGraphic_Text* item,
                                                          CFBDGraphic_Point* tl_point,
                                                          CFBDGraphicSize textHandleSize)
{
    item->tl_point = *tl_point;
    item->TexthandleSize = textHandleSize;
}

CFBDGraphic_Point CFBDGraphic_DrawText(CFBD_GraphicDevice* device_handle,
                                       CFBDGraphic_Text* item,
                                       AppendMethod method);
