#include <Arduino.h>
#include "main.h"

#include "BambuBus.h"
#include "Adafruit_NeoPixel.h"

extern void debug_send_run();

// RGB LED configuration - defined in config.h
// Testing configuration: Use 8 LEDs by changing LED_PA11_NUM in config.h

// RGB strip objects for channels and main board
// Channel RGB objects: strip_channel[Chx], channels 0-3 correspond to PA11/PA8/PB1/PB0
Adafruit_NeoPixel strip_channel[MAX_FILAMENT_CHANNELS] = {
    Adafruit_NeoPixel(LED_PA11_NUM, PA11, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_PA8_NUM, PA8, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_PB1_NUM, PB1, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_PB0_NUM, PB0, NEO_GRB + NEO_KHZ800)
};

// Main board 5050 RGB LED
Adafruit_NeoPixel strip_PD1(LED_PD1_NUM, PD1, NEO_GRB + NEO_KHZ800);

/**
 * Set RGB brightness for all LED strips
 * Values range from 0-255, configured in config.h
 */
void RGB_Set_Brightness() {
    // Main board brightness
    strip_PD1.setBrightness(BRIGHTNESS_MAIN_BOARD);
    
    // Set brightness for all channels
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        strip_channel[i].setBrightness(BRIGHTNESS_CHANNEL);
    }
}

/**
 * Initialize all RGB LED strips
 */
void RGB_init() {
    strip_PD1.begin();
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        strip_channel[i].begin();
    }
}

/**
 * Update all RGB LED strips with current data
 */
void RGB_show_data() {
    strip_PD1.show();
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        strip_channel[i].show();
    }
}

// Global variables for channel color storage
uint8_t channel_colors[MAX_FILAMENT_CHANNELS][4] = {
    {DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B, DEFAULT_COLOR_A},
    {DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B, DEFAULT_COLOR_A},
    {DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B, DEFAULT_COLOR_A},
    {DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B, DEFAULT_COLOR_A}
};

// Runtime RGB color storage to avoid frequent color updates causing communication failures
uint8_t channel_runs_colors[MAX_FILAMENT_CHANNELS][2][3] = {
    // R,G,B  ,, R,G,B
    {{1, 2, 3}, {1, 2, 3}}, // Channel 0
    {{3, 2, 1}, {3, 2, 1}}, // Channel 1
    {{1, 2, 3}, {1, 2, 3}}, // Channel 2
    {{3, 2, 1}, {3, 2, 1}}  // Channel 3
};

// System health monitoring variables
bool MC_STU_ERROR[4] = {false, false, false, false};
uint64_t last_health_check_time = 0;
uint64_t system_uptime_start = 0;
uint32_t watchdog_refresh_count = 0;
bool system_healthy = true;

// External communication health variables (defined in BambuBus.cpp)
extern uint64_t last_heartbeat_time;
extern uint64_t last_communication_time;

/**
 * Initialize system health monitoring and watchdog
 */
void system_health_init()
{
    system_uptime_start = millis();
    last_health_check_time = system_uptime_start;
    
    if (WATCHDOG_ENABLED) {
        // Initialize Independent Watchdog (IWDG)
        IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
        IWDG_SetPrescaler(IWDG_Prescaler_256);  // 40kHz / 256 = 156.25Hz
        IWDG_SetReload(WATCHDOG_TIMEOUT_MS * 156 / 1000);  // Convert to timer ticks
        IWDG_Enable();
        
        DEBUG_MY("Watchdog enabled with ");
        DEBUG_num("", WATCHDOG_TIMEOUT_MS);
        DEBUG_MY("ms timeout\n");
    }
}

/**
 * Refresh watchdog timer to prevent system reset
 */
void refresh_watchdog()
{
    if (WATCHDOG_ENABLED) {
        IWDG_ReloadCounter();
        watchdog_refresh_count++;
    }
}

/**
 * Perform comprehensive system health check
 */
