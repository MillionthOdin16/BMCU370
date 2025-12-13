# Hardware Overview - BMCU-C 370 Hall Version

## Introduction

The BMCU-C (Bambu Multi-Color Unit - Version C) is a four-channel filament management system designed for Bambu Lab 3D printers. This version uses 370 motors and Hall effect sensors for improved performance and reliability compared to earlier versions.

## What is BMCU-C?

BMCU-C represents the latest generation of the BMCU project. Built on the solid foundation of BMCU-B v3.14, it incorporates significant improvements in both hardware and functionality:

- **370 motors (24V 6000RPM)** for faster and more powerful filament handling
- **AS5600 Hall effect sensors** for accurate position and buffer monitoring
- **Bi-directional buffer system** to prevent AMS Hub ejection issues
- **Smart features** including automatic feed on buffer press and retraction on pull-out

## System Architecture

### Main Components

#### 1. Mainboard (Control PCB)
**Microcontroller:** CH32V203C8T6
- **Architecture:** 32-bit RISC-V
- **Clock Speed:** 144 MHz (using HSI - High-Speed Internal oscillator)
- **Flash Memory:** Uses address 0x0800F000 for persistent storage
- **Communication:** RS-485 BambuBus protocol

**Key Features:**
- USB Type-C for programming and power (some variants)
- CH340 USB-to-TTL chip for easy firmware flashing (on some PCB versions)
- Diode protection for microcontroller safety
- DCDC voltage regulation supporting both 12V and 24V motors
- Watchdog timer disabled in firmware for stability

#### 2. Sub-boards (4 units - one per channel)
Each sub-board handles one filament channel and includes:

**AS5600 Hall Effect Sensor:**
- **Type:** 12-bit contactless magnetic rotary position sensor
- **Interface:** I2C (Software I2C implementation)
- **Purpose:** Monitors both filament rotation and buffer position
- **I2C Address:** 0x36
- **Pin Configuration (per channel):**
  - Channel 1: SCL=PB15, SDA=PD0
  - Channel 2: SCL=PB14, SDA=PC15
  - Channel 3: SCL=PB13, SDA=PC14
  - Channel 4: SCL=PB12, SDA=PC13

**ADC Channels (DMA-based):**
- **Buffer Pressure Sensor:** Monitors pneumatic buffer pressure (1.45V - 1.85V range)
- **Filament Detection:** Detects filament presence via voltage levels
- **Voltage Thresholds:**
  - High pressure (red light): > 1.85V
  - Normal range: 1.45V - 1.85V
  - Low pressure (blue light): < 1.45V
  - Filament online: > 1.65V

**WS2812B RGB LEDs:**
- Side LEDs (4020 package): 2 per channel for status indication
- Top LED (fiber optic): Shows channel and buffer status
- Brightness: 15/255 (channel LEDs), 35/255 (mainboard LED)

#### 3. Motors (4 units - one per channel)
**Specifications:**
- **Type:** 370 DC motor
- **Voltage:** 24V
- **Speed:** 6000 RPM no-load
- **Gearing:** BMG (Bondtech-style) dual-drive gears
- **Control:** PWM via motor driver

**Advantages over 130/180 motors:**
- Higher torque for better grip
- Faster filament loading/unloading
- Better resistance to feeding pressure

**Considerations:**
- More noise during operation
- Risk of "filament chewing" if gear mesh is too tight
- May push against five-pass connector if buffer isn't properly adjusted

#### 4. Buffer System
**Design:** Bi-directional pneumatic buffer

**Components:**
- Slider/piston with radial magnet
- Pneumatic connector (typically PC4-M5 or PC4-M6)
- Spring mechanism
- Hall sensor for position detection

**Function:**
- Monitors buffer position via Hall sensor
- Detects high/low pressure states
- Enables smart features:
  - Press buffer head → automatic filament insertion
  - Pull buffer head out → automatic filament retraction

**Voltage States:**
- **> 1.85V:** High pressure - red light, active feed
- **1.45V - 1.85V:** Normal operation
- **< 1.45V:** Low pressure - blue light

