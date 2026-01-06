#include "menu_indicator.h"

#include <stddef.h>

#include "cfbd_define.h"
#include "device/graphic_device.h"
#include "menu.h"
#include "menu_config.h"


static uint8_t s_indicator_buf[((CFBD_MENU_INDICATOR_MAX_WIDTH_PX + 7) / 8) *
                               CFBD_MENU_INDICATOR_MAX_HEIGHT_PX];

/* compute bytes per row for given pixel width (1bpp) */
static inline size_t bytes_per_row_px(uint16_t px_width)
{
    return (px_width + 7) / 8;
}

/* fill a byte buffer with 0xFF (set pixels) or 0x00 (clear) */
static void blit_fill_rect_bytes(uint8_t* buf, uint16_t w_px, uint16_t h_px, uint8_t fill_byte)
{
    size_t rowb = bytes_per_row_px(w_px);
    for (uint16_t r = 0; r < h_px; ++r) {
        uint8_t* rowp = buf + r * rowb;
        for (size_t i = 0; i < rowb; ++i) {
            rowp[i] = fill_byte;
        }
    }
}

void CFBD_InitDefaultMenuIndicator(CFBD_MenuIndicator* menuIndicatorDefault)
{
    menuIndicatorDefault->width = CFBD_MENU_INDICATOR_WIDTH;
    menuIndicatorDefault->x = CFBD_MENU_INDICATOR_POSX;
}

CFBD_Bool CFBD_DrawMenuIndicator(CFBD_Menu* menu, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    CFBD_GraphicDevice* dev = menu->device;

    /* safety clamp (防止越界) */
    if (w > CFBD_MENU_INDICATOR_MAX_WIDTH_PX || h > CFBD_MENU_INDICATOR_MAX_HEIGHT_PX) {
        return CFBD_FALSE;
    }

    blit_fill_rect_bytes(s_indicator_buf, w, h, 0xFF);

    return dev->ops->setArea(dev, x, y, w, h, s_indicator_buf);
}