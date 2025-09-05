#include "enhanced_led_interface.h"
#include "Adafruit_NeoPixel.h"
#include "Debug_log.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// External LED strip objects from main.cpp
extern Adafruit_NeoPixel strip_channel[MAX_FILAMENT_CHANNELS];
extern Adafruit_NeoPixel strip_PD1;

// Predefined colors
const LEDColor COLOR_OFF(0, 0, 0);
const LEDColor COLOR_WHITE(255, 255, 255);
const LEDColor COLOR_RED(255, 0, 0);
const LEDColor COLOR_GREEN(0, 255, 0);
const LEDColor COLOR_BLUE(0, 0, 255);
const LEDColor COLOR_YELLOW(255, 255, 0);
const LEDColor COLOR_ORANGE(255, 165, 0);
const LEDColor COLOR_PURPLE(128, 0, 128);
const LEDColor COLOR_CYAN(0, 255, 255);
const LEDColor COLOR_PINK(255, 192, 203);

// LED state arrays
static LEDChannelState channel_states[MAX_FILAMENT_CHANNELS];
static MainBoardLEDState main_board_state;

// Global settings
static uint8_t global_brightness = 255;
static bool automatic_brightness_enabled = false;

// Forward declarations for static functions
static void update_channel_pattern(int channel, uint32_t current_time);
static void update_main_board_pattern(uint32_t current_time);

/**
 * Initialize enhanced LED interface system
 */
void enhanced_led_init() {
    // Initialize channel states
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        channel_states[i].current_pattern = LEDPattern::OFF;
        channel_states[i].primary_color = COLOR_OFF;
        channel_states[i].secondary_color = COLOR_OFF;
        channel_states[i].error_code = ErrorCode::NO_ERROR;
        channel_states[i].pattern_start_time = millis();
        channel_states[i].last_update_time = 0;
        channel_states[i].animation_step = 0;
        channel_states[i].brightness_level = 255;
        channel_states[i].pattern_active = false;
        channel_states[i].progress_percentage = 0.0f;
    }
    
    // Initialize main board state
    main_board_state.system_pattern = LEDPattern::BREATHING;
    main_board_state.system_color = COLOR_WHITE;
    main_board_state.system_error = ErrorCode::NO_ERROR;
    main_board_state.pattern_start_time = millis();
    main_board_state.animation_step = 0;
    main_board_state.maintenance_alert = false;
    main_board_state.communication_active = false;
    
    #ifdef Debug_log_on
    Debug_log("Enhanced LED interface initialized");
    #endif
}

/**
 * Main LED update function - call this regularly
 */
void enhanced_led_update() {
    uint32_t current_time = millis();
    
    // Update channel LEDs
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        if (current_time - channel_states[i].last_update_time >= LED_PATTERN_UPDATE_RATE_MS) {
            update_channel_pattern(i, current_time);
            channel_states[i].last_update_time = current_time;
        }
    }
    
    // Update main board LED
    update_main_board_pattern(current_time);
    
    // Apply changes to hardware
    RGB_show_data();
}

/**
 * Update pattern for a specific channel
 */
