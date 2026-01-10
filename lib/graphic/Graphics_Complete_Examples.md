/**
 * @page Graphics_Complete_Examples Graphics Framework - Complete Code Examples
 *
 * @tableofcontents
 *
 * Comprehensive code examples demonstrating all graphics subsystem features,
 * organized by functional area: device setup, drawing operations, text widgets,
 * and complex UI compositions.
 *
 * ---
 *
 * @section PART_1 PART 1: Graphics Device Foundation
 *
 * @subsection GEx1_BasicGraphicsDevice Basic Graphics Device Setup
 *
 * The simplest example showing graphics device initialization:
 *
 * @code
 * #include "graphic_device.h"
 * #include "oled_graphic_device.h"
 * #include "oled.h"
 *
 * void example_basic_graphics_setup(void) {
 *     // Initialize OLED display
 *     CFBD_OLED oled;
 *     CFBD_OLED_IICInitsParams iic_params = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = get_i2c_interface(),
 *     };
 *
 *     CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic_params, CFBD_TRUE);
 *     oled.ops->open(&oled);
 *
 *     // Create graphics device and bind to OLED
 *     CFBD_GraphicDevice graphics;
 *     CFBDGraphic_BindOLEDAsDevice(&graphics, &oled);
 *     graphics.ops->open(&graphics);
 *
 *     // Graphics device is now ready to use
 * }
 * @endcode
 *
 * @subsection GEx2_ImmediateDrawing Immediate Drawing Mode
 *
 * Drawing with immediate updates after each operation:
 *
 * @code
 * void example_immediate_drawing(CFBD_GraphicDevice* graphics) {
 *     // Enable immediate draw mode
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(graphics, CFBD_TRUE);
 *
 *     // Clear display (immediate update)
 *     graphics->ops->clear(graphics);
 *
 *     // Each pixel is visible immediately
 *     for (uint16_t x = 0; x < 128; x += 2) {
 *         graphics->ops->setPixel(graphics, x, 32);
 *         // Display updates automatically
 *     }
 * }
 * @endcode
 *
 * @subsection GEx3_DeferredDrawing Deferred Drawing Mode
 *
 * Batch drawing operations for better performance:
 *
 * @code
 * void example_deferred_drawing(CFBD_GraphicDevice* graphics) {
 *     // Enable deferred mode (batch operations)
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(graphics, CFBD_FALSE);
 *
 *     // Clear display (buffered)
 *     graphics->ops->clear(graphics);
 *
 *     // Draw multiple pixels without updating display
 *     for (uint16_t y = 0; y < 64; y += 4) {
 *         for (uint16_t x = 0; x < 128; x += 4) {
 *             graphics->ops->setPixel(graphics, x, y);
 *         }
 *     }
 *
 *     // Update display once with all changes
 *     graphics->ops->update(graphics);
 * }
 * @endcode
 *
 * @subsection GEx4_AreaOperations Area-Based Drawing
 *
 * Efficient rectangular area operations:
 *
 * @code
 * void example_area_operations(CFBD_GraphicDevice* graphics) {
 *     uint8_t bitmap[32] = {0};  // 16x16 pixel bitmap
 *
 *     // Draw bitmap to area
 *     graphics->ops->setArea(graphics, 56, 24, 16, 16, bitmap);
 *
 *     // Update only affected area
 *     graphics->ops->update_area(graphics, 56, 24, 16, 16);
 * }
 * @endcode
 *
 * @subsection GEx5_QueryDeviceProperties Query Device Properties
 *
 * Getting display capabilities:
 *
 * @code
 * void example_query_device(CFBD_GraphicDevice* graphics) {
 *     uint16_t width = 0, height = 0;
 *     CFBD_bool is_rgb = CFBD_FALSE;
 *
 *     // Query display dimensions
 *     graphics->ops->self_consult(graphics, "width", NULL, &width);
 *     graphics->ops->self_consult(graphics, "height", NULL, &height);
 *
 *     // Query color capability
 *     graphics->ops->self_consult(graphics, "rgb", NULL, &is_rgb);
 *
 *     printf("Display: %u x %u %s\n",
 *            width, height,
 *            is_rgb ? "RGB" : "Monochrome");
 * }
 * @endcode
 *
 * @subsection GEx6_ModeToggling Dynamic Mode Switching
 *
 * Switching between immediate and deferred modes:
 *
 * @code
 * void example_mode_switching(CFBD_GraphicDevice* graphics) {
 *     // Check current mode
 *     if (CFBDGraphic_DeviceRequestUpdateAtOnce(graphics)) {
 *         printf("Currently in immediate mode\n");
 *     } else {
 *         printf("Currently in deferred mode\n");
 *     }
 *
 *     // Switch to deferred mode
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(graphics, CFBD_FALSE);
 *
 *     // Perform batch operations
 *     for (int i = 0; i < 100; i++) {
 *         graphics->ops->setPixel(graphics, i, 32);
 *     }
 *     graphics->ops->update(graphics);
 *
 *     // Switch back to immediate
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(graphics, CFBD_TRUE);
 * }
 * @endcode
 *
 * @subsection GEx7_ClearImmediately Clear Display Utility
 *
 * Using the clear immediate convenience function:
 *
 * @code
 * void example_clear_immediate(CFBD_GraphicDevice* graphics) {
 *     // Clear entire display with single operation
 *     CFBDGraphic_DeviceClearImmediate(graphics);
 *
 *     // Equivalent to:
 *     // graphics->ops->clear(graphics);
 *     // graphics->ops->update(graphics);
 * }
 * @endcode
 *
 * ---
 *
 * @section PART_2 PART 2: Animation and Rendering
 *
 * @subsection GEx8_AnimatedFrames Animated Frame Rendering
 *
 * Smooth animation with frame-based timing:
 *
 * @code
 * void example_animated_frames(CFBD_GraphicDevice* graphics) {
 *     CFBD_BaseAnimation anim;
 *     CFBD_InitBaseAnimation(&anim);
 *     anim.anim_frames = 16;
 *     anim.anim_frame_delay_ms = 50;
 *
 *     // Disable immediate mode for batch updates
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(graphics, CFBD_FALSE);
 *
 *     for (uint8_t frame = 0; frame < anim.anim_frames; frame++) {
 *         graphics->ops->clear(graphics);
 *
 *         // Draw frame-dependent content
 *         uint16_t y = 16 + (frame * 3);  // Animate Y position
 *         graphics->ops->setPixel(graphics, 64, y);
 *
 *         graphics->ops->update(graphics);
 *
 *         if (anim.anim_frame_delay_ms > 0) {
 *             HAL_Delay(anim.anim_frame_delay_ms);
 *         }
 *     }
 * }
 * @endcode
 *
 * @subsection GEx9_CompleteAnimation Complete Animated Scene
 *
 * Full example combining multiple concepts:
 *
 * @code
 * #include "graphic_device.h"
 * #include "oled_graphic_device.h"
 * #include "oled.h"
 * #include "widget/animation/animation.h"
 * #include "widget/base_support/common/helpers.h"
 *
 * void example_complete_animation(void) {
 *     // Initialize OLED
 *     CFBD_OLED oled;
 *     CFBD_OLED_IICInitsParams iic_params = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = get_i2c_interface(),
 *     };
 *     CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic_params, CFBD_TRUE);
 *     oled.ops->open(&oled);
 *
 *     // Create graphics device
 *     CFBD_GraphicDevice graphics;
 *     CFBDGraphic_BindOLEDAsDevice(&graphics, &oled);
 *     graphics.ops->open(&graphics);
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&graphics, CFBD_FALSE);
 *
 *     // Setup animation
 *     CFBD_BaseAnimation anim;
 *     CFBD_InitBaseAnimation(&anim);
 *     anim.anim_frames = 8;
 *     anim.anim_frame_delay_ms = 50;
 *
 *     // Animate bouncing ball
 *     for (uint8_t frame = 0; frame < anim.anim_frames; frame++) {
 *         graphics.ops->clear(&graphics);
 *
 *         // Calculate position
 *         int16_t x = 64 + 30 * (frame % 4) - 45;
 *         int16_t y = 32 + (frame * 2);
 *
 *         // Clamp to display bounds
 *         x = clamp_i32(x, 0, 127);
 *         y = clamp_i32(y, 0, 63);
 *
 *         // Draw ball
 *         graphics.ops->setPixel(&graphics, x, y);
 *         graphics.ops->update(&graphics);
 *
 *         HAL_Delay(anim.anim_frame_delay_ms);
 *     }
 *
 *     // Cleanup
 *     graphics.ops->close(&graphics);
 *     oled.ops->close(&oled);
 * }
 * @endcode
 *
 * @subsection GEx10_UtilityFunctions Using Utility Functions
 *
 * Applying helper utilities in graphics code:
 *
 * @code
 * #include "widget/base_support/common/helpers.h"
 *
 * void example_utility_functions(CFBD_GraphicDevice* graphics) {
 *     int width = 128, height = 64;
 *
 *     // Clamp coordinate to valid range
 *     int user_x = get_user_input();
 *     int x = clamp_i32(user_x, 0, width - 1);
 *
 *     // Find center
 *     int center_x = MAX(0, width / 2);
 *     int center_y = MIN(64, height / 2);
 *
 *     // Draw centered point
 *     graphics->ops->setPixel(graphics, center_x, center_y);
 *     graphics->ops->update(graphics);
 * }
 * @endcode
 *
 * @subsection GEx11_ResourceConfiguration Resource Configuration
 *
 * Configuring graphics resources:
 *
 * @code
 * // In your project, before including graphics headers:
 *
 * // Option 1: Keep all defaults
 * #include "graphic/resource/config.h"
 *
 * // Option 2: Disable all, then enable selectively
 * #define _USE_NO_DEFAULT_SOURCES
 * #include "graphic/resource/config.h"
 *
 * // Option 3: Disable specific resources
 * #define DISABLE_ASCII_6X8_SOURCES
 * #include "graphic/resource/config.h"
 * // Now only 8x16 font is available
 * @endcode
 *
 * ---
 *
 * @section PART_3 PART 3: Text Widget Examples
 *
 * @subsection TEx1_BasicText Basic Text Rendering
 *
 * Simple text display on graphics device:
 *
 * @code
 * void example_basic_text(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 16};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *
 *     CFBDGraphic_SetText(&text, "Hello!");
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx2_MultipleTexts Multiple Text Elements
 *
 * Rendering multiple independent text widgets:
 *
 * @code
 * void example_multiple_texts(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text title, status;
 *
 *     // Title at top
 *     CFBDGraphic_Point title_pos = {0, 0};
 *     CFBDGraphicSize title_area = {128, 16};
 *     CFBDGraphic_InitText(&title, title_pos, title_area, ASCII_8x16);
 *     CFBDGraphic_SetText(&title, "Game Status");
 *     CFBDGraphic_DrawText(dev, &title, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Status at bottom
 *     CFBDGraphic_Point status_pos = {0, 50};
 *     CFBDGraphicSize status_area = {128, 14};
 *     CFBDGraphic_InitText(&status, status_pos, status_area, ASCII_6x8);
 *     CFBDGraphic_SetText(&status, "Running... 60FPS");
 *     CFBDGraphic_DrawText(dev, &status, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx3_MultilineText Multiline Text with Breaks
 *
 * Multiple lines with explicit line breaks:
 *
 * @code
 * void example_multiline_text(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 64};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *
 *     // First line
 *     CFBDGraphic_SetText(&text, "Line 1");
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Second line
 *     CFBDGraphic_SetText(&text, "Line 2");
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     // Third line
 *     CFBDGraphic_SetText(&text, "Line 3");
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx4_ContinuousText Continuous Text with Auto-Wrap
 *
 * Text that wraps automatically:
 *
 * @code
 * void example_continuous_text(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 64};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_6x8);
 *
 *     // Long text automatically wraps
 *     CFBDGraphic_SetText(&text, "This is a long text that will wrap");
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx5_DynamicText Dynamic Text with sprintf
 *
 * Formatted text with variables:
 *
 * @code
 * void example_dynamic_text(CFBD_GraphicDevice* dev, int score, int lives) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 55};
 *     CFBDGraphicSize area = {128, 8};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_6x8);
 *
 *     char buffer[32];
 *     sprintf(buffer, "Score:%d Lives:%d", score, lives);
 *     CFBDGraphic_SetText(&text, buffer);
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx6_FontSelection Font Selection with Config
 *
 * Conditional font usage based on compile-time configuration:
 *
 * @code
 * void example_font_selection(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 20};
 *
 * #if ENABLE_ASCII_8x16_SOURCES
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *     CFBDGraphic_SetText(&text, "Large Text");
 * #elif ENABLE_ASCII_6x8_SOURCES
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_6x8);
 *     CFBDGraphic_SetText(&text, "Small Text");
 * #else
 *     printf("No ASCII fonts available\n");
 *     return;
 * #endif
 *
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx7_TextMetrics Querying Text Metrics
 *
 * Get text size without rendering:
 *
 * @code
 * void example_text_metrics(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 64};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *
 *     // Query text size
 *     CFBDGraphicSize font_size = __fetch_font_size(ASCII_8x16);
 *     printf("Font size: %u x %u pixels\n", font_size.width, font_size.height);
 *
 *     // Query next position without drawing
 *     CFBDGraphic_SetText(&text, "Sample");
 *     CFBDGraphic_Point end_pos = CFBDGraphic_DrawText(
 *         dev, &text,
 *         CCGraphic_AsciiTextItem_RequestOldPoint
 *     );
 *     printf("Text would end at (%u, %u)\n", end_pos.x, end_pos.y);
 * }
 * @endcode
 *
 * @subsection TEx8_TextRepositioning Repositioning Text
 *
 * Dynamic repositioning of text elements:
 *
 * @code
 * void example_reposition_text(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 32};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *
 *     CFBDGraphic_SetText(&text, "Hello");
 *
 *     // Draw at original position
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *     dev->ops->update(dev);
 *     HAL_Delay(500);
 *
 *     // Move to new position
 *     CFBDGraphic_Point new_pos = {30, 25};
 *     CFBDGraphic_SetTextTLPointPoint(&text, &new_pos);
 *
 *     dev->ops->clear(dev);
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx9_CenteredText Centered Text
 *
 * Centering text horizontally:
 *
 * @code
 * void example_centered_text(CFBD_GraphicDevice* dev, const char* text_str) {
 *     CFBDGraphicSize font_size = __fetch_font_size(ASCII_8x16);
 *
 *     // Calculate text width
 *     size_t text_len = strlen(text_str);
 *     int text_width = text_len * (font_size.width + CFBDGraphic_TEXT_PADDING_WIDTH);
 *
 *     // Center horizontally (128 pixel display)
 *     int center_x = (128 - text_width) / 2;
 *     CFBDGraphic_Point pos = {center_x, 25};
 *
 *     CFBDGraphic_Text text;
 *     CFBDGraphicSize area = {text_width + 5, 16};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *     CFBDGraphic_SetText(&text, (char*)text_str);
 *     CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx10_ScrollingText Scrolling Text Effect
 *
 * Horizontal text scrolling:
 *
 * @code
 * void example_scrolling_text(CFBD_GraphicDevice* dev) {
 *     const char* text_str = "Scrolling Text Example";
 *     CFBDGraphicSize font_size = __fetch_font_size(ASCII_8x16);
 *     int text_width = strlen(text_str) * (font_size.width + 1);
 *
 *     // Scroll from right to left
 *     for (int x = 128; x > -text_width; x -= 2) {
 *         CFBDGraphic_Text text;
 *         CFBDGraphic_Point pos = {x, 25};
 *         CFBDGraphicSize area = {text_width + 5, 16};
 *         CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *         CFBDGraphic_SetText(&text, (char*)text_str);
 *
 *         dev->ops->clear(dev);
 *         CFBDGraphic_DrawText(dev, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *         dev->ops->update(dev);
 *
 *         HAL_Delay(50);
 *     }
 * }
 * @endcode
 *
 * ---
 *
 * @section PART_4 PART 4: Graphics Widget Examples (Image, Menu, Progress Bar)
 *
 * @subsection WEx1_BasicImage Basic Image Drawing
 *
 * Simple bitmap rendering:
 *
 * @code
 * void example_draw_bitmap(CFBD_GraphicDevice* dev) {
 *     uint8_t heart_bitmap[64] = {
 *         0x00, 0x00, 0x00, 0x00,
 *         0x66, 0x99, 0x99, 0x66,
 *         0xFF, 0xFF, 0xFF, 0xFF,
 *         0xFF, 0xFF, 0xFF, 0xFF,
 *         // ... (16x16 bitmap)
 *     };
 *
 *     CCGraphic_Image heart;
 *     CFBDGraphic_Point pos = {56, 24};
 *     CFBDGraphicSize size = {16, 16};
 *     CFBDGraphic_InitImage(&heart, &pos, &size, heart_bitmap);
 *
 *     dev->ops->clear(dev);
 *     CFBDGraphic_DrawImage(dev, &heart);
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection WEx2_AnimatedImage Animated Image Transitions
 *
 * Multiple images with frame-based animation:
 *
 * @code
 * void example_animated_image(CFBD_GraphicDevice* dev) {
 *     CCGraphic_Image frames[4];
 *     uint8_t frame_data[4][64] = {...};  // 4 animation frames
 *     CFBDGraphic_Point pos = {48, 24};
 *     CFBDGraphicSize size = {32, 16};
 *
 *     // Initialize all frames
 *     for (int i = 0; i < 4; i++) {
 *         CFBDGraphic_InitImage(&frames[i], &pos, &size, frame_data[i]);
 *     }
 *
 *     // Animate through frames
 *     CFBD_BaseAnimation anim = {4, 100};  // 4 frames, 100ms each
 *     for (uint8_t frame = 0; frame < anim.anim_frames; frame++) {
 *         dev->ops->clear(dev);
 *         CFBDGraphic_DrawImage(dev, &frames[frame % 4]);
 *         dev->ops->update(dev);
 *         HAL_Delay(anim.anim_frame_delay_ms);
 *     }
 * }
 * @endcode
 *
 * @subsection WEx3_SimpleMenu Basic Menu
 *
 * Minimal menu with 3 items:
 *
 * @code
 * void on_item_selected(void* ctx) {
 *     printf("Item selected!\n");
 * }
 *
 * void example_simple_menu(CFBD_GraphicDevice* dev) {
 *     // Allocate item array
 *     CFBD_MenuItem items[3];
 *     CFBD_MenuItemGroup item_group = {items, 3, 0};
 *
 *     // Initialize menu
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, dev, &item_group, 128);
 *
 *     // Add items
 *     CFBD_MenuItemCallbackPack cb = {on_item_selected, NULL};
 *     menu.operations->add_item(&menu, "Start", ASCII_8x16, &cb);
 *     menu.operations->add_item(&menu, "Options", ASCII_8x16, NULL);
 *     menu.operations->add_item(&menu, "Exit", ASCII_8x16, NULL);
 *
 *     // Draw menu
 *     menu.operations->immediate_draw(&menu);
 * }
 * @endcode
 *
 * @subsection WEx4_MenuNavigation Interactive Menu Navigation
 *
 * Simulated keyboard input and menu selection:
 *
 * @code
 * void example_menu_navigation(CFBD_GraphicDevice* dev) {
 *     // Setup menu (from previous example)
 *     CFBD_MenuItem items[5];
 *     CFBD_MenuItemGroup item_group = {items, 5, 0};
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, dev, &item_group, 128);
 *
 *     for (int i = 0; i < 5; i++) {
 *         char label[16];
 *         sprintf(label, "Item %d", i + 1);
 *         menu.operations->add_item(&menu, label, ASCII_6x8, NULL);
 *     }
 *
 *     // Simulate navigation
 *     menu.operations->immediate_draw(&menu);
 *
 *     // Down arrow - move to next item
 *     OLED_Menu_SelectNext(&menu);
 *     menu.operations->immediate_draw(&menu);
 *     HAL_Delay(300);
 *
 *     // Down arrow again
 *     OLED_Menu_SelectNext(&menu);
 *     menu.operations->immediate_draw(&menu);
 *     HAL_Delay(300);
 *
 *     // Up arrow - move to previous
 *     OLED_Menu_SelectPrev(&menu);
 *     menu.operations->immediate_draw(&menu);
 *     HAL_Delay(300);
 *
 *     // Enter - activate current item
 *     menu.operations->activate_current(&menu);
 * }
 * @endcode
 *
 * @subsection WEx5_MenuWithIndicator Menu with Custom Indicator
 *
 * Menu with styled selection indicator:
 *
 * @code
 * void example_menu_with_indicator(CFBD_GraphicDevice* dev) {
 *     CFBD_MenuItem items[4];
 *     CFBD_MenuItemGroup item_group = {items, 4, 0};
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, dev, &item_group, 120);
 *
 *     // Configure indicator
 *     CFBD_MenuIndicator indicator;
 *     CFBD_InitDefaultMenuIndicator(&indicator);
 *     indicator.width = 6;      // 6-pixel wide indicator
 *     indicator.x = 2;          // 2 pixels from left edge
 *     menu.operations->set_indicator_property(&menu, &indicator);
 *
 *     // Add items with callbacks
 *     CFBD_MenuItemCallbackPack cb1 = {on_item_selected, (void*)1};
 *     CFBD_MenuItemCallbackPack cb2 = {on_item_selected, (void*)2};
 *     menu.operations->add_item(&menu, "Play", ASCII_8x16, &cb1);
 *     menu.operations->add_item(&menu, "Settings", ASCII_8x16, &cb2);
 *     menu.operations->add_item(&menu, "About", ASCII_8x16, NULL);
 *     menu.operations->add_item(&menu, "Exit", ASCII_8x16, NULL);
 *
 *     // Draw
 *     menu.operations->immediate_draw(&menu);
 * }
 * @endcode
 *
 * @subsection WEx6_MenuWithAnimation Menu with Transitions
 *
 * Menu with animated selection changes:
 *
 * @code
 * void example_animated_menu(CFBD_GraphicDevice* dev) {
 *     CFBD_MenuItem items[3];
 *     CFBD_MenuItemGroup item_group = {items, 3, 0};
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, dev, &item_group, 128);
 *
 *     menu.operations->add_item(&menu, "One", ASCII_8x16, NULL);
 *     menu.operations->add_item(&menu, "Two", ASCII_8x16, NULL);
 *     menu.operations->add_item(&menu, "Three", ASCII_8x16, NULL);
 *
 *     // Enable animation
 *     CFBD_BaseAnimation anim = {6, 40};  // 6 frames, 40ms per frame
 *     menu.operations->set_animation(&menu, &anim);
 *
 *     // Navigate with visual feedback
 *     for (int i = 0; i < 3; i++) {
 *         OLED_Menu_SelectNext(&menu);
 *         for (int f = 0; f < anim.anim_frames; f++) {
 *             menu.operations->immediate_draw(&menu);
 *             HAL_Delay(anim.anim_frame_delay_ms);
 *         }
 *     }
 * }
 * @endcode
 *
 * @subsection WEx7_BasicProgressBar Simple Progress Bar
 *
 * Basic progress bar with fill animation:
 *
 * @code
 * void example_progress_bar(CFBD_GraphicDevice* dev) {
 *     CFBD_ProgressBar progress;
 *     CFBDGraphic_Point pos = {10, 32};
 *     CFBDGraphicSize size = {108, 8};
 *     CFBD_ProgressBar_Init(&progress, dev, &pos, &size, 0, 100);
 *
 *     // Draw at different values
 *     for (int val = 0; val <= 100; val += 10) {
 *         dev->ops->clear(dev);
 *         progress.ops->set_value(&progress, val);
 *         progress.ops->immediate_draw(&progress);
 *         dev->ops->update(dev);
 *         HAL_Delay(100);
 *     }
 * }
 * @endcode
 *
 * @subsection WEx8_ProgressBarWithBorder Progress Bar with Border
 *
 * Styled progress bar with border and padding:
 *
 * @code
 * void example_styled_progress(CFBD_GraphicDevice* dev) {
 *     CFBD_ProgressBar progress;
 *     CFBDGraphic_Point pos = {10, 32};
 *     CFBDGraphicSize size = {108, 10};
 *     CFBD_ProgressBar_Init(&progress, dev, &pos, &size, 0, 100);
 *
 *     // Enable border and padding
 *     uint8_t border = 1;
 *     progress.ops->set_property(&progress, "border", &border);
 *     uint8_t padding = 1;
 *     progress.ops->set_property(&progress, "padding", &padding);
 *
 *     // Animate from 0 to 100
 *     for (int val = 0; val <= 100; val += 5) {
 *         dev->ops->clear(dev);
 *         progress.ops->set_value(&progress, val);
 *         progress.ops->immediate_draw(&progress);
 *         dev->ops->update(dev);
 *         HAL_Delay(50);
 *     }
 * }
 * @endcode
 *
 * @subsection WEx9_ProgressWithAnimation Progress Bar with Transitions
 *
 * Progress bar with frame-based animation:
 *
 * @code
 * void example_animated_progress(CFBD_GraphicDevice* dev) {
 *     CFBD_ProgressBar progress;
 *     CFBDGraphic_Point pos = {10, 32};
 *     CFBDGraphicSize size = {108, 8};
 *     CFBD_ProgressBar_Init(&progress, dev, &pos, &size, 0, 100);
 *
 *     // Configure animation
 *     CFBD_BaseAnimation anim = {4, 50};  // 4-frame, 50ms transitions
 *     progress.ops->set_property(&progress, "animation", &anim);
 *
 *     // Smooth progress animation
 *     for (int target = 10; target <= 100; target += 10) {
 *         for (int frame = 0; frame < anim.anim_frames; frame++) {
 *             int current = (progress.value * (anim.anim_frames - frame) +
 *                           target * frame) / anim.anim_frames;
 *             dev->ops->clear(dev);
 *             progress.ops->set_value(&progress, current);
 *             progress.ops->immediate_draw(&progress);
 *             dev->ops->update(dev);
 *             HAL_Delay(anim.anim_frame_delay_ms);
 *         }
 *     }
 * }
 * @endcode
 *
 * ---
 *
 * @section PART_5 PART 5: Complex UI Compositions
 *
 * @subsection TEx11_MenuText Menu with Text Items
 *
 * Text-based menu display:
 *
 * @code
 * void example_menu_text(CFBD_GraphicDevice* dev) {
 *     CFBDGraphic_Text menu;
 *     CFBDGraphic_Point pos = {10, 10};
 *     CFBDGraphicSize area = {108, 54};
 *     CFBDGraphic_InitText(&menu, pos, area, ASCII_8x16);
 *
 *     dev->ops->clear(dev);
 *
 *     // Title
 *     CFBDGraphic_SetText(&menu, "MENU");
 *     CFBDGraphic_DrawText(dev, &menu, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Menu items
 *     const char* items[] = {"Start", "Settings", "Exit"};
 *     for (int i = 0; i < 3; i++) {
 *         char item_text[16];
 *         sprintf(item_text, "%d. %s", i + 1, items[i]);
 *         CFBDGraphic_SetText(&menu, item_text);
 *         CFBDGraphic_DrawText(dev, &menu, CCGraphic_AsciiTextItem_AppendNextLine);
 *     }
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection TEx12_CompleteUI Complete UI with Mixed Text
 *
 * Complex interface combining multiple text elements:
 *
 * @code
 * void example_complete_ui(CFBD_GraphicDevice* dev, int score, int level) {
 *     CFBDGraphic_Text title, body, footer;
 *
 *     // Title
 *     CFBDGraphic_Point title_pos = {0, 0};
 *     CFBDGraphicSize title_size = {128, 16};
 *     CFBDGraphic_InitText(&title, title_pos, title_size, ASCII_8x16);
 *     CFBDGraphic_SetText(&title, "Game Screen");
 *     CFBDGraphic_DrawText(dev, &title, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Body text
 *     CFBDGraphic_Point body_pos = {0, 20};
 *     CFBDGraphicSize body_size = {128, 36};
 *     CFBDGraphic_InitText(&body, body_pos, body_size, ASCII_6x8);
 *     CFBDGraphic_SetText(&body, "Level in progress...");
 *     CFBDGraphic_DrawText(dev, &body, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Footer with stats
 *     CFBDGraphic_Point footer_pos = {0, 55};
 *     CFBDGraphicSize footer_size = {128, 8};
 *     CFBDGraphic_InitText(&footer, footer_pos, footer_size, ASCII_6x8);
 *     char footer_text[32];
 *     sprintf(footer_text, "Score:%d Level:%d", score, level);
 *     CFBDGraphic_SetText(&footer, footer_text);
 *     CFBDGraphic_DrawText(dev, &footer, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     dev->ops->update(dev);
 * }
 * @endcode
 *
 * @subsection WEx10_MultipleWidgets Multiple Widgets Together
 *
 * Complex UI with image, menu, and progress bar:
 *
 * @code
 * // Global state
 * typedef enum {
 *     SCREEN_MENU,
 *     SCREEN_LOADING,
 *     SCREEN_GAME
 * } ScreenState;
 *
 * void example_multi_widget_ui(CFBD_GraphicDevice* dev) {
 *     ScreenState screen = SCREEN_MENU;
 *
 *     // Setup menu
 *     CFBD_MenuItem items[3];
 *     CFBD_MenuItemGroup item_group = {items, 3, 0};
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, dev, &item_group, 128);
 *     menu.operations->add_item(&menu, "Start Game", ASCII_8x16, NULL);
 *     menu.operations->add_item(&menu, "Settings", ASCII_8x16, NULL);
 *     menu.operations->add_item(&menu, "Exit", ASCII_8x16, NULL);
 *
 *     // Setup progress bar for loading screen
 *     CFBD_ProgressBar progress;
 *     CFBDGraphic_Point prog_pos = {10, 50};
 *     CFBDGraphicSize prog_sz = {108, 8};
 *     CFBD_ProgressBar_Init(&progress, dev, &prog_pos, &prog_sz, 0, 100);
 *     uint8_t border = 1;
 *     progress.ops->set_property(&progress, "border", &border);
 *
 *     // Setup image for loading screen
 *     uint8_t logo_bitmap[128] = {...};  // 32x32 logo
 *     CCGraphic_Image logo;
 *     CFBDGraphic_Point logo_pos = {48, 8};
 *     CFBDGraphicSize logo_sz = {32, 32};
 *     CFBDGraphic_InitImage(&logo, &logo_pos, &logo_sz, logo_bitmap);
 *
 *     while (1) {
 *         switch (screen) {
 *         case SCREEN_MENU:
 *             dev->ops->clear(dev);
 *             menu.operations->immediate_draw(&menu);
 *             // Handle input to change screen
 *             break;
 *
 *         case SCREEN_LOADING:
 *             CFBDGraphic_DrawImage(dev, &logo);
 *             for (int i = 0; i <= 100; i += 10) {
 *                 progress.ops->set_value(&progress, i);
 *                 progress.ops->immediate_draw(&progress);
 *                 dev->ops->update(dev);
 *                 HAL_Delay(100);
 *             }
 *             screen = SCREEN_GAME;
 *             break;
 *         }
 *     }
 * }
 * @endcode
 *
 * @subsection WEx11_WidgetBatching Batch Drawing for Performance
 *
 * Multiple widgets with deferred updates:
 *
 * @code
 * void example_batch_widgets(CFBD_GraphicDevice* dev) {
 *     // Enable deferred mode
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(dev, CFBD_FALSE);
 *
 *     // Setup widgets
 *     CCGraphic_Image bg;
 *     CFBDGraphic_InitImage(&bg, &(CFBDGraphic_Point){0, 0},
 *                          &(CFBDGraphicSize){128, 64}, background_data);
 *
 *     CFBD_MenuItem items[2];
 *     CFBD_MenuItemGroup item_group = {items, 2, 0};
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, dev, &item_group, 128);
 *     menu.operations->add_item(&menu, "Option 1", ASCII_6x8, NULL);
 *     menu.operations->add_item(&menu, "Option 2", ASCII_6x8, NULL);
 *
 *     CFBD_ProgressBar progress;
 *     CFBDGraphic_Point p = {20, 55};
 *     CFBDGraphicSize s = {88, 6};
 *     CFBD_ProgressBar_Init(&progress, dev, &p, &s, 0, 100);
 *
 *     // Draw all widgets, update only once
 *     dev->ops->clear(dev);
 *     CFBDGraphic_DrawImage(dev, &bg);
 *     menu.operations->immediate_draw(&menu);
 *     progress.ops->set_value(&progress, 75);
 *     progress.ops->immediate_draw(&progress);
 *     dev->ops->update(dev);  // Single update for all widgets
 * }
 * @endcode
 *
 * ---
 *
 * @see @ref Graphics_UserGuide - Detailed graphics framework guide
 * @see @ref Widget_UserGuide - Complete widget guide
 * @see @ref Text_UserGuide - Complete text guide
 * @see CFBD_GraphicDevice - Device structure
 * @see CFBD_Menu - Menu structure
 * @see CFBD_ProgressBar - Progress bar structure
 * @see CCGraphic_Image - Image structure
 * @see CFBDGraphic_Text - Text structure
 * @see CFBD_BaseAnimation - Animation structure
 */
