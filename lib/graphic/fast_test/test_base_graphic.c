#include "base/arc.h"
#include "base/circle.h"
#include "base/ellipse.h"
#include "base/line.h"
#include "base/rectangle.h"
#include "base/triangle.h"

// 测试直线绘制
static void test_draw_line(CFBD_GraphicDevice* handler)
{
    // 测试1: 水平线
    CFBDGraphic_Line line1 = {.p_left = {10, 10}, .p_right = {50, 10}};
    CFBDGraphic_DrawLine(handler, &line1);

    // 测试2: 垂直线
    CFBDGraphic_Line line2 = {.p_left = {60, 5}, .p_right = {60, 25}};
    CFBDGraphic_DrawLine(handler, &line2);

    // 测试3: 斜线
    CFBDGraphic_Line line3 = {.p_left = {70, 5}, .p_right = {90, 25}};
    CFBDGraphic_DrawLine(handler, &line3);
}

// 测试矩形绘制
static void test_draw_rectangle(CFBD_GraphicDevice* handler)
{
    // 测试1: 空心矩形
    CFBDGraphicRect rect1 = {.tl = {5, 30}, .br = {35, 50}};
    CFBDGraphic_DrawRect(handler, &rect1);

    // 测试2: 实心矩形
    CFBDGraphicRect rect2 = {.tl = {40, 32}, .br = {60, 48}};
    CFBDGraphic_FillRect(handler, &rect2);
}

// 测试圆形绘制
static void test_draw_circle(CFBD_GraphicDevice* handler)
{
    // 测试1: 空心圆
    CFBDGraphicCircle circle1 = {.center = {80, 15}, .radius = 10};
    CFBDGraphic_DrawCircle(handler, &circle1);

    // 测试2: 实心圆
    CFBDGraphicCircle circle2 = {.center = {105, 15}, .radius = 8};
    CFBDGraphic_DrawFilledCircle(handler, &circle2);
}

// 测试椭圆绘制
static void test_draw_ellipse(CFBD_GraphicDevice* handler)
{
    // 测试1: 空心椭圆
    CCGraphic_Ellipse ellipse1 = {.center = {85, 40}, .X_Radius = 15, .Y_Radius = 10};
    CFBDGraphic_DrawEllipse(handler, &ellipse1);

    // 测试2: 实心椭圆
    CCGraphic_Ellipse ellipse2 = {.center = {110, 55}, .X_Radius = 10, .Y_Radius = 6};
    CFBDGraphic_DrawFilledEllipse(handler, &ellipse2);
}

// 测试三角形绘制
static void test_draw_triangle(CFBD_GraphicDevice* handler)
{
    // 测试1: 空心三角形
    CFBDGraphic_Triangle triangle1 = {.p1 = {15, 55}, .p2 = {5, 62}, .p3 = {25, 62}};
    CCGraphic_DrawTriangle(handler, &triangle1);

    // 测试2: 实心三角形
    CFBDGraphic_Triangle triangle2 = {.p1 = {45, 55}, .p2 = {35, 62}, .p3 = {55, 62}};
    CCGraphic_DrawFilledTriangle(handler, &triangle2);
}

// 测试圆弧绘制
static void test_draw_arc(CFBD_GraphicDevice* handler)
{
    // 测试1: 空心圆弧 (0-90度)
    CCGraphic_Arc arc1 = {.center = {20, 15}, .radius = 12, .start_degree = 0, .end_degree = 90};
    CFBDGraphic_DrawArc(handler, &arc1);

    // 测试2: 空心圆弧 (180-270度)
    CCGraphic_Arc arc2 = {.center = {45, 15}, .radius = 12, .start_degree = 180, .end_degree = 270};
    CFBDGraphic_DrawArc(handler, &arc2);

    // 测试3: 实心圆弧 (45-135度)
    CCGraphic_Arc arc3 = {.center = {70, 55}, .radius = 8, .start_degree = 45, .end_degree = 135};
    CFBDGraphic_DrawFilledArc(handler, &arc3);
}

// 主测试函数 - 按顺序测试所有图形
void test_draw_base_graphic(CFBD_GraphicDevice* handler)
{
    // 1. 测试直线
    test_draw_line(handler);

    // 2. 测试矩形
    test_draw_rectangle(handler);

    // 3. 测试圆形
    test_draw_circle(handler);

    // 4. 测试椭圆
    test_draw_ellipse(handler);

    // 5. 测试三角形
    test_draw_triangle(handler);

    // 6. 测试圆弧
    test_draw_arc(handler);
}
