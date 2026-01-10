/**
 * @page Graphics_Widget_Documentation Graphics Widget Documentation Summary
 *
 * @tableofcontents
 *
 * @section widget_intro Introduction
 *
 * This document summarizes comprehensive Doxygen documentation enhancements
 * made to the Graphics Widget subsystem, complementing the core Graphics Device
 * abstraction with reusable UI components.\n
 *\n
 * @section widget_module_hierarchy Widget Module Hierarchy\n
 *\n
 * @verbatim\n
 * Graphics\n
 * ├── Graphics_Widget (Main Widget Module)\n
 * │   ├── Graphics_Image (Bitmap widget)\n
 * │   ├── Graphics_Menu (Menu widget with subcomponents)\n
 * │   │   ├── Graphics_MenuItem (Individual menu items)\n
 * │   │   ├── Graphics_MenuIndicator (Selection indicator)\n
 * │   │   └── Graphics_MenuConfig (Compile-time configuration)\n
 * │   └── Graphics_ProgressBar (Progress visualization)\n
 * └── [existing Graphics_Device, Graphics_Animation, etc.]\n
 * @endverbatim\n
 *\n
 * @section widget_image_module Graphics_Image Module\n
 *\n
 * **Location**: lib/graphic/widget/base_support/image.h\n
 *\n
 * **Purpose**: Bitmap image rendering at arbitrary positions\n
 *\n
 * **Key Components**:\n
 * - @c CCGraphic_Image - Image structure (point, size, bitmap pointer)\n
 * - @c CFBDGraphic_InitImage() - Initialize image with data\n
 * - @c CFBDGraphic_DrawImage() - Render image to graphics device\n
 *\n
 * **Features**:\n
 * - Flexible bitmap data ownership (caller owned)\n
 * - No size restrictions (limited by device resolution)\n
 * - Respects device drawing mode (immediate/deferred)\n
 *\n
 * **Usage Pattern**:\n
 * @code\n
 * 1. Create bitmap data array\n
 * 2. Call CFBDGraphic_InitImage() with position and size\n
 * 3. Call CFBDGraphic_DrawImage() to render\n
 * 4. Update device if in deferred mode\n
 * @endcode\n
 *\n
 * @section widget_menu_module Graphics_Menu Module\n
 *\n
 * **Location**: lib/graphic/widget/menu/menu.h (with config, indicator, item support)\n
 *\n
 * **Purpose**: Interactive menu system with navigation and selection\n
 *\n
 * **Submodules**:\n
 *\n
 * @subsection menu_indicator Graphics_MenuIndicator\n
 *\n
 * **Purpose**: Visual selection indicator bar\n
 *\n
 * **Components**:\n
 * - @c CFBD_MenuIndicator - Configuration (width, x position)\n
 * - @c CFBD_InitDefaultMenuIndicator() - Initialize from config macros\n
 * - @c CFBD_DrawMenuIndicator() - Render indicator\n
 *\n
 * **Configuration**:\n
 * - Width: 0-8 pixels (0 = disabled)\n
 * - X position: Left edge in pixels\n
 *\n
 * @subsection menu_item Graphics_MenuItem\n
 *\n
 * **Purpose**: Individual menu items with labels and callbacks\n
 *\n
 * **Components**:\n
 * - @c CFBD_MenuItem - Single item structure\n
 * - @c CFBD_MenuItemGroup - Array container\n
 * - @c CFBD_MenuItemCallback - Callback function type\n
 * - @c CFBD_MenuItemCallbackPack - Callback + user data wrapper\n
 * - @c CFBD_MenuItemInit() - Initialize item\n
 *\n
 * **Features**:\n
 * - Zero-terminated string labels (caller owned)\n
 * - Optional selection callback with user data\n
 * - Configurable text rendering properties\n
 *\n
 * @subsection menu_config Graphics_MenuConfig\n
 *\n
 * **Purpose**: Compile-time menu appearance configuration\n
 *\n
 * **Configuration Macros**:\n
 * - @c CFBD_MENU_INDICATOR_WIDTH - Default indicator width (4px)\n
 * - @c CFBD_MENU_INDICATOR_POSX - Default indicator X position (0)\n
 * - @c CFBD_MENU_INDICATOR_MAX_WIDTH_PX - Maximum width (8px)\n
 * - @c CFBD_MENU_INDICATOR_MAX_HEIGHT_PX - Maximum height (16px)\n
 * - @c CFBD_MENU_INDICATOR_GAP - Gap between indicator and items (4px)\n
 * - @c CFBD_MENU_ITEM_Y_GAP - Vertical spacing between items (0px)\n
 *\n
 * @subsection menu_main Graphics_Menu (Main)\n
 *\n
 * **Key Components**:\n
 * - @c CFBD_Menu - Main menu structure\n
 * - @c CFBD_MenuOps - Operation table with 8 callbacks\n
 * - @c CFBD_InitMenu() - Initialize menu\n
 * - @c OLED_Menu_SelectNext() - Navigate down (inline)\n
 * - @c OLED_Menu_SelectPrev() - Navigate up (inline)\n
 *\n
 * **Operations**:\n
 * - bind_item_groups_contains() - Associate items array\n
 * - add_item() - Add item dynamically\n
 * - set_indicator_property() - Configure indicator\n
 * - set_animation() - Set transition animation\n
 * - immediate_draw() - Render complete menu\n
 * - select_index() - Change selection\n
 * - activate_current() - Invoke item callback\n
 * - reset_tl_points() - Reposition menu\n
 *\n
 * @section widget_progressbar_module Graphics_ProgressBar Module\n
 *\n
 * **Location**: lib/graphic/widget/progressbar/progressbar.h\n
 *\n
 * **Purpose**: Progress visualization with fill animation\n
 *\n
 * **Key Components**:\n
 * - @c CFBD_ProgressBar - Main structure\n
 * - @c CFBD_ProgressBarOps - Operation table\n
 * - @c CFBD_ProgressBar_Init() - Initialize progress bar\n
 *\n
 * **Operations**:\n
 * - immediate_draw() - Render current progress\n
 * - set_value() - Update progress value\n
 * - set_property() - Configure visual properties\n
 *\n
 * **Configuration**:\n
 * - \"border\" (uint8_t) - Border enabled/disabled\n
 * - \"padding\" (uint8_t) - Inner padding (0-8px)\n
 * - \"animation\" (CFBD_BaseAnimation*) - Transition timing\n
 *\n
 * @section widget_enhancements Documentation Enhancements\n
 *\n
 * @subsection enhancement_file_level File-Level Documentation\n
 *\n
 * Every widget header enhanced with:\n
 * - **@file** - Comprehensive file description\n
 * - **@brief** - One-line purpose\n
 * - **@details** - Detailed explanation with features/capabilities\n
 * - **@ingroup Graphics_Widget** - Hierarchy placement\n
 * - **@{/@}** - Doxygen module markers\n
 *\n
 * Example:\n
 * @code\n
 * /**\n
 *  * @file image.h\n
 *  * @brief Image widget for rendering bitmap graphics\n
 *  * @details Provides image structures and functions for drawing bitmap data\n
 *  *          at specified positions on graphics devices.\n
 *  * @ingroup Graphics_Widget Graphics_Image\n
 *  * @{\n
 *  */\n
 * @endcode\n
 *\n
 * @subsection enhancement_typedefs Type Documentation\n
 *\n
 * All typedefs and structures documented with:\n
 * - **@typedef/@struct** - Type designation\n
 * - **@brief** - Purpose summary\n
 * - **@details** - Semantics and usage patterns\n
 * - **Member documentation** - Field-level @brief comments\n
 * - **@see** - Cross-references\n
 *\n
 * @subsection enhancement_functions Function Documentation\n
 *\n
 * All functions documented with:\n
 * - **@brief** - One-line purpose\n
 * - **@details** - Behavior, side effects, constraints\n
 * - **@param[in/out]** - Parameter descriptions with constraints\n
 * - **@return** - Return value semantics\n
 * - **@note** - Important usage information\n
 * - **@example** - Practical code examples\n
 * - **@see** - Related types/functions\n
 *\n
 * @subsection enhancement_callbacks Operation Table Documentation\n
 *\n
 * Each operation callback in tables documented with:\n
 * - Purpose and behavior\n
 * - Parameter descriptions with constraints\n
 * - Return value semantics\n
 * - Usage examples\n
 * - Related operations\n
 *\n
 * @section widget_doxygen_output Expected Doxygen Output\n
 *\n
 * @subsection output_modules Modules Tab\n
 *\n
 * Complete widget hierarchy:\n
 * - Graphics_Widget (parent)\n
 *   - Graphics_Image with all typedefs and functions\n
 *   - Graphics_Menu with nested submodules\n
 *     - Graphics_MenuIndicator\n
 *     - Graphics_MenuItem\n
 *     - Graphics_MenuConfig\n
 *   - Graphics_ProgressBar with all typedefs and functions\n
 *\n
 * @subsection output_examples Examples Tab\n
 *\n
 * All @example blocks:\n
 * - Image initialization and drawing\n
 * - Animated image sequences\n
 * - Menu setup and navigation\n
 * - Menu with indicators and animations\n
 * - Progress bar creation and updates\n
 * - Complex multi-widget UIs\n
 * - Batch drawing optimization\n
 *\n
 * @subsection output_pages Pages Tab\n
 *\n
 * - Widget_UserGuide (comprehensive usage guide)\n
 * - Graphics_Widget_Examples (11+ practical examples)\n
 * - Graphics_Widget_Documentation (this page)\n
 *\n
 * @section widget_design_patterns Design Patterns\n
 *\n
 * @subsection pattern_init_render Initialize → Configure → Render\n
 *\n
 * All widgets follow this pattern:\n
 *\n
 * @code\n
 * 1. Initialize with base properties (Init function)\n
 * 2. Configure visual/behavioral properties (ops->set_property)\n
 * 3. Render (ops->immediate_draw or equivalent)\n
 * @endcode\n
 *\n
 * @subsection pattern_operation_tables Operation Tables\n
 *\n
 * Polymorphic behavior through function pointer tables enables:\n
 * - Multiple implementations per widget type\n
 * - Compile-time or runtime selection\n
 * - Future extensibility\n
 *\n
 * @subsection pattern_memory_ownership Caller Ownership\n
 *\n
 * - All structures stack/static allocated\n
 * - Caller owns: display device, bitmap data, item arrays\n
 * - Widgets reference, don't copy data\n
 * - Ensures deterministic memory usage\n
 *\n
 * @section widget_best_practices Best Practices\n
 *\n
 * @subsection bp_initialization Initialization\n
 *\n
 * - Call *_Init() for all widgets before using\n
 * - Initialize in order: dependencies first\n
 * - Keep structures valid for entire usage lifetime\n
 *\n
 * @subsection bp_drawing Drawing\n
 *\n
 * - Use deferred mode for multiple widgets\n
 * - Batch operations then single update()\n
 * - Minimize update calls in tight loops\n
 *\n
 * @subsection bp_animation Animation\n
 *\n
 * - Use 4-8 frames for UI feedback\n
 * - Use 16+ frames for smooth transitions\n
 * - Choose delays based on HAL tick resolution (25-50ms typical)\n
 *\n
 * @subsection bp_error_handling Error Checking\n
 *\n
 * - Always check return values from add_item() - may fail if full\n
 * - Always check return values from set_property() - property may not exist\n
 * - Validate coordinates before drawing (device->ops->setPixel may fail)\n
 *\n
 * @section widget_maintenance Documentation Maintenance\n
 *\n
 * When modifying widget code:\n
 *\n
 * 1. Update @file/@brief/@details comments\n
 * 2. Update member documentation if structure changes\n
 * 3. Add @example if new patterns emerge\n
 * 4. Update operation table documentation if callbacks change\n
 * 5. Regenerate Widget_UserGuide.md if major features added\n
 * 6. Update this Graphics_Widget_Documentation page\n
 *\n
 * @section widget_integration Integration with Core Graphics\n
 *\n
 * Widgets integrate seamlessly with core Graphics Device:\n
 *\n
 * @verbatim\n
 * Core Graphics Features:          Widget Support:\n
 * - Immediate/Deferred Mode   →    All widgets respect device mode\n
 * - Device abstraction        →    All widgets use CFBD_GraphicDevice\n
 * - Animation framework       →    Built-in CFBD_BaseAnimation support\n
 * - Multiple device types     →    Works with any device (OLED, LCD, etc.)\n
 * @endverbatim\n
 *\n
 * @section widget_complete_hierarchy Complete Documentation Structure\n
 *\n
 * @verbatim\n
 * Documentation Coverage:\n
 *\n
 * @page layers:\n
 *   - Graphics_UserGuide (core API, 4 drawing modes)\n
 *   - Widget_UserGuide (widget system, initialization patterns)\n
 *   - Graphics_Examples (core device examples)\n
 *   - Graphics_Widget_Examples (11+ widget examples)\n
 *   - Graphics_Documentation_Summary (core module overview)\n
 *   - Graphics_Widget_Documentation (widget module overview) ← THIS PAGE\n
 *\n
 * @defgroup layers:\n
 *   - Graphics (root)\n
 *     - Graphics_Device (core device interface)\n
 *     - Graphics_Widget (widget layer)\n
 *       - Graphics_Image\n
 *       - Graphics_Menu (with sub-modules)\n
 *       - Graphics_ProgressBar\n
 *     - Graphics_Animation (animation framework)\n
 *     - Graphics_Helpers (utility functions)\n
 *     - Graphics_Resources (resource configuration)\n
 *\n
 * @example blocks: 20+ complete, runnable code examples\n
 *\n
 * @namespace CFBD with hierarchical organization\n
 * @endverbatim\n
 *\n
 * @section widget_summary Summary\n
 *\n
 * The Graphics Widget subsystem provides professional-grade UI components\n
 * with comprehensive Doxygen documentation supporting:\n
 *\n
 * ✓ Complete API documentation with @example blocks\n
 * ✓ Module hierarchy supporting Modules tab\n
 * ✓ 20+ practical code examples for Examples tab\n
 * ✓ 3 @page documents for Pages tab\n
 * ✓ CFBD namespace organization for Namespaces tab\n
 * ✓ Hierarchical structure (Graphics → Widget → Image/Menu/ProgressBar)\n
 * ✓ Best practices and maintenance guidelines\n
 *\n
 * @see @ref Widget_UserGuide - Start here for widget usage\n
 * @see @ref Graphics_Widget_Examples - Copy-paste ready examples\n
 * @see @ref Graphics_UserGuide - Core graphics system\n
 * @see @ref CFBD_GraphicDevice - Core device interface\n
 */
