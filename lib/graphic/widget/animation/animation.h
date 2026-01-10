/**
 * @file animation.h
 * @brief Base animation framework for graphics widget transitions.
 * @ingroup Graphics_Animation
 *
 * @details
 * This module provides the foundation for animation effects used in graphics
 * widgets and UI elements. It supports frame-based animation with configurable
 * frame counts and delays, enabling smooth transitions and visual effects.
 *
 * Animations are driven by a frame counter and delay mechanism:
 * - Each animation consists of a defined number of frames
 * - Each frame can have a delay to control animation speed
 * - Applications can use the animation state to compute intermediate values
 *
 * @see animation_config.h for default animation parameters
 * @see CFBD_BaseAnimation for the animation state structure
 */

#pragma once
#include <stdint.h>

/**
 * @defgroup Graphics_Animation Animation Framework
 * @ingroup Graphics
 * @brief Frame-based animation system for graphics widgets.
 *
 * @details
 * This module provides the core animation framework used throughout the
 * graphics subsystem. It enables smooth visual transitions, UI element
 * animations, and temporal effects.
 *
 * Key features:
 * - Frame-based animation timing
 * - Configurable animation duration and frame count
 * - Customizable delay callbacks for timing control
 * - Integration with widget and graphics subsystems
 *
 * Animations are created with CFBD_InitBaseAnimation() and configured
 * through CFBD_BaseAnimationOps for delay handling.
 *
 * @see animation_config.h for configuration parameters
 * @{\n */

/**
 * @struct CFBD_BaseAnimationOps
 * @brief Operation callbacks for animation timing control.
 *
 * @details
 * This structure contains function pointers for animation operations,
 * particularly timing and delay management. Applications can provide
 * custom implementations for different delay mechanisms.
 */
typedef struct
{
    /**
     * @brief Delay callback function.
     *
     * @details
     * Called by the animation framework when a frame delay is required.
     * Applications should implement this to match their timing mechanisms
     * (busy-wait, OS sleep, etc.).
     *
     * @param ms Number of milliseconds to delay.
     *
     * @note If NULL, the framework assumes delays are handled externally.
     *
     * @example
     * @code
     * void my_delay_ms(uint32_t ms) {
     *     HAL_Delay(ms);  // STM32 HAL delay
     * }
     *
     * CFBD_BaseAnimationOps ops = {
     *     .delay_ms = my_delay_ms
     * };
     * @endcode
     */
    void (*delay_ms)(uint32_t ms);
} CFBD_BaseAnimationOps;

/**
 * @struct CFBD_BaseAnimation
 * @brief Base animation state and configuration.
 *
 * @details
 * This structure maintains the animation state including the number of frames
 * and delay per frame. It is the core data structure for managing animation
 * timing and progression.
 *
 * Typical usage:
 * 1. Initialize with CFBD_InitBaseAnimation()
 * 2. Configure anim_frames and anim_frame_delay_ms as needed
 * 3. Use in conjunction with animation callbacks
 * 4. Query anim_frames to compute animation progress
 *
 * @example
 * @code
 * // Setup animation with 8 frames, 50ms per frame
 * CFBD_BaseAnimation anim;
 * CFBD_InitBaseAnimation(&anim);
 * anim.anim_frames = 8;
 * anim.anim_frame_delay_ms = 50;  // 400ms total animation
 * @endcode
 */
typedef struct __CFBD_BaseAnimation
{
    /**
     * @brief Number of animation frames.
     *
     * @details
     * Total number of frames that comprise the complete animation cycle.
     * Typical values: 4-16 frames for smooth animation perception
     * (minimum 4 frames for noticeable animation, 8+ for smooth transitions).
     */
    uint8_t anim_frames;

    /**
     * @brief Delay in milliseconds per frame.
     *
     * @details
     * Time interval between animation frames. Values:
     * - 0: No delay (maximum speed, frame timing external)
     * - 16-33: ~30-60 FPS (typical interactive UI)
     * - 50+: Slower, cinematic transitions
     *
     * Total animation duration = anim_frames * anim_frame_delay_ms
     */
    uint32_t anim_frame_delay_ms;
} CFBD_BaseAnimation;

/**
 * @brief Initialize a base animation with default settings.
 *
 * @details
 * Sets up a CFBD_BaseAnimation structure with default values from
 * animation_config.h (CFBD_BASEANIMATION_FRAME_CNT and
 * CFBD_BASEANIMATION_DELAY_MS). Can be further customized after
 * initialization.
 *
 * @param pBaseAnimation Pointer to the CFBD_BaseAnimation structure to initialize.
 *                       Must not be NULL.
 *
 * @note After initialization, you may modify anim_frames and anim_frame_delay_ms
 *       to customize the animation behavior.
 *
 * @see CFBD_BaseAnimation for structure details
 * @see animation_config.h for default values
 *
 * @example
 * @code
 * // Initialize animation with defaults
 * CFBD_BaseAnimation fade_animation;
 * CFBD_InitBaseAnimation(&fade_animation);
 *
 * // Customize if needed
 * fade_animation.anim_frames = 16;
 * fade_animation.anim_frame_delay_ms = 30;
 *
 * // Use in animation loop
 * for (uint8_t frame = 0; frame < fade_animation.anim_frames; frame++) {
 *     // Render frame
 *     if (fade_animation.anim_frame_delay_ms > 0) {
 *         HAL_Delay(fade_animation.anim_frame_delay_ms);
 *     }
 * }
 * @endcode
 */
void CFBD_InitBaseAnimation(CFBD_BaseAnimation* pBaseAnimation);

/** @} */ // end of Graphics_Animation group