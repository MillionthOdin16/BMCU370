#pragma once

/**
 * BMCU370 Performance Optimization Header
 * 
 * This file contains optimizations for memory usage, performance,
 * and system efficiency improvements while maintaining compatibility.
 */

#include "config.h"
#include <stdint.h>  // For uint8_t, uint16_t, uint32_t
#include <stddef.h>  // For size_t

// =============================================================================
// Memory Optimization Configuration
// =============================================================================

/**
 * Enable memory optimizations
 * - Use smaller data types where possible
 * - Optimize buffer sizes
 * - Reduce unnecessary global variables
 */
#define ENABLE_MEMORY_OPTIMIZATION      true

/**
 * Communication buffer optimization
 * Original: 1000 bytes
 * Optimized: 512 bytes (sufficient for BambuBus protocol)
 */
#define OPTIMIZED_BAMBU_BUS_BUFFER_SIZE 512

/**
 * Use 16-bit timestamps for short-term timing instead of 64-bit
 * Saves 24 bytes per channel (4 channels × 6 bytes per uint64_t)
 */
#define USE_OPTIMIZED_TIMESTAMPS        true

/**
 * Enable sensor data filtering optimization
 * Use efficient moving average instead of complex filtering
 */
#define ENABLE_OPTIMIZED_FILTERING      true

// =============================================================================
// Performance Enhancement Configuration  
// =============================================================================

/**
 * LED update frequency optimization
 * Reduce unnecessary LED updates to save CPU cycles
 */
#define OPTIMIZED_LED_UPDATE_INTERVAL_MS    50      // Instead of updating every loop
#define ENABLE_LED_UPDATE_OPTIMIZATION      true

/**
 * Sensor reading optimization
 * Batch sensor readings for better efficiency
 */
#define ENABLE_BATCH_SENSOR_READING         true

/**
 * Motion control optimization
 * Use optimized control algorithms
 */
#define ENABLE_OPTIMIZED_MOTION_CONTROL     true

// =============================================================================
// System Health Monitoring
// =============================================================================

/**
 * Enable system performance monitoring
 * Track key metrics for diagnostics
 */
#define ENABLE_PERFORMANCE_MONITORING       true

/**
 * Performance metrics structure
 */
struct PerformanceMetrics {
    uint16_t free_ram_bytes;            ///< Available RAM in bytes
    uint16_t cpu_usage_percent;         ///< CPU usage percentage (0-100)
    uint16_t sensor_read_time_us;       ///< Sensor reading time in microseconds
    uint16_t led_update_time_us;        ///< LED update time in microseconds
    uint16_t communication_latency_ms;  ///< Communication latency in milliseconds
    uint8_t  error_count;               ///< Number of errors since last reset
};

// =============================================================================
// Function Declarations
// =============================================================================

#if ENABLE_PERFORMANCE_MONITORING
/**
 * Initialize performance monitoring system
 */
void performance_init();

/**
 * Update performance metrics
 */
void performance_update();

/**
 * Get current performance metrics
 * @return Pointer to performance metrics structure
 */
const PerformanceMetrics* performance_get_metrics();

/**
 * Get free RAM amount
 * @return Free RAM in bytes
 */
uint16_t performance_get_free_ram();

/**
 * Mark CPU as busy (call at start of intensive operations)
 */
void performance_cpu_busy_start();

/**
 * Mark CPU as idle (call at end of intensive operations)
 */
void performance_cpu_busy_end();

/**
 * Record sensor reading time
 */
void performance_record_sensor_time(uint16_t time_us);

/**
 * Record LED update time
 */
void performance_record_led_time(uint16_t time_us);

/**
 * Record communication latency
 */
void performance_record_comm_latency(uint16_t latency_ms);

/**
 * Increment error counter
 */
void performance_record_error();

/**
 * Reset error counter
 */
void performance_reset_errors();
#else
// Dummy functions when performance monitoring is disabled
#define performance_init()
#define performance_update()
#define performance_cpu_busy_start()
#define performance_cpu_busy_end()
#define performance_record_sensor_time(time_us)
#define performance_record_led_time(time_us)
#define performance_record_comm_latency(latency_ms)
#define performance_record_error()
#define performance_reset_errors()
#endif

#if ENABLE_MEMORY_OPTIMIZATION
/**
 * Optimized memory allocation helper
 */
void* optimized_malloc(size_t size);

/**
 * Optimized memory free helper
 */
void optimized_free(void* ptr);
#endif