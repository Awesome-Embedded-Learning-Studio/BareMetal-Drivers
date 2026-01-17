#include <stdint.h>
#include <string.h>

#include "cfbd_define.h"
#include "configs/cache_config-ssd132x.h"
#include "configs/external_impl_driver.h"
#include "driver/device/oled_ssd132x_privates.h"
#include "oled.h"

static uint8_t OLED_GRAM[CACHED_HEIGHT][CACHED_WIDTH];

extern I2C_HandleTypeDef hi2c1;

static inline CFBD_OLED_IICInitsParams* asIICInitsParams(void* internal)
{
    return (CFBD_OLED_IICInitsParams*) internal;
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
                                        .buf = data,
                                        .len = len,
                                }};
    CFBD_I2CTransfer(handle, cmds, 2, internal->accepted_time_delay);
}

static void set_window(CFBD_OLED_IICInitsParams* internal,
                       uint8_t col_start,
                       uint8_t col_end,
                       uint8_t row_start,
                       uint8_t row_end)
{
    // 设置列地址范围
    send_cmd(internal, 0x15);
    send_cmd(internal, col_start);
    send_cmd(internal, col_end);

    // 设置行地址范围
    send_cmd(internal, 0x75);
    send_cmd(internal, row_start);
    send_cmd(internal, row_end);
}

static uint8_t get_grey_scale(CFBD_OLED* oled)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(oled->oled_internal_handle);
    SSD132XPrivateDatas* privates = internal->device_specifics->private_data;
    return privates->grey_scale;
}

static int init(CFBD_OLED* oled, void* args)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(oled->oled_internal_handle);
    uint8_t* init_cmds = internal->device_specifics->init_session_tables();
    uint16_t init_cmds_sz = internal->device_specifics->init_session_tables_sz;

    // 发送初始化命令序列
    for (int i = 0; i < init_cmds_sz; i++) {
        send_cmd(internal, init_cmds[i]);
    }

    return CFBD_TRUE;
}

/**
 * @brief 设置单个像素
 * @param handle OLED句柄
 * @param x X坐标（0-127）
 * @param y Y坐标（0-95）
 */
static CFBD_Bool setPixel(CFBD_OLED* handle, uint16_t x, uint16_t y)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);

    if (x >= internal->device_specifics->logic_width ||
        y >= internal->device_specifics->logic_height) {
        return CFBD_FALSE;
    }

    // 计算缓冲区位置
    uint8_t col = x / 2;       // 列地址（每列存2个像素）
    uint8_t pixel_pos = x % 2; // 像素位置（0=高4位，1=低4位）

    uint8_t color = get_grey_scale(handle); // 限制为4位

    if (pixel_pos == 0) {
        // 高4位
        OLED_GRAM[y][col] = (OLED_GRAM[y][col] & 0x0F) | (color << 4);
    }
    else {
        // 低4位
        OLED_GRAM[y][col] = (OLED_GRAM[y][col] & 0xF0) | color;
    }

    return CFBD_TRUE;
}

/**
 * @brief 清空显示缓冲区
 */
static CFBD_Bool clear(CFBD_OLED* handle)
{
    memset(OLED_GRAM, 0x00, sizeof(OLED_GRAM));
    return CFBD_TRUE;
}

/**
 * @brief 更新整个显示屏
 */
static CFBD_Bool update(CFBD_OLED* handle)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);

    // 设置全屏窗口
    set_window(internal, 0, CACHED_WIDTH - 1, 0, CACHED_HEIGHT - 1);

    // 发送整个缓冲区数据
    for (uint8_t row = 0; row < CACHED_HEIGHT; row++) {
        send_data(internal, OLED_GRAM[row], CACHED_WIDTH);
    }

    return CFBD_TRUE;
}

/**
 * @brief 清空指定区域
 */
static CFBD_Bool
oled_helper_clear_area(CFBD_OLED* handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);

    if (x >= internal->device_specifics->logic_width ||
        y >= internal->device_specifics->logic_height)
        return CFBD_FALSE;

    if (x + width > internal->device_specifics->logic_width)
        width = internal->device_specifics->logic_width - x;
    if (y + height > internal->device_specifics->logic_height)
        height = internal->device_specifics->logic_height - y;

    for (uint16_t j = 0; j < height; j++) {
        for (uint16_t i = 0; i < width; i++) {
            uint16_t dst_col = (x + i) / 2;
            uint8_t dst_pixel = (x + i) % 2;

            if (dst_pixel == 0) {
                OLED_GRAM[y + j][dst_col] &= 0x0F;
            }
            else {
                OLED_GRAM[y + j][dst_col] &= 0xF0;
            }
        }
    }

    return CFBD_TRUE;
}

/**
 * @brief 绘制区域（从源数据）
 */