void perform_system_health_check()
{
    uint64_t current_time = millis();
    
    if ((current_time - last_health_check_time) < SYSTEM_HEALTH_CHECK_INTERVAL_MS) {
        return;  // Not time for health check yet
    }
    
    bool previous_health = system_healthy;
    system_healthy = true;
    
    // Check ADC sensor health
    bool* adc_health = ADC_DMA_get_health_status();
    uint32_t* fault_counts = ADC_DMA_get_fault_counts();
    
    for (int i = 0; i < 8; i++) {
        if (!adc_health[i]) {
            system_healthy = false;
            DEBUG_MY("ADC channel ");
            DEBUG_num("", i);
            DEBUG_MY(" unhealthy, faults: ");
            DEBUG_num("", fault_counts[i]);
            DEBUG_MY("\n");
        }
    }
    
    // Check communication health
    uint32_t comm_errors, comm_timeouts, comm_retries;
    bool comm_healthy;
    get_communication_stats(&comm_errors, &comm_timeouts, &comm_retries, &comm_healthy);
    
    if (!comm_healthy) {
        system_healthy = false;
        DEBUG_MY("Communication unhealthy - errors: ");
        DEBUG_num("", comm_errors);
        DEBUG_MY(", timeouts: ");
        DEBUG_num("", comm_timeouts);
        DEBUG_MY(", retries: ");
        DEBUG_num("", comm_retries);
        DEBUG_MY("\n");
    }
    
    // Check voltage levels (if monitoring enabled)
    if (VOLTAGE_MONITORING_ENABLED) {
        // Use ADC to check supply voltage - typically on a dedicated channel
        // This would need hardware-specific implementation
        // For now, we'll assume voltage is OK if ADC is working
    }
    
    // Report health status change
    if (previous_health != system_healthy) {
        if (system_healthy) {
            DEBUG_MY("System health restored\n");
        } else {
            DEBUG_MY("System health degraded\n");
        }
    }
    
    // Update error flags for motion control
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        // Check channel-specific health
        int pull_adc_idx = (3 - i) * 2;
        int presence_adc_idx = pull_adc_idx + 1;
        
        bool channel_healthy = true;
        if (pull_adc_idx < 8 && !adc_health[pull_adc_idx]) channel_healthy = false;
        if (presence_adc_idx < 8 && !adc_health[presence_adc_idx]) channel_healthy = false;
        
        MC_STU_ERROR[i] = !channel_healthy;
    }
    
    last_health_check_time = current_time;
}

extern void BambuBUS_UART_Init();
extern void send_uart(const unsigned char *data, uint16_t length);

void setup()
{
    WWDG_DeInit();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, DISABLE); // Disable windowed watchdog
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);
    
    // Initialize system health monitoring and watchdog
    system_health_init();
    
    // Initialize RGB lights
    RGB_init();
    // Update RGB display
    RGB_show_data();
    // Set RGB brightness - this maintains color proportions while limiting maximum values
    RGB_Set_Brightness();

    BambuBus_init();
    DEBUG_init();
    Motion_control_init();
    
    DEBUG_MY("BMCU370 firmware initialized\n");
    DEBUG_MY("System health monitoring: ");
    DEBUG_MY(WATCHDOG_ENABLED ? "enabled" : "disabled");
    DEBUG_MY("\n");
    
    delay(1);
}

/**
 * Set RGB color for a specific channel LED with bounds checking
 * @param channel Channel number (0-3) 
 * @param num LED index within the channel
 * @param R Red component (0-255)
 * @param G Green component (0-255)
 * @param B Blue component (0-255)
 * @return true if color was set successfully, false if parameters were invalid
 */
void Set_MC_RGB(uint8_t channel, int num, uint8_t R, uint8_t G, uint8_t B)
{
    // Input validation - bounds checking
    if (channel >= MAX_FILAMENT_CHANNELS) {
        DEBUG_MY("ERROR: Invalid channel in Set_MC_RGB\n");
        return;
    }
    
    if (num < 0 || num >= 2) { // Each channel has max 2 LEDs (status and pull-online)
        DEBUG_MY("ERROR: Invalid LED num in Set_MC_RGB\n");
        return;
    }
    
    const int set_colors[3] = {R, G, B};
    bool is_new_colors = false;

    // Check if any color component has changed
    for (int colors = 0; colors < 3; colors++) {
        if (channel_runs_colors[channel][num][colors] != set_colors[colors]) {
            channel_runs_colors[channel][num][colors] = set_colors[colors]; // Record new color
            is_new_colors = true; // Color has been updated
        }
    }
    
    // Update LED only if color has changed (reduces unnecessary updates)
    if (is_new_colors) {
        strip_channel[channel].setPixelColor(num, strip_channel[channel].Color(R, G, B));
        strip_channel[channel].show(); // Display new color
    }
}

