# System Architecture Overview

## Introduction

This document provides a comprehensive overview of the BMCU-C 370 Hall version firmware architecture, explaining how different components work together to provide multi-filament management for Bambu Lab 3D printers.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         Bambu Lab Printer                        │
│                         (A1 / A1 mini)                          │
└────────────────┬───────────────────────────────────────────────┘
                 │ BambuBus Protocol (RS-485)
                 │ 6-pin cable (A, B, GND, 24V)
                 │
┌────────────────▼──────────────────────────────────────────────┐
│                    BMCU-C Mainboard                            │
│                   (CH32V203C8T6)                               │
│  ┌──────────────────────────────────────────────────────────┐ │
│  │  Main Loop (main.cpp)                                     │ │
│  │  • BambuBus communication                                 │ │
│  │  • Motion control orchestration                           │ │
│  │  • System status monitoring                               │ │
│  └─────┬──────────────┬──────────────┬──────────────┬────────┘ │
│        │              │              │              │          │
│  ┌─────▼────┐  ┌──────▼─────┐ ┌─────▼──────┐ ┌────▼─────┐   │
│  │BambuBus  │  │  Motion    │ │   Flash    │ │  RGB     │   │
│  │ Module   │  │  Control   │ │   Storage  │ │  LEDs    │   │
│  └─────┬────┘  └──────┬─────┘ └─────┬──────┘ └────┬─────┘   │
│        │              │              │              │          │
└────────┼──────────────┼──────────────┼──────────────┼─────────┘
         │              │              │              │
         │      ┌───────▼──────┐       │              │
         │      │   AS5600     │       │              │
         │      │ Hall Sensors │       │              │
         │      │   (4 units)  │       │              │
         │      └───────┬──────┘       │              │
         │              │              │              │
         │      ┌───────▼──────┐       │              │
         │      │  ADC + DMA   │       │              │
         │      │   (8 ch)     │       │              │
         │      └───────┬──────┘       │              │
         │              │              │              │
         │      ┌───────▼──────┐       │       ┌──────▼─────┐
         │      │   Motors     │       │       │  WS2812B   │
         │      │  (370 4x)    │       │       │  (5 strips)│
         │      └──────────────┘       │       └────────────┘
         │                             │
    [Printer]                    [Persistent
                                  Settings]
