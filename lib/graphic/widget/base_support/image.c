#include "image.h"

#include "base/rectangle.h"
#include "base/size.h"
#include "device/graphic_device.h"

void CFBDGraphic_InitImage(CCGraphic_Image* image,
                           CFBDGraphic_Point* tl_point,
                           CFBDGraphicSize* image_size,
                           uint8_t* sources_register)
{
    image->image_size = *image_size;
    image->point = *tl_point;
    image->sources_register = sources_register;
}

/**
 * @brief Draw image with clipping using small fixed buffer (batch processing)
 * @param device - Graphic device
 * @param image - Image to draw
 * @param clip_rect - Clipping rectangle (viewport bounds)
 * @note Uses 64-byte fixed buffer, processes image in columns batches
 */
void CFBDGraphic_DrawImageClipped(CFBD_GraphicDevice* device,
                                  CCGraphic_Image* image,
                                  CFBDGraphicRect* clip_rect)
{
    if (!image->sources_register || !device || !clip_rect) {
        return;
    }

    // 计算图像的边界
    int16_t img_left = (int16_t) image->point.x;
    int16_t img_top = (int16_t) image->point.y;
    int16_t img_right = img_left + (int16_t) image->image_size.width;
    int16_t img_bottom = img_top + (int16_t) image->image_size.height;

    // 计算裁剪区域的边界
    int16_t clip_left = clip_rect->tl.x;
    int16_t clip_top = clip_rect->tl.y;
    int16_t clip_right = clip_rect->br.x;
    int16_t clip_bottom = clip_rect->br.y;

    // 检查是否完全在裁剪区域外
    if (img_right <= clip_left || img_left >= clip_right || img_bottom <= clip_top ||
        img_top >= clip_bottom) {
        return; // 完全不可见
    }

    // 计算可见区域的交集
    int16_t visible_left = (img_left < clip_left) ? clip_left : img_left;
    int16_t visible_top = (img_top < clip_top) ? clip_top : img_top;
    int16_t visible_right = (img_right > clip_right) ? clip_right : img_right;
    int16_t visible_bottom = (img_bottom > clip_bottom) ? clip_bottom : img_bottom;

    // 计算裁剪后的尺寸
    uint16_t visible_width = (uint16_t) (visible_right - visible_left);
    uint16_t visible_height = (uint16_t) (visible_bottom - visible_top);

    if (visible_width == 0 || visible_height == 0) {
        return;
    }

    // 如果图像完全在裁剪区域内，直接绘制
    if (visible_left == img_left && visible_top == img_top && visible_right == img_right &&
        visible_bottom == img_bottom) {
        CFBDGraphic_DrawImage(device, image);
        return;
    }

    // 计算在原图像中的偏移量
    uint16_t offset_x = (uint16_t) (visible_left - img_left);
    uint16_t offset_y = (uint16_t) (visible_top - img_top);

// 固定64字节缓冲区
#define CLIP_BUFFER_SIZE 64
    uint8_t clipped_buffer[CLIP_BUFFER_SIZE];

    // 计算每列需要的字节数（OLED垂直字节格式）
    uint16_t bytes_per_column = (visible_height + 7) / 8;

    // 计算每批次能处理多少列
    uint16_t columns_per_batch = CLIP_BUFFER_SIZE / bytes_per_column;
    if (columns_per_batch == 0) {
        columns_per_batch = 1; // 至少处理1列
    }

    // 源图像的每列字节数
    uint16_t src_bytes_per_column = (image->image_size.height + 7) / 8;
    uint8_t bit_offset = offset_y % 8;

    // 分批次处理
    for (uint16_t batch_start = 0; batch_start < visible_width; batch_start += columns_per_batch) {
        // 计算本批次实际处理的列数
        uint16_t batch_cols = columns_per_batch;
        if (batch_start + batch_cols > visible_width) {
            batch_cols = visible_width - batch_start;
        }

        // 提取本批次的bitmap数据
        for (uint16_t x = 0; x < batch_cols; x++) {
            uint16_t src_x = offset_x + batch_start + x;

            for (uint16_t byte_y = 0; byte_y < bytes_per_column; byte_y++) {
                uint16_t src_byte_y = (offset_y / 8) + byte_y;

                if (src_byte_y < src_bytes_per_column && src_x < image->image_size.width) {
                    uint8_t src_data =
                            image->sources_register[src_byte_y * image->image_size.width + src_x];

                    // 处理垂直偏移
                    if (bit_offset == 0) {
                        clipped_buffer[byte_y * batch_cols + x] = src_data;
                    }
                    else {
                        // 需要移位拼接
                        uint8_t current_byte = src_data >> bit_offset;

                        if (src_byte_y + 1 < src_bytes_per_column) {
                            uint8_t next_byte =
                                    image->sources_register[(src_byte_y + 1) *
                                                                    image->image_size.width +
                                                            src_x];
                            current_byte |= (next_byte << (8 - bit_offset));
                        }

                        clipped_buffer[byte_y * batch_cols + x] = current_byte;
                    }
                }
                else {
                    clipped_buffer[byte_y * batch_cols + x] = 0;
                }
            }
        }

        // 绘制本批次
        uint16_t batch_x = (uint16_t) (visible_left + batch_start);
        device->ops->setArea(device,
                             batch_x,
                             (uint16_t) visible_top,
                             batch_cols,
                             visible_height,
                             clipped_buffer);

        // 立即更新这一批次
        if (CFBDGraphic_DeviceRequestUpdateAtOnce(device)) {
            device->ops->update_area(device,
                                     batch_x,
                                     (uint16_t) visible_top,
                                     batch_cols,
                                     visible_height);
        }
    }

#undef CLIP_BUFFER_SIZE
}

void CFBDGraphic_DrawImage(CFBD_GraphicDevice* handler, CCGraphic_Image* image)
{
    if (!image->sources_register)
        return;
    handler->ops->setArea(handler,
                          image->point.x,
                          image->point.y,
                          image->image_size.width,
                          image->image_size.height,
                          image->sources_register);
    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handler)) {
        handler->ops->update_area(handler,
                                  image->point.x,
                                  image->point.y,
                                  image->image_size.width,
                                  image->image_size.height);
    }
}
