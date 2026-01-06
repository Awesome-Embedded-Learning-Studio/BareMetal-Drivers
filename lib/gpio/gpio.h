#pragma once
#include "cfbd_define.h"
#include "lib_settings.h"


#if defined(CFBD_IS_ST)
#include "../gpio/gpio-stm-impl.h"
#else
#error "No supports chips"
#endif

typedef uint32_t GPIO_PinType;
typedef void* GPIO_TypeHandle;

typedef struct
{
    /**
     * @brief Every man owns its own gpio privates :)
     *
     */
    GPIO_TypeHandle handle_internals_;
    GPIO_PinType pin_internals_;
} CFBD_GPIOHandle;

typedef void* CFBD_GPIOInitParams;

void CFBD_GPIOInit(CFBD_GPIOHandle* handle, CFBD_GPIOInitParams params);
void CFBD_GPIOSet(CFBD_GPIOHandle* handle);
void CFBD_GPIOUnset(CFBD_GPIOHandle* handle);
void CFBD_GPIOToggle(CFBD_GPIOHandle* handle);

static void inline CFBD_GPIOSetPin(CFBD_GPIOHandle* handle, CFBD_Bool status)
{
    status ? CFBD_GPIOSet(handle) : CFBD_GPIOUnset(handle);
}