```

## Core Modules

### 1. Main Control Loop (main.cpp/main.h)

**Purpose:** Central control and coordination

**Key Responsibilities:**
- System initialization
- Main event loop
- Component orchestration
- RGB LED management
- Status monitoring

**Initialization Sequence:**
```cpp
void setup() {
    1. Disable watchdog timer
    2. Enable GPIO remapping (PD01)
    3. Initialize RGB LEDs
    4. Set RGB brightness levels
    5. Initialize BambuBus communication
    6. Initialize debug logging (if enabled)
    7. Initialize motion control system
}
```

**Main Loop Flow:**
```cpp
void loop() {
    while (1) {
        1. Run BambuBus communication
        2. Check packet type received
        3. Update error status
        4. Refresh system LEDs (every 3s)
        5. Log status changes
        6. Execute motion control
    }
}
```

**Global State:**
- `channel_colors[4][4]` - Filament RGBA colors
- `channel_runs_colors[4][2][3]` - Current RGB states (prevents unnecessary updates)
- `MC_STU_ERROR[4]` - Channel error flags

### 2. BambuBus Communication Module (BambuBus.cpp/BambuBus.h)

**Purpose:** Handle communication with Bambu Lab printer

**Protocol Details:**
- **Physical Layer:** RS-485 differential signaling
- **Data Format:** Custom binary protocol
- **Start Byte:** 0x3D
- **CRC:** CRC8 (polynomial 0x39, initial 0x66) + CRC16
- **Device Types:** 
  - AMS (0x0700)
  - AMS Lite (0x1200)

**Packet Types:**
```cpp
enum class BambuBus_package_type {
    ERROR = -1,              // Communication error
    NONE = 0,                // No packet
    filament_motion_short,   // Short motion command
    filament_motion_long,    // Long motion command
    online_detect,           // Device detection
    REQx6,                   // Request type 6
    NFC_detect,              // NFC read (not supported)
    set_filament_info,       // Set filament data
    MC_online,               // Channel online status
    read_filament_info,      // Read filament data
    set_filament_info_type2, // Alt filament set
    version,                 // Version query
    serial_number,           // Serial number query
    heartbeat,               // Keepalive packet
    ETC                      // Other packets
};
```

**Filament Data Structure:**
```cpp
struct _filament {
    char ID[8];              // Material ID (e.g., "GFG00")
    uint8_t color_R;         // Red component (0-255)
    uint8_t color_G;         // Green component
    uint8_t color_B;         // Blue component
    uint8_t color_A;         // Alpha/transparency
    int16_t temperature_min; // Min temp (e.g., 220°C)
    int16_t temperature_max; // Max temp (e.g., 240°C)
    char name[20];           // Material name (e.g., "PETG")
    float meters;            // Meters used
    uint64_t meters_virtual_count; // Virtual counter
    AMS_filament_stu statu;  // Status (offline/online/NFC_waiting)
    AMS_filament_motion motion_set; // Motion command
    uint16_t pressure;       // Pressure value
};
```

**Flash Storage:**
- **Address:** 0x0800F000
- **Version:** 5 (Bambubus_version)
- **Structure:** 4 filaments + metadata
- **Checksum:** 0x40614061
- **Persistence:** Survives power cycles

**Key Functions:**
- `BambuBus_init()` - Initialize communication
- `BambuBus_run()` - Process incoming packets (returns packet type)
- `Bambubus_read()` - Load filament data from flash
- `Bambubus_save()` - Save filament data to flash
- `get_filament_online(num)` - Check if filament present
- `set_filament_motion(num, motion)` - Set channel motion state

### 3. Motion Control Module (Motion_control.cpp/Motion_control.h)

**Purpose:** Manage motors and filament movement

**Components:**
- AS5600 Hall sensors (4x)
- ADC channels (8x via DMA)
- PWM motor control (4x)
- Buffer monitoring
- Filament detection

**AS5600 Configuration:**
```cpp
// Per-channel I2C pins
Channel 1: SCL=PB15, SDA=PD0
Channel 2: SCL=PB14, SDA=PC15
Channel 3: SCL=PB13, SDA=PC14
Channel 4: SCL=PB12, SDA=PC13
```

**Buffer Monitoring:**
```cpp
// Voltage thresholds
PULL_voltage_up = 1.85f;    // High pressure (red LED)
PULL_voltage_down = 1.45f;  // Low pressure (blue LED)

// Buffer states (MC_PULL_stu[])
 1 = High pressure (feed filament)
 0 = Normal pressure
-1 = Low pressure (reduce feed)
```

**Filament Detection:**
```cpp
// Single microswitch mode (is_two = false)
> 1.65V = Filament online
< 1.65V = Filament offline

