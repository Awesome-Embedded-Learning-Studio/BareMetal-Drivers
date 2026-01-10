/**
 * @file animation_config.h
 * @brief Configuration parameters for the animation framework.
 * @ingroup Graphics_Animation
 *
 * @details
 * This header defines compile-time configuration for animation behavior.
 * Adjust these values to tune animation characteristics and visual smoothness
 * for your application.
 *
 * @see animation.h for the animation framework
 * @see CFBD_BaseAnimation for animation structure
 */

#pragma once

/**
 * @def CFBD_BASEANIMATION_FRAME_CNT
 * @brief Default number of frames for base animations.
 *
 * @details
 * The default frame count for newly created animations. Typical values:
 * - 4-8: Fast transitions (UI feedback)
 * - 8-16: Smooth animations (visual effects)
 * - 16-32: Slow, cinematic transitions
 *
 * @see CFBD_InitBaseAnimation() for animation initialization
 *
 * @note Individual animations can override this value after initialization.
 */
#define CFBD_BASEANIMATION_FRAME_CNT (8)

/**
 * @def CFBD_BASEANIMATION_DELAY_MS
 * @brief Default delay per animation frame in milliseconds.
 *
 * @details
 * Time between animation frames in the default configuration.
 * - 0: No delay (frame timing handled externally)
 * - 16: ~60 FPS animation speed
 * - 33: ~30 FPS animation speed
 * - 50-100: Slower transitions
 *
 * Actual animation duration = frame_count * delay_per_frame
 *
 * @note Individual animations can override this value after initialization.
 *
 * @example
 * @code
 * // Default animation would run for (8 frames * 0 ms = 0 ms)
 * // or external frame timing if delay_ms is 0
 * @endcode
 */
#define CFBD_BASEANIMATION_DELAY_MS (0)
