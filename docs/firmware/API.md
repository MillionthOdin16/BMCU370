# BMCU370 API Documentation

This document provides detailed API documentation for the BMCU370 firmware modules.

## Table of Contents

1. [BambuBus Module](#bambubus-module)
2. [Motion Control Module](#motion-control-module)
3. [RGB LED Control](#rgb-led-control)
4. [Debug and Logging](#debug-and-logging)
5. [Flash Storage](#flash-storage)
6. [Sensor Interface](#sensor-interface)

---

## Firmware Version Management

### Overview

The BMCU370 firmware version is critical for compatibility with Bambu Lab printers. When the printer queries the AMS for its firmware version, the response determines whether the printer will accept the AMS as a valid device. Incorrect or outdated firmware versions may cause the printer to reject the AMS entirely.

### Version Configuration

Firmware versions are configured in `src/config.h` using the following constants:

#### AMS (8-channel) Version
```c
#define AMS_FIRMWARE_VERSION_MAJOR      0x00    // Major version
#define AMS_FIRMWARE_VERSION_MINOR      0x00    // Minor version  
#define AMS_FIRMWARE_VERSION_PATCH      0x06    // Patch version
#define AMS_FIRMWARE_VERSION_BUILD      0x31    // Build version
```
This creates version **00.00.06.49** (displayed as 00.00.06.49 in printer interface).

#### AMS Lite Version
```c
#define AMS_LITE_FIRMWARE_VERSION_MAJOR 0x00    // Major version
#define AMS_LITE_FIRMWARE_VERSION_MINOR 0x01    // Minor version
#define AMS_LITE_FIRMWARE_VERSION_PATCH 0x02    // Patch version  
#define AMS_LITE_FIRMWARE_VERSION_BUILD 0x03    // Build version
```
This creates version **00.01.02.03** (displayed as 00.01.02.03 in printer interface).

### Version Format

Versions are transmitted as 4-byte little-endian arrays:
- **Byte 0**: Build version (LSB)
- **Byte 1**: Patch version  
- **Byte 2**: Minor version
- **Byte 3**: Major version (MSB)

### Important Notes

1. **Compatibility**: These versions were determined through reverse engineering and testing with actual Bambu Lab printers
2. **Updates**: When updating versions, ensure compatibility testing with target printer firmware
3. **Device Types**: Different AMS types (AMS08 vs AMS Lite) report different versions and hardware identifiers
4. **Protocol Response**: Versions are sent in response to BambuBus packet type `0x103` (version query)

### Changing Firmware Versions

To modify reported firmware versions:

1. Edit the version constants in `src/config.h`
2. Rebuild and flash the firmware
3. Test compatibility with target printer firmware versions

**Warning**: Incorrect firmware versions may cause printer compatibility issues.

---

## BambuBus Module

The BambuBus module handles communication with Bambu Lab printers using a proprietary serial protocol based on UART through RS485 bus.

### Protocol Specifications

The BambuBus protocol uses UART communication with the following parameters:
- **Baud Rate**: 1,228,800 bps
- **Data Bits**: 8
- **Parity**: 1 even parity bit
- **Stop Bits**: 1

#### Packet Formats

The protocol supports two packet formats for different communication scenarios:

##### Long Header Packet Format

Used for communication between multiple devices with full addressing:

| Byte | Content | Description |
|------|---------|-------------|
| 0 | 0x3D | Start byte |
| 1 | Flag | Less than 0x80 |
| 2-3 | Sequence | Packet sequence number |
| 4-5 | Length (L) | Total packet length |
| 6 | CRC8 | Header integrity check |
| 7-8 | Target | Target device address |
| 9-10 | Source | Source device address |
| 11-(L-3) | Data | Packet payload |
| (L-2)-(L-1) | CRC16 | Data integrity check |

##### Short Header Packet Format

Used for preset master-slave communication:

| Byte | Content | Description |
|------|---------|-------------|
| 0 | 0x3D | Start byte |
| 1 | Flag+Seq | Flag and sequence (≥0x80) |
| 2 | Length (L) | Total packet length |
| 3 | CRC8 | Header integrity check |
| 4 | Type | Packet type identifier |
| 5-(L-3) | Data | Packet payload |
| (L-2)-(L-1) | CRC16 | Data integrity check |

#### CRC Algorithms

**CRC8**: 
- Polynomial: 0x39
- Initial value: 0x66  
- No XOR, no reverse

**CRC16**:
- Polynomial: 0x1021
- Initial value: 0x913D
- No XOR, no reverse
- Low byte first in array

#### Device Addressing

The protocol uses specific addresses to identify different components:

| Address | Device | Description |
|---------|--------|-------------|
| 0x03 | MC | Motion Controller |
| 0x06 | AP | Upper Computer (X series) |
| 0x07 | AMS | Automatic Material System |
| 0x08 | TH | Tool Head |
| 0x09 | AP2 | Upper Computer (P/A series) |
| 0x0E | AHB | AMS Hub Board |
| 0x0F | EXT | External Control Board |
| 0x12 | AMS Lite | AMS Lite Device |
| 0x13 | CTC | Color Touch Controller |

#### Short Header Packet Types

For AMS/AMS Lite communication:

| Type | Purpose |
|------|---------|
| 0x03 | Read filament movement information |
| 0x04 | Read/change AMS motion status |
| 0x05 | Device online verification |
| 0x06 | Unknown/Reserved |
| 0x07 | Read NFC information |
| 0x20 | Printer heartbeat packet |

### Data Structures

#### `AMS_filament_stu`
Enumeration defining filament status states:
- `offline`: Filament not detected or offline
- `online`: Filament detected and online  
- `NFC_waiting`: Waiting for NFC data read

#### `AMS_filament_motion`
Enumeration defining filament motion commands:
- `before_pull_back`: Preparing to retract filament
- `need_pull_back`: Needs to retract filament
- `need_send_out`: Needs to feed filament
- `on_use`: Currently in use
- `idle`: Idle state

#### `BambuBus_package_type`
Communication package types supported by the protocol.

### Core Functions

#### `void BambuBus_init()`
Initialize the BambuBus communication system.
- Sets up UART communication
- Initializes protocol state machine
- Loads configuration from flash memory

#### `BambuBus_package_type BambuBus_run()`
Main communication loop function.
- **Returns**: Package type received or error code
- Should be called repeatedly in main loop
- Handles incoming messages and responses

#### `bool Bambubus_read()`
Read stored configuration from flash memory.
- **Returns**: `true` if valid data found, `false` otherwise
- Validates data integrity using magic number and version

#### Filament Management Functions

#### `int get_now_filament_num()`
Get currently active filament channel number.
- **Returns**: Channel number (0-3) or 0xFF if none active

#### `void set_filament_online(int num, bool if_online)`
Set filament online status for a channel.
- **Parameters**: 
  - `num`: Channel number (0-3)
  - `if_online`: Online status

#### `bool get_filament_online(int num)`
Get filament online status for a channel.
- **Parameters**: `num`: Channel number (0-3)
- **Returns**: Online status

#### `void reset_filament_meters(int num)`
Reset filament length counter for a channel.
- **Parameters**: `num`: Channel number (0-3)

#### `void add_filament_meters(int num, float meters)`
Add to filament usage counter.
- **Parameters**:
  - `num`: Channel number (0-3)
  - `meters`: Length to add in meters

#### `float get_filament_meters(int num)`
Get remaining filament length.
- **Parameters**: `num`: Channel number (0-3)
- **Returns**: Remaining length in meters

---

## Motion Control Module

Controls filament feeding, retraction, and sensor monitoring.

### Configuration Constants

All motion control parameters are defined in `config.h`:
- `PULL_VOLTAGE_HIGH`: High pressure threshold (1.85V)
- `PULL_VOLTAGE_LOW`: Low pressure threshold (1.45V)
- `ASSIST_SEND_TIME_MS`: Feed assist duration (1200ms)
- `P1X_OUT_FILAMENT_MM`: Retraction distance (200mm)

### Core Functions

#### `void Motion_control_init()`
Initialize motion control system.
- Configures GPIO pins for motor control
- Initializes AS5600 hall sensors
- Sets up ADC for pressure sensing

#### `void Motion_control_run(int error)`
Main motion control loop.
- **Parameters**: `error`: System error code
- Processes sensor readings
- Controls motor PWM outputs
- Updates LED status indicators

#### `void Motion_control_set_PWM(uint8_t CHx, int PWM)`
Set PWM output for a motor channel.
- **Parameters**:
  - `CHx`: Channel number (0-3)
  - `PWM`: PWM duty cycle (-100 to 100)

### Sensor Functions

#### `void MC_PULL_ONLINE_read()`
Read all pressure and position sensors.
- Updates global sensor state arrays
- Processes raw ADC values
- Applies filtering and thresholds

---

## RGB LED Control

Controls status indication through NeoPixel RGB LEDs.

### Configuration

LED configurations are defined in `config.h`:
- Channel LED counts: `LED_PA11_NUM`, `LED_PA8_NUM`, etc.
- Brightness levels: `BRIGHTNESS_MAIN_BOARD`, `BRIGHTNESS_CHANNEL`

### Core Functions

#### `void RGB_init()`
Initialize all RGB LED strips.
- Configures GPIO pins for LED communication
- Initializes NeoPixel objects for each channel

#### `void RGB_Set_Brightness()`
Set brightness levels for all LEDs.
- Uses values from configuration file
- Applies to both channel and main board LEDs

#### `void Set_MC_RGB(uint8_t channel, int num, uint8_t R, uint8_t G, uint8_t B)`
Set RGB color for specific LED.
- **Parameters**:
  - `channel`: Channel number (0-3)
  - `num`: LED index within channel
  - `R, G, B`: Color components (0-255)
- Includes bounds checking and error handling
- Only updates LED if color has changed

#### Convenience Macros

```cpp
// Set channel status LED color
MC_STU_RGB_set(channel, R, G, B)

// Set channel pull-online LED color  
MC_PULL_ONLINE_RGB_set(channel, R, G, B)
```

### LED Status Meanings

#### Channel LEDs
- 🔴 **Red**: Offline/Error state
- 🔵 **Blue**: Low pressure/Standby
- 🟢 **Green**: Normal operation
- 🟡 **Yellow**: Material loading/unloading

#### Main Board LED
- 🔴 **Red breathing**: Not connected to printer
- ⚪ **White breathing**: Normal operation

---

## Debug and Logging

Provides serial debugging and logging functionality.

### Configuration

Debug settings in `Debug_log.h`:
- `Debug_log_on`: Enable/disable debug output
- `DEBUG_UART_BAUDRATE`: UART baud rate (115200)

### Macros

#### `DEBUG_init()`
Initialize debug logging system.

#### `DEBUG_MY(logs)`
Output debug message.
- **Parameters**: `logs`: String to output

#### `DEBUG_num(logs, num)`
Output debug message with number.
- **Parameters**: 
  - `logs`: String prefix
  - `num`: Number to output

#### `DEBUG_float(value, precision)`
Output floating point value.
- **Parameters**:
  - `value`: Float value
  - `precision`: Decimal places

#### `DEBUG_time()`
Output current timestamp.

---

## Flash Storage

Manages non-volatile storage of configuration and filament data.

### Storage Structure

Data is stored at `FLASH_SAVE_ADDRESS` (0x0800F000) and includes:
- Filament profiles for all 4 channels
- Current active filament number
- Usage flags and counters
- Version information and integrity check

### Functions

#### `bool Flash_saves(void* data, size_t size, uint32_t address)`
Save data to flash memory.
- **Parameters**:
  - `data`: Pointer to data structure
  - `size`: Size of data in bytes
  - `address`: Flash memory address
- **Returns**: Success status

### Data Integrity

- Magic number (`FLASH_MAGIC_NUMBER`) validates data integrity
- Version field ensures compatibility
- 4-byte alignment for optimal flash access

---

## Sensor Interface

### AS5600 Hall Sensors

Each filament channel has a magnetic rotary position sensor for precise movement tracking.

#### Configuration
- I2C pins defined in `AS5600_SCL_PINS` and `AS5600_SDA_PINS`
- Software I2C implementation for multiple sensors
- 12-bit resolution (0-4095 counts per revolution)

#### Usage
```cpp
// Initialize sensors
MC_AS5600.init(AS5600_SCL, AS5600_SDA, MAX_FILAMENT_CHANNELS);

// Read position for channel 0
uint16_t position = MC_AS5600.readAngle(0);
```

### ADC Sensors

8-channel DMA-enabled ADC monitors:
- Pressure sensors for filament detection
- Micro-switch positions
- Power supply voltages

#### Reading Sensors
```cpp
float* adc_values = ADC_DMA_get_value();
// adc_values[0-7] contain the 8 ADC channel readings
```

---

## Error Handling

### Error Codes

Defined in `config.h`:
- `ERROR_NONE` (0): No error
- `ERROR_OFFLINE` (-1): Device offline
- `ERROR_TIMEOUT` (-2): Communication timeout
- `ERROR_CRC` (-3): Data integrity error
- `ERROR_INVALID_DATA` (-4): Invalid data format

### Error Recovery

- Automatic retry mechanisms for communication errors
- Sensor validation and fallback modes
- LED indicators for error states
- Debug logging for troubleshooting

---

## Performance Considerations

### Memory Usage
- RAM: ~46.5% utilization (9.5KB of 20KB)
- Flash: ~57.6% utilization (37.7KB of 64KB)

### Real-time Constraints
- Main loop runs at high frequency for responsive control
- Sensor readings updated every few milliseconds
- LED updates throttled to prevent communication issues
- Non-blocking operations where possible

### Power Management
- LED brightness configured for thermal management
- Motor PWM optimized for efficiency
- Sleep modes not currently implemented (always-on operation required)