#include "system_watchdog.h"
#include "Debug_log.h"
#include "Flash_saves.h"
#include "time64.h"
#include <string.h>

// Global watchdog instance
SystemWatchdog system_watchdog;

// Flash storage structure for watchdog statistics
struct alignas(4) watchdog_flash_data {
    uint32_t total_resets;
    uint32_t soft_resets;
    uint32_t peripheral_resets;
    uint32_t hard_resets;
    uint32_t factory_resets;
    uint32_t longest_uptime_seconds;
    uint32_t version;
    uint32_t checksum;
};

#define WATCHDOG_FLASH_ADDRESS  (FLASH_SAVE_ADDRESS + 0x800)  // Separate from main config
#define WATCHDOG_VERSION        1
#define WATCHDOG_CHECKSUM       0x57A7CD09

void SystemWatchdog::init() {
    last_heartbeat = get_time64();
    last_recovery_time = 0;
    recovery_attempts = 0;
    current_recovery_level = RecoveryLevel::NONE;
    watchdog_enabled = true;
    
    // Load statistics from flash
    const watchdog_flash_data* flash_data = (const watchdog_flash_data*)WATCHDOG_FLASH_ADDRESS;
    if (flash_data->version == WATCHDOG_VERSION && flash_data->checksum == WATCHDOG_CHECKSUM) {
        total_resets = flash_data->total_resets;
        soft_resets = flash_data->soft_resets;
        hard_resets = flash_data->hard_resets;
    } else {
        // Initialize statistics
        total_resets = 0;
        soft_resets = 0;
        hard_resets = 0;
    }
    
    total_uptime_seconds = 0;
    
    DEBUG_MY("SystemWatchdog: Initialized (Total resets: %lu)\n", total_resets);
}

void SystemWatchdog::feed() {
    if (!watchdog_enabled) return;
    
    last_heartbeat = get_time64();
    
    // Reset recovery level if we've been stable for a while
    if (current_recovery_level != RecoveryLevel::NONE && 
        (last_heartbeat - last_recovery_time) > RECOVERY_ESCALATION_TIME_MS) {
        current_recovery_level = RecoveryLevel::NONE;
        recovery_attempts = 0;
    }
}

bool SystemWatchdog::check_and_recover() {
    if (!watchdog_enabled) return true;
    
    uint32_t current_time = get_time64();
    uint32_t time_since_heartbeat = current_time - last_heartbeat;
    
    // Update uptime
    total_uptime_seconds = current_time / 1000;
    
    // Check for timeout
    if (time_since_heartbeat > WATCHDOG_TIMEOUT_MS) {
        DEBUG_MY("SystemWatchdog: Timeout detected (%lu ms since last heartbeat)\n", time_since_heartbeat);
        
        // Log the watchdog timeout
        log_recovery_event(current_recovery_level, "Watchdog timeout");
        
        // Escalate recovery level
        escalate_recovery();
        
        // Perform recovery based on current level
        switch (current_recovery_level) {
            case RecoveryLevel::SOFT_RESET:
                perform_soft_reset();
                break;
                
            case RecoveryLevel::PERIPHERAL_RESET:
                perform_peripheral_reset();
                break;
                
            case RecoveryLevel::HARD_RESET:
                perform_hard_reset();
                break;
                
            case RecoveryLevel::FACTORY_RESET:
                perform_factory_reset();
                break;
                
            default:
                break;
        }
        
        last_recovery_time = current_time;
        recovery_attempts++;
        total_resets++;
        
        return false; // Recovery was performed
    }
    
    // Check for warning threshold
    if (time_since_heartbeat > WATCHDOG_WARNING_MS) {
        DEBUG_MY("SystemWatchdog: Warning - %lu ms since last heartbeat\n", time_since_heartbeat);
    }
    
    return true; // System is healthy
}

void SystemWatchdog::set_enabled(bool enabled) {
    watchdog_enabled = enabled;
    if (enabled) {
        feed(); // Reset heartbeat when enabling
    }
    DEBUG_MY("SystemWatchdog: %s\n", enabled ? "Enabled" : "Disabled");
}

uint32_t SystemWatchdog::get_uptime_seconds() {
    return get_time64() / 1000;
}

void SystemWatchdog::get_statistics(WatchdogStats* stats) {
    if (!stats) return;
    
    stats->total_resets = total_resets;
    stats->soft_resets = soft_resets;
    stats->hard_resets = hard_resets;
    stats->current_uptime_seconds = get_uptime_seconds();
    stats->recovery_attempts = recovery_attempts;
    stats->last_recovery_level = current_recovery_level;
    stats->is_enabled = watchdog_enabled;
}

