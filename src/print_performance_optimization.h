#pragma once

/**
 * Print Performance Optimization System
 * 
 * Provides advanced print performance features including temperature-aware feeding,
 * predictive material loading, waste reduction, and intelligent multi-material handling.
 */

#include "main.h"
#include "config.h"

// Print performance optimization configuration
#define PRINT_OPTIMIZATION_ENABLED     true
#define TEMPERATURE_AWARE_FEEDING      true
#define PREDICTIVE_LOADING_ENABLED     true
#define WASTE_REDUCTION_ENABLED        true
#define FEED_RATE_OPTIMIZATION         true

// Temperature-aware feeding settings
#define TEMP_COMPENSATION_ENABLED      true
#define MIN_FEED_TEMP_C               180      // Minimum temperature for feeding
#define OPTIMAL_FEED_TEMP_C           220      // Optimal feeding temperature
#define MAX_FEED_TEMP_C               260      // Maximum safe feeding temperature
#define TEMP_FEED_RATE_FACTOR         0.8f     // Speed reduction factor for low temps

// Predictive loading settings
#define PREDICTIVE_ADVANCE_TIME_S     30       // Start loading 30 seconds before needed
#define GCODE_LOOKAHEAD_ENABLED       true     // Analyze upcoming tool changes
#define MATERIAL_CHANGE_BUFFER_TIME_S 10       // Buffer time for material changes

// Waste reduction settings
#define INTELLIGENT_PURGING_ENABLED   true
#define MIN_PURGE_LENGTH_MM           15.0f    // Minimum purge length
#define MAX_PURGE_LENGTH_MM           50.0f    // Maximum purge length
#define PURGE_FLOW_RATE_FACTOR        1.2f     // Purge flow rate multiplier

// Feed rate optimization settings
#define DYNAMIC_FEED_RATE_ENABLED     true
#define FEED_RATE_LEARNING_ENABLED    true
#define MIN_FEED_RATE_FACTOR          0.5f     // Minimum feed rate (50% of nominal)
#define MAX_FEED_RATE_FACTOR          2.0f     // Maximum feed rate (200% of nominal)
#define FEED_RATE_ADJUSTMENT_STEP     0.1f     // Adjustment step size

/**
 * Material feeding state for optimization
 */
enum class FeedingState {
    IDLE,                   // Not feeding
    PREPARING,              // Preparing for feed operation
    HEATING,                // Waiting for temperature
    FEEDING,                // Active feeding
    PURGING,                // Purging material
    RETRACTING,             // Retracting material
    OPTIMIZING              // Optimizing feed parameters
};

/**
 * Print phase detection for optimization
 */
enum class PrintPhase {
    STARTUP,                // Print starting up
    FIRST_LAYER,            // Critical first layer
    NORMAL_PRINTING,        // Normal printing operation
    MATERIAL_CHANGE,        // Material change in progress
    SUPPORT_PRINTING,       // Printing support material
    INFILL_PRINTING,        // Printing infill
    FINISHING,              // Print finishing
    IDLE                    // No active print
};

/**
 * Material compatibility data
 */
struct MaterialProfile {
    char material_name[16];
    uint16_t optimal_temp_c;
    uint16_t flow_rate_percent;
    float feeding_speed_factor;
    float retraction_speed_factor;
    uint16_t purge_length_mm;
    bool requires_heated_chamber;
    uint8_t compatibility_group;  // Materials in same group mix better
};

/**
 * Print optimization data per channel
 */
struct PrintOptimizationData {
    FeedingState feeding_state;
    PrintPhase current_phase;
    MaterialProfile material_profile;
    
    // Temperature management
    uint16_t current_temp_c;
    uint16_t target_temp_c;
    uint32_t heating_start_time;
    bool temperature_stable;
    
    // Feed rate optimization
    float current_feed_rate_factor;
    float optimal_feed_rate_factor;
    float feed_success_rate;
    uint32_t feed_attempts;
    uint32_t feed_successes;
    
    // Predictive loading
    bool predictive_load_active;
    uint32_t predicted_change_time;
    uint32_t load_start_time;
    
    // Waste reduction
    float calculated_purge_length;
    uint32_t total_waste_mm;
    uint32_t waste_reduction_savings_mm;
    
    // Performance metrics
    uint32_t total_feed_time_ms;
    uint32_t average_feed_time_ms;
    uint32_t material_change_count;
    uint32_t optimization_cycles;
};

// Global print state
struct PrintOptimizationState {
    PrintPhase global_print_phase;
    bool print_active;
    uint32_t print_start_time;
    uint32_t last_material_change_time;
    float estimated_print_time_remaining_s;
    uint8_t active_material_count;
    uint8_t primary_material_channel;
};

// Function declarations
extern void print_optimization_init();
extern void print_optimization_update();

// Temperature management functions
extern void set_target_temperature(int channel, uint16_t temp_c);
extern bool is_temperature_ready(int channel);
extern float get_temperature_compensated_speed(int channel, float base_speed);
extern void update_temperature_monitoring(int channel, uint16_t current_temp);

// Predictive loading functions
extern void enable_predictive_loading(int channel, uint32_t estimated_change_time_s);
extern void disable_predictive_loading(int channel);
extern bool should_start_predictive_load(int channel);
extern void execute_predictive_load(int channel);

// Feed rate optimization functions
extern void optimize_feed_rate(int channel, bool feed_success, uint32_t feed_time_ms);
extern float get_optimized_feed_rate(int channel);
extern void reset_feed_optimization(int channel);
extern void learn_material_profile(int channel, MaterialProfile* profile);

// Waste reduction functions
extern float calculate_optimal_purge_length(int from_channel, int to_channel);
extern void execute_intelligent_purge(int channel, float purge_length);
extern uint32_t get_waste_savings(int channel);
extern void update_waste_tracking(int channel, float waste_amount);

// Print phase detection functions
extern void set_print_phase(PrintPhase phase);
extern PrintPhase get_print_phase();
extern void detect_print_phase_from_gcode(const char* gcode_line);
extern void update_print_progress(float progress_percentage);

// Material profile management
extern void load_material_profile(int channel, const char* material_name);
extern void save_material_profile(int channel, const MaterialProfile* profile);
extern const MaterialProfile* get_material_profile(int channel);
extern void auto_detect_material_profile(int channel);

// Performance analytics functions
extern const PrintOptimizationData* get_print_optimization_data(int channel);
extern const PrintOptimizationState* get_print_optimization_state();
extern float get_optimization_efficiency_score(int channel);
extern void export_optimization_metrics(char* buffer, size_t buffer_size);
extern void reset_optimization_metrics(int channel);