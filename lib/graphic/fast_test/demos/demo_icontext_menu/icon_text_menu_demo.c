/**
 * @file icon_text_menu_demo.c
 * @brief Icon-text menu demonstration (OOP C 风格)
 * @details 展示如何初始化和使用图标文字菜单系统
 *
 * Icon Design Notes (OLED Page-Based Format):
 * - 24x24 pixels icon for 128x64 OLED display
 * - OLED Memory Layout: Pages (vertical 8-pixel rows) × Columns
 * - 24 pixels high = 3 pages (0-7, 8-15, 16-23)
 * - 24 pixels wide = 24 columns
 * - Total: 24 bytes/page × 3 pages = 72 bytes
 * - Each byte: 8 vertical pixels (MSB at top, LSB at bottom)
 * - Layout: Page0[0..23] + Page1[0..23] + Page2[0..23]
 */

#include <stdint.h>

#include "cfbd_define.h"
#include "sys_clock/system_clock.h"
#include "widget/menu/icontext_menu.h"

extern uint8_t icontext_icon_home[72];
extern uint8_t icontext_icon_health[72];
extern uint8_t icontext_icon_settings[72];
extern uint8_t icontext_icon_messages[72];

/**
 * @brief Menu item callback - Home
 */
void on_home_selected(void* user_data)
{
    (void) user_data;
    // Handle home selection
}

/**
 * @brief Menu item callback - Health
 */
void on_health_selected(void* user_data)
{
    (void) user_data;
    // Handle health selection
}

/**
 * @brief Menu item callback - Settings
 */
void on_settings_selected(void* user_data)
{
    (void) user_data;
    // Handle settings selection
}

/**
 * @brief Menu item callback - Messages
 */
void on_messages_selected(void* user_data)
{
    (void) user_data;
    // Handle messages selection
}

/**
 * @brief Icon-text menu demonstration (OOP C style)
 * @details Demonstrates:
 *          1. Menu initialization
 *          2. Adding menu items via operations table
 *          3. Auto-demo of scrolling and selection with animated frame
 */
void icontext_menu_demo(CFBD_GraphicDevice* dev)
{
    CFBD_IconTextMenuItem items_buffer[6]; // Max 6 items on stack
    CFBD_IconTextMenu menu;

    CFBDGraphicRect viewport = {
            .tl = {.x = 0, .y = 8},   // Top Y=8
            .br = {.x = 128, .y = 56} // Bottom Y=56 (128x64 display)
    };

    CFBD_InitIconTextMenu(&menu, dev, &viewport, items_buffer, 6);

    menu.operations->set_dimensions(&menu,
                                    40,  // Item width
                                    36,  // Item height
                                    10); // Spacing

    menu.operations->set_circular(&menu, CFBD_TRUE);

    menu.operations->set_selection_border(&menu, 1);

    /* =======================
     * Step 4: Configure icon size
     * ======================= */
    CFBDGraphicSize icon_size = {.width = 24, .height = 24}; // 24x24 icon

    // Add "Home" item (via operations table)
    CFBD_MenuItemCallbackPack home_cb = {.callback = on_home_selected, .user_data = NULL};
    menu.operations->add_item(&menu, "Home", icontext_icon_home, &icon_size, &home_cb);

    // Add "Health" item
    CFBD_MenuItemCallbackPack health_cb = {.callback = on_health_selected, .user_data = NULL};
    menu.operations->add_item(&menu, "Health", icontext_icon_health, &icon_size, &health_cb);

    // Add "Settings" item
    CFBD_MenuItemCallbackPack settings_cb = {.callback = on_settings_selected, .user_data = NULL};
    menu.operations->add_item(&menu, "Settings", icontext_icon_settings, &icon_size, &settings_cb);

    // Add "Messages" item
    CFBD_MenuItemCallbackPack messages_cb = {.callback = on_messages_selected, .user_data = NULL};
    menu.operations->add_item(&menu, "Messages", icontext_icon_messages, &icon_size, &messages_cb);

    // Select first item initially (自动居中)
    menu.operations->select_index(&menu, 0);
    menu.operations->immediate_draw(&menu);

    /* =======================
     * Step 5: Demo loop - 展示所有图标，带动画边框
     * ======================= */
    // 初始停留在第一个图标 (index 0)
    system_delay_ms(500);

    // 向右滚动：0 -> 1 -> 2 -> 3 (展示所有4个图标)
    // animate_scroll会自动绘制动画，包括边框
    for (size_t i = 0; i < menu.item_count - 1; ++i) {
        menu.operations->animate_scroll(&menu, CFBD_ICONTEXT_SCROLL_RIGHT);
        system_delay_ms(500); // 每个图标停留2秒
    }

    // 在最后一个图标停留
    system_delay_ms(500);

    // 向左滚动：3 -> 2 -> 1 -> 0 (返回第一个)
    for (size_t i = 0; i < menu.item_count - 1; ++i) {
        menu.operations->animate_scroll(&menu, CFBD_ICONTEXT_SCROLL_LEFT);
        system_delay_ms(500);
    }
}