#### 5. Power System
**Input:** 24V DC (from printer's AMS hub connector)

**Power Distribution:**
- 24V to motors (via PWM control)
- 24V to mainboard
- Regulated voltages for microcontroller and sensors
- Some PCB variants include DCDC converter for 12V/24V switching

**Connector:** 6-pin AMS Hub Buffer connector
- Pins: A (data), B (data), GND, 24V

#### 6. Communication Interface
**Protocol:** BambuBus (RS-485 based)

**Device Types Supported:**
- AMS (0x0700)
- AMS Lite (0x1200)

**Connection:**
- Differential pair (A, B) for RS-485
- Connects to printer via 6-pin cable
- PMOS floating ground protection (latest PCB revisions)

## Physical Layout

### Per-Channel Module Structure
```
┌─────────────────────────────────┐
│  Front Cover (M2×8 screws)      │
├─────────────────────────────────┤
│  Filament Entry                 │
│  ↓                              │
│  [RGB LED - Side]               │
│  [Filament Detection Sensor]    │
│  ↓                              │
│  [BMG Dual Gears]               │
│  ↓ ← [370 Motor + AS5600]       │
│  [Buffer Chamber]               │
│  ↓                              │
│  [Slider + Hall Sensor]         │
│  ↓                              │
│  Pneumatic Connector            │
│  ↓                              │
│  Filament Exit                  │
└─────────────────────────────────┘
```

### Four-Channel Assembly
The BMCU-C uses a side-by-side structure without integrated filament storage, allowing:
- Placement in sealed dry boxes
- Flexible filament spool positioning
- Reduced footprint compared to AMS

## LED Status Indicators

### Mainboard LED (PD1)
- **Blue:** Normal communication with printer
- **Red:** Offline or communication error
- **Other colors:** Abnormal state

### Channel Side LEDs (per channel)
- **Off (Black):** No filament inserted
- **White:** Filament online
- **Blue:** Buffer pressed (slider pushed in)
- **Red:** Buffer pulled out (slider extended)

### Channel Top LEDs (Fiber Optic)
- **Blue:** Normal standby state
- **White:** Channel selected/in use
- **Red:** Error - Hall sensor not detecting magnet

## Compatibility

### Supported Printers
**Native Support (as AMS Lite):**
- Bambu Lab A1
- Bambu Lab A1 mini

**With Modifications:**
- Bambu Lab P-series (requires additional configuration)
- Potentially other printers using multi-BMCU setups

### Limitations
- **No NFC/RFID support** - Cannot read Bambu Lab original filament tags
- **Transparent filament detection** may be unreliable
- **Future firmware compatibility** not guaranteed by Bambu Lab

## Comparison with Other Versions

| Feature | BMCU-A (130) | BMCU-B (370 Steel Ball) | BMCU-C (370 Hall) |
|---------|-------------|------------------------|-------------------|
| Motor Type | 130/180 | 370 | 370 |
| Detection | Photoelectric | Photoelectric + Steel Ball | Hall Sensor |
| Buffer | One-way | One-way | Bi-directional |
| Speed | Slower | Fast | Fast |
| Noise | Quieter | Moderate | Moderate |
| Complexity | Higher | Medium | Medium |
| Smart Features | No | Limited | Yes (auto feed/retract) |
| 99% Stuck Issue | More common | Rare | Very rare |
| Hub Ejection Risk | Low | Moderate | Very low |

## Physical Specifications

**Dimensions:** (Varies by 3D printed case design)
- Channel module: ~40mm × 40mm × 120mm (approximate)
- Four-channel assembly: ~160mm × 40mm × 120mm

**Weight:** ~200-300g per channel (with motor and hardware)

**Mounting:** M2 and M3 screws for assembly

## Environmental Considerations

**Operating Temperature:** 0°C to 50°C (recommended)
**Humidity:** Can be housed in dry box for moisture control
**Dust Protection:** Enclosed design protects internals

## Maintenance Points

- Gear cleaning (prevent metal shavings buildup with metal gears)
- Hall sensor magnet alignment check
- Buffer system lubrication
- LED functionality verification
- Electrical connection inspection

## Safety Features

- Diode protection on microcontroller (PCB variant dependent)
- Watchdog timer disabled to prevent unexpected resets
- PMOS floating ground protection for RS-485
- Firmware-based error detection and LED indication

## Next Steps

- See [Bill of Materials](../hardware/bom.md) for component list
- See [Installation Guide](installation.md) for assembly instructions
- See [Troubleshooting](troubleshooting.md) for common issues
