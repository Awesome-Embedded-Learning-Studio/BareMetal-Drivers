#include "application_maker-template.h"

#include <stddef.h>
#include <stdint.h>

#include "app.h"
#include "cfbd_define.h"
#include "config/system_settings.h"
#include "sys_boot/boot.h"
#include "system/clock/clock_initer.h"
#include "uart.h"

static MyBootArgs myBootArgs;
static CFBDBootTuple _tuple;

static uint32_t provide_tick()
{
    return HAL_GetTick();
}

static uint32_t provide_clock_freq()
{
    return HAL_RCC_GetSysClockFreq();
}

/**
 * @brief   Following Vars are specified in uart_inits.c
 *          or anything, that inits the uarts
 *
 */

extern CFBD_UARTHandle uart1_handle;
extern CFBD_ST_UARTPrivate uart1_private;
void uart1_init_callback(CFBD_ST_UARTPrivate* private_data);

static CFBD_Bool appBooter(void* args)
{
    HAL_Init();
    system_clock_init();

    CFBD_Init_STUARTBackend(&uart1_handle, -1, &uart1_private, uart1_init_callback);
    return CFBD_TRUE;
}

/**
 * @brief To make a specified boot, rewrite functions
 *
 * @return CFBD_BootStrapFunc
 */
CFBDBootTuple* CFBD_AppBootMaker()
{
    myBootArgs.should_led_init = 1;
    myBootArgs.shell_sleep = 500;

    _tuple.args = &myBootArgs;
    _tuple.boot_func = appBooter;

    return &_tuple;
}

CFBD_ClockFreqProvider CFBD_AppClockProvider(void)
{
    return provide_clock_freq;
}

CFBD_ClockTickProvider CFBD_AppTickProvider(void)
{
    return provide_tick;
}