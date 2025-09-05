#include "smart_filament_management.h"
#include "config.h"
#include "Debug_log.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Smart filament data for all channels
static SmartFilamentData smart_filament_data[MAX_FILAMENT_CHANNELS];

/**
 * Initialize smart filament management system
 */
void smart_filament_init() {
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        reset_smart_filament_data(i);
    }
    
    #ifdef Debug_log_on
    Debug_log("Smart filament management initialized");
    #endif
}

/**
 * Main update function - call this regularly from main loop
 */
void smart_filament_update() {
    uint32_t current_time = millis();
    
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        SmartFilamentData* data = &smart_filament_data[i];
        
        // Update usage rate calculation
        if (current_time - data->last_usage_calculation_time >= USAGE_CALCULATION_INTERVAL_MS) {
            // Calculate usage rate based on recent samples
            float total_usage = 0;
            for (int j = 0; j < RUNOUT_PREDICTION_SAMPLES; j++) {
                total_usage += data->usage_samples[j];
            }
            
            if (total_usage > 0) {
                // Convert to mm per hour
                float sample_period_hours = (USAGE_CALCULATION_INTERVAL_MS * RUNOUT_PREDICTION_SAMPLES) / 3600000.0f;
                data->usage_rate_mm_per_hour = total_usage / sample_period_hours;
                
                // Update predicted remaining
                if (data->usage_rate_mm_per_hour > 0) {
                    data->predicted_remaining_mm = data->total_usage_mm / data->usage_rate_mm_per_hour;
                }
            }
            
            data->last_usage_calculation_time = current_time;
        }
        
        // Update quality check
        if (current_time - data->last_quality_check_time >= QUALITY_CHECK_INTERVAL_MS) {
            // Calculate feeding variance
            float mean = 0;
            for (int j = 0; j < QUALITY_SAMPLE_SIZE; j++) {
                mean += data->quality_samples[j];
            }
            mean /= QUALITY_SAMPLE_SIZE;
            
            float variance = 0;
            for (int j = 0; j < QUALITY_SAMPLE_SIZE; j++) {
                float diff = data->quality_samples[j] - mean;
                variance += diff * diff;
            }
            variance /= QUALITY_SAMPLE_SIZE;
            data->feeding_variance = sqrt(variance);
            
            // Update quality state
            if (data->feeding_variance < QUALITY_VARIANCE_THRESHOLD * 0.5f) {
                data->quality_state = QualityState::EXCELLENT;
            } else if (data->feeding_variance < QUALITY_VARIANCE_THRESHOLD) {
                data->quality_state = QualityState::GOOD;
            } else if (data->feeding_variance < QUALITY_VARIANCE_THRESHOLD * 2.0f) {
                data->quality_state = QualityState::POOR;
            } else {
                data->quality_state = QualityState::DEGRADED;
            }
            
            data->last_quality_check_time = current_time;
        }
        
        // Check for maintenance alerts
        if (data->operation_cycles >= MAINTENANCE_ALERT_CYCLES ||
            data->operation_time_hours >= MAINTENANCE_ALERT_HOURS) {
            data->maintenance_alert_flags |= 0x01; // Set maintenance needed flag
        }
    }
}

/**
 * Update runout prediction for a specific channel
 */
void update_runout_prediction(int channel, float current_position_mm) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    SmartFilamentData* data = &smart_filament_data[channel];
    
    // Calculate usage since last update
    static float last_positions[MAX_FILAMENT_CHANNELS] = {0};
    float usage_delta = fabsf(current_position_mm - last_positions[channel]);
    last_positions[channel] = current_position_mm;
    
    // Add to usage samples
    data->usage_samples[data->usage_sample_index] = usage_delta;
    data->usage_sample_index = (data->usage_sample_index + 1) % RUNOUT_PREDICTION_SAMPLES;
    
    // Update total usage
    data->total_usage_mm += usage_delta;
    
    // Determine runout state based on predicted remaining
    if (data->predicted_remaining_mm <= RUNOUT_CRITICAL_THRESHOLD_MM) {
        data->runout_state = RunoutState::CRITICAL;
    } else if (data->predicted_remaining_mm <= RUNOUT_WARNING_THRESHOLD_MM) {
        data->runout_state = RunoutState::WARNING;
    } else {
        data->runout_state = RunoutState::NORMAL;
    }
}

/**
 * Update jam detection for a specific channel
 */
void update_jam_detection(int channel, float current_position_mm, bool is_feeding) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    SmartFilamentData* data = &smart_filament_data[channel];
    uint32_t current_time = millis();
    
    if (is_feeding) {
        float movement = fabsf(current_position_mm - data->last_position_mm);
        
        if (movement >= JAM_DETECTION_MIN_MOVEMENT_MM) {
            // Normal movement detected
            data->last_movement_time = current_time;
            data->jam_state = JamState::NORMAL;
            data->jam_recovery_attempts = 0;
        } else if (current_time - data->last_movement_time >= JAM_DETECTION_TIMEOUT_MS) {
            // No movement for too long - possible jam
            if (data->jam_state == JamState::NORMAL) {
                data->jam_state = JamState::SUSPECTED;
                data->jam_detection_start_time = current_time;
                
                #ifdef Debug_log_on
                Debug_log("Jam suspected on channel %d", channel);
                #endif
            } else if (data->jam_state == JamState::SUSPECTED && 
                      current_time - data->jam_detection_start_time >= JAM_DETECTION_TIMEOUT_MS) {
                data->jam_state = JamState::CONFIRMED;
                
                #ifdef Debug_log_on
                Debug_log("Jam confirmed on channel %d", channel);
                #endif
            }
        }
        
        data->last_position_mm = current_position_mm;
    } else {
        // Not feeding - reset jam detection
        data->last_movement_time = current_time;
        if (data->jam_state != JamState::RECOVERY) {
            data->jam_state = JamState::NORMAL;
        }
    }
}

