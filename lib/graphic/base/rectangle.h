/**
 * @file rectangle.h
 * @brief Rectangle drawing and manipulation utilities for 2D graphics.
 * @defgroup Rectangle_Module Rectangle Operations
 * @ingroup Graphics_Module
 * @{
 */

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

/**
 * @struct CFBDGraphicRect
 * @brief Represents a rectangle defined by top-left and bottom-right corner points.
 *
 * This structure defines a rectangular region using two diagonal corner points.
 * The corners do not need to be in any particular order; the rect_normalize()
 * function can be used to ensure consistent corner positioning.
 *
 * @details
 * The rectangle is defined by:
 * - @c tl (top-left): One corner of the rectangle
 * - @c br (bottom-right): The diagonal corner of the rectangle
 *
 * Key properties:
 * - Corners can be specified in any order (will be normalized if needed)
 * - Supports all standard rectangle operations (area, width, height, intersection, etc.)
 * - Can be empty (zero width or height)
 * - Coordinates are clamped to valid range by device driver
 *
 * @example
 * @code
 * // Create a rectangle from top-left (50, 50) to bottom-right (150, 100)
 * CFBDGraphicRect rect1 = {{50, 50}, {150, 100}};
 *
 * // Create a rectangle (corners will be normalized if needed)
 * CFBDGraphicRect rect2 = {{150, 100}, {50, 50}};
 * CFBDGraphicRect normalized = rect_normalize(rect2);  // {{50, 50}, {150, 100}}
 *
 * // Create a square
 * CFBDGraphicRect square = {{100, 100}, {200, 200}};
 *
 * // Using rect_from_xywh for x, y, width, height format
 * CFBDGraphicRect rect3 = rect_from_xywh(50, 50, 100, 50);
 * @endcode
 */
typedef struct
{
    CFBDGraphic_Point tl; /**< Top-left corner point of the rectangle */
    CFBDGraphic_Point br; /**< Bottom-right corner point of the rectangle */
} CFBDGraphicRect;

/**
 * @brief Renders the outline of a rectangle on the graphics device.
 *
 * This function draws the four edges of a rectangle without filling the interior.
 *
 * @param device Pointer to the graphics device handler for rendering.
 * @param rect   Pointer to the rectangle structure to draw.
 *
 * @return void
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{10, 10}, {100, 50}};
 * CFBDGraphic_DrawRect(device, &rect);
 * @endcode
 *
 * @see CFBDGraphicRect
 * @see CFBDGraphic_FillRect
 */
void CFBDGraphic_DrawRect(CFBD_GraphicDevice* device, CFBDGraphicRect* rect);

/**
 * @brief Renders a filled rectangle on the graphics device.
 *
 * This function draws a completely filled rectangle using the active color.
 * All pixels within the rectangular region are rendered.
 *
 * @param device Pointer to the graphics device handler for rendering.
 * @param rect   Pointer to the rectangle structure to fill.
 *
 * @return void
 *
 * @example
 * @code
 * CFBDGraphicRect filled_rect = {{20, 20}, {200, 100}};
 * CFBDGraphic_FillRect(device, &filled_rect);
 * @endcode
 *
 * @see CFBDGraphicRect
 * @see CFBDGraphic_DrawRect
 */
void CFBDGraphic_FillRect(CFBD_GraphicDevice* device, CFBDGraphicRect* rect);

/**
 * @defgroup RectUtil_Normalization Rectangle Normalization
 * @brief Utilities for normalizing rectangle corner points.
 * @ingroup Rectangle_Module
 * @{
 */

/**
 * @brief Ensures rectangle corners are in canonical order (top-left and bottom-right).
 *
 * Normalizes a rectangle so that the top-left corner is at minimum coordinates
 * and the bottom-right corner is at maximum coordinates, regardless of input order.
 *
 * @param r Input rectangle with corners in any order
 * @return CFBDGraphicRect Normalized rectangle with proper corner ordering
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{150, 100}, {50, 200}};
 * CFBDGraphicRect norm = rect_normalize(rect);
 * // norm = {{50, 100}, {150, 200}}
 * @endcode
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

/**
 * @defgroup RectUtil_Dimensions Rectangle Dimensions
 * @brief Functions to calculate rectangle dimensions.
 * @ingroup Rectangle_Module
 * @{
 */

/**
 * @brief Calculates the width of a rectangle.
 *
 * Returns the horizontal distance between left and right edges.
 * The rectangle is automatically normalized before calculation.
 *
 * @param r Pointer to the rectangle
 * @return uint32_t The width of the rectangle in pixels
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {200, 150}};
 * uint32_t width = rect_width(&rect);  // Returns 150
 * @endcode
 */
