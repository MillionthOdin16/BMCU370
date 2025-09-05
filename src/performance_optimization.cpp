#include "performance_optimization.h"
#include "Debug_log.h"
#include <Arduino.h>
#include <string.h>  // For memset

#if ENABLE_PERFORMANCE_MONITORING

// Performance metrics storage
static PerformanceMetrics metrics;
static uint32_t last_update_time = 0;
static uint32_t cpu_busy_time = 0;
static uint32_t cpu_start_time = 0;

/**
 * Initialize performance monitoring system
 */
void performance_init() {
    memset(&metrics, 0, sizeof(PerformanceMetrics));
    last_update_time = millis();
    cpu_start_time = millis();
    
    DEBUG_MY("Performance monitoring initialized\n");
}

/**
 * Mark CPU as busy (call at start of intensive operations)
 */
void performance_cpu_busy_start() {
    cpu_start_time = micros();
}

/**
 * Mark CPU as idle (call at end of intensive operations)
 */
void performance_cpu_busy_end() {
    if (cpu_start_time > 0) {
        cpu_busy_time += (micros() - cpu_start_time);
        cpu_start_time = 0;
    }
}

/**
 * Get free RAM amount using stack pointer method
 */
uint16_t performance_get_free_ram() {
    extern char _end;
    extern char __stack;
    
    // Calculate free RAM between heap end and stack pointer
    char* stack_ptr;
    asm volatile ("mv %0, sp" : "=r" (stack_ptr));
    
    return (uint16_t)(stack_ptr - &_end);
}

/**
 * Update performance metrics
 */
void performance_update() {
    uint32_t current_time = millis();
    uint32_t elapsed_time = current_time - last_update_time;
    
    // Update metrics every second
    if (elapsed_time >= 1000) {
        // Update free RAM
        metrics.free_ram_bytes = performance_get_free_ram();
        
        // Calculate CPU usage as percentage
        if (elapsed_time > 0) {
            uint32_t cpu_busy_ms = cpu_busy_time / 1000; // Convert us to ms
            metrics.cpu_usage_percent = (uint16_t)((cpu_busy_ms * 100) / elapsed_time);
            if (metrics.cpu_usage_percent > 100) metrics.cpu_usage_percent = 100;
        }
        
        // Reset counters
        cpu_busy_time = 0;
        last_update_time = current_time;
        
        #ifdef Debug_log_on
        if (elapsed_time >= 5000) { // Log every 5 seconds
            DEBUG_MY("Performance: RAM=");
            DEBUG_MY(String(metrics.free_ram_bytes).c_str());
            DEBUG_MY(" bytes, CPU=");
            DEBUG_MY(String(metrics.cpu_usage_percent).c_str());
            DEBUG_MY("%, Errors=");
            DEBUG_MY(String(metrics.error_count).c_str());
            DEBUG_MY("\n");
        }
        #endif
    }
}

/**
 * Get current performance metrics
 */
const PerformanceMetrics* performance_get_metrics() {
    return &metrics;
}

/**
 * Record sensor reading time
 */
void performance_record_sensor_time(uint16_t time_us) {
    metrics.sensor_read_time_us = time_us;
}

/**
 * Record LED update time
 */
void performance_record_led_time(uint16_t time_us) {
    metrics.led_update_time_us = time_us;
}

/**
 * Record communication latency
 */
void performance_record_comm_latency(uint16_t latency_ms) {
    metrics.communication_latency_ms = latency_ms;
}

/**
 * Increment error counter
 */
void performance_record_error() {
    if (metrics.error_count < 255) {
        metrics.error_count++;
    }
}

/**
 * Reset error counter
 */
void performance_reset_errors() {
    metrics.error_count = 0;
}

#endif // ENABLE_PERFORMANCE_MONITORING

#if ENABLE_MEMORY_OPTIMIZATION

/**
 * Simple optimized memory allocation
 * For this microcontroller, we'll just use standard malloc but add tracking
 */
void* optimized_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        #if ENABLE_PERFORMANCE_MONITORING
        performance_record_error();
        #endif
        DEBUG_MY("Memory allocation failed for size: ");
        DEBUG_MY(String(size).c_str());
        DEBUG_MY("\n");
    }
    return ptr;
}

/**
 * Optimized memory free
 */
void optimized_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

#endif // ENABLE_MEMORY_OPTIMIZATION