# BMCU370 Firmware Development
BMCU370 is an embedded firmware project for Bambu Lab 3D printer multi-material units (AMS/AMS Lite), running on the CH32V203C8T6 RISC-V microcontroller at 144MHz. The firmware handles filament management, hall sensors, RGB LEDs, and BambuBus communication protocol.

Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.

## Working Effectively
- Bootstrap and build the repository:
  - `pip install platformio` -- takes 5-10 seconds
  - `pio run` -- takes 45-90 seconds clean build. NEVER CANCEL. Set timeout to 180+ seconds.
  - Memory usage after successful build: 48.8% RAM (9,996/20,480 bytes), 62.5% Flash (40,984/65,536 bytes)
- Test build and get size information:
  - `pio run --target size` -- takes 5 seconds. Shows detailed memory usage breakdown.
- Clean builds when needed:
  - `pio run --target clean` -- takes 1 second
  - Next `pio run` will be full rebuild: takes 45-90 seconds. NEVER CANCEL. Set timeout to 180+ seconds.
- Upload firmware (requires hardware programmer):
  - `pio run --target upload` -- will fail without physical WCH-Link/ST-Link programmer connected

## Validation
- Always run `pio run` after making code changes to verify compilation
- Check memory usage with `pio run --target size` to ensure firmware still fits in 64KB flash/20KB RAM
- Monitor build output for warnings - some are expected (unused variables in framework)
- This is embedded firmware - there is no automated test suite or UI to validate
- Hardware validation requires physical BMCU370 device connected to Bambu Lab printer

## Build Artifacts and Files
The following are outputs from frequently run commands. Reference them instead of running bash commands to save time.

### Repository Structure
```
.
├── .github/workflows/          # CI/CD automation (dev-build.yml, build-firmware.yml)
├── docs/                       # Comprehensive documentation
├── src/                        # Source code (C++ with Arduino framework)
├── scripts/version_extract.py  # Build version information utility
├── platformio.ini              # PlatformIO build configuration
├── README.md                   # Complete project documentation
└── CHANGELOG.md                # Detailed version history
```

### Core Source Files
```
src/
├── main.cpp/h                  # Main application and RGB LED control
├── config.h                    # Hardware configuration constants
├── BambuBus.cpp/h             # Communication protocol with printers
├── Motion_control.cpp/h        # Filament feeding and motor control
├── Flash_saves.cpp/h          # Non-volatile settings storage
├── ADC_DMA.cpp/h              # Hall sensor analog readings
├── Debug_log.cpp/h            # UART debug output (115200 baud)
├── many_soft_AS5600.cpp/h     # I2C hall sensor interface
├── Adafruit_NeoPixel.cpp/h    # RGB LED strip control
└── time64.cpp/h               # 64-bit timestamp utilities
```

### PlatformIO Build Configuration
```ini
[env:genericCH32V203C8T6]
platform = https://github.com/Community-PIO-CH32V/platform-ch32v.git
board = genericCH32V203C8T6
framework = arduino
lib_deps = robtillaart/CRC@^1.0.3
build_flags = -D SYSCLK_FREQ_144MHz_HSI=144000000
```

### Python Version Information
```
$ python3 scripts/version_extract.py
BMCU370 Build Information
========================================
Git Version: dev-2
AMS Firmware Version: 00.00.06.49
AMS Lite Firmware Version: 00.01.02.03
Build Date: [current timestamp]
```

### Build Artifacts
After successful `pio run`:
```
.pio/build/genericCH32V203C8T6/
├── firmware.bin     # Main firmware binary (40,992 bytes) - flash to 0x08000000
├── firmware.elf     # Debug symbols (62,832 bytes)
├── firmware.map     # Memory map
└── firmware.lst     # Assembly listing
```

