#pragma once

/**
 * Smart Filament Management System
 * 
 * Provides intelligent filament monitoring, runout prediction, jam detection,
 * and quality monitoring capabilities for enhanced user experience.
 */

#include "main.h"
#include "config.h"

// Smart filament management configuration
#define RUNOUT_PREDICTION_ENABLED       true
#define JAM_DETECTION_ENABLED          true
#define QUALITY_MONITORING_ENABLED     true
#define USAGE_ANALYTICS_ENABLED        true

// Filament runout prediction settings
#define RUNOUT_PREDICTION_SAMPLES      10      // Number of usage samples for prediction
#define RUNOUT_WARNING_THRESHOLD_MM    500.0f  // Warn when less than 500mm remaining
#define RUNOUT_CRITICAL_THRESHOLD_MM   100.0f  // Critical when less than 100mm remaining
#define USAGE_CALCULATION_INTERVAL_MS  5000    // Calculate usage every 5 seconds

// Jam detection settings
#define JAM_DETECTION_TIMEOUT_MS       3000    // Timeout for movement detection
#define JAM_DETECTION_MIN_MOVEMENT_MM  0.5f    // Minimum expected movement
#define JAM_RECOVERY_ATTEMPTS          3       // Number of automatic recovery attempts
#define JAM_RETRY_INTERVAL_MS          1000    // Wait between retry attempts

// Quality monitoring settings  
#define QUALITY_SAMPLE_SIZE            20      // Number of samples for quality assessment
#define QUALITY_VARIANCE_THRESHOLD     2.0f    // Maximum acceptable variance in feeding
#define QUALITY_CHECK_INTERVAL_MS      10000   // Check quality every 10 seconds

// Usage analytics settings
#define ANALYTICS_HISTORY_SIZE         100     // Number of usage records to keep
#define MAINTENANCE_ALERT_CYCLES       1000    // Alert after 1000 cycles
#define MAINTENANCE_ALERT_HOURS        100     // Alert after 100 hours of operation

/**
 * Filament runout prediction state
 */
enum class RunoutState {
    NORMAL,         // Normal operation, plenty of filament
    WARNING,        // Low filament warning
    CRITICAL,       // Very low filament, should change soon
    EMPTY           // Filament exhausted
};

/**
 * Jam detection state
 */
enum class JamState {
    NORMAL,         // Normal feeding operation
    SUSPECTED,      // Possible jam detected
    CONFIRMED,      // Jam confirmed, attempting recovery
    RECOVERY,       // Automatic recovery in progress
    FAILED          // Recovery failed, manual intervention needed
};

/**
 * Material quality assessment
 */
enum class QualityState {
    EXCELLENT,      // High quality, consistent feeding
    GOOD,           // Good quality, minor variations
    POOR,           // Poor quality, significant issues
    DEGRADED        // Severely degraded, replacement recommended
};

/**
 * Smart filament channel data
 */
struct SmartFilamentData {
    // Runout prediction
    RunoutState runout_state;
    float predicted_remaining_mm;
    float usage_rate_mm_per_hour;
    float usage_samples[RUNOUT_PREDICTION_SAMPLES];
    uint8_t usage_sample_index;
    uint32_t last_usage_calculation_time;
    
    // Jam detection
    JamState jam_state;
    uint32_t jam_detection_start_time;
    uint8_t jam_recovery_attempts;
    float last_position_mm;
    uint32_t last_movement_time;
    
    // Quality monitoring
    QualityState quality_state;
    float feeding_variance;
    float quality_samples[QUALITY_SAMPLE_SIZE];
    uint8_t quality_sample_index;
    uint32_t last_quality_check_time;
    
    // Usage analytics
    uint32_t total_usage_mm;
    uint32_t operation_cycles;
    uint32_t operation_time_hours;
    uint32_t maintenance_alert_flags;
    uint32_t session_start_time;
};

// Function declarations
extern void smart_filament_init();
extern void smart_filament_update();

// Runout prediction functions
extern void update_runout_prediction(int channel, float current_position_mm);
extern RunoutState get_runout_state(int channel);
extern float get_predicted_remaining(int channel);
extern float get_usage_rate(int channel);

// Jam detection functions
extern void update_jam_detection(int channel, float current_position_mm, bool is_feeding);
extern JamState get_jam_state(int channel);
extern bool attempt_jam_recovery(int channel);
extern void reset_jam_detection(int channel);

// Quality monitoring functions
extern void update_quality_monitoring(int channel, float feeding_speed, float target_speed);
extern QualityState get_quality_state(int channel);
extern float get_feeding_variance(int channel);
extern bool is_quality_degraded(int channel);

// Usage analytics functions
extern void update_usage_analytics(int channel, float distance_mm);
extern uint32_t get_total_usage(int channel);
extern uint32_t get_operation_cycles(int channel);
extern bool needs_maintenance(int channel);
extern void reset_maintenance_alert(int channel);

// Diagnostic functions
extern const SmartFilamentData* get_smart_filament_data(int channel);
extern void reset_smart_filament_data(int channel);
extern void export_usage_analytics(int channel, char* buffer, size_t buffer_size);