/**
 * Attempt automatic jam recovery
 */
bool attempt_jam_recovery(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return false;
    
    SmartFilamentData* data = &smart_filament_data[channel];
    
    if (data->jam_recovery_attempts >= JAM_RECOVERY_ATTEMPTS) {
        data->jam_state = JamState::FAILED;
        
        #ifdef Debug_log_on
        Debug_log("Jam recovery failed on channel %d after %d attempts", channel, data->jam_recovery_attempts);
        #endif
        
        return false;
    }
    
    data->jam_state = JamState::RECOVERY;
    data->jam_recovery_attempts++;
    
    #ifdef Debug_log_on
    Debug_log("Attempting jam recovery on channel %d (attempt %d)", channel, data->jam_recovery_attempts);
    #endif
    
    // Recovery strategy: short reverse then forward movement
    // This would integrate with existing motion control system
    // For now, just set the state and let the motion control handle it
    
    return true;
}

/**
 * Update quality monitoring
 */
void update_quality_monitoring(int channel, float feeding_speed, float target_speed) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    SmartFilamentData* data = &smart_filament_data[channel];
    
    // Calculate feeding accuracy as percentage of target
    float accuracy = (target_speed > 0) ? (feeding_speed / target_speed) : 1.0f;
    
    // Add to quality samples
    data->quality_samples[data->quality_sample_index] = accuracy;
    data->quality_sample_index = (data->quality_sample_index + 1) % QUALITY_SAMPLE_SIZE;
}

/**
 * Update usage analytics
 */
void update_usage_analytics(int channel, float distance_mm) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    SmartFilamentData* data = &smart_filament_data[channel];
    
    data->total_usage_mm += (uint32_t)distance_mm;
    
    // Update operation time
    uint32_t current_time = millis();
    if (data->session_start_time > 0) {
        uint32_t session_time_ms = current_time - data->session_start_time;
        data->operation_time_hours += session_time_ms / 3600000; // Convert to hours
    }
    data->session_start_time = current_time;
}

/**
 * Get functions for accessing smart filament data
 */
RunoutState get_runout_state(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return RunoutState::NORMAL;
    return smart_filament_data[channel].runout_state;
}

float get_predicted_remaining(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 0;
    return smart_filament_data[channel].predicted_remaining_mm;
}

float get_usage_rate(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 0;
    return smart_filament_data[channel].usage_rate_mm_per_hour;
}

JamState get_jam_state(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return JamState::NORMAL;
    return smart_filament_data[channel].jam_state;
}

QualityState get_quality_state(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return QualityState::EXCELLENT;
    return smart_filament_data[channel].quality_state;
}

float get_feeding_variance(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 0;
    return smart_filament_data[channel].feeding_variance;
}

bool is_quality_degraded(int channel) {
    return get_quality_state(channel) == QualityState::DEGRADED;
}

uint32_t get_total_usage(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 0;
    return smart_filament_data[channel].total_usage_mm;
}

uint32_t get_operation_cycles(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return 0;
    return smart_filament_data[channel].operation_cycles;
}

bool needs_maintenance(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return false;
    return smart_filament_data[channel].maintenance_alert_flags != 0;
}

void reset_maintenance_alert(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    smart_filament_data[channel].maintenance_alert_flags = 0;
    smart_filament_data[channel].operation_cycles = 0;
    smart_filament_data[channel].operation_time_hours = 0;
}

void reset_jam_detection(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    SmartFilamentData* data = &smart_filament_data[channel];
    data->jam_state = JamState::NORMAL;
    data->jam_recovery_attempts = 0;
    data->last_movement_time = millis();
}

const SmartFilamentData* get_smart_filament_data(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return nullptr;
    return &smart_filament_data[channel];
}

void reset_smart_filament_data(int channel) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    SmartFilamentData* data = &smart_filament_data[channel];
    memset(data, 0, sizeof(SmartFilamentData));
    
    // Set initial states
    data->runout_state = RunoutState::NORMAL;
    data->jam_state = JamState::NORMAL;
    data->quality_state = QualityState::EXCELLENT;
    data->session_start_time = millis();
}

void export_usage_analytics(int channel, char* buffer, size_t buffer_size) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS || !buffer) return;
    
    const SmartFilamentData* data = &smart_filament_data[channel];
    
    snprintf(buffer, buffer_size,
        "Ch%d: Usage=%lumm, Rate=%.1fmm/h, Cycles=%lu, Time=%luh, Quality=%.2f, Maint=%s",
        channel,
        data->total_usage_mm,
        data->usage_rate_mm_per_hour,
        data->operation_cycles,
        data->operation_time_hours,
        data->feeding_variance,
        (data->maintenance_alert_flags ? "YES" : "NO")
    );
}