void Show_SYS_RGB(int BambuBUS_status)
{
    // Update main board RGB light
    if (BambuBUS_status == -1) // Offline
    {
        strip_PD1.setPixelColor(0, strip_PD1.Color(8, 0, 0)); // Red
        strip_PD1.show();
    }
    else if (BambuBUS_status == 0) // Online
    {
        strip_PD1.setPixelColor(0, strip_PD1.Color(8, 9, 9)); // White
        strip_PD1.show();
    }
    // Update error channels, light up red LEDs
    for (int i = 0; i < 4; i++)
    {
        if (MC_STU_ERROR[i])
        {
            // Red color
            strip_channel[i].setPixelColor(0, strip_channel[i].Color(255, 0, 0));
            strip_channel[i].show(); // Display new color
        }
    }
}

BambuBus_package_type is_first_run = BambuBus_package_type::NONE;
void loop()
{
    while (1)
    {
        // Refresh watchdog to prevent system reset
        refresh_watchdog();
        
        // Perform periodic system health checks
        perform_system_health_check();
        
        // Update communication health monitoring
        update_communication_health();
        
        BambuBus_package_type stu = BambuBus_run();
        // int stu =-1;
        static int error = 0;
        bool motion_can_run = false;
        uint16_t device_type = get_now_BambuBus_device_type();
        
        if (stu != BambuBus_package_type::NONE) // have data/offline
        {
            motion_can_run = true;
            if (stu == BambuBus_package_type::ERROR) // offline
            {
                error = -1;
                // Offline - red light
            }
            else // have data
            {
                error = 0;
                // Update heartbeat time for communication monitoring
                if (stu == BambuBus_package_type::heartbeat) {
                    last_heartbeat_time = millis();
                }
            }
            
            // Update RGB every 3 seconds (defined in config.h) with health consideration
            static unsigned long last_sys_rgb_time = 0;
            unsigned long now = get_time64();
            if (now - last_sys_rgb_time >= RGB_UPDATE_INTERVAL_MS) {
                // Modify error status based on system health
                int display_error = error;
                if (!system_healthy) {
                    // Flash different pattern for system health issues
                    display_error = -2;  // Special health error code
                }
                Show_SYS_RGB(display_error);
                last_sys_rgb_time = now;
            }
        }
        else
        {
            // No data - check for communication timeout
            static uint64_t last_no_data_check = 0;
            uint64_t current_time = millis();
            
            if ((current_time - last_no_data_check) > 1000) { // Check every second
                if ((current_time - last_communication_time) > COMMUNICATION_TIMEOUT_MS) {
                    motion_can_run = true;  // Allow motion control to handle timeout
                    error = -1;  // Communication timeout
                }
                last_no_data_check = current_time;
            }
        } // wait for data
        
        // Log output with enhanced status information
        static BambuBus_package_type is_first_run = BambuBus_package_type::NONE;
        if (is_first_run != stu)
        {
            is_first_run = stu;
            if (stu == BambuBus_package_type::ERROR)
            {                                   // offline
                DEBUG_MY("BambuBus_offline\n"); // Offline
            }
            else if (stu == BambuBus_package_type::heartbeat)
            {
                DEBUG_MY("BambuBus_online - heartbeat received\n"); // Online
            }
            else if (device_type == BambuBus_AMS_lite)
            {
                DEBUG_MY("Run_To_AMS_lite\n"); // Online as AMS Lite
            }
            else if (device_type == BambuBus_AMS)
            {
                DEBUG_MY("Run_To_AMS\n"); // Online as AMS
            }
            else
            {
                DEBUG_MY("Running Unknown ???\n");
            }
            
            // Log system health status
            DEBUG_MY("System health: ");
            DEBUG_MY(system_healthy ? "OK" : "DEGRADED");
            DEBUG_MY(", Watchdog refreshes: ");
            DEBUG_num("", watchdog_refresh_count);
            DEBUG_MY("\n");
        }

        if (motion_can_run)
        {
            Motion_control_run(error);
        }
        
        // Small delay to prevent excessive CPU usage while maintaining responsiveness
        delay_any_us(100);  // 100 microsecond delay for better responsiveness
    }
}
