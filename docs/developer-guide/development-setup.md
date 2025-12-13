# Development Environment Setup

## Overview

This guide covers setting up a development environment for BMCU-C 370 Hall version firmware development.

## System Requirements

### Operating Systems
- **Windows 10/11** (recommended)
- **macOS** (10.14 or later)
- **Linux** (Ubuntu 20.04+ or equivalent)

### Software Requirements
- **PlatformIO** (recommended method)
- **VS Code** with PlatformIO extension, OR
- **PlatformIO Core CLI**
- **Git** for version control
- **USB-to-TTL adapter** or onboard CH340 (for flashing)

## Quick Setup (Recommended)

### 1. Install Visual Studio Code

Download and install VS Code from: https://code.visualstudio.com/

### 2. Install PlatformIO Extension

1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install
5. Reload VS Code when prompted

### 3. Clone the Repository

```bash
git clone https://github.com/MillionthOdin16/BMCU370.git
cd BMCU370
```bash

Or clone the original:
```bash
git clone https://github.com/Xing-C/BMCU370x.git
cd BMCU370x
```

### 4. Open Project in PlatformIO

1. Open VS Code
2. File → Open Folder
3. Select the cloned BMCU370 directory
4. PlatformIO will automatically detect `platformio.ini`
5. Wait for dependencies to install

## Project Configuration

### PlatformIO Configuration

The project uses `platformio.ini` with the following configuration:

```ini
[env:genericCH32V203C8T6]
platform = https://github.com/Community-PIO-CH32V/platform-ch32v.git
board = genericCH32V203C8T6
framework = arduino
lib_deps = robtillaart/CRC@^1.0.3
build_flags = -D SYSCLK_FREQ_144MHz_HSI=144000000
```

**Configuration Details:**
- **Platform:** Community CH32V platform (RISC-V)
- **Board:** CH32V203C8T6 (32-bit RISC-V core)
- **Framework:** Arduino (simplified development)
- **Clock Speed:** 144 MHz (HSI - High-Speed Internal oscillator)
- **Dependencies:** CRC library version 1.0.3 for BambuBus protocol

### Dependencies

The project automatically installs:

**PlatformIO Libraries:**
- `robtillaart/CRC@^1.0.3` - CRC8 and CRC16 calculations

**Framework Libraries (included with CH32V Arduino):**
- CH32V20x peripheral library
- Arduino core for CH32V

## Building the Firmware

### Using VS Code + PlatformIO

**Method 1: GUI**
1. Open project in VS Code
2. Click PlatformIO icon in sidebar
3. Under "Project Tasks" → "genericCH32V203C8T6"
4. Click "Build"
5. Wait for compilation to complete

**Method 2: Command Palette**
1. Press Ctrl+Shift+P (Cmd+Shift+P on macOS)
2. Type "PlatformIO: Build"
3. Press Enter

**Method 3: Shortcut**
- Press Ctrl+Alt+B (build shortcut)

### Using PlatformIO Core CLI

```bash
# Navigate to project directory
cd BMCU370

# Build the project
pio run

# Build and show verbose output
pio run -v
```

### Build Output

Successful build output location:
```
.pio/build/genericCH32V203C8T6/firmware.hex
.pio/build/genericCH32V203C8T6/firmware.bin
.pio/build/genericCH32V203C8T6/firmware.elf
```

## Flashing the Firmware

### Hardware Setup

**Option 1: Onboard CH340 (if your PCB has it)**
- Connect USB-C cable directly to BMCU-C mainboard
- No additional hardware needed
- Drivers: CH340 USB-to-serial (usually auto-installed)

**Option 2: External USB-to-TTL Adapter**

Required connections:
```
USB-TTL    →  BMCU-C
GND        →  GND
TX         →  RX (USART1 RX)
RX         →  TX (USART1 TX)
3.3V       →  3.3V (optional, for power)
```

**Bootloader Entry:**
1. Press and hold BOOT0 button
2. Press and release RESET button
3. Release BOOT0 button
4. Device is now in bootloader mode

### Flashing via PlatformIO

**Using GUI:**
1. Connect USB cable
2. Put device in bootloader mode (if needed)
3. In PlatformIO, click "Upload"
4. Wait for flashing to complete

**Using CLI:**
```bash
# Upload firmware
pio run --target upload

# Upload with specific port
pio run --target upload --upload-port /dev/ttyUSB0
```

**Windows port examples:** COM3, COM4, etc.
**Linux port examples:** /dev/ttyUSB0, /dev/ttyACM0
**macOS port examples:** /dev/cu.usbserial-XXXX