static inline uint32_t rect_width(const CFBDGraphicRect* r)
{
    CFBDGraphicRect n = rect_normalize(*r);
    return (uint32_t) asInt32_t(n.br.x) - (uint32_t) asInt32_t(n.tl.x);
}

/**
 * @brief Calculates the height of a rectangle.
 *
 * Returns the vertical distance between top and bottom edges.
 * The rectangle is automatically normalized before calculation.
 *
 * @param r Pointer to the rectangle
 * @return uint32_t The height of the rectangle in pixels
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {200, 150}};
 * uint32_t height = rect_height(&rect);  // Returns 100
 * @endcode
 */
static inline uint32_t rect_height(const CFBDGraphicRect* r)
{
    CFBDGraphicRect n = rect_normalize(*r);
    return (uint32_t) asInt32_t(n.br.y) - (uint32_t) asInt32_t(n.tl.y);
}

/**
 * @brief Checks if a rectangle is empty (has zero width or height).
 *
 * @param r Pointer to the rectangle to check
 * @return CFBD_Bool CFBD_TRUE if width or height is zero, CFBD_FALSE otherwise
 *
 * @example
 * @code
 * CFBDGraphicRect rect1 = {{50, 50}, {150, 150}};
 * CFBDGraphicRect rect2 = {{50, 50}, {50, 150}};
 *
 * rect_is_empty(&rect1);  // Returns CFBD_FALSE (valid rectangle)
 * rect_is_empty(&rect2);  // Returns CFBD_TRUE (zero width)
 * @endcode
 */
static inline CFBD_Bool rect_is_empty(const CFBDGraphicRect* r)
{
    return rect_width(r) == 0 || rect_height(r) == 0;
}

/** @} */ // End of RectUtil_Dimensions

/**
 * @defgroup RectUtil_Conversion Rectangle Format Conversion
 * @brief Conversion between different rectangle representations.
 * @ingroup Rectangle_Module
 * @{
 */

/**
 * @brief Creates a rectangle from x, y, width, and height values.
 *
 * Converts from the common (x, y, width, height) format to corner-based format.
 * Coordinates are automatically clamped and the result is normalized.
 *
 * @param x X-coordinate of top-left corner
 * @param y Y-coordinate of top-left corner
 * @param w Width of the rectangle
 * @param h Height of the rectangle
 * @return CFBDGraphicRect The resulting rectangle in corner format
 *
 * @example
 * @code
 * // Create a 100x50 rectangle starting at (50, 75)
 * CFBDGraphicRect rect = rect_from_xywh(50, 75, 100, 50);
 * // rect = {{50, 75}, {150, 125}}
 * @endcode
 */
static inline CFBDGraphicRect rect_from_xywh(int32_t x, int32_t y, int32_t w, int32_t h)
{
    CFBDGraphicRect r;
    r.tl.x = clamp_u16_from_i32(x);
    r.tl.y = clamp_u16_from_i32(y);
    r.br.x = clamp_u16_from_i32(x + w);
    r.br.y = clamp_u16_from_i32(y + h);
    return rect_normalize(r);
}

/**
 * @brief Converts a rectangle from corner format to x, y, width, height format.
 *
 * Extracts position and dimension information from a corner-based rectangle.
 * The rectangle is normalized before conversion to ensure consistent results.
 *
 * @param r     Pointer to the input rectangle in corner format
 * @param out_x Pointer to store the x-coordinate
 * @param out_y Pointer to store the y-coordinate
 * @param out_w Pointer to store the width
 * @param out_h Pointer to store the height
 *
 * @return void
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 75}, {150, 125}};
 * int32_t x, y, w, h;
 * rect_to_xywh(&rect, &x, &y, &w, &h);
 * // x=50, y=75, w=100, h=50
 * @endcode
 */
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

/** @} */ // End of RectUtil_Conversion

/**
 * @defgroup RectUtil_Containment Point and Rectangle Containment
 * @brief Functions to check containment and intersection relationships.
 * @ingroup Rectangle_Module
 * @{
 */

/**
 * @brief Checks if a point is contained within a rectangle.
 *
 * Returns true if the point lies within or on the boundary of the rectangle.
 *
 * @param r Pointer to the rectangle
 * @param p The point to check
 * @return CFBD_Bool CFBD_TRUE if point is inside/on rectangle, CFBD_FALSE otherwise
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphic_Point p1 = {100, 100};
 * CFBDGraphic_Point p2 = {200, 200};
 *
 * rect_contains_point(&rect, p1);  // Returns CFBD_TRUE
 * rect_contains_point(&rect, p2);  // Returns CFBD_FALSE
 * @endcode
 */
