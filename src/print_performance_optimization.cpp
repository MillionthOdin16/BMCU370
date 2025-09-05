#include "print_performance_optimization.h"
#include "config.h"
#include "Debug_log.h"
#include <string.h>

// Print optimization data for all channels
static PrintOptimizationData print_optimization_data[MAX_FILAMENT_CHANNELS];
static PrintOptimizationState global_print_state;

// Material profiles database
static MaterialProfile material_profiles[] = {
    {"PLA", 200, 100, 1.0f, 1.0f, 15, false, 1},
    {"PETG", 230, 100, 0.9f, 1.1f, 20, false, 2}, 
    {"ABS", 250, 100, 0.8f, 1.2f, 25, true, 3},
    {"TPU", 220, 90, 0.6f, 0.8f, 30, false, 4},
    {"PVA", 200, 110, 1.1f, 0.9f, 35, false, 5}
};

/**
 * Initialize print performance optimization system
 */
void print_optimization_init() {
    // Initialize channel data
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        PrintOptimizationData* data = &print_optimization_data[i];
        memset(data, 0, sizeof(PrintOptimizationData));
        
        data->feeding_state = FeedingState::IDLE;
        data->current_phase = PrintPhase::IDLE;
        data->current_feed_rate_factor = 1.0f;
        data->optimal_feed_rate_factor = 1.0f;
        data->feed_success_rate = 1.0f;
        data->calculated_purge_length = MIN_PURGE_LENGTH_MM;
        
        // Load default material profile (PETG)
        memcpy(&data->material_profile, &material_profiles[1], sizeof(MaterialProfile));
    }
    
    // Initialize global state
    memset(&global_print_state, 0, sizeof(PrintOptimizationState));
    global_print_state.global_print_phase = PrintPhase::IDLE;
    global_print_state.print_active = false;
    
    #ifdef Debug_log_on
    Debug_log("Print performance optimization initialized");
    #endif
}

/**
 * Main update function for print optimization
 */
void print_optimization_update() {
    uint32_t current_time = millis();
    
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        PrintOptimizationData* data = &print_optimization_data[i];
        
        // Update temperature stability
        if (data->target_temp_c > 0) {
            uint16_t temp_diff = abs((int)data->current_temp_c - (int)data->target_temp_c);
            data->temperature_stable = (temp_diff < 5); // ±5°C tolerance
        }
        
        // Check for predictive loading
        if (data->predictive_load_active && should_start_predictive_load(i)) {
            execute_predictive_load(i);
        }
        
        // Update feeding state based on conditions
        if (data->feeding_state == FeedingState::PREPARING && data->temperature_stable) {
            data->feeding_state = FeedingState::FEEDING;
        }
    }
}

/**
 * Set target temperature for a channel
 */
void set_target_temperature(int channel, uint16_t temp_c) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    data->target_temp_c = temp_c;
    data->heating_start_time = millis();
    data->temperature_stable = false;
    
    if (temp_c > 0) {
        data->feeding_state = FeedingState::HEATING;
    }
}

/**
 * Check if temperature is ready for feeding
 */
bool is_temperature_ready(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return false;
    
    return print_optimization_data[channel].temperature_stable;
}

/**
 * Get temperature compensated feeding speed
 */
float get_temperature_compensated_speed(int channel, float base_speed) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return base_speed;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    
    if (!TEMPERATURE_AWARE_FEEDING || data->current_temp_c < MIN_FEED_TEMP_C) {
        return base_speed;
    }
    
    // Reduce speed for temperatures below optimal
    if (data->current_temp_c < data->material_profile.optimal_temp_c) {
        float temp_factor = (float)data->current_temp_c / data->material_profile.optimal_temp_c;
        temp_factor = (temp_factor < TEMP_FEED_RATE_FACTOR) ? TEMP_FEED_RATE_FACTOR : temp_factor;
        return base_speed * temp_factor;
    }
    
    return base_speed;
}

/**
 * Enable predictive loading for a channel
 */
void enable_predictive_loading(int channel, uint32_t estimated_change_time_s) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    data->predictive_load_active = true;
    data->predicted_change_time = millis() + (estimated_change_time_s * 1000);
    
    #ifdef Debug_log_on
    Debug_log("Predictive loading enabled for channel %d, change in %d seconds", channel, estimated_change_time_s);
    #endif
}

/**
 * Check if predictive loading should start
 */
bool should_start_predictive_load(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return false;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    
    if (!data->predictive_load_active) return false;
    
    uint32_t advance_time_ms = PREDICTIVE_ADVANCE_TIME_S * 1000;
    return (millis() + advance_time_ms >= data->predicted_change_time);
}

/**
 * Execute predictive loading
 */
void execute_predictive_load(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    data->feeding_state = FeedingState::PREPARING;
    data->load_start_time = millis();
    
    #ifdef Debug_log_on
    Debug_log("Starting predictive load for channel %d", channel);
    #endif
    
    // Set target temperature if needed
    if (data->material_profile.optimal_temp_c > 0) {
        set_target_temperature(channel, data->material_profile.optimal_temp_c);
    }
}

/**
 * Optimize feed rate based on performance
 */
