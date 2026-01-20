/**
 * @file widget_wrappers.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Old days designs adapters
 * @version 0.1
 * @date 2026-01-19
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "cfbd_define.h"
#include "widget.h"

#define PREANNOUNCE_TYPE(Type) typedef struct __##Type Type

PREANNOUNCE_TYPE(CFBD_GraphicArc);
PREANNOUNCE_TYPE(CFBDGraphicCircle);

void CFBD_WrapArc2Widget(CFBD_Widget* blank_widget, CFBD_GraphicArc* arc, CFBD_Bool isFilled);
void CFBD_WrapCircle2Widget(CFBD_Widget* blank_widget,
                            CFBDGraphicCircle* circle,
                            CFBD_Bool isFilled);

#undef PREANNOUNCE_TYPE
