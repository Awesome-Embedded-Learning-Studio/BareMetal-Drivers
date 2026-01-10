/**
 * @page Graphics_Complete_UserGuide Graphics Framework - Complete User Guide
 *
 * @tableofcontents
 *
 * ---
 *
 * @section PART_I PART I: Graphics Subsystem Overview
 *
 * @section GraphicsOverview Overview
 *
 * The Graphics subsystem provides a complete framework for rendering graphical
 * content on various display devices. It is built on a modular architecture
 * consisting of:
 *
 * - **Graphics Device Abstraction**: Unified interface for different display hardware
 * - **OLED Device Binding**: Integration with OLED displays
 * - **Animation Framework**: Smooth transitions and visual effects
 * - **Widget System**: High-level UI components (Text, Menu, ProgressBar, Image)
 * - **Resource Management**: Fonts, textures, and other rendering assets
 *
 * @section GraphicsArchitecture Architecture
 *
 * The Graphics subsystem is organized in layers:
 *
 * ```
 * Application Code
 *        |
 *        v
 * Widget Framework (text, menu, animation, image, etc.)
 *        |
 *        v
 * Graphics Device Abstraction (graphic_device.h)
 *        |
 *        +-> OLED Device Binding (oled_graphic_device.h)
 *        |
 *        v
 * Hardware Drivers (OLED, LCD, etc.)
 * ```
 *
 * @section GettingStartedGraphics Getting Started with Graphics
 *
 * @subsection Step1_Headers Include Required Headers
 *
 * ```cpp
 * #include "graphic_device.h"
 * #include "oled_graphic_device.h"
 * #include "oled.h"
 * #include "driver/backend/oled_iic.h"
 * ```
 *
 * @subsection Step2_InitializeOLED Initialize OLED Display
 *
 * ```cpp
 * CFBD_OLED oled;
 * CFBD_OLED_IICInitsParams iic_config = {
 *     .device_address = SSD1309_DRIVER_ADDRESS,
 *     .i2c_handle = &my_i2c,
 * };
 *
 * CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic_config, CFBD_TRUE);
 * oled.ops->open(&oled);
 * ```
 *
 * @subsection Step3_CreateGraphicsDevice Create Graphics Device
 *
 * ```cpp
 * CFBD_GraphicDevice graphics;
 * CFBDGraphic_BindOLEDAsDevice(&graphics, &oled);
 * graphics.ops->open(&graphics);
 * ```
 *
 * @subsection Step4_RenderContent Render Graphics Content
 *
 * ```cpp
 * graphics.ops->clear(&graphics);
 * graphics.ops->setPixel(&graphics, 64, 32);
 * graphics.ops->update(&graphics);
 * ```
 *
 * @section DrawingModes Drawing Modes
 *
 * The graphics device supports two drawing modes:
 *
 * **Immediate Mode**: Each drawing operation immediately updates the display
 * ```cpp
 * CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&graphics, CFBD_TRUE);
 * graphics.ops->setPixel(&graphics, 64, 32);  // Visible immediately
 * ```
 *
 * **Deferred Mode**: Drawing operations are buffered; display is updated explicitly
 * ```cpp
 * CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&graphics, CFBD_FALSE);
 * graphics.ops->setPixel(&graphics, 64, 32);
 * graphics.ops->setPixel(&graphics, 65, 32);
 * graphics.ops->update(&graphics);  // Both pixels visible now
 * ```
 *
 * @section AnimationUsage Using Animations
 *
 * The Animation framework provides frame-based animations:
 *
 * ```cpp
 * // Initialize animation with defaults
 * CFBD_BaseAnimation anim;
 * CFBD_InitBaseAnimation(&anim);
 * anim.anim_frames = 16;
 * anim.anim_frame_delay_ms = 30;
 *
 * // Use in animation loop
 * for (uint8_t frame = 0; frame < anim.anim_frames; frame++) {
 *     float progress = (float)frame / anim.anim_frames;
 *     // Render frame based on progress
 *     if (anim.anim_frame_delay_ms > 0) {
 *         HAL_Delay(anim.anim_frame_delay_ms);
 *     }
 * }
 * ```
 *
 * @section ResourceConfiguration Resource Configuration
 *
 * Graphics resources (fonts, etc.) are configured in resource/config.h:
 *
 * ```cpp
 * // Enable/disable font resources
 * #define ENABLE_ASCII_6x8_SOURCES 1
 * #define ENABLE_ASCII_8x16_SOURCES 1
 *
 * // Or disable all defaults and customize
 * #define _USE_NO_DEFAULT_SOURCES
 * ```
 *
 * @section HelperUtilities Utility Functions
 *
 * The helpers module provides common utilities:
 *
 * ```cpp
 * // Min/Max operations
 * int larger = MAX(100, 200);
 * int smaller = MIN(100, 200);
 *
 * // Value clamping
 * int32_t x = clamp_i32(user_input, 0, 127);
 * ```
 *
 * @section CompleteGraphicsExample Complete Graphics Setup Example
 *
 * ```cpp
 * #include "graphic_device.h"
 * #include "oled_graphic_device.h"
 * #include "oled.h"
 * #include "widget/animation/animation.h"
 *
 * void graphics_demo(void) {
 *     // Initialize OLED
 *     CFBD_OLED oled;
 *     CFBD_OLED_IICInitsParams iic_config = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = &my_i2c,
 *     };
 *     CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic_config, CFBD_TRUE);
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
 *     // Clear display
 *     graphics.ops->clear(&graphics);
 *     graphics.ops->update(&graphics);
 *
 *     // Draw animated pattern
 *     for (uint8_t frame = 0; frame < anim.anim_frames; frame++) {
 *         graphics.ops->clear(&graphics);
 *
 *         // Draw frame-dependent content
 *         for (uint16_t x = 0; x < 128; x += 2) {
 *             graphics.ops->setPixel(&graphics, x, 32 + frame);
 *         }
 *
 *         graphics.ops->update(&graphics);
 *         HAL_Delay(anim.anim_frame_delay_ms);
 *     }
 *
 *     // Cleanup
 *     graphics.ops->close(&graphics);
 *     oled.ops->close(&oled);
 * }
 * ```
 *
 * @section GraphicsPerformanceTips Performance Tips
 *
 * - **Deferred Mode**: Use for batch operations to reduce display updates
 * - **Area Updates**: Use update_area() for partial display refresh
 * - **Animation Frames**: 4-8 frames for UI feedback, 16+ for smooth transitions
 * - **Resource Management**: Disable unused resources (fonts) to save memory
 *
 * @section ErrorHandling Error Handling
 *
 * Always check return values for graphics operations:
 *
 * ```cpp
 * if (graphics.ops->setPixel(&graphics, x, y)) {
 *     // Operation succeeded
 * } else {
 *     // Operation failed (invalid coordinates, device error, etc.)
 * }
 * ```
 *
 * ---
 *
 * @section PART_II PART II: Text Widget System
 *
 * @section text_intro Introduction to Text Widget
 *
 * The Graphics Text Widget provides comprehensive text rendering capabilities
 * with support for multiple monospace fonts, multiline layout, and flexible
 * positioning. Text is rendered character-by-character using pre-compiled
 * font bitmaps.
 *
 * @section text_features Key Features
 *
 * - **Multiple Font Sizes**: 6x8 and 8x16 pixel ASCII fonts
 * - **Multiline Support**: Automatic line wrapping and manual line breaks
 * - **Flexible Layout**: Continuous flow or explicit newlines
 * - **Position Queries**: Get next character position without drawing
 * - **Compile-Time Configuration**: Enable/disable fonts via config macros
 * - **Device Agnostic**: Works with any graphics device (OLED, LCD, etc.)
 *
 * @section text_font_architecture Font System
 *
 * @subsection font_sizes Supported Font Sizes
 *
 * @verbatim
 * ASCII Fonts (Compile-Time Selectable):
 * ┌─────────────────────────────────────────────┐
 * │ ASCII_6x8:  6 pixels wide x 8 pixels tall   │
 * │             Compact, dense text layout      │
 * │             Good for: status messages       │
 * ├─────────────────────────────────────────────┤
 * │ ASCII_8x16: 8 pixels wide x 16 pixels tall  │
 * │             Standard readability            │
 * │             Good for: main menu text        │
 * └─────────────────────────────────────────────┘
 * @endverbatim
 *
 * @subsection font_enable Font Enablement
 *
 * Fonts are enabled/disabled via resource/config.h macros:
 *
 * @code
 * // In your project configuration:
 * #define ENABLE_ASCII_6x8_SOURCES    // Enable 6x8 font
 * #define ENABLE_ASCII_8x16_SOURCES   // Enable 8x16 font
 * @endcode
 *
 * @section text_quick_start Quick Start with Text
 *
 * @subsection quick_basic Basic Text Rendering
 *
 * @code
 * #include "graphic/widget/text.h"
 *
 * void display_text(CFBD_GraphicDevice* device) {
 *     // Initialize text widget
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 64};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *
 *     // Set and render text
 *     CFBDGraphic_SetText(&text, "Hello World!");
 *     CFBDGraphic_DrawText(device, &text,
 *                         CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Update display
 *     device->ops->update(device);
 * }
 * @endcode
 *
 * @subsection quick_multiline Multiline Text
 *
 * @code
 * void display_multiline(CFBD_GraphicDevice* device) {
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBDGraphicSize area = {128, 64};
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *
 *     // First line
 *     CFBDGraphic_SetText(&text, "Menu:");
 *     CFBDGraphic_DrawText(device, &text,
 *                         CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Second line
 *     CFBDGraphic_SetText(&text, "1. Start");
 *     CFBDGraphic_DrawText(device, &text,
 *                         CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     // Third line
 *     CFBDGraphic_SetText(&text, "2. Settings");
 *     CFBDGraphic_DrawText(device, &text,
 *                         CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     device->ops->update(device);
 * }
 * @endcode
 *
 * @section text_layout Layout Methods
 *
 * @subsection layout_continuous Continuous (Default)
 *
 * Characters flow continuously on same line until width limit, then wrap.
 *
 * @code
 * CCGraphic_AsciiTextItem_AppendContinously
 * @endcode
 *
 * Use for: Paragraph text, automatic wrapping, word processing
 *
 * @subsection layout_newline Explicit Newline
 *
 * Forces next text segment to start on new line.
 *
 * @code
 * CCGraphic_AsciiTextItem_AppendNextLine
 * @endcode
 *
 * Use for: Menu items, structured lists, formatted text
 *
 * @subsection layout_query Query Position
 *
 * Get position after hypothetical rendering without actual drawing.
 *
 * @code
 * CCGraphic_AsciiTextItem_RequestOldPoint
 * @endcode
 *
 * Use for: Calculate text metrics, layout planning, responsive design
 *
 * @section text_positioning Positioning and Layout
 *
 * @subsection pos_setup Initial Setup
 *
 * @code
 * CFBDGraphic_Text text;
 * CFBDGraphic_Point pos = {x_start, y_start};
 * CFBDGraphicSize area = {width, height};
 * CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 * @endcode
 *
 * @subsection pos_relocate Repositioning
 *
 * @code
 * // Move text to new position and resize area
 * CFBDGraphic_Point new_pos = {20, 30};
 * CFBDGraphicSize new_area = {88, 34};
 * CCGraphicWidget_AsciiTextItem_relocate(&text, &new_pos, new_area);
 * @endcode
 *
 * @subsection pos_resume Resume at Position
 *
 * @code
 * // Continue text at specific position
 * CFBDGraphic_Point continue_pos = {0, 16};  // Next line
 * CFBDGraphic_SetTextIndexedPoint(&text, &continue_pos);
 * @endcode
 *
 * @section text_font_selection Font Selection
 *
 * @subsection font_compile Compile-Time Selection
 *
 * @code
 * // In project config:
 * #define ENABLE_ASCII_8x16_SOURCES  // Enable 8x16 only
 * #undef ENABLE_ASCII_6x8_SOURCES    // Disable 6x8
 *
 * // In code:
 * #if ENABLE_ASCII_8x16_SOURCES
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 * #else
 *     printf("8x16 font not available\n");
 * #endif
 * @endcode
 *
 * @subsection font_runtime Runtime Query
 *
 * @code
 * // Get font dimensions
 * CFBDGraphicSize font_size = __fetch_font_size(ASCII_8x16);
 * printf("Font: %u x %u pixels\n", font_size.width, font_size.height);
 *
 * // Get character bitmap
 * uint8_t* char_bitmap = __select_from_ascii_font_size(ASCII_8x16, 'A');
 * if (char_bitmap == UNSUPPORTIVE_FONT_SOURCE) {
 *     printf("Font not available\n");
 * }
 * @endcode
 *
 * @section text_padding Spacing and Padding
 *
 * @code
 * // Character horizontal spacing (padding between chars)
 * #define CFBDGraphic_TEXT_PADDING_WIDTH (1)  // 1 pixel
 *
 * // Line vertical spacing
 * #define CFBDGraphic_TEXT_PADDING_HEIGHT (0) // 0 pixels
 * @endcode
 *
 * Configure these macros to adjust text layout density.
 *
 * @section text_best_practices Text Best Practices
 *
 * @subsection bp_font_check Always Check Font Availability
 *
 * @code
 * uint8_t* font = __select_from_ascii_font_size(ASCII_8x16, 'A');
 * if (font == UNSUPPORTIVE_FONT_SOURCE) {
 *     // Font not enabled - provide fallback
 *     return handle_missing_font();
 * }
 * @endcode
 *
 * @subsection bp_string_validity Maintain String Validity
 *
 * @code
 * // GOOD: Static string
 * CFBDGraphic_SetText(&text, "Static Text");
 * CFBDGraphic_DrawText(device, &text, ...);
 *
 * // GOOD: Stack buffer in local scope
 * char buffer[32];
 * sprintf(buffer, "Score: %d", score);
 * CFBDGraphic_SetText(&text, buffer);
 * CFBDGraphic_DrawText(device, &text, ...);
 * // buffer still valid during draw
 *
 * // BAD: Pointer to temporary
 * CFBDGraphic_SetText(&text, "Value: ");
 * // sprintf result to temporary - DON'T DO THIS
 * @endcode
 *
 * @subsection bp_deferred_mode Use Deferred Mode for Multiple Texts
 *
 * @code
 * // Multiple text elements - use deferred mode
 * CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(device, CFBD_FALSE);
 *
 * // Draw multiple text elements
 * CFBDGraphic_DrawText(device, &title_text, ...);
 * CFBDGraphic_DrawText(device, &body_text, ...);
 * CFBDGraphic_DrawText(device, &footer_text, ...);
 *
 * // Single update for all
 * device->ops->update(device);
 * @endcode
 *
 * @subsection bp_bounds_check Plan for Text Bounds
 *
 * @code
 * // Calculate required size
 * CFBDGraphicSize font_size = __fetch_font_size(ASCII_8x16);
 * int text_width = 11 * (font_size.width + CFBDGraphic_TEXT_PADDING_WIDTH);
 * int text_height = font_size.height;
 *
 * // Allocate adequate area
 * CFBDGraphicSize area = {text_width + 10, text_height + 10};
 * CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 * @endcode
 *
 * @section text_complete_example Complete Text System Example
 *
 * @code
 * #include "graphic/device/graphic_device.h"
 * #include "graphic/widget/text.h"
 *
 * void display_system_menu(CFBD_GraphicDevice* device) {
 *     // Initialize text for title
 *     CFBDGraphic_Text title;
 *     CFBDGraphic_Point title_pos = {0, 0};
 *     CFBDGraphicSize title_area = {128, 16};
 *     CFBDGraphic_InitText(&title, title_pos, title_area, ASCII_8x16);
 *
 *     // Initialize text for menu items
 *     CFBDGraphic_Text menu;
 *     CFBDGraphic_Point menu_pos = {10, 18};
 *     CFBDGraphicSize menu_area = {108, 46};
 *     CFBDGraphic_InitText(&menu, menu_pos, menu_area, ASCII_8x16);
 *
 *     // Draw title
 *     CFBDGraphic_SetText(&title, "MENU");
 *     CFBDGraphic_DrawText(device, &title, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     // Draw menu items
 *     CFBDGraphic_SetText(&menu, "Start");
 *     CFBDGraphic_DrawText(device, &menu, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     CFBDGraphic_SetText(&menu, "Settings");
 *     CFBDGraphic_DrawText(device, &menu, CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     CFBDGraphic_SetText(&menu, "About");
 *     CFBDGraphic_DrawText(device, &menu, CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     CFBDGraphic_SetText(&menu, "Exit");
 *     CFBDGraphic_DrawText(device, &menu, CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     // Update display
 *     device->ops->update(device);
 * }
 *
 * void display_score(CFBD_GraphicDevice* device, int score) {
 *     CFBDGraphic_Text score_text;
 *     CFBDGraphic_Point pos = {80, 55};
 *     CFBDGraphicSize area = {48, 8};
 *     CFBDGraphic_InitText(&score_text, pos, area, ASCII_6x8);
 *
 *     char score_buf[12];
 *     sprintf(score_buf, "Score:%d", score);
 *     CFBDGraphic_SetText(&score_text, score_buf);
 *     CFBDGraphic_DrawText(device, &score_text, CCGraphic_AsciiTextItem_AppendContinously);
 *
 *     device->ops->update(device);
 * }
 * @endcode
 *
 * ---
 *
 * @section PART_III PART III: Widget System
 *
 * @section widget_intro Introduction to Graphics Widgets
 *
 * The Graphics Widget system provides reusable, customizable UI components
 * built on top of the core Graphics Device abstraction. Widgets include:
 *
 * - **Image** - Bitmap rendering at arbitrary positions
 * - **Menu** - Hierarchical item selection with navigation
 * - **ProgressBar** - Progress visualization with animation
 * - **Text** - Text rendering (documented in previous section)
 * - **Animation** - Frame-based timing framework
 *
 * @section widget_architecture Widget Architecture
 *
 * @verbatim
 *  ┌─────────────────────────────────────────────┐
 *  │      Graphics Widgets                       │
 *  ├─────────────────────────────────────────────┤
 *  │ Image | Menu | ProgressBar | Text          │
 *  └─────────────────────────────────────────────┘
 *                      ↓
 *  ┌─────────────────────────────────────────────┐
 *  │    Graphics Device Abstraction              │
 *  │  (immediate_draw, deferred_draw, update)    │
 *  └─────────────────────────────────────────────┘
 *                      ↓
 *  ┌─────────────────────────────────────────────┐
 *  │    Display Driver (OLED, LCD, etc.)         │
 *  │              Transport Layer                 │
 *  │              (I2C, SPI, GPIO)               │
 *  └─────────────────────────────────────────────┘
 * @endverbatim
 *
 * @section widget_design Design Principles
 *
 * - **No Dynamic Allocation** - All widgets use stack-allocated structures
 * - **Reference-Based** - Widgets reference device and data structures owned by caller
 * - **Operation Tables** - Callbacks enable polymorphic behavior without inheritance
 * - **Animation Support** - All visual widgets support frame-based animation
 * - **Flexible Drawing** - Immediate or deferred mode for optimization
 *
 * @section widget_animation_framework Animation Framework
 *
 * All widgets support frame-based animation through CFBD_BaseAnimation:
 *
 * @code
 * #include "graphic/widget/animation/animation.h"
 *
 * // Animation configuration
 * CFBD_BaseAnimation anim;
 * CFBD_InitBaseAnimation(&anim);
 * anim.anim_frames = 16;        // 16 frames
 * anim.anim_frame_delay_ms = 25; // 25ms per frame = ~400ms total
 *
 * // Typical frame counts:
 * // UI feedback: 4-8 frames
 * // Smooth transitions: 8-16 frames
 * // Detailed animations: 16-32 frames
 * @endcode
 *
 * @section widget_image_guide Image Widget
 *
 * @subsection image_overview Overview
 *
 * The Image widget renders bitmap data at specified positions. Images must be
 * pre-allocated as arrays, and the widget references this data.
 *
 * @subsection image_init Initialization
 *
 * @code
 * #include "graphic/widget/base_support/image.h"
 *
 * // Create bitmap data (32x32 pixels, 1 byte per pixel)
 * uint8_t logo_bitmap[128] = {
 *     0xFF, 0xFF, ...,  // Row 0
 *     0x00, 0xFF, ...,  // Row 1
 *     // ...
 * };
 *
 * // Initialize image
 * CCGraphic_Image logo;
 * CFBDGraphic_Point pos = {48, 16};      // X=48, Y=16
 * CFBDGraphicSize sz = {32, 32};        // 32x32 pixels
 * CFBDGraphic_InitImage(&logo, &pos, &sz, logo_bitmap);
 * @endcode
 *
 * @subsection image_drawing Drawing
 *
 * @code
 * // Draw image on graphics device
 * CFBDGraphic_DrawImage(graphics_device, &logo);
 *
 * // If in deferred mode, manually update
 * graphics_device->ops->update(graphics_device);
 * @endcode
 *
 * @section widget_menu_guide Menu Widget
 *
 * @subsection menu_overview Overview
 *
 * The Menu widget provides interactive list selection with:
 * - Keyboard navigation (next/previous item)
 * - Visual indicator bar showing current selection
 * - Callback support for item activation
 * - Animated transitions
 * - Configurable dimensions and fonts
 *
 * @subsection menu_init Initialization
 *
 * @code
 * #include "graphic/widget/menu/menu.h"
 * #include "graphic/widget/menu/menu_indicator.h"
 * #include "graphic/widget/animation/animation.h"
 *
 * // Pre-allocate menu items array
 * #define MAX_MENU_ITEMS 8
 * CFBD_MenuItem item_array[MAX_MENU_ITEMS];
 * CFBD_MenuItemGroup items = {
 *     .pItems = item_array,
 *     .capacity = MAX_MENU_ITEMS,
 *     .count = 0
 * };
 *
 * // Initialize menu
 * CFBD_Menu menu;
 * CFBDGraphic_Point menu_pos = {0, 0};
 * CFBD_InitMenu(&menu, graphics_device, &items, 128);
 * menu.tl_point = menu_pos;
 * @endcode
 *
 * @subsection menu_add_items Adding Items
 *
 * @code
 * // Callback when item selected
 * void on_start_game(void* ctx) {
 *     int* state = (int*)ctx;
 *     *state = STATE_RUNNING;
 * }
 *
 * // Configure indicator
 * CFBD_MenuIndicator indicator;
 * CFBD_InitDefaultMenuIndicator(&indicator);
 * indicator.width = 4;
 * menu.operations->set_indicator_property(&menu, &indicator);
 *
 * // Add menu items
 * CFBD_MenuItemCallbackPack cb = {on_start_game, &app_state};
 * menu.operations->add_item(&menu, "Start", ASCII_8x16, &cb);
 * menu.operations->add_item(&menu, "Settings", ASCII_8x16, NULL);
 * menu.operations->add_item(&menu, "About", ASCII_8x16, NULL);
 * @endcode
 *
 * @subsection menu_render Rendering
 *
 * @code
 * // Draw menu on graphics device
 * menu.operations->immediate_draw(&menu);
 * @endcode
 *
 * @subsection menu_nav Navigation
 *
 * @code
 * // Handle keyboard events
 * void on_up_button(CFBD_Menu* menu) {
 *     OLED_Menu_SelectPrev(menu);
 *     menu->operations->immediate_draw(menu);
 * }
 *
 * void on_down_button(CFBD_Menu* menu) {
 *     OLED_Menu_SelectNext(menu);
 *     menu->operations->immediate_draw(menu);
 * }
 *
 * void on_select_button(CFBD_Menu* menu) {
 *     menu->operations->activate_current(menu);
 * }
 * @endcode
 *
 * @subsection menu_animation Menu Animation
 *
 * @code
 * // Enable smooth transitions
 * CFBD_BaseAnimation anim = {
 *     .anim_frames = 8,
 *     .anim_frame_delay_ms = 30
 * };
 * menu.operations->set_animation(&menu, &anim);
 * @endcode
 *
 * @section widget_progressbar_guide ProgressBar Widget
 *
 * @subsection progressbar_overview Overview
 *
 * The ProgressBar widget visualizes numeric progress with:
 * - Configurable min/max value range
 * - Optional border and padding
 * - Animated fill transitions
 * - Custom properties via property map
 *
 * @subsection progressbar_init Initialization
 *
 * @code
 * #include "graphic/widget/progressbar/progressbar.h"
 *
 * // Create progress bar
 * CFBD_ProgressBar progress;
 * CFBDGraphic_Point pos = {10, 50};
 * CFBDGraphicSize sz = {108, 8};
 * CFBD_ProgressBar_Init(&progress, graphics_device, &pos, &sz, 0, 100);
 * @endcode
 *
 * @subsection progressbar_config Configuration
 *
 * @code
 * // Enable border
 * uint8_t border = 1;
 * progress.ops->set_property(&progress, "border", &border);
 *
 * // Set padding (space between border and fill)
 * uint8_t padding = 2;
 * progress.ops->set_property(&progress, "padding", &padding);
 *
 * // Configure animation
 * CFBD_BaseAnimation anim = {4, 50};  // 4 frames, 50ms each
 * progress.ops->set_property(&progress, "animation", &anim);
 * @endcode
 *
 * @subsection progressbar_update Progress Updates
 *
 * @code
 * // Update progress (0-100)
 * for (int value = 0; value <= 100; value += 5) {
 *     progress.ops->set_value(&progress, value);
 *     progress.ops->immediate_draw(&progress);
 *     HAL_Delay(100);
 * }
 * @endcode
 *
 * @section widget_best_practices Widget Best Practices
 *
 * @subsection bp_memory Memory Management
 *
 * - Pre-allocate all widget structures statically or on stack
 * - Pass pointers to graphics device and supporting data
 * - Ensure pointers remain valid for widget lifetime
 * - Use static arrays for menu items and bitmap data
 *
 * @subsection bp_performance Performance
 *
 * - Use deferred mode (request_updates = false) for batch widget updates
 * - Call device->ops->update() once after drawing multiple widgets
 * - Minimize animation frame delay for UI responsiveness (25-50ms recommended)
 * - Disable animations for constrained environments
 *
 * @subsection bp_drawing Drawing Order
 *
 * @code
 * // Recommended drawing order:
 * graphics->ops->clear(graphics);
 * CFBDGraphic_DrawImage(graphics, &bg_image);      // Background
 * menu.operations->immediate_draw(&menu);           // UI elements
 * progress.ops->immediate_draw(&progress);          // Foreground elements
 * graphics->ops->update(graphics);                  // Commit all changes
 * @endcode
 *
 * @subsection bp_error_handling Error Handling
 *
 * @code
 * // Always check return values
 * if (!graphics->ops->setPixel(graphics, x, y)) {
 *     // Handle coordinate out of bounds
 * }
 *
 * if (!menu.operations->add_item(&menu, "Item", size, &cb)) {
 *     // Handle menu full (no more item slots)
 * }
 *
 * if (!progress.ops->set_property(&progress, "unknown", NULL)) {
 *     // Handle unknown property
 * }
 * @endcode
 *
 * @section widget_complete_example Complete Widget Integration
 *
 * @code
 * #include "graphic/device/graphic_device.h"
 * #include "graphic/device/oled/oled_graphic_device.h"
 * #include "graphic/widget/base_support/image.h"
 * #include "graphic/widget/menu/menu.h"
 * #include "graphic/widget/progressbar/progressbar.h"
 *
 * // Global state
 * int app_state = STATE_MENU;
 * int selected_game = 0;
 * int progress_percent = 0;
 *
 * // Callback for menu selection
 * void on_game_selected(void* ctx) {
 *     int* state = (int*)ctx;
 *     *state = STATE_LOADING;
 * }
 *
 * void main(void) {
 *     // Initialize OLED
 *     CFBD_OLED oled;
 *     CFBD_OLED_IICInitsParams iic = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = i2c_interface,
 *     };
 *     CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic, CFBD_TRUE);
 *     oled.ops->open(&oled);
 *
 *     // Bind graphics device
 *     CFBD_GraphicDevice graphics;
 *     CFBDGraphic_BindOLEDAsDevice(&graphics, &oled);
 *     graphics.ops->open(&graphics);
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&graphics, CFBD_FALSE);
 *
 *     // Setup menu
 *     CFBD_MenuItem items[4];
 *     CFBD_MenuItemGroup item_group = {items, 4, 0};
 *     CFBD_Menu menu;
 *     CFBD_InitMenu(&menu, &graphics, &item_group, 128);
 *
 *     CFBD_MenuItemCallbackPack cb = {on_game_selected, &app_state};
 *     menu.operations->add_item(&menu, "Tetris", ASCII_8x16, &cb);
 *     menu.operations->add_item(&menu, "Breakout", ASCII_8x16, &cb);
 *     menu.operations->add_item(&menu, "Settings", ASCII_8x16, NULL);
 *
 *     // Setup progress bar for loading
 *     CFBD_ProgressBar progress;
 *     CFBDGraphic_Point prog_pos = {10, 50};
 *     CFBDGraphicSize prog_sz = {108, 8};
 *     CFBD_ProgressBar_Init(&progress, &graphics, &prog_pos, &prog_sz, 0, 100);
 *
 *     uint8_t border = 1, padding = 1;
 *     progress.ops->set_property(&progress, "border", &border);
 *     progress.ops->set_property(&progress, "padding", &padding);
 *
 *     // Main loop
 *     while (1) {
 *         switch (app_state) {
 *         case STATE_MENU:
 *             menu.operations->immediate_draw(&menu);
 *             handle_menu_input(&menu);
 *             break;
 *
 *         case STATE_LOADING:
 *             graphics.ops->clear(&graphics);
 *             CFBDGraphic_DrawImage(&graphics, &loading_image);
 *             for (int i = 0; i <= 100; i += 10) {
 *                 progress.ops->set_value(&progress, i);
 *                 progress.ops->immediate_draw(&progress);
 *                 graphics.ops->update(&graphics);
 *                 HAL_Delay(100);
 *             }
 *             app_state = STATE_GAME;
 *             break;
 *         }
 *     }
 * }
 * @endcode
 *
 * ---
 *
 * @section References Complete Documentation References
 *
 * - @ref Graphics_Device "Graphics Device Abstraction"
 * - @ref Graphics_Animation "Animation Framework"
 * - @ref Graphics_Helpers "Utility Helpers"
 * - @ref Graphics_Resources "Resource Configuration"
 * - @ref Graphics_Examples "Code Examples"
 * - @ref Graphics_Widget_Examples "Widget Examples"
 * - @ref Graphics_Complete_Examples "Complete Examples"
 */
