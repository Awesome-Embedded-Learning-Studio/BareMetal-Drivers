#include <stddef.h>
#include <stdint.h>

#include "app.h"
#include "cfbd_define.h"
#include "cfbd_log.h"
#include "sys_clock/system_clock.h"

extern CFBD_Logger uart_logger;

/* ================= demo helpers ================= */

static void demo_banner(CFBD_Logger* logger)
{
    CFBD_LOGI(logger, "==========================================");
    CFBD_LOGI(logger, "   CFBD Bare-Metal Logger Demo (STM32)   ");
    CFBD_LOGI(logger, "   Build: %s %s", __DATE__, __TIME__);
    CFBD_LOGI(logger, "==========================================");
}

static void demo_progress(CFBD_Logger* logger)
{
    for (int i = 0; i <= 100; i += 10) {
        CFBD_LOG_I(logger, "BOOT", "System initializing... %d%%", i);
        system_delay_ms(120);
    }
}

static void demo_levels(CFBD_Logger* logger)
{
    CFBD_LOG_V(logger, "SYSTEM", "Verbose message: clock tree checking");
    CFBD_LOG_D(logger, "DRIVER", "GPIO initialized");
    CFBD_LOG_I(logger, "APP", "Application version: %s", "1.0.0");
    CFBD_LOG_W(logger, "MEMORY", "Heap usage high: %d%%", 85);
    CFBD_LOG_E(logger, "I2C", "Device not responding at address 0x%X", 0x48);
    CFBD_LOG_F(logger, "SYSTEM", "Critical error: watchdog timeout!");
}

static void demo_filter(CFBD_Logger* logger)
{
    CFBD_LOGI(logger, "LOGGER", "Set filter level: WARNING");
    CFBD_SetFilteredOffLevel(logger, CFBD_LOG_LEVEL_WARN);

    CFBD_LOGD(logger, "This DEBUG will NOT show");
    CFBD_LOGI(logger, "This INFO will NOT show");
    CFBD_LOGW(logger, "This WARNING WILL show");
    CFBD_LOGE(logger, "This ERROR WILL show");

    CFBD_SetFilteredOffLevel(logger, CFBD_LOG_LEVEL_VERBOSE);
    CFBD_LOGI(logger, "LOGGER", "Filter restored: VERBOSE");
}

/* ================= main ================= */

int main(void)
{
    /* Launch application framework */
    getApp(CFBD_TRUE);

    CFBD_Logger* logger = &uart_logger;
    logger->enable_colorize = CFBD_TRUE;

    system_delay_ms(800);

    /* 1. Banner */
    demo_banner(logger);

    /* 2. Boot progress animation */
    demo_progress(logger);

    /* 3. Full log level showcase */
    demo_levels(logger);

    /* 4. No-tag shortcut macro */
    CFBD_LOGI(logger, "Simple info message without tag");

    /* 5. Filter level demo */
    demo_filter(logger);

    CFBD_LOGI(logger, "SYSTEM", "Entering main loop...");

    /* 6. Heartbeat / runtime demo */
    uint32_t tick = 0;
    while (1) {
        system_delay_ms(1000);

        if ((tick % 5) == 0) {
            CFBD_LOGD(logger, "HEARTBEAT", "System alive, tick=%u", tick);
        }

        if ((tick % 7) == 0) {
            CFBD_LOGW(logger, "SENSOR", "ADC jitter detected");
        }

        if ((tick % 13) == 0) {
            CFBD_LOGE(logger, "NET", "Packet dropped");
        }

        tick++;
    }
}
