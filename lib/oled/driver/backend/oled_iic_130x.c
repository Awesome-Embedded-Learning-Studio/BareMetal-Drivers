#include "oled_iic_130x.h"

#include <stdint.h>
#include <string.h>

#include "cfbd_define.h"
#include "configs/cache_config-ssd130x.h"
#include "iic.h"
#include "oled.h"

static uint8_t OLED_GRAM[CACHED_HEIGHT][CACHED_WIDTH];

static inline CFBD_OLED_IICInitsParams* asIICInitsParams(void* internal)
{
    return internal;
}

static void send_data(CFBD_OLED_IICInitsParams* internal, uint8_t* data, uint16_t len)
{
    CFBD_I2CHandle* handle = internal->i2cHandle;
    uint8_t prefix = internal->device_specifics->data_prefix;
    CFBD_I2C_Message cmds[2] = {{
                                        .addr = internal->device_address >> 1,
                                        .flags = 0,
                                        .buf = &prefix,
                                        .len = 1,
                                },
                                {
                                        .addr = internal->device_address >> 1,
                                        .flags = 0,
                                        .buf = NULL,
                                        .len = 1,
                                }};
    for (uint16_t i = 0; i < len; i++) {
        cmds[1].buf = &data[i];
        CFBD_I2CTransfer(handle, cmds, 2, internal->accepted_time_delay);
    }
}

static void send_cmd(CFBD_OLED_IICInitsParams* internal, uint8_t cmd)
{
    CFBD_I2CHandle* handle = internal->i2cHandle;
    uint8_t prefix = internal->device_specifics->cmd_prefix;
    CFBD_I2C_Message cmds[2] = {{
                                        .addr = internal->device_address >> 1,
                                        .flags = 0,
                                        .buf = &prefix,
                                        .len = 1,
                                },
                                {
                                        .addr = internal->device_address >> 1,
                                        .flags = 0,
                                        .buf = &cmd,
                                        .len = 1,
                                }};
    CFBD_I2CTransfer(handle, cmds, 2, internal->accepted_time_delay);
}

static void
__pvt_oled_set_cursor(CFBD_OLED_IICInitsParams* handle, const uint8_t y, const uint8_t x)
{
    send_cmd(handle, 0xB0 | y);
    send_cmd(handle, 0x10 | ((x & 0xF0) >> 4));
    send_cmd(handle, 0x00 | (x & 0x0F));
}

// Impls
static int init(CFBD_OLED* oled, void* init_args)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(oled->oled_internal_handle);
    uint8_t* init_cmds = internal->device_specifics->init_session_tables();
    uint16_t init_cmds_sz = internal->device_specifics->init_session_tables_sz;
    for (int i = 0; i < init_cmds_sz; i++) {
        send_cmd(internal, init_cmds[i]);
    }

    return CFBD_TRUE;
}

static CFBD_Bool setPixel(CFBD_OLED* handle, uint16_t x, uint16_t y)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    if (x < internal->device_specifics->logic_width &&
        y < internal->device_specifics->logic_height) {
        OLED_GRAM[y / 8][x] |= 0x01 << (y % 8);
    }

    return CFBD_TRUE;
}

static CFBD_Bool clear(CFBD_OLED* handle)
{
    memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
    return CFBD_TRUE;
}

static CFBD_Bool update(CFBD_OLED* handle)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    for (uint8_t j = 0; j < CACHED_HEIGHT; j++) {
        __pvt_oled_set_cursor(handle->oled_internal_handle, j, 0);
        send_data(handle->oled_internal_handle,
                  OLED_GRAM[j],
                  internal->device_specifics->logic_width);
    }
    return CFBD_TRUE;
}

static CFBD_Bool
oled_helper_clear_area(CFBD_OLED* handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    const uint16_t POINT_X_MAX = internal->device_specifics->logic_width;
    const uint16_t POINT_Y_MAX = internal->device_specifics->logic_height;
    if (x >= POINT_X_MAX)
        return CFBD_FALSE;
    if (y >= POINT_Y_MAX)
        return CFBD_FALSE;

    if (x + width > POINT_X_MAX)
        width = POINT_X_MAX - x;
    if (y + height > POINT_Y_MAX)
        height = POINT_Y_MAX - y;

    for (uint8_t i = y; i < y + height; i++) {
        for (uint8_t j = x; j < x + width; j++) {
            OLED_GRAM[i / 8][j] &= ~(0x01 << (i % 8));
        }
    }

    return CFBD_TRUE;
}