// Dual microswitch mode (is_two = true)
< 0.6V  = Offline
1.4-1.7V = Outer switch (assist feed needed)
> 2.2V  = Both switches (fully loaded)
```

**Motor Control States:**
```cpp
enum class AMS_filament_motion {
    before_pull_back,  // Preparing to retract
    need_pull_back,    // Retract filament
    need_send_out,     // Send filament
    on_use,            // Actively printing
    idle               // No action
};
```

**Smart Features:**
- **Auto-feed on buffer press:** Detects slider push, assists loading
- **Auto-retract on buffer pull:** Detects slider pull, retracts filament
- **Pressure-based feed:** Adjusts motor based on buffer pressure
- **Distance tracking:** Monitors meters used per filament

**Key Functions:**
- `Motion_control_init()` - Initialize motors and sensors
- `Motion_control_run(error)` - Main motion control logic
- `Motion_control_set_PWM(channel, pwm)` - Set motor speed
- `MC_PULL_ONLINE_read()` - Read ADC values
- `MC_AS5600_read()` - Read Hall sensor angles

### 4. AS5600 Hall Sensor Driver (many_soft_AS5600.cpp/h)

**Purpose:** Read magnetic rotary position sensors

**Implementation:** Software I2C (bit-banged)
- Allows multiple I2C buses (one per sensor)
- More flexible GPIO assignment
- Trade-off: slightly slower than hardware I2C

**Sensor Specifications:**
- **Resolution:** 12-bit (4096 positions)
- **I2C Address:** 0x36
- **Registers:**
  - 0x0B: Status register
  - 0x0C: Raw angle (12-bit)
  - 0x0E: Angle (12-bit with filtering)
  - 0x1A: AGC (Automatic Gain Control)
  - 0x1B: Magnitude

**Magnet Status:**
```cpp
enum _AS5600_magnet_stu {
    low = 1,      // Magnet too weak
    high = 2,     // Magnet too strong
    offline = -1, // No magnet detected
    normal = 0    // Optimal range
};
```

**Usage:**
- **Filament rotation:** Measures how much filament has moved
- **Buffer position:** Detects slider in/out state
- **Speed calculation:** Derives rotation speed from angle changes

**Key Functions:**
- `init(SCL_pins[], SDA_pins[], count)` - Initialize sensors
- `updata_stu()` - Read magnet status
- `updata_angle()` - Read current angles
- `online[]` - Array indicating sensor connectivity
- `raw_angle[]` - Array of current positions

### 5. ADC + DMA Module (ADC_DMA.cpp/h)

**Purpose:** Analog-to-digital conversion for sensors

**Configuration:**
- **Channels:** 8 (4x buffer pressure + 4x filament detection)
- **Resolution:** 12-bit (0-4095)
- **Voltage Range:** 0-3.3V
- **Transfer:** DMA (Direct Memory Access) for efficiency
- **Conversion:** Continuous, triggered automatically

**DMA Benefits:**
- No CPU intervention during conversion
- Always-updated values
- Lower latency
- Freed CPU for other tasks

**Output:**
- Returns float array with voltage values (0.0 - 3.3V)
- Indexed by channel (0-7)

**Mapping:**
```
ADC[0] = Channel 4 buffer pressure
ADC[1] = Channel 4 filament detection
ADC[2] = Channel 3 buffer pressure
ADC[3] = Channel 3 filament detection
ADC[4] = Channel 2 buffer pressure
ADC[5] = Channel 2 filament detection
ADC[6] = Channel 1 buffer pressure
ADC[7] = Channel 1 filament detection
```

**Key Functions:**
- `ADC_DMA_init()` - Initialize ADC and DMA
- `ADC_DMA_get_value()` - Get current voltage readings

### 6. RGB LED Module (Adafruit_NeoPixel.cpp/h + main.cpp)

**Purpose:** Visual status indication

**LED Configuration:**
```cpp
// Per-channel side LEDs (PA11, PA8, PB1, PB0)
LED_PA11_NUM = 2  // Channel 1
LED_PA8_NUM = 2   // Channel 2
LED_PB1_NUM = 2   // Channel 3
LED_PB0_NUM = 2   // Channel 4

