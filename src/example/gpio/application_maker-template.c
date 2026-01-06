#include "app.h"
#include "sys_boot/boot.h"
#include "cfbd_define.h"
#include "system/clock/clock_initer.h"
#include "config/system_settings.h"
#include "application_maker-template.h"

#include <stddef.h>
#include <stdint.h>



static MyBootArgs myBootArgs;
static CFBDBootTuple _tuple;

static uint32_t provide_clock_freq() {
    return HAL_RCC_GetSysClockFreq();
}

static CFBD_Bool appBooter(void* args){
    HAL_Init();
    system_clock_init();
    return CFBD_TRUE;
}

/**
 * @brief To make a specified boot, rewrite functions
 * 
 * @return CFBD_BootStrapFunc 
 */
CFBDBootTuple* CFBD_AppBootMaker(){
    myBootArgs.should_led_init = 1;
    myBootArgs.shell_sleep = 500;

    _tuple.args = &myBootArgs;
    _tuple.boot_func = appBooter;

    return &_tuple;
}

CFBD_ClockFreqProvider CFBD_AppClockProvider(void){
    return provide_clock_freq;
}