static CFBD_Bool oled_helper_draw_area(CFBD_OLED* handle,
                                       uint16_t x,
                                       uint16_t y,
                                       uint16_t width,
                                       uint16_t height,
                                       uint8_t* sources)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);

    if (x >= internal->device_specifics->logic_width ||
        y >= internal->device_specifics->logic_height)
        return CFBD_FALSE;

    // 边界检查
    uint16_t draw_w = (x + width > internal->device_specifics->logic_width)
                              ? (internal->device_specifics->logic_width - x)
                              : width;
    uint16_t draw_h = (y + height > internal->device_specifics->logic_height)
                              ? (internal->device_specifics->logic_height - y)
                              : height;

    for (uint16_t i = 0; i < draw_w; i++) {     // 遍历列
        for (uint16_t j = 0; j < draw_h; j++) { // 遍历行
            uint16_t src_byte_idx = (j / 8) * width + i;
            uint8_t src_bit_idx = j % 8; // LSB-top: 位 0 在最上方

            uint8_t bit = (sources[src_byte_idx] >> src_bit_idx) & 0x01;
            uint8_t pixel_value = (bit == 0) ? 0x00 : get_grey_scale(handle);

            // --- 写入 SSD1327 的 4bpp GRAM ---
            uint16_t curr_x = x + i;
            uint16_t curr_y = y + j;
            uint16_t dst_col = curr_x / 2;

            if (curr_x % 2 == 0) {
                // 偶数列写高 4 位 (D7-D4)
                OLED_GRAM[curr_y][dst_col] =
                        (OLED_GRAM[curr_y][dst_col] & 0x0F) | (pixel_value << 4);
            }
            else {
                // 奇数列写低 4 位 (D3-D0)
                OLED_GRAM[curr_y][dst_col] = (OLED_GRAM[curr_y][dst_col] & 0xF0) | pixel_value;
            }
        }
    }

    return CFBD_TRUE;
}

/**
 * @brief 更新指定区域到显示屏
 */
static CFBD_Bool
oled_helper_update_area(CFBD_OLED* handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);

    // 边界检查与修正
    if (x >= internal->device_specifics->logic_width)
        return CFBD_FALSE;
    if (y >= internal->device_specifics->logic_height)
        return CFBD_FALSE;

    if (x + width > internal->device_specifics->logic_width)
        width = internal->device_specifics->logic_width - x;
    if (y + height > internal->device_specifics->logic_height)
        height = internal->device_specifics->logic_height - y;

    // 计算列范围
    uint8_t col_start = x / 2;
    uint8_t col_end = (x + width - 1) / 2;

    // 设置窗口并发送数据
    set_window(internal, col_start, col_end, y, y + height - 1);

    for (uint16_t row = y; row < y + height; row++) {
        send_data(internal, &OLED_GRAM[row][col_start], col_end - col_start + 1);
    }

    return CFBD_TRUE;
}

static CFBD_Bool oled_helper_reverse(CFBD_OLED* handle)
{
    for (uint16_t i = 0; i < CACHED_HEIGHT; i++) {
        for (uint16_t j = 0; j < CACHED_WIDTH; j++) {
            uint8_t high = (~OLED_GRAM[i][j]) & 0xF0;
            uint8_t low = (~OLED_GRAM[i][j]) & 0x0F;
            OLED_GRAM[i][j] = high | low;
        }
    }

    return CFBD_TRUE;
}

/**
 * @brief 反转指定区域
 */
static CFBD_Bool
oled_helper_reversearea(CFBD_OLED* handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);

    if (x >= internal->device_specifics->logic_width)
        return CFBD_FALSE;
    if (y >= internal->device_specifics->logic_height)
        return CFBD_FALSE;

    if (x + width > internal->device_specifics->logic_width)
        width = internal->device_specifics->logic_width - x;
    if (y + height > internal->device_specifics->logic_height)
        height = internal->device_specifics->logic_height - y;

    // 反转指定区域
    for (uint16_t j = y; j < y + height; j++) {
        for (uint16_t i = x; i < x + width; i++) {
            uint8_t col = i / 2;
            uint8_t pixel_pos = i % 2;

            if (pixel_pos == 0) {
                // 反转高4位
                OLED_GRAM[j][col] ^= 0xF0;
            }
            else {
                // 反转低4位
                OLED_GRAM[j][col] ^= 0x0F;
            }
        }
    }

    return CFBD_TRUE;
}

/**
 * @brief 打开显示
 */
static CFBD_Bool open_oled(CFBD_OLED* handle)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    send_cmd(internal, 0xAF); // Display ON
    return CFBD_TRUE;
}

/**
 * @brief 关闭显示
 */
static CFBD_Bool close_oled(CFBD_OLED* handle)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(handle->oled_internal_handle);
    send_cmd(internal, 0xAE); // Display OFF
    return CFBD_TRUE;
}

static CFBD_Bool iic_query(CFBD_OLED* oled, const char* property, void* args, void* request_data)
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

    if (strcmp("color", property) == 0) {
        uint8_t* grey_scale = (uint8_t*) request_data;
        SSD132XPrivateDatas* privates =
                (SSD132XPrivateDatas*) internal->device_specifics->private_data;
        *grey_scale = privates->grey_scale;
        return CFBD_TRUE;
    }

    return CFBD_FALSE;
}

static CFBD_Bool iic_sets(CFBD_OLED* oled, const char* property, void* args, void* request_data)
{
    CFBD_OLED_IICInitsParams* internal = asIICInitsParams(oled->oled_internal_handle);

    if (strcmp("color", property) == 0) {
        uint8_t* grey_scale = (uint8_t*) request_data;
        SSD132XPrivateDatas* privates =
                (SSD132XPrivateDatas*) internal->device_specifics->private_data;
        privates->grey_scale = *grey_scale & 0x0F; // Ok get the least fourth
        return CFBD_TRUE;
    }

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

void CFBD_OLED_IIC132XInit(CFBD_OLED* handle, CFBD_OLED_IICInitsParams* pvt_handle)
{
    handle->oled_internal_handle = pvt_handle;
    handle->driver_type = CFBD_OLEDDriverType_IIC;
    handle->ops = &iic_ops;
}