// Mainboard LED (PD1)
LED_PD1_NUM = 1   // System status
```

**LED Type:** WS2812B (NeoPixel)
- **Protocol:** Single-wire serial
- **Format:** GRB (Green-Red-Blue order)
- **Timing:** 800 kHz
- **Control:** Bit-banged PWM
- **Brightness:** Software-controlled (0-255)

**Brightness Settings:**
```cpp
Mainboard: 35/255 (~14%)
Channels:  15/255 (~6%)
```
*Note: Reduced to prevent overheating and power consumption*

**LED Functions:**
- `RGB_init()` - Initialize all LED strips
- `RGB_Set_Brightness()` - Set brightness levels
- `RGB_show_data()` - Update all LEDs
- `Set_MC_RGB(channel, num, R, G, B)` - Set channel LED
- `Show_SYS_RGB(status)` - Update system LED

**Status Display Logic:**
- Only updates when color changes (reduces bus traffic)
- 3-second refresh interval for system LED
- Immediate update for status changes
- Error channels forced to red every refresh

### 7. Flash Storage Module (Flash_saves.cpp/h)

**Purpose:** Non-volatile data persistence

**Flash Specifications:**
- **Address:** 0x0800F000
- **Page Size:** 4096 bytes (4KB)
- **Erase Granularity:** Page (must erase full page)
- **Write Granularity:** Half-word (16-bit)

**Storage Structure:**
```cpp
struct flash_save_struct {
    _filament filament[4];           // Filament data
    int BambuBus_now_filament_num;   // Current active channel
    uint8_t filament_use_flag;       // Usage flags
    uint32_t version;                // Data version
    uint32_t check;                  // Checksum (0x40614061)
} __attribute__((aligned(4)));
```

**Write Process:**
1. Disable interrupts (critical section)
2. Unlock flash
3. Erase pages
4. Write data (16-bit at a time)
5. Lock flash
6. Re-enable interrupts

**Data Integrity:**
- Version check ensures compatibility
- Checksum validates data
- Aligned to 4-byte boundary
- Atomicity via interrupt disable

**Key Functions:**
- `Flash_saves(data, length, address)` - Write to flash
- `Bambubus_read()` - Read from flash (in BambuBus.cpp)
- `Bambubus_save()` - Trigger save operation

### 8. Debug Logging Module (Debug_log.cpp/h)

**Purpose:** Development and troubleshooting support

**Configuration:**
```cpp
#define Debug_log_on  // Enable/disable compilation
```

**USART3 Setup:**
- **Pins:** TX=PB10, RX=PB11
- **Baud Rate:** Configurable (Debug_log_baudrate)
- **Word Length:** 9-bit
- **Parity:** Even
- **Stop Bits:** 1
- **DMA:** Enabled for TX (channel 2)

**DMA Benefits:**
- Non-blocking transmission
- CPU continues execution
- Large buffer support
- Efficient data transfer

**Debug Macros:**
```cpp
DEBUG_MY(string)        // Print string
DEBUG_float(value, decimals) // Print float
```

**Key Functions:**
- `Debug_log_init()` - Initialize USART3 + DMA
- `Debug_log_write(data)` - Send string
- `Debug_log_write_num(data, num)` - Send N bytes

### 9. Time Management (time64.cpp/h)

**Purpose:** 64-bit timestamps for timing

**Functions:**
- `get_time64()` - Get current millisecond timestamp
- Prevents 32-bit overflow (49 days)
- Used for intervals and timeouts

## Data Flow

### Startup Sequence

```
1. Power On / Reset
   ↓
2. setup()
   ├─ Disable Watchdog
   ├─ Initialize RGB LEDs
   ├─ Initialize BambuBus (load flash data)
   ├─ Initialize Debug Log
   └─ Initialize Motion Control
       ├─ Initialize AS5600 sensors
       └─ Initialize ADC + DMA
   ↓
3. loop()
   └─ (continuous operation)
```

### Main Loop Data Flow

```
┌─────────────────────────────────────────────┐
│  Main Loop (continuous)                      │
└─────────────────┬───────────────────────────┘
                  │
        ┌─────────▼──────────┐
        │  BambuBus_run()    │
        │  • Receive packet  │
        │  • Parse protocol  │
        │  • Update state    │
        └─────────┬──────────┘
                  │
        ┌─────────▼──────────┐
        │  Check Status      │
        │  • Online/Offline  │
        │  • Packet Type     │
        └─────────┬──────────┘
                  │
        ┌─────────▼──────────┐
        │  Update LEDs       │
        │  • System status   │
        │  • Error channels  │
        └─────────┬──────────┘
                  │
        ┌─────────▼──────────┐
        │Motion_control_run()│
        │  • Read sensors    │
        │  • Update motors   │
        │  • Handle motions  │
        └─────────┬──────────┘
                  │
                  └──────────┐
                             │
                  ┌──────────▼──────────┐
                  │  Repeat Loop        │
                  └─────────────────────┘
```

### Filament Loading Sequence

```
1. Filament Inserted
   ↓
2. ADC detects voltage > 1.65V
   ↓
3. set_filament_online(channel, true)
   ↓
4. Side LED turns WHITE
   ↓
5. BambuBus reports to printer
   ↓
6. Printer sends "need_send_out" command
   ↓
7. Motor activates (PWM control)
   ↓
8. AS5600 monitors rotation
   ↓
9. Buffer pressure monitored
   ↓
10. Filament reaches printer
    ↓
11. Motion state → "on_use"
    ↓
12. Top LED turns WHITE (active)
```

## Memory Map

### Flash Memory Layout

```
0x08000000  ┌─────────────────────┐
            │  Bootloader         │
            ├─────────────────────┤
            │  Firmware Code      │
            │  (.text section)    │
            ├─────────────────────┤
            │  Read-only Data     │
            │  (.rodata section)  │