### Alternative Flashing Methods

**WCH-LinkE (Official WCH Programmer):**
- Supports SWD debugging
- Faster programming
- Requires WCH-LinkE hardware
- Use WCH-LinkUtility software

**ISP Flash Tool:**
- Download from WCH website
- Select CH32V203C8T6
- Choose firmware.hex or .bin file
- Click Download

## Debugging

### Serial Debug Output

The firmware includes optional debug logging via USART3:

**Enable Debug:**
In `Debug_log.h`, ensure:
```cpp
#define Debug_log_on
```

**Debug Configuration:**
- **Port:** USART3
- **Pins:** TX=PB10, RX=PB11
- **Baud Rate:** Defined in `Debug_log_baudrate`
- **Word Length:** 9-bit
- **Parity:** Even
- **Stop Bits:** 1

**Viewing Debug Output:**
```bash
# Linux/macOS
screen /dev/ttyUSB1 115200

# Or use PlatformIO monitor
pio device monitor -b 115200
```

**Windows:** Use PuTTY, TeraTerm, or Arduino Serial Monitor

### Debug Messages

The firmware outputs:
- `BambuBus_offline` - Lost connection to printer
- `BambuBus_online` - Connection established
- `Run_To_AMS_lite` - Recognized as AMS Lite (0x1200)
- `Run_To_AMS` - Recognized as AMS (0x0700)

### LED-Based Debugging

Quick visual debugging without serial:
- **Mainboard LED states** (see hardware-overview.md)
- **Channel LED patterns** indicate sensor status
- **Color combinations** show system state

## Project Structure

```
BMCU370/
├── platformio.ini          # PlatformIO configuration
├── README.md               # Project overview
├── LICENSE                 # GPL 2.0 license
├── .gitignore             # Git ignore rules
├── src/                   # Source code directory
│   ├── main.cpp           # Main program loop
│   ├── main.h             # Main header file
│   ├── BambuBus.cpp       # BambuBus protocol implementation
│   ├── BambuBus.h         # BambuBus header
│   ├── Motion_control.cpp # Motor control logic
│   ├── Motion_control.h   # Motor control header
│   ├── many_soft_AS5600.cpp # AS5600 Hall sensor driver
│   ├── many_soft_AS5600.h   # AS5600 header
│   ├── ADC_DMA.cpp        # ADC with DMA for analog sensors
│   ├── ADC_DMA.h          # ADC header
│   ├── Adafruit_NeoPixel.cpp # WS2812B LED driver
│   ├── Adafruit_NeoPixel.h   # NeoPixel header
│   ├── Flash_saves.cpp    # Flash memory management
│   ├── Flash_saves.h      # Flash header
│   ├── Debug_log.cpp      # Debug logging via USART3
│   ├── Debug_log.h        # Debug header
│   ├── time64.cpp         # 64-bit timestamp functions
│   └── time64.h           # Time header
├── docs/                  # Documentation (this directory)
└── .pio/                  # PlatformIO build artifacts (ignored)
```bash

## Development Workflow

### Typical Development Cycle

1. **Make code changes** in `src/` directory
2. **Build the project** to check for compile errors
3. **Fix any errors** reported by compiler
4. **Flash to device** for testing
5. **Monitor debug output** if needed
6. **Test functionality** with printer connection
7. **Iterate** as needed

### Version Control

```bash
# Create feature branch
git checkout -b feature/my-improvement

# Make changes and commit
git add .
git commit -m "Improve buffer detection logic"

# Push to repository
git push origin feature/my-improvement
```

### Code Style

**General Guidelines:**
- Use existing code style for consistency
- Comment complex logic (Chinese or English)
- Keep functions focused and modular
- Use meaningful variable names

**Existing Patterns:**
- Camelcase for functions: `Motion_control_init()`
- Uppercase for constants: `PULL_VOLTAGE_SEND_MAX`
- Arrays for multi-channel data: `speed_as5600[4]`

## Testing

### Unit Testing (Manual)

Test individual components:

**AS5600 Hall Sensors:**
```cpp
// In setup(), add:
MC_AS5600.updata_stu();
for (int i = 0; i < 4; i++) {
    DEBUG_MY("Channel ");
    DEBUG_float(i, 0);
    DEBUG_MY(" online: ");
    DEBUG_MY(MC_AS5600.online[i] ? "Yes\n" : "No\n");
}
```

**ADC Reading:**
```cpp
float *adc = ADC_DMA_get_value();
for (int i = 0; i < 8; i++) {
    DEBUG_MY("ADC[");
    DEBUG_float(i, 0);
    DEBUG_MY("]=");
    DEBUG_float(adc[i], 3);
    DEBUG_MY("\n");
}
```

