#include "demo_manager.h"

#include "device/graphic_device.h"
#include "sys_clock/system_clock.h"

/**
 * @brief Run demo list according to mode
 */
void CFBD_DemoManager_Run(CFBD_GraphicDevice* dev,
                          const CFBD_DemoDescriptor* demos,
                          uint32_t demo_count,
                          CFBD_DemoRunMode mode,
                          uint32_t index)
{
    switch (mode) {
        case CFBD_DEMO_RUN_SINGLE:
            if (index < demo_count) {
                dev->ops->clear(dev);
                dev->ops->update(dev);
                demos[index].entry(dev);
            }
            break;

        case CFBD_DEMO_RUN_QUEUE:
            for (uint32_t i = 0; i < demo_count; ++i) {
                dev->ops->clear(dev);
                dev->ops->update(dev);
                demos[i].entry(dev);
                if (demos[i].duration > 0) {
                    system_delay_ms(demos[i].duration);
                }
            }
            break;

        case CFBD_DEMO_RUN_LOOP:
            while (1) {
                for (uint32_t i = 0; i < demo_count; ++i) {
                    dev->ops->clear(dev);
                    dev->ops->update(dev);
                    demos[i].entry(dev);
                    if (demos[i].duration > 0) {
                        system_delay_ms(demos[i].duration);
                    }
                }
            }
            break;
    }
}