static CFBD_Bool oled_helper_draw_area(CFBD_OLED* handle,
                                       uint16_t x,
                                       uint16_t y,
                                       uint16_t width,
                                       uint16_t height,
                                       uint8_t* sources)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    const uint16_t POINT_X_MAX = internal->device_specifics->logic_width;
    const uint16_t POINT_Y_MAX = internal->device_specifics->logic_height;
    if (x >= POINT_X_MAX)
        return CFBD_FALSE;
    if (y >= POINT_Y_MAX)
        return CFBD_FALSE;

    // clear the area before being set
    oled_helper_clear_area(handle, x, y, width, height);

    for (uint16_t j = 0; j < (height - 1) / 8 + 1; j++) {
        for (uint16_t i = 0; i < width; i++) {
            if (x + i > CACHED_WIDTH) {
                break;
            }
            if (y / 8 + j > CACHED_HEIGHT - 1) {
                return CFBD_TRUE;
            }

            OLED_GRAM[y / 8 + j][x + i] |= sources[j * width + i] << (y % 8);

            if (y / 8 + j + 1 > CACHED_HEIGHT - 1) {
                continue;
            }

            OLED_GRAM[y / 8 + j + 1][x + i] |= sources[j * width + i] >> (8 - y % 8);
        }
    }

    return CFBD_TRUE;
}

static CFBD_Bool
oled_helper_update_area(CFBD_OLED* handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    const uint16_t POINT_X_MAX = internal->device_specifics->logic_width;
    const uint16_t POINT_Y_MAX = internal->device_specifics->logic_height;
    if (x >= POINT_X_MAX)
        return CFBD_FALSE;
    if (y >= POINT_Y_MAX)
        return CFBD_FALSE;

    if (x + width > POINT_X_MAX)
        width = POINT_X_MAX - x;
    if (y + height > POINT_Y_MAX)
        height = POINT_Y_MAX - y;

    for (uint8_t i = y / 8; i < (y + height - 1) / 8 + 1; i++) {
        /*设置光标位置为相关页的指定列*/
        __pvt_oled_set_cursor(internal, i, x);
        /*连续写入Width个数据，将显存数组的数据写入到OLED硬件*/
        send_data(internal, &OLED_GRAM[i][x], width);
    }

    return CFBD_TRUE;
}

static CFBD_Bool oled_helper_reverse(CFBD_OLED* handle)
{
    for (uint16_t i = 0; i < CACHED_HEIGHT; i++) {
        for (uint16_t j = 0; j < CACHED_WIDTH; j++) {
            OLED_GRAM[i][j] ^= 0xFF;
        }
    }

    return CFBD_TRUE;
}

static CFBD_Bool
oled_helper_reversearea(CFBD_OLED* handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    const uint16_t POINT_X_MAX = internal->device_specifics->logic_width;
    const uint16_t POINT_Y_MAX = internal->device_specifics->logic_height;
    if (x > POINT_X_MAX)
        return CFBD_FALSE;
    if (y > POINT_Y_MAX)
        return CFBD_FALSE;

    if (x + width > POINT_X_MAX)
        width = POINT_X_MAX - x;
    if (y + height > POINT_Y_MAX)
        height = POINT_Y_MAX - y;

    for (uint16_t i = y; i < y + height; i++) {
        for (uint16_t j = x; j < x + width; j++) {
            OLED_GRAM[i / 8][j] ^= (0x01 << (i % 8));
        }
    }

    return CFBD_TRUE;
}

static CFBD_Bool open_oled(CFBD_OLED* handle)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    send_cmd(internal, 0x8D);
    send_cmd(internal, 0x14);
    send_cmd(internal, 0xAF);

    return CFBD_TRUE;
}

static CFBD_Bool close_oled(CFBD_OLED* handle)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    send_cmd(internal, 0x8D);
    send_cmd(internal, 0x10);
    send_cmd(internal, 0xAE);

    return CFBD_TRUE;
}

static CFBD_Bool iic_query(CFBD_OLED* oled,
                           const char* property, // property for query
                           void* args,           // if property request args
                           void* request_data)   // contains results
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(oled->oled_internal_handle);
    if (strcmp("rgb", property) == 0) {
        CFBD_Bool* support_rgb = (CFBD_Bool*) request_data;
        *support_rgb = CFBD_FALSE;
        return CFBD_TRUE;
    }

    if (strcmp("width", property) == 0) {
        uint16_t* width = (uint16_t*) request_data;
        *width = internal->device_specifics->logic_width;
        return CFBD_TRUE;
    }

    if (strcmp("height", property) == 0) {
        uint16_t* height = (uint16_t*) request_data;
        *height = internal->device_specifics->logic_height;
        return CFBD_TRUE;
    }

    return CFBD_FALSE;
}

static CFBD_Bool iic_sets(CFBD_OLED* oled, const char* property, void* args, void* request_data)
{
    // Oh no
    return CFBD_FALSE;
}

static const CFBD_OLEDOperations iic_ops = {.init = init,
                                            .setPixel = setPixel,
                                            .setArea = oled_helper_draw_area,

                                            .clear = clear,
                                            .update = update,
                                            .revert = oled_helper_reverse,

                                            .clear_area = oled_helper_clear_area,
                                            .update_area = oled_helper_update_area,
                                            .revert_area = oled_helper_reversearea,

                                            .close = close_oled,
                                            .open = open_oled,

                                            .self_consult = iic_query,
                                            .self_property_setter = iic_sets};

void CFBD_OLED_IIC130XInit(CFBD_OLED* handle, CFBD_OLED_IICInitsParams* pvt_handle)
{
    handle->oled_internal_handle = pvt_handle;
    handle->driver_type = CFBD_OLEDDriverType_IIC;
    handle->ops = &iic_ops;
}