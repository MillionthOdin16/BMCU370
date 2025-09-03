# BMCU370 Firmware

**Bambu Multi-Color Unit (BMCU) - CH32V203 Microcontroller Firmware**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](#building)
[![License](https://img.shields.io/badge/license-see%20LICENSE-blue)](./LICENSE)
[![Platform](https://img.shields.io/badge/platform-CH32V203-orange)](#hardware)

BMCU星尘修改版最新（BMCU-C 370霍尔版 V0.1-0020）源码，原项目链接：[Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)。有一些个人小优化。

*English: BMCU Xing-C modified version latest (BMCU-C Hall V0.1-0020) source code with personal optimizations.*

## Overview

The BMCU370 is a sophisticated multi-material 3D printing controller designed for Bambu Lab printers. This firmware provides:

- **Multi-channel filament management** (4 channels)
- **Automatic Material System (AMS)** compatibility
- **Hall sensor-based position feedback** using AS5600 sensors
- **RGB LED status indication** with NeoPixel strips
- **BambuBus protocol communication** with printers
- **Non-volatile storage** for filament profiles and settings
- **Real-time motion control** with feedback loops

## Hardware Specifications

- **Microcontroller**: CH32V203C8T6 (RISC-V core, 144MHz)
- **Memory**: 20KB RAM, 64KB Flash
- **Communication**: UART-based BambuBus protocol
- **Sensors**: 4x AS5600 Hall sensors for rotary position
- **Actuators**: 4-channel PWM motor control
- **Indicators**: RGB LED strips (NeoPixel compatible)
- **ADC**: 8-channel DMA-enabled analog input

## Resources

- **English Wiki**: https://wiki.yuekai.fr/
- **中文Wiki**: https://bmcu.wanzii.cn/
- **BambuBus Protocol Documentation**: [Bambu-Research-Group/Bambu-Bus](https://github.com/Bambu-Research-Group/Bambu-Bus)
  - Comprehensive BambuBus protocol specifications and tools
  - Packet parsing utilities and simulators
  - Research data from the Bambu Research Group

## Building

### Prerequisites

1. **PlatformIO IDE** or **PlatformIO CLI**
   ```bash
   pip install platformio
   ```

2. **CH32V Development Platform** (automatically installed by PlatformIO)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/MillionthOdin16/BMCU370.git
cd BMCU370

# Build the firmware
pio run

# Upload to device (requires ST-Link or compatible programmer)
pio run --target upload
```

### Project Structure

```
src/
├── main.cpp/h           # Main application and RGB control
├── BambuBus.cpp/h       # Communication protocol implementation
├── Motion_control.cpp/h # Filament motion and motor control
├── Flash_saves.cpp/h    # Non-volatile storage management
├── ADC_DMA.cpp/h        # ADC with DMA for sensor readings
├── Debug_log.cpp/h      # Debug logging system
├── many_soft_AS5600.cpp/h # Hall sensor interface
├── Adafruit_NeoPixel.cpp/h # RGB LED control library
└── time64.cpp/h         # 64-bit timestamp utilities
```

## Configuration

### Hardware Configuration

Key hardware parameters are defined in `main.h`:
- **LED_PA11_NUM**: Number of LEDs on channel PA11 (default: 2)
- **LED_PA8_NUM**: Number of LEDs on channel PA8 (default: 2)
- **LED_PB1_NUM**: Number of LEDs on channel PB1 (default: 2)
- **LED_PB0_NUM**: Number of LEDs on channel PB0 (default: 2)
- **LED_PD1_NUM**: Main board status LED count (default: 1)

### Filament Settings

Each filament channel supports:
- **Material Type**: Default PETG (customizable via NFC)
- **Temperature Range**: 220-240°C (default)
- **Color Information**: RGBA values for visual identification
- **Length Tracking**: Precise filament usage measurement

## Features

### Multi-Channel Filament Management
- Supports up to 4 filament channels simultaneously
- Individual channel status monitoring via RGB indicators
- Automatic filament detection and insertion assistance

### Communication Protocol
- **BambuBus Protocol**: Custom UART-based communication with Bambu printers (1,228,800 bps)
- **Packet Formats**: Supports both long and short header packets with CRC verification
- **Device Addressing**: Multi-device communication with specific address assignments
- **CRC Error Detection**: Built-in data integrity checking using CRC8/CRC16
- **Hot-swap Support**: Dynamic device detection and configuration

### Sensor Integration
- **Hall Sensors (AS5600)**: Magnetic rotary position sensors for each channel
- **Pressure Sensors**: Filament insertion/removal detection
- **Temperature Monitoring**: Real-time temperature feedback

### LED Status System
- **Channel Status**: Individual RGB indicators per filament channel
  - 🔴 Red: Offline/Error
  - 🔵 Blue: Standby/Low pressure
  - 🟢 Green: Active/Normal operation
  - 🟡 Yellow: Material loading/unloading
- **Main Board Status**: System-wide status indication
  - 🔴 Red breathing: Not connected to printer
  - ⚪ White breathing: Normal operation

## Update History

### Version 0020 (July 17, 2025)
- ✅ Fixed lighting logic errors causing some states not to illuminate
- ✅ Fixed unexpected channel online issues
- ✅ Corrected anti-disconnect logic (was previously ineffective)
- ✅ Rewrote lighting system, fixed flickering issues, reduced refresh rate
- ✅ Added 3-second red light retry for channel errors during operation

### Version 0019 (July 6, 2025)
- ✅ Compatible with dual micro-switch Hall sensor versions
- ✅ P1X1 printer now supports 16-color configuration
- ✅ Fixed filament information saving issues with latest firmware (00.01.06.62)
- ✅ Improved online logic detection to prevent false channel online states
- ✅ Enhanced motor control logic with voltage-specific handling
- ✅ Reduced buffer and main board LED brightness for better thermal management
- ✅ Removed A1 control dependency in material retraction

## Troubleshooting

### Common Issues

**Problem**: LEDs not lighting up correctly
- **Cause**: Channel detection error or power supply issue
- **Solution**: Check connections and power supply voltage (5V required for RGB LEDs)

**Problem**: Filament not feeding properly
- **Cause**: Hall sensor calibration or mechanical obstruction
- **Solution**: Verify AS5600 sensor positions and clear filament path

**Problem**: Communication timeout with printer
- **Cause**: BambuBus protocol error or cable issue
- **Solution**: Check UART connections and baud rate settings (115200)

### Debug Output

Enable debug logging by ensuring `Debug_log_on` is defined in `Debug_log.h`:
```cpp
#define Debug_log_on
```

Connect UART at 115200 baud to view debug messages.