static void update_channel_pattern(int channel, uint32_t current_time) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    LEDChannelState* state = &channel_states[channel];
    uint32_t elapsed_time = current_time - state->pattern_start_time;
    LEDColor color = COLOR_OFF;
    
    switch (state->current_pattern) {
        case LEDPattern::SOLID:
            color = state->primary_color;
            break;
            
        case LEDPattern::BREATHING:
            color = calculate_breathing_color(state->primary_color, elapsed_time, STATUS_PATTERN_CYCLE_MS);
            break;
            
        case LEDPattern::PULSING:
            color = calculate_pulsing_color(state->primary_color, elapsed_time, 500);
            break;
            
        case LEDPattern::BLINKING:
            color = ((elapsed_time / 500) % 2 == 0) ? state->primary_color : COLOR_OFF;
            break;
            
        case LEDPattern::RAINBOW:
            color = calculate_rainbow_color(elapsed_time, STATUS_PATTERN_CYCLE_MS);
            break;
            
        case LEDPattern::ERROR_CODE:
            display_error_code_pattern(channel, state->error_code, elapsed_time);
            return; // Error code handling manages its own colors
            
        case LEDPattern::PROGRESS:
            color = calculate_progress_color(state->primary_color, state->progress_percentage);
            break;
            
        case LEDPattern::WARNING:
            // Alternating orange and red for warnings
            color = ((elapsed_time / 300) % 2 == 0) ? COLOR_ORANGE : COLOR_RED;
            break;
            
        case LEDPattern::MAINTENANCE:
            // Purple breathing for maintenance
            color = calculate_breathing_color(COLOR_PURPLE, elapsed_time, STATUS_PATTERN_CYCLE_MS);
            break;
            
        case LEDPattern::LOADING:
            // Rotating blue pattern for loading
            {
                uint8_t step = (elapsed_time / 100) % strip_channel[channel].numPixels();
                for (int i = 0; i < strip_channel[channel].numPixels(); i++) {
                    if (i == step) {
                        strip_channel[channel].setPixelColor(i, COLOR_BLUE.r, COLOR_BLUE.g, COLOR_BLUE.b);
                    } else {
                        strip_channel[channel].setPixelColor(i, 0, 0, 0);
                    }
                }
                return; // Custom handling for loading pattern
            }
            
        case LEDPattern::JAM_RECOVERY:
            // Fast red pulsing for jam recovery
            color = calculate_pulsing_color(COLOR_RED, elapsed_time, 200);
            break;
            
        case LEDPattern::QUALITY_INDICATOR:
            // Color based on quality: Green=good, Yellow=fair, Red=poor
            if (state->progress_percentage > 0.8f) {
                color = COLOR_GREEN;
            } else if (state->progress_percentage > 0.5f) {
                color = COLOR_YELLOW;
            } else {
                color = COLOR_RED;
            }
            break;
            
        case LEDPattern::RUNOUT_WARNING:
            // Alternating yellow and orange for runout warning
            color = ((elapsed_time / 400) % 2 == 0) ? COLOR_YELLOW : COLOR_ORANGE;
            break;
            
        case LEDPattern::OFF:
        default:
            color = COLOR_OFF;
            break;
    }
    
    // Apply brightness adjustment
    uint8_t brightness = (global_brightness * state->brightness_level) / 255;
    color.r = (color.r * brightness) / 255;
    color.g = (color.g * brightness) / 255;
    color.b = (color.b * brightness) / 255;
    
    // Set all pixels in the channel to the calculated color
    for (int i = 0; i < strip_channel[channel].numPixels(); i++) {
        strip_channel[channel].setPixelColor(i, color.r, color.g, color.b);
    }
}

/**
 * Update main board LED pattern
 */
static void update_main_board_pattern(uint32_t current_time) {
    uint32_t elapsed_time = current_time - main_board_state.pattern_start_time;
    LEDColor color = COLOR_OFF;
    
    // Handle system errors first
    if (main_board_state.system_error != ErrorCode::NO_ERROR) {
        display_error_code_pattern(-1, main_board_state.system_error, elapsed_time);
        return;
    }
    
    // Handle maintenance alert
    if (main_board_state.maintenance_alert) {
        color = calculate_breathing_color(COLOR_PURPLE, elapsed_time, STATUS_PATTERN_CYCLE_MS);
    }
    // Handle communication status
    else if (main_board_state.communication_active) {
        color = calculate_breathing_color(COLOR_GREEN, elapsed_time, STATUS_PATTERN_CYCLE_MS);
    }
    // Default system pattern
    else {
        switch (main_board_state.system_pattern) {
            case LEDPattern::BREATHING:
                color = calculate_breathing_color(main_board_state.system_color, elapsed_time, STATUS_PATTERN_CYCLE_MS);
                break;
            case LEDPattern::SOLID:
                color = main_board_state.system_color;
                break;
            default:
                color = COLOR_WHITE;
                break;
        }
    }
    
    // Apply brightness
    uint8_t brightness = global_brightness;
    color.r = (color.r * brightness) / 255;
    color.g = (color.g * brightness) / 255;
    color.b = (color.b * brightness) / 255;
    
    // Set main board LED
    strip_PD1.setPixelColor(0, color.r, color.g, color.b);
}

/**
 * Calculate breathing pattern color
 */
LEDColor calculate_breathing_color(LEDColor base_color, uint32_t time_ms, uint32_t cycle_duration_ms) {
    float phase = (2.0f * M_PI * time_ms) / cycle_duration_ms;
    float brightness_factor = (sin(phase) + 1.0f) / 2.0f; // 0.0 to 1.0
    
    return LEDColor(
        (uint8_t)(base_color.r * brightness_factor),
        (uint8_t)(base_color.g * brightness_factor),
        (uint8_t)(base_color.b * brightness_factor)
    );
}

/**
 * Calculate pulsing pattern color
 */
LEDColor calculate_pulsing_color(LEDColor base_color, uint32_t time_ms, uint32_t pulse_duration_ms) {
    uint32_t pulse_position = time_ms % pulse_duration_ms;
    float brightness_factor = (pulse_position < pulse_duration_ms / 2) ? 1.0f : 0.0f;
    
    return LEDColor(
        (uint8_t)(base_color.r * brightness_factor),
        (uint8_t)(base_color.g * brightness_factor),
        (uint8_t)(base_color.b * brightness_factor)
    );
}

