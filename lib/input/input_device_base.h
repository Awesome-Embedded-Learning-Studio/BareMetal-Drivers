#pragma once
#include "cfbd_define.h"

typedef struct __CFBDInputEventContext CFBDInputEventContext;
typedef struct __CFBDInputDeviceBase CFBDInputDeviceBase;
typedef struct __CFBDInputEvent CFBDInputEvent;
typedef struct __CFBD_DeviceIDGenerator CFBD_DeviceIDGenerator;
typedef uint32_t CFBDInputDeviceID_t;

typedef struct __CFBDInputDeviceBaseOps
{
    CFBD_Bool (*bind_context)(CFBDInputDeviceBase* device, CFBDInputEventContext* new_context);
    CFBD_Bool (*unbind_context)(CFBDInputDeviceBase* device);
    CFBD_Bool (*report_once)(CFBDInputDeviceBase* device, CFBDInputEvent* event);
} CFBDInputDeviceBaseOperation;

typedef struct __CFBDInputDeviceBase
{
    CFBDInputDeviceID_t device_id;
    CFBDInputEventContext* context;
    CFBDInputDeviceBaseOperation* ops;
    void* device_internals; // private datas
} CFBDInputDeviceBase;      // Each devices should register this as base class

void setDeviceIDGenerator(CFBD_DeviceIDGenerator* id); // use if wanna specified

void CFBD_CreateInputDeviceBase(CFBDInputDeviceBase* base,
                                CFBDInputEventContext* context,
                                void* assigned_privates);
