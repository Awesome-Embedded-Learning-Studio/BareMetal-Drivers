#include "test_widget.h"

#include "cfbd_graphic_define.h"
#include "demo_support/demo_manager.h"

/* Demo entry declarations */
void icontext_menu_demo(CFBD_GraphicDevice* dev);
void demo_progressbar(CFBD_GraphicDevice* dev);
void test_text(CFBD_GraphicDevice* dev);
void demo_end_screen(CFBD_GraphicDevice* dev);

const CFBD_DemoDescriptor g_demos[] = {{"IconText Menu", icontext_menu_demo, 3000},
                                       {"Progress Bar", demo_progressbar, 2500},
                                       {"Text Alignment", test_text, 2000},
                                       {"Demo End", demo_end_screen, 3000}};

#define g_demo_count sizeof(g_demos) / sizeof(g_demos[0])

void test_widget(CFBD_GraphicDevice* handler)
{
    CFBD_DemoManager_Run(handler, g_demos, g_demo_count, CFBD_DEMO_RUN_LOOP, 0);
}