### Integration Testing

Test with actual printer:
1. Flash firmware
2. Connect to printer
3. Load filament in each channel
4. Perform test print with filament changes
5. Monitor LED behavior and system logs

### Regression Testing

After changes:
- ✓ All four channels detect filament
- ✓ Buffer states correctly indicated
- ✓ Motors respond to commands
- ✓ BambuBus communication works
- ✓ Flash saves/restores filament data
- ✓ LED colors match status

## Common Development Tasks

### Modifying Buffer Thresholds

Edit in `Motion_control.cpp`:
```cpp
float PULL_voltage_up = 1.85f;   // High pressure threshold
float PULL_voltage_down = 1.45f; // Low pressure threshold
```

### Changing LED Brightness

Edit in `main.cpp` → `RGB_Set_Brightness()`:
```cpp
strip_PD1.setBrightness(35);      // Mainboard (0-255)
strip_channel[0].setBrightness(15); // Channels (0-255)
```

### Adjusting Motor Speed

Edit PWM values in `Motion_control.cpp`:
```cpp
Motion_control_set_PWM(channel, pwm_value); // 0-255
```

### Modifying BambuBus Settings

Edit in `BambuBus.cpp`:
```cpp
#define Bambubus_version 5  // Protocol version
```

## Troubleshooting Development Issues

### Build Errors

**"Platform not found"**
- Ensure internet connection (downloads platform from GitHub)
- Try: `pio platform install https://github.com/Community-PIO-CH32V/platform-ch32v.git`

**"CRC library not found"**
- Run: `pio lib install "robtillaart/CRC@^1.0.3"`

**"Board not found"**
- Check `platformio.ini` has correct board name
- Update platform: `pio platform update`

### Upload Errors

**"Port not found"**
- Check USB connection
- Install CH340 drivers
- Verify port in Device Manager (Windows) or `ls /dev/tty*` (Linux/macOS)

**"Upload failed"**
- Enter bootloader mode manually
- Try different USB cable
- Check TX/RX not swapped (external programmer)

### Runtime Issues

**"Watchdog resets"**
- Watchdog is disabled in firmware: `WWDG_DeInit()`
- If re-enabling, ensure loop() doesn't block too long

**"Flash write fails"**
- Check flash address: `0x0800F000`
- Ensure data structure alignment: `alignas(4)`
- Verify flash isn't write-protected

## Advanced Topics

### Custom Hardware Variants

Modify `platformio.ini` for different boards:
```ini
[env:custom_variant]
board = genericCH32V203C8T6
build_flags =
    -D SYSCLK_FREQ_144MHz_HSI=144000000
    -D CUSTOM_HARDWARE_V2
```

### Performance Optimization

**Clock Speed:**
- Current: 144 MHz HSI (internal oscillator)
- Alternative: HSE with external crystal (more stable)
- Modify: `SYSCLK_FREQ_144MHz_HSI` in build_flags

**DMA Usage:**
- ADC already uses DMA for efficiency
- USART3 debug uses DMA for TX
- Consider DMA for other high-speed transfers

### Porting to Other Microcontrollers

**Requirements for port:**
- 4x PWM outputs (motors)
- 8x ADC inputs (4x buffer, 4x detection)
- 4x I2C (software or hardware, for AS5600)
- 1x USART (BambuBus RS-485)
- 5x WS2812B outputs (5 RGB LED chains)
- Flash storage for filament data
- Adequate RAM (minimum 8KB)
- Processing power (100+ MHz recommended)

## Resources

### Official Documentation
- **CH32V Reference Manual:** [WCH Website](http://www.wch-ic.com/)
- **PlatformIO Docs:** https://docs.platformio.org/
- **Arduino Reference:** https://www.arduino.cc/reference/

### Community Resources
- **BMCU Wiki:** https://wiki.yuekai.fr/en/BMCU
- **Gitee Repository:** https://gitee.com/at_4061N/BMCU
- **Community Forums:** See wiki for links

### Tools
- **WCH-LinkUtility:** Official programming tool
- **Serial Monitor:** PlatformIO, PuTTY, screen
- **Logic Analyzer:** For protocol debugging
- **Oscilloscope:** For timing analysis

## Next Steps

After setting up your environment:
1. Read [Architecture Overview](architecture.md)
2. Study [BambuBus Protocol](bambubus-protocol.md)
3. Make your first code change and test!

---

**Happy Developing!** 🚀

For questions or issues, refer to the community resources or create an issue on GitHub.
