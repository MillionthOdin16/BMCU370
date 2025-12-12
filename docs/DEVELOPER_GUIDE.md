# BMCU 370 C Developer Guide

## Table of Contents
1. [Development Environment Setup](#development-environment-setup)
2. [Code Structure](#code-structure)
3. [Building and Flashing](#building-and-flashing)
4. [Debugging](#debugging)
5. [Adding New Features](#adding-new-features)
6. [API Reference](#api-reference)
7. [Contributing Guidelines](#contributing-guidelines)

---

## Development Environment Setup

### Prerequisites

- **Operating System**: Windows, Linux, or macOS
- **Python**: 3.6 or later
- **Git**: For version control
- **Text Editor**: VS Code recommended, any IDE with PlatformIO support

### Installing PlatformIO

#### VS Code Extension (Recommended)

1. Install Visual Studio Code
2. Open Extensions (Ctrl+Shift+X / Cmd+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install
5. Reload VS Code

#### Command Line Interface

```bash
# Install via pip
pip install platformio

# Verify installation
pio --version
```

### Setting Up the Project

```bash
# Clone repository
git clone https://github.com/MillionthOdin16/BMCU370.git
cd BMCU370

# Initialize PlatformIO (if needed)
pio init --ide vscode

# Install dependencies
pio lib install
```

### WCH-Link Programmer Setup

**Windows**:
1. Download WCH-Link driver from http://www.wch-ic.com/
2. Extract and run installer
3. Connect WCH-Link to USB port
4. Verify in Device Manager

**Linux**:
```bash
# Install udev rules
sudo wget https://github.com/openwch/ch32v003fun/raw/master/misc/99-WCH-LinkE.rules -O /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

**macOS**:
- USB drivers typically work out-of-box
- May require allowing system extension in Security & Privacy settings

---

## Code Structure

### Directory Layout

```
BMCU370/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── main.h             # Main header and common definitions
│   ├── BambuBus.cpp/h     # BambuBus protocol implementation
│   ├── Motion_control.cpp/h  # Motor control and PID
│   ├── many_soft_AS5600.cpp/h # Hall sensor I2C driver
│   ├── ADC_DMA.cpp/h      # ADC with DMA
│   ├── Flash_saves.cpp/h  # Flash memory operations
│   ├── Debug_log.cpp/h    # Serial debugging
│   ├── time64.cpp/h       # 64-bit timing
│   └── Adafruit_NeoPixel.cpp/h # RGB LED driver
├── docs/                   # Documentation
├── platformio.ini         # PlatformIO configuration
├── README.md              # Project overview
└── LICENSE                # GPL v2.0

```

### Module Dependencies

```
main.cpp
  ├── BambuBus (communication protocol)
  ├── Motion_control (motor management)
  │   ├── many_soft_AS5600 (Hall sensors)
  │   └── ADC_DMA (pressure/detection)
  ├── Flash_saves (non-volatile storage)
  ├── Debug_log (serial output)
  ├── time64 (precise timing)
  └── Adafruit_NeoPixel (RGB LEDs)
```

### Key Data Structures

#### Filament Information
```cpp
struct _filament {
    char ID[8];              // Filament ID (e.g., "GFG00")
    uint8_t color_R;         // Red component (0-255)
    uint8_t color_G;         // Green component (0-255)
    uint8_t color_B;         // Blue component (0-255)
    uint8_t color_A;         // Alpha/extra data
    int16_t temperature_min; // Min nozzle temp (°C)
    int16_t temperature_max; // Max nozzle temp (°C)
    char name[20];           // Material name
    float meters;            // Total filament used
    uint64_t meters_virtual_count; // Internal counter
    AMS_filament_stu statu;  // Online status enum
    AMS_filament_motion motion_set; // Motion command enum
    uint16_t pressure;       // Pressure reading
};
```

#### Flash Storage
```cpp
struct flash_save_struct {
    _filament filament[4];   // 4 channels
    int BambuBus_now_filament_num; // Active channel (0-3, 0xFF=none)
    uint8_t filament_use_flag; // State flags
    uint32_t version;        // Data version (must match Bambubus_version)
    uint32_t check;          // Validation (0x40614061)
};
```

#### Motor Control Configuration
```cpp
struct Motion_control_save_struct {
    int Motion_control_dir[4]; // Motor directions
    int check;                 // Validation (0x40614061)
};
```

---

## Building and Flashing

### Build Commands

#### Using PlatformIO CLI

```bash
# Clean build directory
pio run --target clean

# Build firmware
pio run

# Build with verbose output
pio run --verbose

# Check code size
pio run --target size
```

#### Using VS Code

1. Open PlatformIO sidebar (alien icon)
2. Expand "env:genericCH32V203C8T6"
3. Click desired action:
   - **Build**: Compile firmware
   - **Upload**: Flash to device
   - **Clean**: Remove build artifacts
   - **Upload and Monitor**: Flash and open serial monitor

### Uploading Firmware

#### Via PlatformIO
```bash
# Upload to device
pio run --target upload

# Upload with specific port (if multiple programmers)
pio run --target upload --upload-port /dev/ttyUSB0
```

#### Manual Upload (Using WCHISPTool)

1. Build firmware: `pio run`
2. Locate binary: `.pio/build/genericCH32V203C8T6/firmware.bin`
3. Open WCHISPTool
4. Select CH32V203C8T6
5. Load firmware.bin
6. Click "Download"

### Compiler Flags

Current configuration in `platformio.ini`:

```ini
[env:genericCH32V203C8T6]
platform = https://github.com/Community-PIO-CH32V/platform-ch32v.git
board = genericCH32V203C8T6
framework = arduino
lib_deps = robtillaart/CRC@^1.0.3
build_flags= -D SYSCLK_FREQ_144MHz_HSI=144000000
```

**Build Flags Explained**:
- `-D SYSCLK_FREQ_144MHz_HSI=144000000`: Overclocks MCU to 144 MHz

**Adding Custom Flags**:
```ini
build_flags = 
    -D SYSCLK_FREQ_144MHz_HSI=144000000
    -D DEBUG_ENABLED=1              # Enable debug features
    -D CUSTOM_FEATURE=1             # Your custom flag
    -Wall                            # Enable all warnings
    -Wextra                          # Extra warnings
```

---

## Debugging

### Serial Debug Output

#### Enable Debug Logging

In `Debug_log.h`:
```cpp
#define Debug_log_on  // Comment out to disable debug
#define Debug_log_baudrate 115200
```

#### Debug Macros

```cpp
DEBUG_init();              // Initialize debug UART
DEBUG_MY("Hello");         // Print string
DEBUG_num(data, len);      // Print data array
DEBUG_time();              // Print timestamp
uint64_t t = DEBUG_get_time(); // Get microseconds
```

#### Monitoring Serial Output

**PlatformIO**:
```bash
pio device monitor --baud 115200
```

**With filters**:
```bash
pio device monitor --baud 115200 --filter colorize
```

**Screen (Linux/Mac)**:
```bash
screen /dev/ttyUSB0 115200
```

**PuTTY (Windows)**:
- Serial line: COM#
- Speed: 115200

### Common Debug Patterns

#### Add Temporary Debug Output
```cpp
DEBUG_MY("Entering function X\n");
DEBUG_MY("Variable value: ");
DEBUG_num(&my_variable, 1);
DEBUG_MY("\n");
```

#### Timing Analysis
```cpp
uint64_t start = DEBUG_get_time();
// ... code to measure ...
uint64_t duration = DEBUG_get_time() - start;
DEBUG_MY("Execution time: ");
DEBUG_num(&duration, 1);
DEBUG_MY(" microseconds\n");
```

#### State Machine Debugging
```cpp
void Motion_control_run(int error) {
    static int last_state = -1;
    if (current_state != last_state) {
        DEBUG_MY("State change: ");
        DEBUG_num(&last_state, 1);
        DEBUG_MY(" -> ");
        DEBUG_num(&current_state, 1);
        DEBUG_MY("\n");
        last_state = current_state;
    }
    // ... rest of function ...
}
```

### Hardware Debugging

#### JTAG/SWD Debugging

Using WCH-Link with OpenOCD (advanced):

```bash
# Start OpenOCD server
openocd -f interface/wch-link.cfg -f target/ch32v20x.cfg

# In another terminal, start GDB
riscv-none-embed-gdb .pio/build/genericCH32V203C8T6/firmware.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) break main
(gdb) continue
```

#### LED Indicators for Debug

Add visual debugging:
```cpp
void debug_flash_led(int channel, int times) {
    for (int i = 0; i < times; i++) {
        MC_STU_RGB_set(channel, 255, 0, 0); // Red
        RGB_show_data();
        delay(200);
        MC_STU_RGB_set(channel, 0, 0, 0); // Off
        RGB_show_data();
        delay(200);
    }
}
```

---

## Adding New Features

### Example: Adding a New Filament State

#### 1. Define New State Enum

In `BambuBus.h`:
```cpp
enum class AMS_filament_motion {
    before_pull_back,
    need_pull_back,
    need_send_out,
    on_use,
    idle,
    my_new_state  // Add your state
};
```

#### 2. Handle State in Motion Control

In `Motion_control.cpp`:
```cpp
void Motion_control_run(int error) {
    for (int i = 0; i < 4; i++) {
        AMS_filament_motion motion = get_filament_motion(i);
        
        switch (motion) {
            // ... existing cases ...
            
            case AMS_filament_motion::my_new_state:
                // Your custom behavior
                handle_my_new_state(i);
                break;
        }
    }
}

void handle_my_new_state(int channel) {
    // Implement your logic
    DEBUG_MY("Handling new state for channel ");
    DEBUG_num(&channel, 1);
    DEBUG_MY("\n");
}
```

#### 3. Update BambuBus Handler

In `BambuBus.cpp`, add state transitions:
```cpp
case BambuBus_package_type::filament_motion_long:
    // Parse command from printer
    if (command_indicates_new_state) {
        set_filament_motion(channel, AMS_filament_motion::my_new_state);
    }
    break;
```

#### 4. Update LED Indicators

In `main.cpp`:
```cpp
void update_channel_leds() {
    for (int i = 0; i < 4; i++) {
        AMS_filament_motion motion = get_filament_motion(i);
        
        if (motion == AMS_filament_motion::my_new_state) {
            // Show special color (e.g., purple)
            MC_STU_RGB_set(i, 128, 0, 128);
        }
        // ... other states ...
    }
}
```

### Example: Adding Temperature Monitoring

#### 1. Add Hardware (if needed)

Connect temperature sensor to available ADC channel or I2C bus.

#### 2. Create Module

`Temperature_monitor.h`:
```cpp
#pragma once
#include "main.h"

extern void Temperature_init();
extern float Temperature_read(int channel);
extern bool Temperature_is_safe(int channel);
```

`Temperature_monitor.cpp`:
```cpp
#include "Temperature_monitor.h"

static float temps[4] = {0};

void Temperature_init() {
    // Initialize ADC or I2C for temperature sensors
}

float Temperature_read(int channel) {
    // Read sensor and convert to °C
    return temps[channel];
}

bool Temperature_is_safe(int channel) {
    float temp = Temperature_read(channel);
    return (temp < 80.0); // Example threshold
}
```

#### 3. Integrate in Main Loop

In `main.cpp`:
```cpp
#include "Temperature_monitor.h"

void setup() {
    // ... existing setup ...
    Temperature_init();
}

void loop() {
    // ... existing loop ...
    
    for (int i = 0; i < 4; i++) {
        if (!Temperature_is_safe(i)) {
            // Shut down motor or alert
            Motion_control_set_PWM(i, 0);
            MC_STU_RGB_set(i, 255, 0, 0); // Red warning
            DEBUG_MY("Temperature warning on channel ");
            DEBUG_num(&i, 1);
            DEBUG_MY("\n");
        }
    }
}
```

---

## API Reference

### BambuBus Module

#### Initialization
```cpp
void BambuBus_init();
```
Initializes UART communication and loads saved filament data from flash.

#### Main Loop Processing
```cpp
BambuBus_package_type BambuBus_run();
```
Processes incoming BambuBus packets. Call in main loop.
**Returns**: Type of packet received (or NONE if no packet).

#### Filament Status
```cpp
bool get_filament_online(int num);
```
Check if filament is present in channel.
**Parameters**: `num` - Channel (0-3)
**Returns**: `true` if filament online, `false` otherwise

```cpp
void set_filament_online(int num, bool if_online);
```
Manually set filament online status.
**Parameters**: 
- `num` - Channel (0-3)
- `if_online` - Online state

#### Motion Control
```cpp
AMS_filament_motion get_filament_motion(int num);
```
Get current motion command for channel.
**Returns**: Motion state enum

```cpp
void set_filament_motion(int num, AMS_filament_motion motion);
```
Set motion command for channel.
**Parameters**:
- `num` - Channel (0-3)
- `motion` - Desired motion state

#### Usage Tracking
```cpp
void reset_filament_meters(int num);
```
Reset usage counter to zero.

```cpp
void add_filament_meters(int num, float meters);
```
Increment usage counter (automatically called by motion control).

```cpp
float get_filament_meters(int num);
```
Get total filament used in meters.

#### Flash Operations
```cpp
bool Bambubus_read();
```
Load filament data from flash.
**Returns**: `true` if valid data found, `false` if invalid/missing

```cpp
void Bambubus_set_need_to_save();
```
Mark data as needing flash save (actual save happens periodically).

#### Utility
```cpp
int get_now_filament_num();
```
Get currently active channel.
**Returns**: 0-3 for active channel, 0xFF for none

```cpp
bool BambuBus_if_on_print();
```
Check if any channel is actively printing.
**Returns**: `true` if any channel not idle

### Motion Control Module

#### Initialization
```cpp
void Motion_control_init();
```
Initialize AS5600 sensors, ADC, motors, and load saved configuration.

#### Main Loop
```cpp
void Motion_control_run(int error);
```
Execute motor control logic. Call frequently (main loop).
**Parameters**: `error` - Error flags (future use)

#### Motor Control
```cpp
void Motion_control_set_PWM(uint8_t CHx, int PWM);
```
Directly set motor PWM.
**Parameters**:
- `CHx` - Channel (0-3)
- `PWM` - PWM value (-1000 to +1000, negative = reverse)

### AS5600 Hall Sensor Module

#### Class Definition
```cpp
class AS5600_soft_IIC_many {
public:
    void init(uint32_t *GPIO_SCL, uint32_t *GPIO_SDA, int num);
    void updata_stu();
    void updata_angle();
    
    bool *online;         // Sensor communication status
    _AS5600_magnet_stu *magnet_stu; // Magnet detection
    uint16_t *raw_angle;  // Raw angle (0-4095)
    int numbers;          // Number of sensors
};
```

#### Usage Example
```cpp
AS5600_soft_IIC_many sensors;
uint32_t scl_pins[] = {PB15, PB14, PB13, PB12};
uint32_t sda_pins[] = {PD0, PC15, PC14, PC13};

sensors.init(scl_pins, sda_pins, 4);

// In main loop
sensors.updata_angle();
if (sensors.online[0]) {
    uint16_t angle = sensors.raw_angle[0];
    // Use angle data...
}
```

### Flash Storage Module

#### Save Data
```cpp
bool Flash_saves(void *buf, uint32_t length, uint32_t address);
```
Write data to flash memory.
**Parameters**:
- `buf` - Pointer to data buffer
- `length` - Number of bytes to write
- `address` - Flash address (must be page-aligned)
**Returns**: `true` on success, `false` on failure

**Important**:
- Data must be 4-byte aligned
- Length must be multiple of 4
- Automatically erases target page before writing
- Address must be within flash range (0x08000000-0x0800FFFF)

### LED Control Module

#### Initialization
```cpp
void RGB_init();
```
Initialize all LED strips.

#### Set Brightness
```cpp
void RGB_Set_Brightness();
```
Apply brightness levels (defined in code).

#### Update LEDs
```cpp
void RGB_show_data();
```
Push buffered colors to LEDs (blocking operation ~1ms).

#### Set Channel Color
```cpp
void Set_MC_RGB(uint8_t channel, int num, uint8_t R, uint8_t G, uint8_t B);
```
Set RGB color for channel LED.
**Parameters**:
- `channel` - Channel (0-3)
- `num` - LED index (0 or 1)
- `R, G, B` - Color components (0-255)

**Macros**:
```cpp
MC_STU_RGB_set(channel, R, G, B);  // Set channel status LED
MC_PULL_ONLINE_RGB_set(channel, R, G, B);  // Set pull/online LED
```

### Debug Module

#### Logging Functions
```cpp
void DEBUG_MY(const char *text);      // Print string
void DEBUG_num(void *data, int num);  // Print data array
void DEBUG_time();                     // Print timestamp
uint64_t DEBUG_get_time();             // Get microseconds since boot
```

---

## Contributing Guidelines

### Code Style

#### Naming Conventions
- **Functions**: `Snake_case_with_Capitals` (existing style)
- **Variables**: `snake_case_lowercase`
- **Constants**: `UPPER_CASE_SNAKE`
- **Classes**: `PascalCase`

#### Formatting
- **Indentation**: 4 spaces (no tabs)
- **Braces**: Opening brace on same line
- **Line Length**: Aim for <100 characters

#### Comments
- Use `//` for single-line comments
- Use `/* */` for multi-line blocks
- Document public functions with purpose and parameters
- Chinese comments acceptable (this is a bilingual project)

### Git Workflow

#### Branch Strategy
```bash
# Create feature branch
git checkout -b feature/my-new-feature

# Make changes and commit
git add .
git commit -m "Add: description of changes"

# Push to remote
git push origin feature/my-new-feature
```

#### Commit Messages
Format: `Type: Brief description`

**Types**:
- `Add`: New feature or file
- `Fix`: Bug fix
- `Update`: Modify existing feature
- `Remove`: Delete code/file
- `Docs`: Documentation only
- `Refactor`: Code restructuring without behavior change

**Examples**:
```
Add: Temperature monitoring for motor protection
Fix: Filament detection threshold for dual microswitch mode
Update: PID parameters for smoother motion
Docs: Add API reference for BambuBus module
```

### Testing

#### Before Submitting PR

1. **Build Test**:
```bash
pio run
```
Ensure no compilation errors.

2. **Flash Test**:
```bash
pio run --target upload
```
Verify firmware uploads and boots.

3. **Functionality Test**:
- Test all 4 channels
- Verify filament detection
- Check motor operation
- Confirm LED behavior

4. **Integration Test** (if possible):
- Connect to Bambu printer
- Run test print
- Verify BambuBus communication

#### Regression Testing

Ensure existing features still work:
- Filament loading/unloading
- Color changes during print
- Flash save/restore
- LED indicators

### Pull Request Process

1. **Fork repository** (if not a collaborator)
2. **Create feature branch** from `main`
3. **Make changes** with clear commits
4. **Test thoroughly**
5. **Push to your fork/branch**
6. **Open Pull Request** with description:
   - What problem does it solve?
   - How was it tested?
   - Any breaking changes?

#### PR Template
```markdown
## Description
Brief summary of changes

## Motivation
Why is this change needed?

## Testing
How was this tested?
- [ ] Compiled successfully
- [ ] Tested on hardware
- [ ] Tested with printer
- [ ] No regressions

## Related Issues
Closes #123
```

### Documentation

When adding features:
1. Update relevant `.md` files in `docs/`
2. Add code comments for public APIs
3. Include usage examples
4. Note any configuration changes

---

## Best Practices

### Memory Management

**Flash Writes**: Minimize due to limited write cycles (~10,000)
```cpp
// Bad: Save on every update
void update_value(int val) {
    data.value = val;
    Bambubus_save();  // Wears flash!
}

// Good: Mark for periodic save
void update_value(int val) {
    data.value = val;
    Bambubus_set_need_to_save();  // Saves once later
}
```

**Stack Usage**: Limited to ~20KB RAM total
```cpp
// Bad: Large stack allocation
void process() {
    uint8_t huge_buffer[10000];  // Stack overflow!
}

// Good: Use static or global
static uint8_t huge_buffer[10000];
void process() {
    // Use huge_buffer...
}
```

### Timing Considerations

**Non-Blocking Operations**: Avoid long delays in main loop
```cpp
// Bad: Blocks everything
void update_leds() {
    delay(100);  // Nothing else runs!
    RGB_show_data();
}

// Good: Check timer
static uint64_t last_update = 0;
void update_leds() {
    uint64_t now = DEBUG_get_time();
    if (now - last_update >= 100000) {  // 100ms
        RGB_show_data();
        last_update = now;
    }
}
```

### Error Handling

**Validate Inputs**:
```cpp
void set_filament_online(int num, bool if_online) {
    if (num < 0 || num >= 4) return;  // Guard against invalid channel
    // ... rest of function ...
}
```

**Check Return Values**:
```cpp
bool success = Flash_saves(&data, sizeof(data), address);
if (!success) {
    DEBUG_MY("Flash save failed!\n");
    // Handle error...
}
```

---

**Document Version**: 1.0  
**Last Updated**: December 2024  
**Compatible Firmware**: BMCU-C 370 Hall V0.1-0020
