#include "advanced_optimization.h"
#include "Debug_log.h"
#include <string.h>

// =============================================================================
// Optimized Sensor Filtering Implementation
// =============================================================================

#if ENABLE_OPTIMIZED_FILTERING

/**
 * Initialize optimized filter
 */
void filter_init(OptimizedFilter* filter) {
    if (!filter) return;
    
    memset(filter->history, 0, sizeof(filter->history));
    filter->index = 0;
    filter->sum = 0;
    filter->initialized = false;
}

/**
 * Add sample to optimized filter and get filtered result
 * Uses fixed-point arithmetic for efficiency on microcontroller
 */
float filter_update(OptimizedFilter* filter, float sample) {
    if (!filter) return sample;
    
    // Convert float to fixed-point (16.16 format scaled down for int16_t)
    int16_t fixed_sample = (int16_t)(sample * 100.0f); // Scale by 100 for precision
    
    if (!filter->initialized) {
        // Initialize all history with first sample for faster settling
        for (int i = 0; i < 4; i++) {
            filter->history[i] = fixed_sample;
        }
        filter->sum = fixed_sample * 4;
        filter->initialized = true;
        return sample;
    }
    
    // Remove old sample from sum
    filter->sum -= filter->history[filter->index];
    
    // Add new sample
    filter->history[filter->index] = fixed_sample;
    filter->sum += fixed_sample;
    
    // Update index with wrapping
    filter->index = (filter->index + 1) & 3; // Efficient modulo 4 using bitwise AND
    
    // Return filtered result (convert back to float)
    return (float)(filter->sum) / (4.0f * 100.0f);
}

/**
 * Reset filter to initial state
 */
void filter_reset(OptimizedFilter* filter) {
    if (!filter) return;
    
    filter->initialized = false;
    filter_init(filter);
}

#endif // ENABLE_OPTIMIZED_FILTERING

// =============================================================================
// Communication Optimization Implementation
// =============================================================================

#if ENABLE_MEMORY_OPTIMIZATION

/**
 * Initialize communication ring buffer
 */
void comm_ring_init(CommBufferRing* ring, uint8_t* buffer, uint16_t size) {
    if (!ring || !buffer || size == 0) return;
    
    ring->buffer = buffer;
    ring->size = size;
    ring->head = 0;
    ring->tail = 0;
    ring->count = 0;
}

/**
 * Add data to ring buffer
 */
uint16_t comm_ring_put(CommBufferRing* ring, const uint8_t* data, uint16_t length) {
    if (!ring || !data || length == 0) return 0;
    
    uint16_t available = ring->size - ring->count;
    if (length > available) {
        length = available; // Truncate to available space
    }
    
    uint16_t added = 0;
    while (added < length) {
        ring->buffer[ring->head] = data[added];
        ring->head = (ring->head + 1) % ring->size;
        ring->count++;
        added++;
    }
    
    return added;
}

/**
 * Get data from ring buffer
 */
uint16_t comm_ring_get(CommBufferRing* ring, uint8_t* data, uint16_t length) {
    if (!ring || !data || length == 0) return 0;
    
    if (length > ring->count) {
        length = ring->count; // Limit to available data
    }
    
    uint16_t retrieved = 0;
    while (retrieved < length) {
        data[retrieved] = ring->buffer[ring->tail];
        ring->tail = (ring->tail + 1) % ring->size;
        ring->count--;
        retrieved++;
    }
    
    return retrieved;
}

/**
 * Get available space in ring buffer
 */
uint16_t comm_ring_available_space(const CommBufferRing* ring) {
    if (!ring) return 0;
    return ring->size - ring->count;
}

/**
 * Get used space in ring buffer
 */
uint16_t comm_ring_used_space(const CommBufferRing* ring) {
    if (!ring) return 0;
    return ring->count;
}

#endif // ENABLE_MEMORY_OPTIMIZATION

// =============================================================================
// Power Management Implementation
// =============================================================================

#if ENABLE_POWER_OPTIMIZATION

// Simple power management state
static bool power_in_idle = false;

/**
 * Check if system can enter low power mode
 * Basic check - can be extended with more sophisticated logic
 */
bool power_can_enter_idle() {
    // Add checks here for:
    // - No active communication
    // - No motor movement
    // - No pending LED updates
    // For now, return false to be safe
    return false; // Conservative approach for safety
}

/**
 * Enter low power mode during idle periods
 * This is a placeholder for power optimization
 */
void power_enter_idle(uint16_t duration_ms) {
    if (!power_can_enter_idle()) return;
    
    power_in_idle = true;
    
    // Simple delay instead of actual power management for safety
    // In a real implementation, this would use:
    // - WFI (Wait For Interrupt) instruction
    // - Clock gating for unused peripherals
    // - Reduced clock frequency
    delay(duration_ms);
    
    power_in_idle = false;
}

/**
 * Wake up from low power mode
 */
void power_wake_up() {
    if (power_in_idle) {
        power_in_idle = false;
        // In a real implementation, restore full clock speed and peripherals
    }
}

#endif // ENABLE_POWER_OPTIMIZATION