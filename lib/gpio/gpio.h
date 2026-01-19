/**
 * @file gpio.h
 * @brief GPIO abstraction and helpers used by platform and application code.
 *
 * @details
 * This header provides a minimal, portable GPIO interface used by the
 * CFBD project. It defines lightweight handle types, an initialization
 * parameter opaque type and basic operations for setting, clearing and
 * toggling GPIO lines. The concrete backing implementation is selected
 * by platform macros (for example `CFBD_IS_ST` includes the STM
 * implementation).
 *
 * The API is intentionally small to keep portability across targets
 * straightforward; callers should ensure the provided handle is valid
 * and the underlying platform driver has been initialized.
 *
 * @note The header assumes a platform-specific implementation header
 *       (e.g. `gpio-stm-impl.h`) supplies the necessary low-level
 *       definitions used by the functions declared here.
 */

#pragma once
#include "cfbd_define.h"
#include "lib_settings.h"

#if defined(CFBD_IS_ST)
#include "../gpio/gpio-stm-impl.h"
#else
#error "No supports chips"
#endif

/**
 * @typedef GPIO_PinType
 * @brief Integral type used to identify a single GPIO pin.
 *
 * @details The concrete meaning (bit-mask, number, encoded port/pin)
 * depends on the platform implementation header. Use `GPIO_PinType`
 * when storing or passing pin identifiers to the API.
 */
typedef uint32_t GPIO_PinType;

/**
 * @typedef GPIO_TypeHandle
 * @brief Opaque handle type referring to platform-specific GPIO state.
 *
 * @details Implementations should cast this pointer to an internal
 * structure defined by the platform backend. The type is intentionally
 * opaque to keep the public API independent from implementation
 * details.
 */
typedef void* GPIO_TypeHandle;

/**
 * @struct CFBD_GPIOHandle
 * @brief Public GPIO handle combining platform handle and pin id.
 *
 * @details
 * This small POD aggregates a platform handle and a pin identifier. It
 * is the primary handle passed to the GPIO API. The `handle_internals_`
 * field contains the backend-specific state while `pin_internals_`
 * identifies which pin to operate on.
 */
typedef struct
{
    /**
     * @brief Platform-specific handle or peripheral pointer.
     */
    GPIO_TypeHandle handle_internals_;

    /**
     * @brief Platform-specific pin identifier (see `GPIO_PinType`).
     */
    GPIO_PinType pin_internals_;
} CFBD_GPIOHandle;

/**
 * @typedef CFBD_GPIOInitParams
 * @brief Opaque pointer type for GPIO initialization parameters.
 *
 * @details
 * Allows platform or application-specific initialization data to be
 * passed to `CFBD_GPIOInit`. Cast to the concrete type expected by
 * the underlying implementation when processing.
 */
typedef void* CFBD_GPIOInitParams;

/**
 * @brief Initialize a GPIO handle for subsequent operations.
 *
 * @details
 * Performs any required platform-specific setup of `handle` using the
 * supplied `params`. After successful initialization the `handle`
 * should be ready for `CFBD_GPIOSet`/`CFBD_GPIOUnset`/`CFBD_GPIOToggle`.
 *
 * @param handle Pointer to a `CFBD_GPIOHandle` structure to initialize.
 * @param params Opaque initialization parameters (may be NULL).
 */
void CFBD_GPIOInit(CFBD_GPIOHandle* handle, CFBD_GPIOInitParams params);

/**
 * @brief Set (drive high) the GPIO represented by `handle`.
 *
 * @param handle Pointer to an initialized `CFBD_GPIOHandle`.
 */
void CFBD_GPIOSet(CFBD_GPIOHandle* handle);

/**
 * @brief Clear (drive low) the GPIO represented by `handle`.
 *
 * @param handle Pointer to an initialized `CFBD_GPIOHandle`.
 */
void CFBD_GPIOUnset(CFBD_GPIOHandle* handle);

/**
 * @brief Toggle the GPIO represented by `handle`.
 *
 * @param handle Pointer to an initialized `CFBD_GPIOHandle`.
 */
void CFBD_GPIOToggle(CFBD_GPIOHandle* handle);

/**
 * @brief Inline helper to set or clear a GPIO based on `status`.
 *
 * @details
 * Convenience wrapper that calls `CFBD_GPIOSet` when `status` is
 * true, otherwise calls `CFBD_GPIOUnset`.
 *
 * @param handle Pointer to an initialized `CFBD_GPIOHandle`.
 * @param status If `CFBD_TRUE`, set the pin; if `CFBD_FALSE`, clear it.
 */
static void inline CFBD_GPIOSetPin(CFBD_GPIOHandle* handle, CFBD_Bool status)
{
    status ? CFBD_GPIOSet(handle) : CFBD_GPIOUnset(handle);
}

CFBD_Bool CFBD_GPIOReadPin(CFBD_GPIOHandle* handle);