## Critical Build Details
- **NEVER CANCEL BUILDS**: Initial build downloads CH32V platform and tools (~60MB). Takes 45-90 seconds. Subsequent builds are much faster (4-10 seconds).
- **Timeout Requirements**: Set timeout to 180+ seconds for any `pio run` command
- **Memory Constraints**: CH32V203C8T6 has only 64KB flash, 20KB RAM. Monitor usage with `pio run --target size`
- **Build Warnings**: Framework generates some expected warnings about unused variables - these are normal
- **Platform Dependencies**: Uses community CH32V platform, not official PlatformIO platform

## Hardware Configuration
Key parameters in `src/config.h`:
- **LED Counts**: LED_PA11_NUM, LED_PA8_NUM, LED_PB1_NUM, LED_PB0_NUM (default: 2 each)
- **LED Brightness**: BRIGHTNESS_MAIN_BOARD (35), BRIGHTNESS_CHANNEL (15)
- **Firmware Versions**: AMS_FIRMWARE_VERSION_* and AMS_LITE_FIRMWARE_VERSION_* (critical for printer compatibility)
- **Voltage Thresholds**: PULL_VOLTAGE_HIGH (1.85V), PULL_VOLTAGE_LOW (1.45V)
- **Communication**: DEBUG_UART_BAUDRATE (115200), BAMBU_BUS_VERSION (5)

## Debug and Development
- Enable debug logging: Ensure `#define Debug_log_on` in `src/Debug_log.h`
- Debug output: Connect UART at 115200 baud to view debug messages
- Debug macros: `DEBUG_MY()`, `DEBUG_num()`, `DEBUG_float()`, `DEBUG_time()`
- Version extraction: `python3 scripts/version_extract.py --json` for build metadata

## Key Technical Details
- **Target Hardware**: CH32V203C8T6 RISC-V microcontroller @ 144MHz
- **Flash Address**: 0x08000000 (standard for CH32V series)
- **Framework**: Arduino-style C++ with PlatformIO build system
- **Protocol**: BambuBus v5 for communication with Bambu Lab printers
- **Sensors**: AS5600 hall sensors on I2C, ADC with DMA for pressure sensing
- **Actuators**: PWM motor control, NeoPixel RGB LED strips
- **Storage**: Flash-based non-volatile configuration storage

## Firmware Version Management
**CRITICAL**: The firmware version reported to printers determines compatibility. Bambu Lab printers may reject units with incompatible versions.

Current versions (in `src/config.h`):
- **AMS (8-channel)**: 00.00.06.49 (0x00, 0x00, 0x06, 0x31)
- **AMS Lite**: 00.01.02.03 (0x00, 0x01, 0x02, 0x03)

Only modify these if you understand printer compatibility requirements.

## CI/CD System
The repository has automated builds:
- **Dev builds**: Triggered on every commit/PR. Artifacts retained 30 days.
- **Release builds**: Triggered on version tags (v1.0.0). Creates GitHub releases.
- **Caching**: Reduces build time from 5-10 minutes to 2-5 minutes
- **Artifacts**: Firmware binaries, debug symbols, build info, checksums

Always check GitHub Actions for build status after pushing changes.

## Upload and Programming
- **Programmer Required**: WCH-Link recommended, ST-Link compatible
- **Upload Command**: `pio run --target upload` (will fail without programmer)
- **Flash Address**: 0x08000000
- **Programming Tools**: WCH-LinkUtility or compatible flashing software
- **Upload Targets**: Available via `pio run --list-targets`

## Compatibility
- **Printers**: Bambu Lab X1/X1C, A1/A1 mini, P1P/P1S series
- **Protocol**: BambuBus v5 with CRC validation
- **Hardware**: CH32V203C8T6 microcontroller with specific pin assignments
- **Power**: 5V for RGB LEDs, 3.3V logic levels

## Common Troubleshooting
- **Build fails**: Check internet connection (downloads platform on first build)
- **Memory overflow**: Reduce feature set or optimize code - only 64KB flash available
- **LED issues**: Verify channel configuration in `config.h`
- **Communication issues**: Check BambuBus baud rate and protocol version
- **Motor direction**: Firmware has automatic direction learning system

Always run `pio run` and `pio run --target size` to validate changes before committing.