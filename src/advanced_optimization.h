#pragma once

/**
 * BMCU370 Advanced Performance Optimizations
 * 
 * This header contains optimized implementations for sensor filtering
 * and communication efficiency improvements.
 */

#include "config.h"
#include <stdint.h>

// =============================================================================
// Optimized Sensor Filtering
// =============================================================================

#if ENABLE_OPTIMIZED_FILTERING

/**
 * Simple moving average filter structure
 * Uses fixed-point arithmetic for efficiency
 */
struct OptimizedFilter {
    int16_t history[4];     // 4-sample history (minimal memory)
    uint8_t index;          // Current index
    int16_t sum;            // Running sum for efficiency
    bool initialized;       // Filter initialization flag
};

/**
 * Initialize optimized filter
 * @param filter Pointer to filter structure
 */
void filter_init(OptimizedFilter* filter);

/**
 * Add sample to optimized filter and get filtered result
 * @param filter Pointer to filter structure
 * @param sample New sample to add
 * @return Filtered result
 */
float filter_update(OptimizedFilter* filter, float sample);

/**
 * Reset filter to initial state
 * @param filter Pointer to filter structure
 */
void filter_reset(OptimizedFilter* filter);

#endif // ENABLE_OPTIMIZED_FILTERING

// =============================================================================
// Communication Optimization
// =============================================================================

/**
 * Optimized communication buffer management
 * Reduces memory fragmentation and improves efficiency
 */
#if ENABLE_MEMORY_OPTIMIZATION

/**
 * Communication buffer ring structure
 * Implements efficient circular buffer for communication
 */
struct CommBufferRing {
    uint8_t* buffer;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
};

/**
 * Initialize communication ring buffer
 * @param ring Pointer to ring buffer structure
 * @param buffer Pointer to buffer memory
 * @param size Size of buffer
 */
void comm_ring_init(CommBufferRing* ring, uint8_t* buffer, uint16_t size);

/**
 * Add data to ring buffer
 * @param ring Pointer to ring buffer structure
 * @param data Pointer to data to add
 * @param length Length of data
 * @return Number of bytes actually added
 */
uint16_t comm_ring_put(CommBufferRing* ring, const uint8_t* data, uint16_t length);

/**
 * Get data from ring buffer
 * @param ring Pointer to ring buffer structure
 * @param data Pointer to buffer for retrieved data
 * @param length Maximum length to retrieve
 * @return Number of bytes actually retrieved
 */
uint16_t comm_ring_get(CommBufferRing* ring, uint8_t* data, uint16_t length);

/**
 * Get available space in ring buffer
 * @param ring Pointer to ring buffer structure
 * @return Available space in bytes
 */
uint16_t comm_ring_available_space(const CommBufferRing* ring);

/**
 * Get used space in ring buffer
 * @param ring Pointer to ring buffer structure
 * @return Used space in bytes
 */
uint16_t comm_ring_used_space(const CommBufferRing* ring);

#endif // ENABLE_MEMORY_OPTIMIZATION

// =============================================================================
// Power Management Optimization
// =============================================================================

/**
 * Simple power management for idle periods
 */
#define ENABLE_POWER_OPTIMIZATION      true

#if ENABLE_POWER_OPTIMIZATION

/**
 * Enter low power mode during idle periods
 * @param duration_ms Duration to stay in low power mode
 */
void power_enter_idle(uint16_t duration_ms);

/**
 * Wake up from low power mode
 */
void power_wake_up();

/**
 * Check if system can enter low power mode
 * @return true if safe to enter low power mode
 */
bool power_can_enter_idle();

#endif // ENABLE_POWER_OPTIMIZATION