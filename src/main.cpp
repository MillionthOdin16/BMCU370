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
    {{1, 2, 3}, {1, 2, 3}}, // 通道1
    {{3, 2, 1}, {3, 2, 1}}, // 通道2
    {{1, 2, 3}, {1, 2, 3}}, // 通道3
    {{3, 2, 1}, {3, 2, 1}}  // 通道4
};

extern void BambuBUS_UART_Init();
extern void send_uart(const unsigned char *data, uint16_t length);

void setup()
{
    WWDG_DeInit();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, DISABLE); // 关闭看门狗
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);
    // 初始化RGB灯
    RGB_init();
    // 更新RGB显示
    RGB_show_data();
    // 设定RGB亮度 这意味着会保持颜色的比例同时限制最大值。
    RGB_Set_Brightness();

    BambuBus_init();
    DEBUG_init();
    Motion_control_init();
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

// Channel error status flags - initialize to no errors
bool MC_STU_ERROR[MAX_FILAMENT_CHANNELS] = {false, false, false, false};
void Show_SYS_RGB(int BambuBUS_status)
{
    // 更新主板RGB灯
    if (BambuBUS_status == -1) // 离线
    {
        strip_PD1.setPixelColor(0, strip_PD1.Color(8, 0, 0)); // 红色
        strip_PD1.show();
    }
    else if (BambuBUS_status == 0) // 在线
    {
        strip_PD1.setPixelColor(0, strip_PD1.Color(8, 9, 9)); // 白色
        strip_PD1.show();
    }
    // 更新错误通道，亮起红灯
    for (int i = 0; i < 4; i++)
    {
        if (MC_STU_ERROR[i])
        {
            // 红色
            strip_channel[i].setPixelColor(0, strip_channel[i].Color(255, 0, 0));
            strip_channel[i].show(); // 显示新颜色
        }
    }
}

BambuBus_package_type is_first_run = BambuBus_package_type::NONE;
void loop()
{
    while (1)
    {
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
                // 离线-红色灯
            }
            else // have data
            {
                error = 0;
                // if (stu == BambuBus_package_type::heartbeat)
                // {
                // 正常工作-白色灯
                // Normal operation - white light
            }
            
            // Update RGB every 3 seconds (defined in config.h)
            static unsigned long last_sys_rgb_time = 0;
            unsigned long now = get_time64();
            if (now - last_sys_rgb_time >= RGB_UPDATE_INTERVAL_MS) {
                Show_SYS_RGB(error);
                last_sys_rgb_time = now;
            }
        }
        else
        {
        } // wait for data
        // log 输出
        if (is_first_run != stu)
        {
            is_first_run = stu;
            if (stu == BambuBus_package_type::ERROR)
            {                                   // offline
                DEBUG_MY("BambuBus_offline\n"); // 离线
            }
            else if (stu == BambuBus_package_type::heartbeat)
            {
                DEBUG_MY("BambuBus_online\n"); // 在线
            }
            else if (device_type == BambuBus_AMS_lite)
            {
                DEBUG_MY("Run_To_AMS_lite\n"); // 在线
            }
            else if (device_type == BambuBus_AMS)
            {
                DEBUG_MY("Run_To_AMS\n"); // 在线
            }
            else
            {
                DEBUG_MY("Running Unknown ???\n");
            }
        }

        if (motion_can_run)
        {
            Motion_control_run(error);
        }
    }
}
