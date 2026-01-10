/**
 * @file system_clock.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief System timing primitives and delay helpers.
 * @ingroup Timing_Module
 *
 * @details
 * This header exposes minimal timing helpers used throughout the
 * application code for short, platform-dependent delays. The concrete
 * behavior (blocking, sleep, busy-wait, tick resolution) depends on the
 * platform-specific implementation behind these APIs.
 *
 * Implementations should document any real-time or concurrency
 * restrictions (for example, whether the functions may be called from
 * interrupt context). Consumers of these APIs should avoid long
 * blocking delays in time-critical or interrupt contexts.
 *
 * @note Units are explicitly milliseconds (`ms`) and microseconds (`us`).
 *
 * @version 0.1
 * @date 2025-03-30
 * @copyright Copyright (c) 2025
 *
 * @page timing_guide System Timing and Delay Guide
 * @section timing_overview Overview
 * The system clock module provides simple delay primitives for timing-dependent
 * operations in embedded applications. These functions are intentionally minimal
 * to work across diverse platform implementations.
 *
 * @section timing_concepts Key Concepts
 * - **Millisecond delays** (`system_delay_ms`) for longer timing sequences
 * - **Microsecond delays** (`system_delay_us`) for precise sub-millisecond timing
 * - **Platform-specific implementation** for accuracy and power efficiency
 *
 * @section timing_use_cases Common Use Cases
 * - Debouncing GPIO inputs
 * - Timing I2C/SPI transaction sequences
 * - Adding delays between hardware initialization steps
 * - Bit-banging protocol timing
 *
 * @section timing_examples Practical Examples
 *
 * @subsection timing_gpio_debounce GPIO Debouncing
 * @code{.c}
 * #include "lib/application/sys_clock/system_clock.h"
 * #include "lib/gpio/gpio.h"
 *
 * CFBD_Bool read_debounced_button(CFBD_GPIOHandle* button_pin) {
 *     // Initial read
 *     CFBD_Bool first_read = CFBD_GPIORead(button_pin);
 *
 *     // Wait for switch to settle
 *     system_delay_ms(20);
 *
 *     // Confirm state hasn't changed
 *     CFBD_Bool second_read = CFBD_GPIORead(button_pin);
 *     return (first_read == second_read) ? first_read : CFBD_FALSE;
 * }
 * @endcode
 *
 * @subsection timing_i2c_sequence I2C Bus Timing
 * @code{.c}
 * #include "lib/application/sys_clock/system_clock.h"
 *
 * void i2c_bit_banged_start_condition(void) {
 *     // SDA and SCL high
 *     set_sda_high();
 *     set_scl_high();
 *     system_delay_us(5);
 *
 *     // Bring SDA low while SCL is high
 *     set_sda_low();
 *     system_delay_us(5);
 *
 *     // Bring SCL low
 *     set_scl_low();
 * }
 * @endcode
 *
 * @section timing_platform Platform Considerations
 * - **Accuracy**: Actual delay may vary depending on system load and interrupt activity
 * - **Interrupt Safety**: Check platform documentation for ISR usage restrictions
 * - **Power**: Consider sleep/standby modes for longer waits
 * - **Busy-wait**: Current implementations typically use busy-wait loops
 *
 * @section timing_best_practices Best Practices
 * 1. Use millisecond delays for non-critical timing sequences
 * 2. Use microsecond delays only when necessary for precision
 * 3. Avoid calling from interrupt handlers (unless documented as safe)
 * 4. Consider application clock frequency when porting between platforms
 * 5. Combine with hardware timers for more accurate long-duration timing
 */

#ifndef SYSCLK_H
#define SYSCLK_H

#include <stdint.h>

/**
 * @defgroup Timing_Module System Clock and Timing
 * @brief Short blocking delay primitives for timing operations.
 * @details
 * Provides minimal, portable timing helpers for embedded systems development.
 * These primitives are intended for short delays in initialization sequences,
 * debouncing, and protocol timing. For longer or more precise timing requirements,
 * consider using hardware timers or dedicated scheduling systems.
 * @{
 */

