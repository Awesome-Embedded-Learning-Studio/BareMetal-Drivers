
#pragma once
#include "configs/external_impl_driver.h"
#include "oled.h"

/**
 * @brief To init a iic based oled
 *
 * @param handle
 * @param pvt_handle
 */
void CFBD_OLED_IICInit(CFBD_OLED* handle, CFBD_OLED_IICInitsParams* pvt_handle);
