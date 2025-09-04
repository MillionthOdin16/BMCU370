#pragma once

/**
 * Enhanced Main Header for BMCU370 with Reliability Improvements
 * 
 * This file demonstrates how the new reliability and feature enhancements
 * integrate with the existing BMCU370 firmware architecture.
 */

#include <Arduino.h>
#include "config.h"
#include "system_watchdog.h"
#include "error_manager.h"

// Enhanced configuration with new reliability parameters
// (These would be added to config.h in a real implementation)

// =============================================================================
// Enhanced Reliability Configuration
// =============================================================================

#define SYSTEM_HEALTH_CHECK_INTERVAL_MS     5000        ///< System health monitoring interval
#define SENSOR_VALIDATION_ENABLED           true         ///< Enable sensor validation
#define AUTO_RECOVERY_ENABLED               true         ///< Enable automatic error recovery
#define PERFORMANCE_MONITORING_ENABLED      true         ///< Enable performance monitoring

// =============================================================================
// Advanced LED Pattern Configuration
// =============================================================================

#define LED_PATTERN_UPDATE_INTERVAL_MS      50           ///< LED pattern update rate
#define LED_BREATHING_ENABLED               true         ///< Enable breathing animations
#define LED_ERROR_FLASH_RATE_MS             250          ///< Error flash rate

// =============================================================================
// Enhanced Function Declarations
// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif

// Enhanced system initialization
void enhanced_system_init();
void enhanced_system_health_check();
void enhanced_error_recovery_handler();

// Enhanced sensor functions with validation
bool read_as5600_with_validation(uint8_t channel, uint16_t* angle);
bool read_adc_with_validation(uint8_t channel, float* voltage);
void validate_all_sensors();

// Enhanced LED control with patterns
void set_led_pattern(uint8_t channel, uint8_t led, const char* pattern, uint32_t color1, uint32_t color2 = 0);
void update_led_patterns();
void show_system_status_enhanced(int bambubus_status, bool has_errors);

// Enhanced motion control with error detection
void motion_control_enhanced_init();
bool motion_control_enhanced_run(uint8_t channel, int16_t target_position);
bool detect_motion_anomalies(uint8_t channel);

// Enhanced communication with reliability
bool bambubus_send_with_retry(uint8_t* data, uint16_t length);
void bambubus_monitor_health();
bool bambubus_is_healthy();

// Performance monitoring
void update_performance_metrics();
void log_performance_event(const char* event, uint32_t duration_ms);

// Configuration management
bool load_enhanced_configuration();
bool save_enhanced_configuration();
void reset_to_enhanced_defaults();

#ifdef __cplusplus
}
#endif

// =============================================================================
// Enhanced System State Structures
// =============================================================================

/**
 * Enhanced system state with reliability monitoring
 */
struct EnhancedSystemState {
    // Existing state
    bool system_online;
    uint8_t active_channel;
    uint32_t last_heartbeat;
    
    // Enhanced reliability state
    bool watchdog_active;
    bool sensors_healthy;
    bool communication_healthy;
    uint32_t error_count_last_hour;
    float system_performance_score;  // 0.0 to 1.0
    
    // Maintenance state
    bool maintenance_required;
    uint32_t next_calibration_due;
    uint32_t total_runtime_hours;
    
    // Advanced features state
    bool advanced_patterns_enabled;
    bool predictive_maintenance_enabled;
    bool auto_recovery_active;
};

/**
 * Enhanced channel state with detailed monitoring
 */
struct EnhancedChannelState {
    // Existing channel state
    bool online;
    float filament_meters;
    uint16_t pressure_reading;
    
    // Enhanced monitoring
    bool sensor_healthy;
    float sensor_accuracy_score;
    uint32_t last_successful_operation;
    uint16_t error_count_today;
    
    // Predictive maintenance
    uint32_t operation_cycles;
    float wear_estimate;
    bool maintenance_due;
    
    // Performance metrics
    float avg_operation_speed;
    float reliability_score;
    uint32_t successful_operations;
    uint32_t failed_operations;
};

// Global enhanced state variables
extern EnhancedSystemState enhanced_system_state;
extern EnhancedChannelState enhanced_channel_state[MAX_FILAMENT_CHANNELS];

// =============================================================================
// Enhanced Main Loop Integration
// =============================================================================

/**
 * Enhanced main loop with reliability and performance monitoring
 * This shows how the improvements integrate with the existing main loop
 */