/**
 * @brief Delay execution for at least the given number of milliseconds.
 *
 * @details
 * Blocks the calling context for approximately `delay_ms` milliseconds.
 * The actual resolution and maximum accuracy depend on the platform
 * timer implementation. The function is intended for short delays such
 * as debouncing or simple timing sequences. For longer waits or low
 * power idle, prefer platform-specific sleep APIs if available.
 *
 * Common use cases:
 * - GPIO debouncing (typically 10-50 ms)
 * - Sensor settling time
 * - UI interaction delays
 * - Initialization sequencing
 *
 * @param delay_ms Number of milliseconds to delay (>= 0). A value of 0
 *                 should return immediately. Values greater than a few
 *                 seconds should generally be avoided due to busy-wait
 *                 power consumption.
 *
 * @par Thread-safety
 * The function may or may not be safe to call from interrupt context;
 * check the platform implementation before invoking from ISRs.
 *
 * @par Performance Notes
 * Implementations typically use busy-wait loops or hardware timers.
 * The actual delay may be slightly longer than requested due to
 * instruction execution overhead.
 *
 * @par Example
 * @code{.c}
 * #include "lib/application/sys_clock/system_clock.h"
 *
 * // Wait 100 ms for a sensor to stabilize
 * system_delay_ms(100);
 *
 * // Multiple delays in sequence
 * for (int i = 0; i < 5; i++) {
 *     system_delay_ms(500);  // 500 ms between actions
 * }
 * @endcode
 *
 * @since 0.1
 * @see system_delay_us()
 */
void system_delay_ms(uint32_t delay_ms);

/**
 * @brief Delay execution for at least the given number of microseconds.
 *
 * @details
 * Blocks the calling context for approximately `us` microseconds. This
 * helper is useful for short, sub-millisecond timing (peripheral timing
 * requirements, bit-banged protocols, etc.). Accuracy and minimum
 * granularity depend on the underlying timer implementation.
 *
 * The resolution on most platforms is typically 1-10 microseconds, though
 * exact behavior is implementation-defined. For very precise timing, consider
 * using dedicated hardware peripherals.
 *
 * Common use cases:
 * - I2C/SPI timing sequences
 * - Pulse width generation for bit-banging
 * - Hardware-specific timing requirements
 * - Precise interval measurements below millisecond granularity
 *
 * @param us Number of microseconds to delay (>= 0). A value of 0
 *           should return immediately. Large values (> 1000000) should
 *           generally use `system_delay_ms()` instead.
 *
 * @par Performance
 * Implementations may use busy-wait loops; avoid calling with large
 * values in performance-sensitive or low-power scenarios. Accuracy
 * may degrade for delays shorter than 10 microseconds depending on
 * platform overhead.
 *
 * @par Precision Warning
 * Timing accuracy depends heavily on:
 * - System clock frequency stability
 * - Instruction execution time
 * - Interrupt latency (if not called from ISR with interrupts disabled)
 * Do not rely on precise timing for critical applications without
 * additional hardware support.
 *
 * @par Example
 * @code{.c}
 * #include "lib/application/sys_clock/system_clock.h"
 *
 * // Create a small pulse of 5 microseconds
 * system_delay_us(5);
 *
 * // Bit-banged I2C start condition timing
 * set_sda_low();
 * system_delay_us(2);  // Setup time
 * set_scl_low();
 *
 * // Multiple sub-millisecond delays
 * for (int bit = 0; bit < 8; bit++) {
 *     transmit_bit(bit);
 *     system_delay_us(10);  // Inter-bit timing
 * }
 * @endcode
 *
 * @since 0.1
 * @see system_delay_ms()
 */
void system_delay_us(uint32_t us);

/** @} */

#endif /* SYSCLK_H */