void SystemWatchdog::trigger_recovery(RecoveryLevel level) {
    DEBUG_MY("SystemWatchdog: Manual recovery triggered (level %d)\n", (int)level);
    
    current_recovery_level = level;
    log_recovery_event(level, "Manual trigger");
    
    switch (level) {
        case RecoveryLevel::SOFT_RESET:
            perform_soft_reset();
            break;
            
        case RecoveryLevel::PERIPHERAL_RESET:
            perform_peripheral_reset();
            break;
            
        case RecoveryLevel::HARD_RESET:
            perform_hard_reset();
            break;
            
        case RecoveryLevel::FACTORY_RESET:
            perform_factory_reset();
            break;
            
        default:
            break;
    }
    
    last_recovery_time = get_time64();
    recovery_attempts++;
}

void SystemWatchdog::reset_statistics() {
    total_resets = 0;
    soft_resets = 0;
    hard_resets = 0;
    recovery_attempts = 0;
    current_recovery_level = RecoveryLevel::NONE;
    
    // Clear flash statistics
    watchdog_flash_data clear_data = {0};
    clear_data.version = WATCHDOG_VERSION;
    clear_data.checksum = WATCHDOG_CHECKSUM;
    Flash_saves(&clear_data, sizeof(clear_data), WATCHDOG_FLASH_ADDRESS);
    
    DEBUG_MY("SystemWatchdog: Statistics reset\n");
}

void SystemWatchdog::perform_soft_reset() {
    DEBUG_MY("SystemWatchdog: Performing soft reset\n");
    
    soft_resets++;
    
    // Re-initialize critical systems without hardware reset
    extern void BambuBus_init();
    extern void Motion_control_init();
    extern void RGB_init();
    
    // Reinitialize systems
    BambuBus_init();
    Motion_control_init(); 
    RGB_init();
    
    feed(); // Reset watchdog after recovery
}

void SystemWatchdog::perform_peripheral_reset() {
    DEBUG_MY("SystemWatchdog: Performing peripheral reset\n");
    
    // Reset I2C peripherals
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
    
    // Reset UART peripherals
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);
    
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE);
    
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);
    
    // Reset ADC
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
    
    // Reinitialize after peripheral reset
    perform_soft_reset();
}

void SystemWatchdog::perform_hard_reset() {
    DEBUG_MY("SystemWatchdog: Performing hard reset\n");
    
    hard_resets++;
    
    // Save statistics before reset
    watchdog_flash_data save_data = {
        .total_resets = total_resets,
        .soft_resets = soft_resets,
        .peripheral_resets = 0, // Add this counter if needed
        .hard_resets = hard_resets,
        .factory_resets = 0, // Add this counter if needed
        .longest_uptime_seconds = total_uptime_seconds,
        .version = WATCHDOG_VERSION,
        .checksum = WATCHDOG_CHECKSUM
    };
    Flash_saves(&save_data, sizeof(save_data), WATCHDOG_FLASH_ADDRESS);
    
    // Perform system reset
    NVIC_SystemReset();
}

void SystemWatchdog::perform_factory_reset() {
    DEBUG_MY("SystemWatchdog: Performing factory reset\n");
    
    // Clear all configuration data
    extern void Bambubus_factory_reset();  // Function to add to BambuBus.cpp
    
    // Then perform hard reset
    perform_hard_reset();
}

void SystemWatchdog::escalate_recovery() {
    switch (current_recovery_level) {
        case RecoveryLevel::NONE:
            current_recovery_level = RecoveryLevel::SOFT_RESET;
            break;
            
        case RecoveryLevel::SOFT_RESET:
            if (recovery_attempts >= MAX_RECOVERY_ATTEMPTS) {
                current_recovery_level = RecoveryLevel::PERIPHERAL_RESET;
            }
            break;
            
        case RecoveryLevel::PERIPHERAL_RESET:
            if (recovery_attempts >= MAX_RECOVERY_ATTEMPTS * 2) {
                current_recovery_level = RecoveryLevel::HARD_RESET;
            }
            break;
            
        case RecoveryLevel::HARD_RESET:
            if (recovery_attempts >= MAX_RECOVERY_ATTEMPTS * 3) {
                current_recovery_level = RecoveryLevel::FACTORY_RESET;
            }
            break;
            
        case RecoveryLevel::FACTORY_RESET:
            // Already at maximum escalation level
            break;
    }
    
    DEBUG_MY("SystemWatchdog: Escalated to recovery level %d\n", (int)current_recovery_level);
}

void SystemWatchdog::log_recovery_event(RecoveryLevel level, const char* reason) {
    uint32_t current_time = get_time64();
    
    DEBUG_MY("SystemWatchdog: Recovery Event - Level: %d, Reason: %s, Time: %lu ms\n", 
             (int)level, reason, current_time);
    
    // Could also log to flash-based error log if implemented
}