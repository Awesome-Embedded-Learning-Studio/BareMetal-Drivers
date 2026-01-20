#pragma once
#include <stddef.h>

#include "base/point.h"
#include "base/size.h"
#include "cfbd_graphic_define.h"

typedef struct __CFBD_Widget CFBD_Widget;

typedef struct
{
    void (*paint)(CFBD_Widget* w, CFBD_GraphicDevice* graphic_device);
} CFBD_WidgetRequestOverrideOperations;

typedef struct
{
    // When NULL parent, parent of old will auto clear arrays
    void (*set_parent)(CFBD_Widget* self, CFBD_Widget* parent);
} CFBD_WidgetOperations;

/**
 * @brief Clean and LightWighted Containers for the widget sessions
 *
 */
typedef struct __CFBD_Widget
{
    CFBD_Widget* parent;
    CFBD_Widget** children; // child can be dymanic
    size_t children_container_sz;
    size_t children_cnt;

    // one must assigned the ops for able use
    CFBD_WidgetRequestOverrideOperations* override_ops;

    CFBD_WidgetOperations* ops;

    CFBDGraphic_Point tl_point;
    CFBDGraphicSize size;

    void* external_data; // ah, yeah!
} CFBD_Widget;

typedef struct
{
    CFBDGraphic_Point tl_point;
    CFBDGraphicSize size;
    CFBD_Widget** children; // child containers
    size_t children_container_sz;
    CFBD_Widget* parent; // owns parent? NULL if no :)
} CFBD_WidgetCreationPack;

void CFBD_WidgetCreateBase(CFBD_Widget* self, CFBD_WidgetCreationPack* pack);
void CFBD_WidgetAssignedChildPlace(CFBD_Widget* self,
                                   CFBD_Widget** children_array,
                                   size_t children_container_sz);