static inline CFBD_Bool rect_contains_point(const CFBDGraphicRect* r, CFBDGraphic_Point p)
{
    CFBDGraphicRect n = rect_normalize(*r);
    return (p.x >= n.tl.x && p.x <= n.br.x && p.y >= n.tl.y && p.y <= n.br.y);
}

/**
 * @brief Checks if two rectangles intersect (including edge contact).
 *
 * Returns true if the two rectangles overlap or touch at their edges.
 *
 * @param a Pointer to the first rectangle
 * @param b Pointer to the second rectangle
 * @return CFBD_Bool CFBD_TRUE if rectangles intersect, CFBD_FALSE otherwise
 *
 * @example
 * @code
 * CFBDGraphicRect rect1 = {{0, 0}, {100, 100}};
 * CFBDGraphicRect rect2 = {{50, 50}, {150, 150}};
 * CFBDGraphicRect rect3 = {{200, 200}, {300, 300}};
 *
 * rect_intersects(&rect1, &rect2);  // Returns CFBD_TRUE (overlapping)
 * rect_intersects(&rect1, &rect3);  // Returns CFBD_FALSE (no overlap)
 * @endcode
 */
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

/** @} */ // End of RectUtil_Containment

/**
 * @defgroup RectUtil_Anchor Anchor Flags
 * @brief Flags for specifying rectangle alignment anchors.
 * @ingroup Rectangle_Module
 * @{
 */

/**
 * @enum CCGraphicAnchor
 * @brief Defines anchor points for rectangle alignment operations.
 *
 * These flags specify horizontal and vertical anchor positions.
 * Use bitwise OR to combine one horizontal and one vertical flag.
 *
 * @example
 * @code
 * // Center alignment
 * uint8_t center_anchor = RECT_ANCHOR_HCENTER | RECT_ANCHOR_VCENTER;
 *
 * // Top-left alignment
 * uint8_t topleft = RECT_ANCHOR_LEFT | RECT_ANCHOR_TOP;
 *
 * // Right-center alignment
 * uint8_t rightcenter = RECT_ANCHOR_RIGHT | RECT_ANCHOR_VCENTER;
 *
 * CFBDGraphicRect result = rect_align_in_parent(&parent, &child, center_anchor);
 * @endcode
 */
typedef enum
{
    RECT_ANCHOR_LEFT = 1 << 0,    /**< Align to left edge (horizontal) */
    RECT_ANCHOR_HCENTER = 1 << 1, /**< Align to horizontal center */
    RECT_ANCHOR_RIGHT = 1 << 2,   /**< Align to right edge (horizontal) */

    RECT_ANCHOR_TOP = 1 << 3,     /**< Align to top edge (vertical) */
    RECT_ANCHOR_VCENTER = 1 << 4, /**< Align to vertical center */
    RECT_ANCHOR_BOTTOM = 1 << 5   /**< Align to bottom edge (vertical) */
} CCGraphicAnchor;

/** @} */ // End of RectUtil_Anchor

/**
 * @defgroup RectUtil_Advanced Advanced Rectangle Operations
 * @brief Complex rectangle manipulation and analysis functions.
 * @ingroup Rectangle_Module
 * @{
 */

/**
 * @brief Computes the intersection of two rectangles.
 *
 * Returns a rectangle representing the overlapping region of two input rectangles.
 * If the rectangles don't intersect, returns an empty rectangle.
 *
 * @param a Pointer to the first rectangle
 * @param b Pointer to the second rectangle
 * @return CFBDGraphicRect The intersection rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect1 = {{0, 0}, {100, 100}};
 * CFBDGraphicRect rect2 = {{50, 50}, {150, 150}};
 * CFBDGraphicRect intersection = rect_intersection(&rect1, &rect2);
 * // intersection = {{50, 50}, {100, 100}}
 * @endcode
 */
/* 交集 / 并集 */

CFBDGraphicRect rect_intersection(const CFBDGraphicRect* a, const CFBDGraphicRect* b);

/**
 * @brief Computes the smallest rectangle that contains both input rectangles.
 *
 * Returns the bounding rectangle that encloses both input rectangles.
 *
 * @param a Pointer to the first rectangle
 * @param b Pointer to the second rectangle
 * @return CFBDGraphicRect The union rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect1 = {{10, 10}, {50, 50}};
 * CFBDGraphicRect rect2 = {{100, 100}, {150, 150}};
 * CFBDGraphicRect bounding = rect_union(&rect1, &rect2);
 * // bounding = {{10, 10}, {150, 150}}
 * @endcode
 */