0x0800F000  ├─────────────────────┤
            │  Persistent Data    │  ← Filament storage
            │  (4KB)              │
0x08010000  └─────────────────────┘
```

### RAM Layout

```
0x20000000  ┌─────────────────────┐
            │  Stack              │
            ├─────────────────────┤
            │  Heap               │
            ├─────────────────────┤
            │  .bss (uninit)      │
            ├─────────────────────┤
            │  .data (init)       │
            │  • Global variables │
            │  • Arrays           │
0x20005000  └─────────────────────┘
```

## Timing and Performance

### System Clock
- **Frequency:** 144 MHz
- **Source:** HSI (High-Speed Internal oscillator)
- **Stability:** ±1% accuracy

### Loop Timing
- **BambuBus packet processing:** ~1-10ms (varies by packet)
- **Motion control update:** ~50-100ms typical
- **ADC conversion:** Continuous (DMA-driven)
- **LED update:** On-change only (reduces overhead)

### Critical Timing
- **WS2812B protocol:** 800 kHz ±10%
- **AS5600 I2C:** ~100 kHz (software implementation)
- **Motor PWM:** Varies (motion-dependent)

## Interrupt Priorities

```
Priority 0 (Highest)
  └─ (None currently)

Priority 1
  ├─ USART1 (BambuBus RX)
  └─ USART3 (Debug RX)

Priority 2
  └─ DMA transfers

(Default)
  └─ Other peripherals
```

## Error Handling

### Error Detection
1. **BambuBus communication loss** → MC_STU_ERROR flag set
2. **Hall sensor offline** → magnet_stu = offline
3. **Flash write failure** → Returns false
4. **ADC out of range** → Monitored in motion control

### Error Response
- **Visual:** Red LEDs on affected channels
- **Functional:** Motors stop, safe state
- **Logging:** Debug messages (if enabled)
- **Recovery:** Automatic retry on next loop

### Watchdog
- **Status:** Disabled (`WWDG_DeInit()`)
- **Reason:** Prevents unwanted resets during development
- **Production:** Consider re-enabling with proper timing

## Power Management

### Power Budget
- **Microcontroller:** ~50mA
- **Motors (peak):** 4x 1-2A = 4-8A
- **LEDs:** 5x 60mA max = 300mA
- **Sensors:** 4x AS5600 ~10mA = 40mA
- **Total Peak:** ~8.4A @ 24V

### Power Optimization
- **LED brightness reduced** to limit current
- **DMA reduces CPU load** → Lower power
- **Motors only active when needed**
- **Sleep modes:** Not currently implemented

## Security Considerations

### Flash Protection
- Write protection available (not currently enabled)
- Could prevent accidental firmware corruption

### Communication
- CRC validation on BambuBus packets
- No encryption (RS-485 is local connection)

### Data Integrity
- Checksum on flash data
- Version validation
- Alignment requirements enforced

## Extensibility

### Adding New Features

**New Sensor:**
1. Add driver in new .cpp/.h files
2. Initialize in setup()
3. Read in motion_control_run()
4. Integrate into control logic

**New LED Pattern:**
1. Add function in main.cpp
2. Call from appropriate trigger point
3. Consider update frequency

**Protocol Extension:**
1. Add packet type to enum
2. Implement parser in BambuBus.cpp
3. Handle in main loop switch

### Firmware Updates
- Flash via USB-TTL or WCH-LinkE
- Firmware size: ~40-60KB (plenty of space)
- Flash data preserved at 0x0800F000

## Debugging Strategies

### LED-Based Debugging
- Mainboard LED: Communication status
- Top LEDs: Hall sensor status
- Side LEDs: Detection and buffer state

### Serial Debugging
- Enable Debug_log_on
- Connect USART3 (PB10/PB11)
- Monitor packet flow and state changes

### Logic Analyzer
- Monitor BambuBus A/B lines
- Check I2C timing (AS5600)
- Verify WS2812B signals

## Next Steps

- [BambuBus Protocol](bambubus-protocol.md) - Deep dive into communication
- [Hall Sensor System](hall-sensor.md) - AS5600 details
- [Motor Control](motor-control.md) - Motion control algorithms
- [Code Reference](code-reference.md) - API documentation

---

This architecture enables reliable, multi-channel filament management with smart buffer control and seamless printer integration.
