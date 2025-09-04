#pragma once

#include "main.h"
#include "config.h"

/**
 * Enhanced Error Management System for BMCU370
 * 
 * Provides comprehensive error logging, categorization, and automatic
 * recovery mechanisms with persistent storage and analytics.
 */

#ifdef __cplusplus
extern "C" {
#endif

// Error management configuration
#define ERROR_LOG_SIZE              32          ///< Number of error entries in circular buffer
#define ERROR_DESCRIPTION_LENGTH    48          ///< Maximum length of error descriptions
#define ERROR_RECOVERY_ATTEMPTS     3           ///< Maximum automatic recovery attempts
#define ERROR_RATE_WINDOW_MS        60000       ///< Time window for error rate calculation (1 minute)

/**
 * Error severity levels
 */
enum class ErrorSeverity : uint8_t {
    INFO = 0,       ///< Informational messages - no action needed
    WARNING = 1,    ///< Warning conditions - monitor but continue operation
    ERROR = 2,      ///< Error conditions - attempt automatic recovery
    CRITICAL = 3    ///< Critical failures - immediate intervention required
};

/**
 * Error code categories for systematic classification
 */
enum ErrorCategory : uint16_t {
    // System errors (0x0000 - 0x00FF)
    SYSTEM_WATCHDOG_TIMEOUT     = 0x0001,
    SYSTEM_MEMORY_ERROR         = 0x0002,
    SYSTEM_FLASH_ERROR          = 0x0003,
    SYSTEM_CLOCK_ERROR          = 0x0004,
    SYSTEM_POWER_ERROR          = 0x0005,
    
    // Communication errors (0x0100 - 0x01FF)
    COMM_BAMBUBUS_TIMEOUT       = 0x0101,
    COMM_BAMBUBUS_CRC_ERROR     = 0x0102,
    COMM_BAMBUBUS_PROTOCOL_ERROR = 0x0103,
    COMM_UART_OVERFLOW          = 0x0104,
    COMM_UART_FRAMING_ERROR     = 0x0105,
    
    // Sensor errors (0x0200 - 0x02FF)
    SENSOR_AS5600_OFFLINE       = 0x0201,
    SENSOR_AS5600_MAGNET_ERROR  = 0x0202,
    SENSOR_AS5600_CALIBRATION   = 0x0203,
    SENSOR_ADC_OUT_OF_RANGE     = 0x0204,
    SENSOR_ADC_NOISE            = 0x0205,
    SENSOR_PRESSURE_FAULT       = 0x0206,
    
    // Motion control errors (0x0300 - 0x03FF)
    MOTION_FILAMENT_JAM         = 0x0301,
    MOTION_MOTOR_STALL          = 0x0302,
    MOTION_POSITION_ERROR       = 0x0303,
    MOTION_SPEED_ERROR          = 0x0304,
    MOTION_TIMEOUT              = 0x0305,
    
    // Configuration errors (0x0400 - 0x04FF)
    CONFIG_INVALID_PARAMETER    = 0x0401,
    CONFIG_FLASH_CORRUPTION     = 0x0402,
    CONFIG_VERSION_MISMATCH     = 0x0403,
    CONFIG_CHECKSUM_ERROR       = 0x0404,
    
    // Hardware errors (0x0500 - 0x05FF)
    HARDWARE_LED_FAULT          = 0x0501,
    HARDWARE_GPIO_ERROR         = 0x0502,
    HARDWARE_I2C_BUS_ERROR      = 0x0503,
    HARDWARE_TEMPERATURE_ERROR  = 0x0504
};

/**
 * Error log entry structure
 */
struct ErrorEntry {
    uint32_t timestamp;                                     ///< Error occurrence time (ms)
    ErrorSeverity severity;                                 ///< Error severity level
    uint16_t error_code;                                    ///< Specific error code
    uint8_t channel;                                        ///< Associated channel (0xFF if N/A)
    uint8_t recovery_attempts;                              ///< Number of recovery attempts made
    char description[ERROR_DESCRIPTION_LENGTH];             ///< Human-readable error description
    uint32_t context_data[2];                              ///< Additional context information
};

/**
 * Error statistics structure
 */
struct ErrorStatistics {
    uint32_t total_errors;                                  ///< Total errors logged
    uint32_t info_count;                                    ///< INFO level errors
    uint32_t warning_count;                                 ///< WARNING level errors  
    uint32_t error_count;                                   ///< ERROR level errors
    uint32_t critical_count;                                ///< CRITICAL level errors
    uint32_t recovery_success_count;                        ///< Successful automatic recoveries
    uint32_t recovery_failure_count;                        ///< Failed recovery attempts
    float error_rate_per_minute;                           ///< Current error rate
    uint16_t most_frequent_error;                          ///< Most frequently occurring error code
    uint32_t uptime_since_last_critical;                   ///< Uptime since last critical error
};

/**
 * Enhanced Error Management System Class
 */
class ErrorManager {
private:
    // Circular buffer for error log
    ErrorEntry error_log[ERROR_LOG_SIZE];
    uint8_t log_write_index;
    uint8_t log_count;
    
    // Error statistics
    ErrorStatistics stats;
    uint32_t last_stats_update;
    
    // Error rate tracking
    uint32_t error_timestamps[ERROR_LOG_SIZE];
    uint8_t timestamp_index;
    
    // Recovery management
    struct RecoveryInfo {
        uint16_t error_code;
        uint8_t attempts;
        uint32_t last_attempt_time;
        bool recovery_in_progress;
    } recovery_info[8];  // Track recovery for multiple error types
    
public:
    /**
     * Initialize the error management system
     */
    void init();
    
    /**
     * Log an error event
     * @param severity Error severity level
     * @param error_code Specific error code from ErrorCategory
     * @param channel Associated channel (0xFF if not channel-specific)
     * @param description Human-readable description
     * @param context1 Additional context data (optional)
     * @param context2 Additional context data (optional)
     */
    void log_error(ErrorSeverity severity, uint16_t error_code, uint8_t channel = 0xFF,
                   const char* description = nullptr, uint32_t context1 = 0, uint32_t context2 = 0);
    
    /**
     * Attempt automatic recovery for an error
     * @param error_code Error code to recover from
     * @param channel Associated channel (0xFF if not channel-specific)
     * @return true if recovery was attempted, false if max attempts exceeded
     */
    bool attempt_recovery(uint16_t error_code, uint8_t channel = 0xFF);
    
    /**
     * Get current error statistics
     * @param stats Pointer to statistics structure to fill
     */
    void get_statistics(ErrorStatistics* stats);
    
    /**
     * Get the most recent error entries
     * @param entries Array to fill with error entries
     * @param max_entries Maximum number of entries to return
     * @return Number of entries actually returned
     */
    uint8_t get_recent_errors(ErrorEntry* entries, uint8_t max_entries);
    
    /**
     * Clear all error logs and reset statistics
     */
    void clear_error_log();
    
    /**
     * Clear only recoverable errors (keep critical errors for analysis)
     */
    void clear_recoverable_errors();
    
    /**
     * Check if system is in a healthy state
     * @return true if no critical errors and error rate is acceptable
     */
    bool is_system_healthy();
    
    /**
     * Get error count for a specific error code
     * @param error_code Error code to count
     * @return Number of occurrences of this error
     */
    uint16_t get_error_count(uint16_t error_code);
    
    /**
     * Export error log in JSON format for external analysis
     * @param buffer Buffer to write JSON data
     * @param buffer_size Size of the buffer
     * @return Number of characters written
     */
    uint16_t export_error_log_json(char* buffer, uint16_t buffer_size);
    
    /**
     * Update error rate calculation
     * Call this periodically from main loop
     */
    void update_error_rate();
    
    /**
     * Save error statistics to flash memory
     */
    void save_statistics_to_flash();
    
    /**
     * Load error statistics from flash memory
     */
    void load_statistics_from_flash();

private:
    /**
     * Update error statistics when a new error is logged
     */
    void update_statistics(ErrorSeverity severity, uint16_t error_code);
    
    /**
     * Find recovery info entry for a specific error code
     */
    RecoveryInfo* find_recovery_info(uint16_t error_code);
    
    /**
     * Perform recovery action based on error code
     */
    bool execute_recovery_action(uint16_t error_code, uint8_t channel);
    
    /**
     * Calculate current error rate
     */
    void calculate_error_rate();
    
    /**
     * Convert error severity to string
     */
    const char* severity_to_string(ErrorSeverity severity);
    
    /**
     * Convert error code to human-readable string
     */
    const char* error_code_to_string(uint16_t error_code);
};

// Global error manager instance
extern ErrorManager error_manager;

// Convenience macros for error logging
#define LOG_INFO(code, channel, desc, ...)      error_manager.log_error(ErrorSeverity::INFO, code, channel, desc, ##__VA_ARGS__)
#define LOG_WARNING(code, channel, desc, ...)   error_manager.log_error(ErrorSeverity::WARNING, code, channel, desc, ##__VA_ARGS__)
#define LOG_ERROR(code, channel, desc, ...)     error_manager.log_error(ErrorSeverity::ERROR, code, channel, desc, ##__VA_ARGS__)
#define LOG_CRITICAL(code, channel, desc, ...)  error_manager.log_error(ErrorSeverity::CRITICAL, code, channel, desc, ##__VA_ARGS__)

// Macros for specific error types
#define LOG_SENSOR_ERROR(channel, code, desc)   LOG_ERROR(code, channel, desc)
#define LOG_COMM_ERROR(code, desc)              LOG_ERROR(code, 0xFF, desc)
#define LOG_SYSTEM_ERROR(code, desc)            LOG_CRITICAL(code, 0xFF, desc)

#ifdef __cplusplus
}
#endif