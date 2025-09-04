#pragma once

#include "main.h"
#include "config.h"

/**
 * System Watchdog for BMCU370 Firmware
 * 
 * Provides system monitoring and automatic recovery from firmware hangs,
 * communication failures, and other critical system errors.
 */

#ifdef __cplusplus
extern "C" {
#endif

// Watchdog configuration
#define WATCHDOG_TIMEOUT_MS         5000        ///< Watchdog timeout in milliseconds
#define WATCHDOG_WARNING_MS         3000        ///< Warning threshold before timeout
#define MAX_RECOVERY_ATTEMPTS       3           ///< Maximum recovery attempts before hard reset
#define RECOVERY_ESCALATION_TIME_MS 30000       ///< Time between recovery level escalations

/**
 * Recovery levels for progressive error handling
 */
enum class RecoveryLevel {
    NONE = 0,           ///< No recovery needed
    SOFT_RESET,         ///< Software reset (restart main loop)
    PERIPHERAL_RESET,   ///< Reset peripherals (I2C, UART, etc.)
    HARD_RESET,         ///< Full system reset
    FACTORY_RESET       ///< Factory reset with configuration clear
};

/**
 * System watchdog class for monitoring and recovery
 */
class SystemWatchdog {
private:
    uint32_t last_heartbeat;
    uint32_t last_recovery_time;
    uint8_t recovery_attempts;
    RecoveryLevel current_recovery_level;
    bool watchdog_enabled;
    
    // Statistics
    uint32_t total_resets;
    uint32_t soft_resets;
    uint32_t hard_resets;
    uint32_t total_uptime_seconds;
    
public:
    /**
     * Initialize the watchdog system
     */
    void init();
    
    /**
     * Feed the watchdog to prevent timeout
     * Call this regularly from main loop
     */
    void feed();
    
    /**
     * Check watchdog status and perform recovery if needed
     * @return true if system is healthy, false if recovery was performed
     */
    bool check_and_recover();
    
    /**
     * Enable or disable watchdog monitoring
     * @param enabled Whether watchdog should be active
     */
    void set_enabled(bool enabled);
    
    /**
     * Get current system uptime
     * @return Uptime in seconds since last reset
     */
    uint32_t get_uptime_seconds();
    
    /**
     * Get watchdog statistics
     * @param stats Pointer to structure to fill with statistics
     */
    void get_statistics(struct WatchdogStats* stats);
    
    /**
     * Manually trigger a specific recovery level
     * @param level Recovery level to execute
     */
    void trigger_recovery(RecoveryLevel level);
    
    /**
     * Reset watchdog statistics and counters
     */
    void reset_statistics();

private:
    /**
     * Perform soft reset (restart main systems)
     */
    void perform_soft_reset();
    
    /**
     * Reset all peripherals
     */
    void perform_peripheral_reset();
    
    /**
     * Perform hard system reset
     */
    void perform_hard_reset();
    
    /**
     * Perform factory reset (clear all configuration)
     */
    void perform_factory_reset();
    
    /**
     * Escalate to next recovery level
     */
    void escalate_recovery();
    
    /**
     * Log recovery event for debugging
     */
    void log_recovery_event(RecoveryLevel level, const char* reason);
};

/**
 * Statistics structure for watchdog monitoring
 */
struct WatchdogStats {
    uint32_t total_resets;
    uint32_t soft_resets;
    uint32_t peripheral_resets;
    uint32_t hard_resets;
    uint32_t factory_resets;
    uint32_t current_uptime_seconds;
    uint32_t longest_uptime_seconds;
    uint32_t recovery_attempts;
    RecoveryLevel last_recovery_level;
    bool is_enabled;
};

// Global watchdog instance
extern SystemWatchdog system_watchdog;

#ifdef __cplusplus
}
#endif