CFBDGraphicRect rect_union(const CFBDGraphicRect* a, const CFBDGraphicRect* b);

/**
 * @brief Translates a rectangle by a given offset.
 *
 * Moves the rectangle by adding dx to X-coordinates and dy to Y-coordinates.
 *
 * @param r  Pointer to the input rectangle
 * @param dx Horizontal offset in pixels (can be negative)
 * @param dy Vertical offset in pixels (can be negative)
 * @return CFBDGraphicRect The translated rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphicRect moved = rect_offset(&rect, 10, -20);
 * // moved = {{60, 30}, {160, 130}}
 * @endcode
 */
CFBDGraphicRect rect_offset(const CFBDGraphicRect* r, int32_t dx, int32_t dy);

/**
 * @brief Shrinks or expands a rectangle by specified amounts on each side.
 *
 * Adjusts the rectangle by moving edges inward (negative values) or outward (positive).
 *
 * @param r      Pointer to the input rectangle
 * @param left   Adjustment for left edge (positive = shrink)
 * @param top    Adjustment for top edge (positive = shrink)
 * @param right  Adjustment for right edge (positive = shrink)
 * @param bottom Adjustment for bottom edge (positive = shrink)
 * @return CFBDGraphicRect The inset rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphicRect inset = rect_inset(&rect, 10, 10, 10, 10);
 * // inset = {{60, 60}, {140, 140}}
 * @endcode
 */
CFBDGraphicRect
rect_inset(const CFBDGraphicRect* r, int32_t left, int32_t top, int32_t right, int32_t bottom);

/**
 * @brief Scales a rectangle about its center point.
 *
 * Resizes the rectangle while keeping its center point fixed.
 *
 * @param r  Pointer to the input rectangle
 * @param sx Horizontal scale factor (e.g., 1.5 for 50% larger)
 * @param sy Vertical scale factor
 * @return CFBDGraphicRect The scaled rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{100, 100}, {200, 150}};
 * CFBDGraphicRect enlarged = rect_scale_about_center(&rect, 1.5f, 1.5f);
 * @endcode
 */
CFBDGraphicRect rect_scale_about_center(const CFBDGraphicRect* r, float sx, float sy);

/**
 * @brief Aligns a child rectangle within a parent rectangle using anchor flags.
 *
 * Positions a child rectangle within a parent using the specified alignment anchors.
 *
 * @param parent Pointer to the parent rectangle
 * @param child  Pointer to the child rectangle to align
 * @param anchor Combination of CCGraphicAnchor flags for alignment
 * @return CFBDGraphicRect The aligned child rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect parent = {{0, 0}, {320, 240}};
 * CFBDGraphicRect child = {{0, 0}, {100, 50}};
 *
 * // Center child in parent
 * CFBDGraphicRect centered = rect_align_in_parent(
 *     &parent, &child,
 *     RECT_ANCHOR_HCENTER | RECT_ANCHOR_VCENTER
 * );
 * @endcode
 */
CFBDGraphicRect
rect_align_in_parent(const CFBDGraphicRect* parent, const CFBDGraphicRect* child, uint8_t anchor);

/**
 * @brief Finds the closest point on a rectangle to a given point.
 *
 * Returns the point on or within the rectangle that is nearest to the input point.
 *
 * @param r Pointer to the rectangle
 * @param p The reference point
 * @return CFBDGraphic_Point The closest point on the rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphic_Point p = {200, 100};
 * CFBDGraphic_Point closest = rect_closest_point(&rect, p);
 * // closest = {150, 100} (right edge of rectangle)
 * @endcode
 */
CFBDGraphic_Point rect_closest_point(const CFBDGraphicRect* r, CFBDGraphic_Point p);

/**
 * @brief Calculates the distance from a rectangle to a point.
 *
 * Returns the minimum Euclidean distance from the point to the rectangle.
 * If the point is inside the rectangle, distance is 0.
 *
 * @param r Pointer to the rectangle
 * @param p The reference point
 * @return uint32_t The distance in pixels
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphic_Point p = {200, 100};
 * uint32_t dist = rect_distance_to_point(&rect, p);
 * // dist = 50
 * @endcode
 */
uint32_t rect_distance_to_point(const CFBDGraphicRect* r, CFBDGraphic_Point p);

