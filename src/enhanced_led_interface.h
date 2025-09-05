#pragma once

/**
 * Enhanced LED Interface System
 * 
 * Provides rich visual feedback with advanced LED patterns, error codes,
 * status indication, and user-friendly visual communication.
 */

#include "main.h"
#include "config.h"
#include "smart_filament_management.h"

// Enhanced LED interface configuration
#define ENHANCED_LED_ENABLED           true
#define LED_PATTERN_UPDATE_RATE_MS     50      // Update patterns every 50ms for smooth animation
#define LED_BRIGHTNESS_LEVELS          8       // Number of brightness levels for breathing
#define ERROR_CODE_DISPLAY_DURATION_MS 5000    // Show error codes for 5 seconds
#define STATUS_PATTERN_CYCLE_MS        2000    // Complete status pattern cycle time

/**
 * LED pattern types for different states and animations
 */
enum class LEDPattern {
    SOLID,              // Solid color
    BREATHING,          // Smooth breathing animation
    PULSING,            // Fast pulsing animation
    BLINKING,           // Regular blinking
    RAINBOW,            // Rainbow color cycle
    ERROR_CODE,         // Display error code with blinks
    PROGRESS,           // Progress indication
    WARNING,            // Warning pattern
    MAINTENANCE,        // Maintenance needed pattern
    LOADING,            // Loading/feeding pattern
    JAM_RECOVERY,       // Jam recovery in progress
    QUALITY_INDICATOR,  // Material quality indication
    RUNOUT_WARNING,     // Filament runout warning
    OFF                 // LEDs off
};

/**
 * Color definitions for different states
 */
struct LEDColor {
    uint8_t r, g, b;
    
    LEDColor(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) 
        : r(red), g(green), b(blue) {}
};

// Predefined colors
extern const LEDColor COLOR_OFF;
extern const LEDColor COLOR_WHITE;
extern const LEDColor COLOR_RED;
extern const LEDColor COLOR_GREEN;
extern const LEDColor COLOR_BLUE;
extern const LEDColor COLOR_YELLOW;
extern const LEDColor COLOR_ORANGE;
extern const LEDColor COLOR_PURPLE;
extern const LEDColor COLOR_CYAN;
extern const LEDColor COLOR_PINK;

/**
 * Error code definitions for visual display
 */
enum class ErrorCode {
    NO_ERROR = 0,
    FILAMENT_JAM = 1,           // 1 blink - filament jam detected
    RUNOUT_WARNING = 2,         // 2 blinks - filament running low
    RUNOUT_CRITICAL = 3,        // 3 blinks - filament critically low
    QUALITY_POOR = 4,           // 4 blinks - poor material quality
    MAINTENANCE_NEEDED = 5,     // 5 blinks - maintenance required
    SENSOR_ERROR = 6,           // 6 blinks - sensor malfunction
    COMMUNICATION_ERROR = 7,    // 7 blinks - communication issue
    MOTOR_ERROR = 8,            // 8 blinks - motor problem
    TEMPERATURE_ERROR = 9,      // 9 blinks - temperature issue
    CALIBRATION_NEEDED = 10     // 10 blinks - calibration required
};

/**
 * LED channel state for enhanced patterns
 */
struct LEDChannelState {
    LEDPattern current_pattern;
    LEDColor primary_color;
    LEDColor secondary_color;
    ErrorCode error_code;
    uint32_t pattern_start_time;
    uint32_t last_update_time;
    uint8_t animation_step;
    uint8_t brightness_level;
    bool pattern_active;
    float progress_percentage;  // For progress indication (0.0 - 1.0)
};

/**
 * Main board LED state for system-wide status
 */
struct MainBoardLEDState {
    LEDPattern system_pattern;
    LEDColor system_color;
    ErrorCode system_error;
    uint32_t pattern_start_time;
    uint8_t animation_step;
    bool maintenance_alert;
    bool communication_active;
};

// Function declarations
extern void enhanced_led_init();
extern void enhanced_led_update();

// Channel LED control functions
extern void set_channel_pattern(int channel, LEDPattern pattern, LEDColor primary_color, LEDColor secondary_color = COLOR_OFF);
extern void set_channel_error_code(int channel, ErrorCode error);
extern void set_channel_progress(int channel, float progress_percentage);
extern void clear_channel_error(int channel);

// Main board LED control functions
extern void set_system_pattern(LEDPattern pattern, LEDColor color);
extern void set_system_error_code(ErrorCode error);
extern void set_maintenance_alert(bool active);
extern void set_communication_status(bool active);

// Smart status integration functions
extern void update_smart_status_display();
extern void display_filament_status(int channel);
extern void display_jam_status(int channel);
extern void display_runout_status(int channel);
extern void display_quality_status(int channel);
extern void display_maintenance_status(int channel);

// Pattern generation functions
extern LEDColor calculate_breathing_color(LEDColor base_color, uint32_t time_ms, uint32_t cycle_duration_ms);
extern LEDColor calculate_pulsing_color(LEDColor base_color, uint32_t time_ms, uint32_t pulse_duration_ms);
extern LEDColor calculate_rainbow_color(uint32_t time_ms, uint32_t cycle_duration_ms);
extern LEDColor calculate_progress_color(LEDColor base_color, float progress);

// Error code display functions
extern void display_error_code_pattern(int channel, ErrorCode error, uint32_t time_ms);
extern bool is_error_code_active(int channel);

// Brightness control functions
extern void set_adaptive_brightness(uint8_t level);
extern uint8_t get_current_brightness();
extern void enable_automatic_brightness(bool enable);

// Diagnostic functions
extern void test_all_led_patterns();
extern void display_channel_info(int channel);
extern const LEDChannelState* get_channel_led_state(int channel);
extern const MainBoardLEDState* get_main_board_led_state();