# BMCU 370 C (Hall Version) Technical Documentation

## Table of Contents
1. [Overview](#overview)
2. [Hardware Specifications](#hardware-specifications)
3. [System Architecture](#system-architecture)
4. [Communication Protocols](#communication-protocols)
5. [Filament Management](#filament-management)
6. [Motor Control System](#motor-control-system)
7. [LED System](#led-system)
8. [Firmware Components](#firmware-components)

---

## Overview

The BMCU (Bambu Multi-Color Unit) 370 C is a hardware controller designed for multi-filament 3D printing systems. This version (Hall V0.1-0020) uses Hall effect sensors (AS5600) for precise filament position tracking and integrates with Bambu Lab printers via the BambuBus communication protocol.

### Key Features
- **4-channel filament management** - Supports up to 4 filament channels simultaneously
- **Hall effect sensing** - AS5600 magnetic rotary position sensors for accurate filament tracking
- **Real-time motor control** - PID-controlled stepper motors for precise filament feeding
- **BambuBus protocol** - Native integration with Bambu Lab AMS ecosystem
- **RGB LED indicators** - Per-channel and main board status lighting
- **Flash storage** - Non-volatile storage for filament profiles and configuration
- **Pressure sensing** - ADC-based pressure monitoring for feed quality control

### Version Information
- **Firmware Version**: BMCU-C 370 Hall V0.1-0020
- **BambuBus Protocol Version**: 5
- **License**: GNU General Public License v2.0
- **Based On**: [Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)

---

## Hardware Specifications

### Microcontroller
- **Model**: CH32V203C8T6
- **Architecture**: RISC-V 32-bit
- **Clock Speed**: 144 MHz (overclocked from standard 72 MHz via HSI)
- **Flash Memory**: 64 KB
  - Program space: 0x08000000 - 0x0800EFFF
  - Motion control data: 0x0800E000 (4 KB page)
  - Filament data storage: 0x0800F000 (4 KB page)
- **RAM**: 20 KB
- **Operating Voltage**: 3.3V

### Hall Effect Sensors (AS5600)
Four independent AS5600 magnetic rotary position sensors, one per channel:

- **Resolution**: 12-bit (4096 positions per revolution)
- **Interface**: I2C (software-implemented)
- **Update Rate**: Real-time position tracking
- **Magnet Detection**: Status monitoring for proper sensor operation

**Pin Configuration**:
| Channel | SCL Pin | SDA Pin |
|---------|---------|---------|
| CH1     | PB15    | PD0     |
| CH2     | PB14    | PC15    |
| CH3     | PB13    | PC14    |
| CH4     | PB12    | PC13    |

### ADC System
8-channel ADC with DMA for monitoring pressure and filament detection:

**Channel Mapping**:
- Channels 0-1: CH4 pressure sensor and online detection
- Channels 2-3: CH3 pressure sensor and online detection
- Channels 4-5: CH2 pressure sensor and online detection
- Channels 6-7: CH1 pressure sensor and online detection

**Voltage Thresholds**:
- **Pressure High**: > 1.85V (filament blockage/high resistance)
- **Pressure Normal**: 1.45V - 1.85V (optimal range)
- **Pressure Low**: < 1.45V (no filament/low resistance)
- **Online Detection**: > 1.65V (filament present)

### RGB LED System
WS2812B-compatible addressable RGB LEDs (GRB color order, 800 kHz):

| Component | Pin | LED Count | Brightness | Purpose |
|-----------|-----|-----------|------------|---------|
| Main Board | PD1 | 1 | 35/255 | System status indicator |
| Channel 1 | PA11 | 2 | 15/255 | Filament 1 status |
| Channel 2 | PA8 | 2 | 15/255 | Filament 2 status |
| Channel 3 | PB1 | 2 | 15/255 | Filament 3 status |
| Channel 4 | PB0 | 2 | 15/255 | Filament 4 status |

### Communication Interface
- **Protocol**: BambuBus (UART-based)
- **Supported Devices**: 
  - BambuBus AMS (0x0700)
  - BambuBus AMS Lite (0x1200)

---

## System Architecture

### Software Stack

```
┌─────────────────────────────────────┐
│         Application Layer            │
│  (Filament Management, LED Control)  │
└─────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────┐
│        Control Layer                 │
│  (Motion Control, PID, BambuBus)    │
└─────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────┐
│         Hardware Abstraction         │
│  (ADC, Flash, AS5600, RGB, UART)    │
└─────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────┐
│       Hardware (CH32V203C8T6)       │
└─────────────────────────────────────┘
```

### Main Components

1. **BambuBus Communication Module** (`BambuBus.cpp/h`)
   - Handles bidirectional communication with printer
   - Manages filament metadata (color, type, temperature)
   - Processes motion commands and status requests

2. **Motion Control Module** (`Motion_control.cpp/h`)
   - PID-based motor speed control
   - Filament feeding and retraction logic
   - Pressure monitoring and safety handling

3. **AS5600 Interface** (`many_soft_AS5600.cpp/h`)
   - Software I2C implementation for multi-sensor support
   - Angle and speed calculation
   - Magnet status detection

4. **ADC DMA Module** (`ADC_DMA.cpp/h`)
   - Continuous analog-to-digital conversion
   - DMA-based data transfer for efficiency
   - Voltage threshold monitoring

5. **RGB LED Controller** (`Adafruit_NeoPixel.cpp/h`)
   - WS2812B protocol implementation
   - Per-channel color and brightness control
   - Synchronized display updates

6. **Flash Storage** (`Flash_saves.cpp/h`)
   - Non-volatile data persistence
   - Filament profile storage
   - Configuration backup

7. **Debug Logging** (`Debug_log.cpp/h`)
   - UART-based debug output (115200 baud)
   - Timestamped logging
   - Diagnostic information

---

## Communication Protocols

### BambuBus Protocol

BambuBus is a proprietary UART-based protocol used by Bambu Lab for AMS communication.

#### Packet Structure
```
[0x3D] [Type] [Addr_H] [Addr_L] [Length] [Data...] [CRC8]
```

- **Start Byte**: 0x3D (fixed header)
- **Type**: Packet type identifier
- **Address**: 16-bit device address
- **Length**: Payload length indicator
- **Data**: Variable-length payload
- **CRC8**: Checksum (polynomial: 0x39, init: 0x66)

#### Packet Types
| Type | Name | Description |
|------|------|-------------|
| 0 | NONE | No operation |
| 1 | filament_motion_short | Short motion command |
| 2 | filament_motion_long | Extended motion command |
| 3 | online_detect | Filament presence query |
| 4 | REQx6 | Request type 6 |
| 5 | NFC_detect | NFC tag detection |
| 6 | set_filament_info | Update filament metadata |
| 7 | MC_online | Multi-channel online status |
| 8 | read_filament_info | Query filament data |
| 9 | set_filament_info_type2 | Alternative metadata format |
| 10 | version | Firmware version query |
| 11 | serial_number | Device serial number |
| 12 | heartbeat | Keep-alive packet |
| 13 | ETC | Miscellaneous commands |

#### Filament States

**Online Status** (`AMS_filament_stu`):
- `offline` - No filament detected
- `online` - Filament present and ready
- `NFC_waiting` - Waiting for NFC tag read

**Motion Status** (`AMS_filament_motion`):
- `idle` - No motion required
- `need_send_out` - Feed filament to printer
- `on_use` - Actively feeding during print
- `need_pull_back` - Retract filament
- `before_pull_back` - Pre-retraction state

---

## Filament Management

### Data Structure

Each of the 4 channels stores the following filament information:

```cpp
struct _filament {
    char ID[8];              // Filament identifier (e.g., "GFG00")
    uint8_t color_R;         // Red component (0-255)
    uint8_t color_G;         // Green component (0-255)
    uint8_t color_B;         // Blue component (0-255)
    uint8_t color_A;         // Alpha/additional color data
    int16_t temperature_min; // Minimum nozzle temperature (°C)
    int16_t temperature_max; // Maximum nozzle temperature (°C)
    char name[20];           // Material name (e.g., "PETG", "PLA")
    float meters;            // Total filament used (meters)
    uint64_t meters_virtual_count; // Internal usage counter
    AMS_filament_stu statu;  // Online status
    AMS_filament_motion motion_set; // Motion command
    uint16_t pressure;       // Pressure sensor reading
};
```

### Flash Storage

Filament data is persisted to flash memory at address `0x0800F000`:

```cpp
struct flash_save_struct {
    _filament filament[4];   // 4 channel profiles
    int BambuBus_now_filament_num; // Currently active channel (0-3, or 0xFF for none)
    uint8_t filament_use_flag; // Usage state flags
    uint32_t version;        // Data structure version
    uint32_t check;          // Magic number (0x40614061) for validation
};
```

**Data Integrity**:
- Version field must match `Bambubus_version` (currently 5)
- Check field must equal `0x40614061` for valid data
- Auto-save triggered on configuration changes

### Filament Tracking

The system tracks filament usage in real-time:

1. **Position Sensing**: AS5600 Hall sensors measure rotation angle (0-4095)
2. **Speed Calculation**: Angular velocity computed from position delta
3. **Distance Accumulation**: Meters fed/retracted added to running total
4. **Flash Persistence**: Usage data saved periodically to survive power loss

---

## Motor Control System

### PID Controller

Each channel has an independent PID controller for precise motor speed regulation:

**PID Formula**:
```
Output = P×Error + I×∫Error×dt + D×(dError/dt)
```

**Parameters**:
- **P** (Proportional): Direct response to error
- **I** (Integral): Accumulated error correction
- **D** (Derivative): Rate of change damping
- **PWM Limit**: ±1000 (maximum duty cycle)

**Integral Anti-Windup**:
- I term clamped to ±(PWM_range/2) to prevent saturation

### Motion States

The system implements a state machine for filament handling:

1. **Idle**: No motion, motors off
2. **Feed**: Pushing filament toward extruder
3. **Retract**: Pulling filament back to AMS
4. **Assist Feed**: Helping filament reach inner microswitch
5. **On Use**: Active printing mode

### Pressure Control

Voltage-based pressure sensing ensures optimal feed quality:

- **High Pressure (>1.85V)**: Reduce motor speed to prevent jamming
- **Normal (1.45-1.85V)**: Maintain current speed
- **Low Pressure (<1.45V)**: Increase speed or detect filament end

### Microswitch Detection

Two operational modes (configured via `is_two` flag):

**Single Microswitch Mode** (`is_two = false`):
- **>1.65V**: Filament online
- **<1.65V**: Filament offline

**Dual Microswitch Mode** (`is_two = true`):
- **<0.6V**: Offline (neither switch triggered)
- **1.4-1.7V**: Outer switch only (needs assist feed)
- **>1.7V**: Both switches (filament fully inserted)
- **<1.4V**: Inner switch only (confirm vs. bounce detection)

### Retraction Distance

Configurable per installation type:
- **Internal Installation**: 200 mm default (`P1X_OUT_filament_meters`)
- **External Installation**: 700 mm (configurable)

---

## LED System

### Color Coding

**Main Board LED** (PD1):
- **Red Breathing**: Not connected to printer
- **White Breathing**: Normal operation, ready/idle
- **Various**: Status-dependent during operation

**Channel LEDs** (PA11, PA8, PB1, PB0):
Each channel has 2 LEDs showing:
- **Filament Color**: Displays the loaded filament's RGB color from metadata
- **Status Indicators**:
  - **Red**: Error state or channel not online
  - **Green**: Feeding operation in progress
  - **Blue**: Retraction operation
  - **Custom RGB**: Filament color when idle/online

### LED Update Strategy

To prevent communication interference:
1. **Color Caching**: RGB values stored in `channel_runs_colors[4][2][3]`
2. **Change Detection**: Only update when color values change
3. **Reduced Frequency**: Limit refresh rate to avoid bus conflicts
4. **Brightness Limiting**: Keep power consumption low (15/255 for channels, 35/255 for main)

### Error Indication

When a channel experiences errors:
- LED attempts to update to red every 3 seconds
- Ensures visibility even if channel comes online during operation
- Prevents "dark channel" issue on late insertion

---

## Firmware Components

### Module Descriptions

#### 1. main.cpp/h
**Purpose**: Application entry point and main control loop

**Key Functions**:
- `setup()`: Hardware initialization sequence
  - Disable watchdog timer (WWDG)
  - Initialize RGB LEDs with brightness settings
  - Start BambuBus communication
  - Initialize motion control and debug logging
- `loop()`: Main execution cycle (called by Arduino framework)
- `Set_MC_RGB()`: Thread-safe RGB color update with change detection
- `RGB_init()`: Configure all LED strips
- `RGB_show_data()`: Synchronous LED display update

**Global State**:
- `channel_colors[4][4]`: Filament color metadata (RGBA)
- `channel_runs_colors[4][2][3]`: Cached LED RGB values
- `strip_channel[4]`: NeoPixel objects for 4 channels
- `strip_PD1`: Main board NeoPixel object

#### 2. BambuBus.cpp/h
**Purpose**: BambuBus protocol implementation

**Key Functions**:
- `BambuBus_init()`: Initialize UART and load saved data
- `BambuBus_run()`: Process incoming packets, returns packet type
- `Bambubus_read()`: Load filament data from flash
- `Bambubus_save()`: Persist filament data to flash
- `get_filament_online(num)`: Check if channel has filament
- `set_filament_motion(num, motion)`: Command channel motion
- `add_filament_meters(num, meters)`: Update usage tracking
- `BambuBus_if_on_print()`: Check if any channel is active

**CRC Implementation**:
- CRC8 for packet validation (polynomial: 0x39, init: 0x66)
- CRC16 for extended data integrity

**Flash Layout**:
- Address: `0x0800F000`
- Size: Aligned to 4-byte boundary for flash write
- Validation: Magic number and version checking

#### 3. Motion_control.cpp/h
**Purpose**: Motor control and filament handling

**Key Functions**:
- `Motion_control_init()`: Setup AS5600 sensors, ADC, and load saved config
- `Motion_control_run(error)`: Main control loop (called frequently)
- `Motion_control_set_PWM(CHx, PWM)`: Direct PWM output to motor
- `MC_PULL_ONLINE_read()`: Read ADC values for pressure and detection
- `MC_PULL_ONLINE_init()`: Initialize ADC DMA

**PID Controller Class**:
- `init_PID(P, I, D)`: Set PID parameters
- `calculate(Error, time_E)`: Compute control output
- `clear()`: Reset integral and derivative terms

**State Management**:
- `MC_PULL_stu[4]`: Pressure state per channel (-1/0/1 for low/normal/high)
- `MC_ONLINE_key_stu[4]`: Filament detection state (0/1/2/3)
- `speed_as5600[4]`: Computed filament speed (filtered)

**Flash Storage**:
- Address: `0x0800E000`
- Content: Motor direction configuration per channel
- Validation: Magic number `0x40614061`

#### 4. many_soft_AS5600.cpp/h
**Purpose**: Software I2C driver for multiple AS5600 sensors

**Key Functions**:
- `init(SCL_pins, SDA_pins, count)`: Configure GPIO for software I2C
- `updata_stu()`: Read magnet detection status from all sensors
- `updata_angle()`: Read raw angle (0-4095) from all sensors

**Class Members**:
- `online[4]`: Sensor communication status
- `magnet_stu[4]`: Magnet field quality (low/normal/high/offline)
- `raw_angle[4]`: 12-bit absolute position

**I2C Protocol**:
- Software bit-banging implementation
- Supports multiple independent buses (4 sensors simultaneously)
- Register read operations for AS5600

#### 5. ADC_DMA.cpp/h
**Purpose**: Analog-to-digital conversion with DMA

**Key Functions**:
- `ADC_DMA_init()`: Configure ADC and DMA channel
- `ADC_DMA_get_value()`: Retrieve latest ADC readings (8 channels)

**Configuration**:
- Continuous conversion mode
- DMA circular buffer
- Voltage reference: Internal (3.3V)

#### 6. Flash_saves.cpp/h
**Purpose**: Non-volatile storage operations

**Key Functions**:
- `Flash_saves(buffer, length, address)`: Write data to flash
  - Unlocks flash controller
  - Erases target page (4 KB)
  - Writes data in 4-byte words
  - Re-locks flash controller
  - Returns success/failure status

**Safety**:
- Page size: 4096 bytes (`FLASH_PAGE_SIZE`)
- Automatic page erase before write
- Alignment to 32-bit word boundaries

#### 7. Debug_log.cpp/h
**Purpose**: Serial debugging output

**Key Functions**:
- `Debug_log_init()`: Initialize UART at 115200 baud
- `Debug_log_write(data)`: Send string to debug console
- `Debug_log_write_num(data, num)`: Send data with numeric count
- `Debug_log_count64()`: Get 64-bit microsecond timestamp
- `Debug_log_time()`: Print current timestamp

**Macros**:
- `DEBUG_MY(text)`: Log message
- `DEBUG_num(data, count)`: Log data array
- `DEBUG_time()`: Log timestamp
- Conditional compilation via `Debug_log_on`

#### 8. time64.cpp/h
**Purpose**: 64-bit time tracking

**Functions**:
- Provides extended time resolution beyond 32-bit Arduino `millis()`
- Used for precise timing in motion control

#### 9. Adafruit_NeoPixel.cpp/h
**Purpose**: WS2812B RGB LED driver

**Features**:
- Bit-banging WS2812 protocol
- GRB color order support
- Brightness control (global dimming)
- Multiple strip management

**Key Methods**:
- `begin()`: Initialize GPIO pin
- `setPixelColor(index, r, g, b)`: Set color for LED
- `setBrightness(level)`: Set global brightness (0-255)
- `show()`: Update all LEDs with current buffer

---

## Build and Development

### PlatformIO Configuration

**Platform**: CH32V community platform (RISC-V)
- Repository: https://github.com/Community-PIO-CH32V/platform-ch32v.git
- Board: `genericCH32V203C8T6`
- Framework: Arduino-compatible

**Dependencies**:
- `robtillaart/CRC@^1.0.3` - CRC calculation library

**Build Flags**:
- `-D SYSCLK_FREQ_144MHz_HSI=144000000` - Overclock to 144 MHz

### Memory Map

```
Flash (64 KB):
0x08000000 - 0x0800DFFF : Program code (~56 KB)
0x0800E000 - 0x0800EFFF : Motion control config (4 KB)
0x0800F000 - 0x0800FFFF : Filament data (4 KB)

RAM (20 KB):
0x20000000 - 0x20004FFF : Variables, stack, heap
```

### Development Environment

**Recommended Tools**:
- **IDE**: PlatformIO (VS Code extension) or PlatformIO CLI
- **Programmer**: WCH-Link or compatible RISC-V debugger
- **Serial Monitor**: 115200 baud for debug output

**Build Commands**:
```bash
# Build firmware
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor --baud 115200
```

---

## Recent Changes (V0.1-0020)

Based on the changelog, version 0020 (July 17, 2025) includes:

1. **Lighting System Fixes**:
   - Fixed logic errors causing some states not to illuminate
   - Rewrote lighting system to fix flickering issues
   - Reduced LED refresh frequency for stability
   - Red LED update every 3 seconds when channel error occurs

2. **Channel Management**:
   - Fixed unexpected channel coming online
   - Corrected anti-disconnect logic (previously ineffective)

3. **Motor Control**:
   - Modified motor control logic with different calls for high/low voltage states
   - Removed A1 control during filament retraction

4. **Feature Enhancements** (from 0019):
   - P1X1 now supports 16-color mode (firmware dependent)
   - Fixed filament information saving issue with P1X1 firmware 00.01.06.62+
   - Compatible with slicer software 2.1.1.52+

5. **Brightness Adjustments**:
   - Further reduced buffer and main board LED brightness
   - Main board: 35/255
   - Channels: 15/255

6. **Dual Microswitch Support**:
   - Compatible with both single and dual microswitch Hall versions
   - Configurable via `is_two` flag

---

## Safety and Limitations

### Known Considerations

1. **Overclocking**: MCU runs at 144 MHz (2× standard) - may reduce reliability in extreme temperatures
2. **Flash Wear**: Limited write cycles (~10,000) - save operations should be minimized
3. **Voltage Thresholds**: ADC thresholds tuned for specific hardware - may need calibration for variants
4. **CRC Validation**: BambuBus packets without valid CRC are silently dropped
5. **LED Interference**: High-frequency LED updates can interfere with UART - refresh rate limited

### Best Practices

- **Filament Loading**: Ensure filament fully triggers microswitches before operation
- **Pressure Monitoring**: Watch for persistent high-pressure alerts (>1.85V)
- **AS5600 Magnets**: Verify magnet alignment and distance per AS5600 datasheet
- **Power Supply**: Ensure stable 3.3V with sufficient current for motors and LEDs
- **Firmware Updates**: Always backup filament profiles before flashing new firmware

---

## Appendix

### Related Resources

- **English Wiki**: https://wiki.yuekai.fr/
- **Chinese Wiki**: https://bmcu.wanzii.cn/
- **Original Project**: https://github.com/Xing-C/BMCU370x
- **Hardware Design**: https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu
- **Gitee Mirror**: https://gitee.com/at_4061N/BMCU

### Glossary

- **AMS**: Automatic Material System (Bambu Lab's multi-color unit)
- **BambuBus**: Proprietary communication protocol for Bambu Lab devices
- **Hall Sensor**: Magnetic field sensor (AS5600) for rotary position detection
- **NFC**: Near-Field Communication (for filament identification, future feature)
- **PID**: Proportional-Integral-Derivative controller
- **PWM**: Pulse-Width Modulation (motor speed control)

---

**Document Version**: 1.0  
**Last Updated**: December 2024  
**Firmware Version**: BMCU-C 370 Hall V0.1-0020