/**
 * Calculate rainbow color
 */
LEDColor calculate_rainbow_color(uint32_t time_ms, uint32_t cycle_duration_ms) {
    float hue = (360.0f * time_ms) / cycle_duration_ms;
    
    // Simple HSV to RGB conversion
    float c = 1.0f;  // Chroma
    float x = c * (1 - fabsf(fmodf(hue / 60.0f, 2) - 1));
    float m = 0;
    
    float r, g, b;
    if (hue < 60) {
        r = c; g = x; b = 0;
    } else if (hue < 120) {
        r = x; g = c; b = 0;
    } else if (hue < 180) {
        r = 0; g = c; b = x;
    } else if (hue < 240) {
        r = 0; g = x; b = c;
    } else if (hue < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return LEDColor(
        (uint8_t)((r + m) * 255),
        (uint8_t)((g + m) * 255),
        (uint8_t)((b + m) * 255)
    );
}

/**
 * Display error code pattern
 */
void display_error_code_pattern(int channel, ErrorCode error, uint32_t time_ms) {
    if (error == ErrorCode::NO_ERROR) return;
    
    uint32_t blink_cycle = 500; // 500ms per blink
    uint32_t pause_duration = 2000; // 2 second pause between sequences
    uint32_t total_cycle = ((uint32_t)error * blink_cycle * 2) + pause_duration;
    
    uint32_t position_in_cycle = time_ms % total_cycle;
    
    LEDColor color = COLOR_OFF;
    if (position_in_cycle < (uint32_t)error * blink_cycle * 2) {
        // In blink sequence
        uint32_t blink_position = position_in_cycle % (blink_cycle * 2);
        if (blink_position < blink_cycle) {
            color = COLOR_RED; // Error codes are displayed in red
        }
    }
    
    // Apply to channel or main board
    if (channel >= 0 && channel < MAX_FILAMENT_CHANNELS) {
        for (int i = 0; i < strip_channel[channel].numPixels(); i++) {
            strip_channel[channel].setPixelColor(i, color.r, color.g, color.b);
        }
    } else if (channel == -1) {
        strip_PD1.setPixelColor(0, color.r, color.g, color.b);
    }
}

/**
 * Public interface functions
 */
void set_channel_pattern(int channel, LEDPattern pattern, LEDColor primary_color, LEDColor secondary_color) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    channel_states[channel].current_pattern = pattern;
    channel_states[channel].primary_color = primary_color;
    channel_states[channel].secondary_color = secondary_color;
    channel_states[channel].pattern_start_time = millis();
    channel_states[channel].pattern_active = true;
}

void set_channel_error_code(int channel, ErrorCode error) {
    if (channel < 0 || channel >= MAX_FILAMENT_CHANNELS) return;
    
    channel_states[channel].error_code = error;
    if (error != ErrorCode::NO_ERROR) {
        channel_states[channel].current_pattern = LEDPattern::ERROR_CODE;
        channel_states[channel].pattern_start_time = millis();
    }
}

void set_system_pattern(LEDPattern pattern, LEDColor color) {
    main_board_state.system_pattern = pattern;
    main_board_state.system_color = color;
    main_board_state.pattern_start_time = millis();
}

void set_system_error_code(ErrorCode error) {
    main_board_state.system_error = error;
    main_board_state.pattern_start_time = millis();
}

void set_maintenance_alert(bool active) {
    main_board_state.maintenance_alert = active;
}

void set_communication_status(bool active) {
    main_board_state.communication_active = active;
}

void update_smart_status_display() {
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        display_filament_status(i);
    }
}

void display_filament_status(int channel) {
    // Integrate with smart filament management
    RunoutState runout = get_runout_state(channel);
    JamState jam = get_jam_state(channel);
    QualityState quality = get_quality_state(channel);
    
    // Priority: Jam > Runout > Quality > Normal
    if (jam == JamState::CONFIRMED || jam == JamState::RECOVERY) {
        set_channel_pattern(channel, LEDPattern::JAM_RECOVERY, COLOR_RED);
    } else if (runout == RunoutState::CRITICAL) {
        set_channel_error_code(channel, ErrorCode::RUNOUT_CRITICAL);
    } else if (runout == RunoutState::WARNING) {
        set_channel_pattern(channel, LEDPattern::RUNOUT_WARNING, COLOR_YELLOW);
    } else if (quality == QualityState::DEGRADED) {
        set_channel_error_code(channel, ErrorCode::QUALITY_POOR);
    } else {
        // Normal operation - show based on activity
        set_channel_pattern(channel, LEDPattern::BREATHING, COLOR_GREEN);
    }
}

void set_adaptive_brightness(uint8_t level) {
    global_brightness = level;
}

uint8_t get_current_brightness() {
    return global_brightness;
}