#include "input_id_allocator.h"

#include <stdint.h>

#include "input_device_base.h"

static CFBDInputDeviceID_t default_gen()
{
    static uint32_t GlobalAllocateID = 0;
    return GlobalAllocateID++;
}

CFBD_DeviceIDGenerator* CFBD_CreateDeviceIDDefaultGenerator()
{
    static CFBD_DeviceIDGenerator gen;
    CFBD_CreateDeviceIDCustomGenerator(&gen, default_gen);
    return &gen;
}

void CFBD_CreateDeviceIDCustomGenerator(CFBD_DeviceIDGenerator* gen,
                                        CFBDInputDeviceID_t (*get_next)())
{
    gen->get_next = get_next;
}