void optimize_feed_rate(int channel, bool feed_success, uint32_t feed_time_ms) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    
    data->feed_attempts++;
    if (feed_success) {
        data->feed_successes++;
        data->total_feed_time_ms += feed_time_ms;
        data->average_feed_time_ms = data->total_feed_time_ms / data->feed_successes;
    }
    
    // Calculate success rate
    data->feed_success_rate = (float)data->feed_successes / data->feed_attempts;
    
    // Adjust feed rate based on success rate
    if (data->feed_success_rate < 0.8f && data->current_feed_rate_factor > MIN_FEED_RATE_FACTOR) {
        // Reduce speed if success rate is low
        data->current_feed_rate_factor -= FEED_RATE_ADJUSTMENT_STEP;
        if (data->current_feed_rate_factor < MIN_FEED_RATE_FACTOR) {
            data->current_feed_rate_factor = MIN_FEED_RATE_FACTOR;
        }
    } else if (data->feed_success_rate > 0.95f && data->current_feed_rate_factor < MAX_FEED_RATE_FACTOR) {
        // Increase speed if success rate is high
        data->current_feed_rate_factor += FEED_RATE_ADJUSTMENT_STEP;
        if (data->current_feed_rate_factor > MAX_FEED_RATE_FACTOR) {
            data->current_feed_rate_factor = MAX_FEED_RATE_FACTOR;
        }
    }
    
    data->optimization_cycles++;
}

/**
 * Calculate optimal purge length between materials
 */
float calculate_optimal_purge_length(int from_channel, int to_channel) {
    if (from_channel < 0 || from_channel >= MAX_FILAMENT_CHANNELS ||
        to_channel < 0 || to_channel >= MAX_FILAMENT_CHANNELS) {
        return MIN_PURGE_LENGTH_MM;
    }
    
    MaterialProfile* from_material = &print_optimization_data[from_channel].material_profile;
    MaterialProfile* to_material = &print_optimization_data[to_channel].material_profile;
    
    // Base purge length
    float base_purge = MIN_PURGE_LENGTH_MM;
    
    // Increase purge length for incompatible materials
    if (from_material->compatibility_group != to_material->compatibility_group) {
        base_purge += 10.0f;
    }
    
    // Increase for temperature differences
    int temp_diff = abs((int)from_material->optimal_temp_c - (int)to_material->optimal_temp_c);
    if (temp_diff > 30) {
        base_purge += temp_diff * 0.2f;
    }
    
    // Apply material-specific purge requirements
    base_purge = (base_purge + to_material->purge_length_mm) / 2.0f;
    
    // Ensure within limits
    if (base_purge < MIN_PURGE_LENGTH_MM) base_purge = MIN_PURGE_LENGTH_MM;
    if (base_purge > MAX_PURGE_LENGTH_MM) base_purge = MAX_PURGE_LENGTH_MM;
    
    return base_purge;
}

/**
 * Load material profile for a channel
 */
void load_material_profile(int channel, const char* material_name) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS || !material_name) return;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    
    // Search for material in database
    for (size_t i = 0; i < sizeof(material_profiles) / sizeof(material_profiles[0]); i++) {
        if (strcmp(material_profiles[i].material_name, material_name) == 0) {
            memcpy(&data->material_profile, &material_profiles[i], sizeof(MaterialProfile));
            
            #ifdef Debug_log_on
            Debug_log("Loaded material profile '%s' for channel %d", material_name, channel);
            #endif
            return;
        }
    }
    
    // If not found, use default PETG profile
    memcpy(&data->material_profile, &material_profiles[1], sizeof(MaterialProfile));
    
    #ifdef Debug_log_on
    Debug_log("Material '%s' not found, using default PETG profile for channel %d", material_name, channel);
    #endif
}

/**
 * Get current feed rate for a channel
 */
float get_optimized_feed_rate(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 1.0f;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    return data->current_feed_rate_factor * data->material_profile.feeding_speed_factor;
}

/**
 * Set print phase
 */
void set_print_phase(PrintPhase phase) {
    global_print_state.global_print_phase = phase;
    global_print_state.print_active = (phase != PrintPhase::IDLE);
    
    if (phase == PrintPhase::STARTUP) {
        global_print_state.print_start_time = millis();
    }
}

/**
 * Get print phase
 */
PrintPhase get_print_phase() {
    return global_print_state.global_print_phase;
}

/**
 * Update temperature monitoring
 */
void update_temperature_monitoring(int channel, uint16_t current_temp) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    print_optimization_data[channel].current_temp_c = current_temp;
}

/**
 * Get optimization data
 */
const PrintOptimizationData* get_print_optimization_data(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return nullptr;
    return &print_optimization_data[channel];
}

/**
 * Get optimization state
 */
const PrintOptimizationState* get_print_optimization_state() {
    return &global_print_state;
}

/**
 * Calculate optimization efficiency score
 */
float get_optimization_efficiency_score(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 0.0f;
    
    PrintOptimizationData* data = &print_optimization_data[channel];
    
    if (data->optimization_cycles == 0) return 1.0f;
    
    // Combine success rate and feed rate efficiency
    float efficiency = data->feed_success_rate * 0.7f + 
                      (data->current_feed_rate_factor / MAX_FEED_RATE_FACTOR) * 0.3f;
    
    return efficiency;
}