inline void enhanced_main_loop_cycle() {
    static uint32_t last_health_check = 0;
    static uint32_t last_performance_update = 0;
    static uint32_t last_led_update = 0;
    
    uint32_t current_time = get_time64();
    
    // Feed watchdog (critical - do this first)
    system_watchdog.feed();
    
    // Existing BambuBus communication
    BambuBus_package_type bus_status = BambuBus_run();
    
    // Enhanced error handling
    if (bus_status == BambuBus_package_type::ERROR) {
        LOG_COMM_ERROR(COMM_BAMBUBUS_TIMEOUT, "BambuBus communication timeout");
        if (AUTO_RECOVERY_ENABLED) {
            error_manager.attempt_recovery(COMM_BAMBUBUS_TIMEOUT);
        }
    }
    
    // Periodic system health check
    if (current_time - last_health_check >= SYSTEM_HEALTH_CHECK_INTERVAL_MS) {
        enhanced_system_health_check();
        last_health_check = current_time;
    }
    
    // Update performance metrics
    if (PERFORMANCE_MONITORING_ENABLED && 
        current_time - last_performance_update >= 1000) {  // Every second
        update_performance_metrics();
        last_performance_update = current_time;
    }
    
    // Update LED patterns
    if (current_time - last_led_update >= LED_PATTERN_UPDATE_INTERVAL_MS) {
        update_led_patterns();
        show_system_status_enhanced(bus_status == BambuBus_package_type::ERROR ? -1 : 0, 
                                   !error_manager.is_system_healthy());
        last_led_update = current_time;
    }
    
    // Existing motion control with enhancements
    Motion_control_run(bus_status == BambuBus_package_type::ERROR ? -1 : 0);
    
    // Validate sensors periodically
    if (SENSOR_VALIDATION_ENABLED) {
        static uint32_t last_sensor_validation = 0;
        if (current_time - last_sensor_validation >= 10000) {  // Every 10 seconds
            validate_all_sensors();
            last_sensor_validation = current_time;
        }
    }
    
    // Update error rates
    error_manager.update_error_rate();
}

// =============================================================================
// Usage Examples and Integration Points
// =============================================================================

/**
 * Example: Enhanced sensor reading with validation and error handling
 */
inline bool enhanced_read_as5600(uint8_t channel, uint16_t* angle) {
    if (channel >= MAX_FILAMENT_CHANNELS) {
        LOG_ERROR(CONFIG_INVALID_PARAMETER, channel, "Invalid channel number");
        return false;
    }
    
    // Attempt to read sensor
    bool success = read_as5600_with_validation(channel, angle);
    
    if (!success) {
        LOG_SENSOR_ERROR(channel, SENSOR_AS5600_OFFLINE, "AS5600 read failed");
        
        // Mark sensor as unhealthy
        enhanced_channel_state[channel].sensor_healthy = false;
        
        // Attempt recovery if enabled
        if (AUTO_RECOVERY_ENABLED) {
            success = error_manager.attempt_recovery(SENSOR_AS5600_OFFLINE, channel);
        }
    } else {
        // Update health status
        enhanced_channel_state[channel].sensor_healthy = true;
        enhanced_channel_state[channel].last_successful_operation = get_time64();
    }
    
    return success;
}

/**
 * Example: Enhanced filament motion with anomaly detection
 */
inline bool enhanced_filament_motion(uint8_t channel, int16_t target_position) {
    uint32_t start_time = get_time64();
    
    // Perform motion with enhanced monitoring
    bool success = motion_control_enhanced_run(channel, target_position);
    
    uint32_t operation_time = get_time64() - start_time;
    
    if (success) {
        // Log successful operation
        enhanced_channel_state[channel].successful_operations++;
        log_performance_event("FilamentMotion", operation_time);
        
        // Update wear estimates
        enhanced_channel_state[channel].operation_cycles++;
        
        // Check for anomalies
        if (detect_motion_anomalies(channel)) {
            LOG_WARNING(MOTION_SPEED_ERROR, channel, "Motion anomaly detected");
        }
    } else {
        // Log failed operation
        enhanced_channel_state[channel].failed_operations++;
        LOG_ERROR(MOTION_TIMEOUT, channel, "Filament motion failed");
        
        // Attempt recovery
        if (AUTO_RECOVERY_ENABLED) {
            error_manager.attempt_recovery(MOTION_TIMEOUT, channel);
        }
    }
    
    return success;
}

/**
 * Example: System status reporting with enhanced information
 */
inline void report_enhanced_system_status() {
    ErrorStatistics error_stats;
    WatchdogStats watchdog_stats;
    
    error_manager.get_statistics(&error_stats);
    system_watchdog.get_statistics(&watchdog_stats);
    
    DEBUG_MY("=== Enhanced System Status ===\n");
    DEBUG_MY("Uptime: %lu seconds\n", watchdog_stats.current_uptime_seconds);
    DEBUG_MY("System Health: %s\n", error_manager.is_system_healthy() ? "HEALTHY" : "DEGRADED");
    DEBUG_MY("Error Rate: %.2f errors/minute\n", error_stats.error_rate_per_minute);
    DEBUG_MY("Total Resets: %lu\n", watchdog_stats.total_resets);
    DEBUG_MY("Performance Score: %.2f\n", enhanced_system_state.system_performance_score);
    
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        DEBUG_MY("Channel %d: %s, Reliability: %.2f, Operations: %lu/%lu\n",
                i,
                enhanced_channel_state[i].online ? "ONLINE" : "OFFLINE",
                enhanced_channel_state[i].reliability_score,
                enhanced_channel_state[i].successful_operations,
                enhanced_channel_state[i].failed_operations);
    }
    DEBUG_MY("==============================\n");
}