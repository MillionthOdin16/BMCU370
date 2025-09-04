#pragma once

/**
 * BMCU370 Configuration Header
 * 
 * This file contains all hardware-specific constants and configuration
 * parameters for the BMCU370 firmware. Modify these values to match
 * your specific hardware setup.
 */

// =============================================================================
// Hardware Configuration
// =============================================================================

// RGB LED Configuration
#define LED_PA11_NUM    2    ///< Number of RGB LEDs on channel PA11
#define LED_PA8_NUM     2    ///< Number of RGB LEDs on channel PA8
#define LED_PB1_NUM     2    ///< Number of RGB LEDs on channel PB1
#define LED_PB0_NUM     2    ///< Number of RGB LEDs on channel PB0
#define LED_PD1_NUM     1    ///< Number of RGB LEDs on main board (PD1)

// RGB LED Brightness (0-255)
#define BRIGHTNESS_MAIN_BOARD   35   ///< Main board LED brightness
#define BRIGHTNESS_CHANNEL      15   ///< Channel LED brightness

// System Clock Configuration
#define SYSTEM_CLOCK_HZ         144000000UL  ///< System clock frequency in Hz

// =============================================================================
// Communication Configuration
// =============================================================================

#define DEBUG_UART_BAUDRATE     115200      ///< Debug UART baud rate
#define BAMBU_BUS_VERSION       5           ///< BambuBus protocol version

// =============================================================================
// Firmware Version Configuration
// =============================================================================
// 
// IMPORTANT: The AMS firmware version reported to the printer is critical for
// compatibility. Bambu Lab printers may reject AMS units with firmware versions
// that are too old or incompatible. These versions are sent in response to
// version query packets from the printer.
//
// Version format: Little-endian 4-byte array representing Major.Minor.Patch.Build
// Example: {0x31, 0x06, 0x00, 0x00} = version 00.00.06.49

// AMS (8-channel) Firmware Version - version 00.00.06.49
#define AMS_FIRMWARE_VERSION_MAJOR      0x00    ///< Major version (displayed as XX in XX.XX.XX.XX)
#define AMS_FIRMWARE_VERSION_MINOR      0x00    ///< Minor version (displayed as XX in XX.XX.XX.XX)
#define AMS_FIRMWARE_VERSION_PATCH      0x06    ///< Patch version (displayed as 06 in XX.XX.06.XX)
#define AMS_FIRMWARE_VERSION_BUILD      0x31    ///< Build version (displayed as 49 in XX.XX.XX.49)

// AMS Lite Firmware Version - version 00.01.02.03
#define AMS_LITE_FIRMWARE_VERSION_MAJOR 0x00    ///< Major version (displayed as 00 in 00.01.02.03)
#define AMS_LITE_FIRMWARE_VERSION_MINOR 0x01    ///< Minor version (displayed as 01 in 00.01.02.03)
#define AMS_LITE_FIRMWARE_VERSION_PATCH 0x02    ///< Patch version (displayed as 02 in 00.01.02.03)
#define AMS_LITE_FIRMWARE_VERSION_BUILD 0x03    ///< Build version (displayed as 03 in 00.01.02.03)

// =============================================================================
// Motion Control Configuration
// =============================================================================

// Voltage thresholds for filament detection (in Volts)
#define PULL_VOLTAGE_HIGH       1.85f       ///< High pressure threshold (red LED)
#define PULL_VOLTAGE_LOW        1.45f       ///< Low pressure threshold (blue LED)
#define PULL_VOLTAGE_SEND_MAX   1.7f        ///< Maximum voltage for sending filament

// Timing constants (in milliseconds)
#define ASSIST_SEND_TIME_MS     1200        ///< Filament send assist duration
#define RGB_UPDATE_INTERVAL_MS  3000        ///< RGB update interval for error states

// Filament distances (in millimeters)
#define P1X_OUT_FILAMENT_MM     200.0f      ///< Internal filament retraction distance
#define P1X_OUT_FILAMENT_EXT_MM 700.0f      ///< External filament retraction distance

// Speed filtering constant
#define SPEED_FILTER_K          100         ///< Speed calculation filter coefficient

// =============================================================================
// Flash Memory Configuration
// =============================================================================

#define FLASH_SAVE_ADDRESS      0x0800F000UL ///< Flash memory address for persistent data
#define FLASH_MAGIC_NUMBER      0x40614061UL ///< Magic number for flash data validation

// =============================================================================
// Sensor Configuration
// =============================================================================

// AS5600 Hall sensor I2C pins
#define AS5600_SCL_PINS         {PB15, PB14, PB13, PB12}  ///< SCL pins for each channel
#define AS5600_SDA_PINS         {PD0, PC15, PC14, PC13}   ///< SDA pins for each channel

// Mathematical constants
#define AS5600_PI               3.1415926535897932384626433832795

// =============================================================================
// Default Filament Configuration
// =============================================================================

#define DEFAULT_FILAMENT_ID     "GFG00"     ///< Default filament ID
#define DEFAULT_FILAMENT_NAME   "PETG"      ///< Default filament material name
#define DEFAULT_TEMP_MIN        220         ///< Default minimum temperature (°C)
#define DEFAULT_TEMP_MAX        240         ///< Default maximum temperature (°C)

// Default RGBA color values (0-255)
#define DEFAULT_COLOR_R         0xFF        ///< Default red component
#define DEFAULT_COLOR_G         0xFF        ///< Default green component  
#define DEFAULT_COLOR_B         0xFF        ///< Default blue component
#define DEFAULT_COLOR_A         0xFF        ///< Default alpha component

#define DEFAULT_HUMIDITY_WET    12          ///< Default humidity threshold (%)

// =============================================================================
// Channel Configuration
// =============================================================================

#define MAX_FILAMENT_CHANNELS   4           ///< Maximum number of filament channels
#define MAX_FILAMENT_NUM        4           ///< Maximum filament number (same as channels)

// Channel state definitions
#define CHANNEL_OFFLINE         0           ///< Channel offline state
#define CHANNEL_ONLINE_BOTH     1           ///< Channel online with both micro-switches
#define CHANNEL_ONLINE_OUTER    2           ///< Channel online with outer switch only
#define CHANNEL_ONLINE_INNER    3           ///< Channel online with inner switch only

// =============================================================================
// Timing Delays
// =============================================================================

/**
 * Microsecond delay calculation helper
 * @param time Delay time in microseconds
 */
#define DELAY_US_DIVISOR(time)  ((uint64_t)(8000000.0 / (time)))

/**
 * Millisecond delay calculation helper  
 * @param time Delay time in milliseconds
 */
#define DELAY_MS_DIVISOR(time)  ((uint64_t)(80000.0 / (time)))

// =============================================================================
// Error Codes
// =============================================================================

#define ERROR_NONE              0           ///< No error
#define ERROR_OFFLINE           -1          ///< Device offline error
#define ERROR_TIMEOUT           -2          ///< Communication timeout error
#define ERROR_CRC               -3          ///< CRC checksum error
#define ERROR_INVALID_DATA      -4          ///< Invalid data format error

// =============================================================================
// Device Type Identifiers
// =============================================================================

#define DEVICE_TYPE_NONE        0x0000      ///< No device detected
#define DEVICE_TYPE_AMS         0x0700      ///< AMS device type
#define DEVICE_TYPE_AMS_LITE    0x1200      ///< AMS Lite device type