/**
 * @brief Expands a rectangle to include a given point.
 *
 * If the point is outside the rectangle, expands the rectangle to include it.
 * If the point is already inside, returns unchanged rectangle.
 *
 * @param r Pointer to the input rectangle
 * @param p The point to include
 * @return CFBDGraphicRect The expanded rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphic_Point p = {200, 200};
 * CFBDGraphicRect expanded = rect_expand_to_include_point(&rect, p);
 * // expanded = {{50, 50}, {200, 200}}
 * @endcode
 */
CFBDGraphicRect rect_expand_to_include_point(const CFBDGraphicRect* r, CFBDGraphic_Point p);

/**
 * @brief Clips a line segment to a rectangle boundary.
 *
 * Modifies the line endpoints to fit within the rectangle. Returns false if the entire line is
 * outside.
 *
 * @param r  Pointer to the clipping rectangle (will be modified to clipped area)
 * @param p0 Pointer to line start point (will be modified)
 * @param p1 Pointer to line end point (will be modified)
 * @return CFBD_Bool CFBD_TRUE if any part of line remains, CFBD_FALSE if completely outside
 *
 * @example
 * @code
 * CFBDGraphicRect clip_rect = {{50, 50}, {150, 150}};
 * CFBDGraphic_Point p0 = {0, 100};
 * CFBDGraphic_Point p1 = {200, 100};
 *
 * CFBD_Bool visible = rect_clip_line(&clip_rect, &p0, &p1);
 * if (visible) {
 *     // Draw clipped line
 * }
 * @endcode
 */
CFBD_Bool rect_clip_line(CFBDGraphicRect* r, CFBDGraphic_Point* p0, CFBDGraphic_Point* p1);

/**
 * @brief Clamps a rectangle to stay within screen boundaries.
 *
 * Adjusts the rectangle so it doesn't exceed the specified screen dimensions.
 *
 * @param r        Pointer to the input rectangle
 * @param screen_w Screen width in pixels
 * @param screen_h Screen height in pixels
 * @return CFBDGraphicRect The clamped rectangle
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{-10, -10}, {330, 250}};
 * CFBDGraphicRect clamped = rect_clamp_to_screen(&rect, 320, 240);
 * // clamped = {{0, 0}, {320, 240}}
 * @endcode
 */
CFBDGraphicRect
rect_clamp_to_screen(const CFBDGraphicRect* r, uint16_t screen_w, uint16_t screen_h);

/**
 * @brief Calculates the area of a rectangle.
 *
 * Returns the number of square pixels within the rectangle.
 *
 * @param r Pointer to the rectangle
 * @return uint32_t The area in square pixels
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * uint32_t area = rect_area(&rect);
 * // area = 100 * 100 = 10000
 * @endcode
 */
static inline uint32_t rect_area(const CFBDGraphicRect* r)
{
    if (rect_is_empty(r))
        return 0;
    return rect_width(r) * rect_height(r);
}

/**
 * @brief Gets the size of a rectangle as a CFBDGraphicSize structure.
 *
 * Returns the dimensions in width and height format.
 *
 * @param r Pointer to the rectangle
 * @return CFBDGraphicSize The size (width and height)
 *
 * @example
 * @code
 * CFBDGraphicRect rect = {{50, 50}, {150, 150}};
 * CFBDGraphicSize size = rect_size(&rect);
 * // size.width = 100, size.height = 100
 * @endcode
 */
static inline CFBDGraphicSize rect_size(const CFBDGraphicRect* r)
{
    CFBDGraphicSize sz = {0, 0};
    if (rect_is_empty(r))
        return sz;
    sz.height = rect_height(r);
    sz.width = rect_width(r);
    return sz;
}

/**
 * @brief Calculates the overlapping area of two rectangles.
 *
 * Returns the area of the intersection region. If rectangles don't overlap, returns 0.
 *
 * @param a Pointer to the first rectangle
 * @param b Pointer to the second rectangle
 * @return uint32_t The overlapping area in square pixels
 *
 * @example
 * @code
 * CFBDGraphicRect rect1 = {{0, 0}, {100, 100}};
 * CFBDGraphicRect rect2 = {{50, 50}, {150, 150}};
 * uint32_t overlap = rect_overlap_area(&rect1, &rect2);
 * // overlap = 50 * 50 = 2500
 * @endcode
 */
static inline uint32_t rect_overlap_area(const CFBDGraphicRect* a, const CFBDGraphicRect* b)
{
    CFBDGraphicRect inter = rect_intersection(a, b);
    return rect_area(&inter);
}

/** @} */ // End of RectUtil_Advanced

#ifdef __cplusplus
}
#endif

/** @} */ // End of Rectangle_Module group