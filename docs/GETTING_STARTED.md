# BMCU 370 C Getting Started Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Hardware Requirements](#hardware-requirements)
3. [Software Requirements](#software-requirements)
4. [Assembly and Wiring](#assembly-and-wiring)
5. [Firmware Installation](#firmware-installation)
6. [Initial Configuration](#initial-configuration)
7. [Testing and Verification](#testing-and-verification)
8. [Integration with Bambu Lab Printer](#integration-with-bambu-lab-printer)

---

## Introduction

This guide will help you set up and configure your BMCU 370 C (Hall Version) multi-filament controller for use with Bambu Lab 3D printers. The BMCU 370 C provides 4-channel filament management with Hall effect sensor-based position tracking.

### What You'll Need
- BMCU 370 C hardware board
- 4× AS5600 Hall sensors with magnets (pre-installed on PCB)
- Compatible Bambu Lab 3D printer (P1P, P1S, X1C, etc.)
- Computer with USB port for programming
- Basic soldering skills (if assembly required)

### Expected Time
- **First-time setup**: 1-2 hours
- **Firmware update**: 15-30 minutes

---

## Hardware Requirements

### BMCU 370 C Board Components

**Included on PCB**:
- CH32V203C8T6 microcontroller
- 4× AS5600 Hall effect sensors
- 4× stepper motor drivers
- 8× ADC input circuits (pressure and detection)
- WS2812B RGB LEDs (1 main board + 8 channel indicators)
- BambuBus communication interface

**Required Accessories**:
- **Power Supply**: 12V or 24V DC (check your board specifications)
- **BambuBus Cable**: For connection to Bambu Lab printer
- **Filament Tubes**: PTFE tubing for each channel
- **Mounting Hardware**: Screws, standoffs, enclosure (optional)

### AS5600 Hall Sensor Setup

Each filament channel requires proper Hall sensor alignment:

1. **Magnet Positioning**:
   - Diametric magnets preferred (not axial)
   - Distance from sensor: 0.5-3mm (optimal: 1mm)
   - Centered over sensor IC

2. **Mechanical Assembly**:
   - Mount sensors on drive gears or rollers
   - Ensure magnet rotates freely without wobble
   - Verify clearance for full 360° rotation

### Pinout Reference

**AS5600 Hall Sensors**:
| Channel | SCL Pin | SDA Pin | Function |
|---------|---------|---------|----------|
| CH1     | PB15    | PD0     | Software I2C bus 1 |
| CH2     | PB14    | PC15    | Software I2C bus 2 |
| CH3     | PB13    | PC14    | Software I2C bus 3 |
| CH4     | PB12    | PC13    | Software I2C bus 4 |

**RGB LEDs**:
| Channel | Pin  | Color Purpose |
|---------|------|---------------|
| Main    | PD1  | System status |
| CH1     | PA11 | Filament 1 status |
| CH2     | PA8  | Filament 2 status |
| CH3     | PB1  | Filament 3 status |
| CH4     | PB0  | Filament 4 status |

**Communication**:
- **BambuBus**: UART interface (refer to schematic for TX/RX pins)
- **Debug**: UART @ 115200 baud (optional)

---

## Software Requirements

### Development Tools

#### PlatformIO (Recommended)

**Installation**:
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Open VS Code Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install

**Verify Installation**:
```bash
pio --version
```

#### Alternative: PlatformIO Core CLI

For command-line users:
```bash
# Install Python 3.6+
python --version

# Install PlatformIO Core
pip install platformio

# Verify installation
pio --version
```

### WCH-Link Programmer

**Required for CH32V series**:
- Download WCH-Link drivers from [WCH official site](http://www.wch-ic.com/)
- Install drivers for your operating system
- Connect WCH-Link programmer to your computer

**Alternative Programmers**:
- Any RISC-V compatible JTAG/SWD programmer
- Ensure compatibility with CH32V203 series

---

## Assembly and Wiring

### Pre-Assembly Checks

1. **Visual Inspection**:
   - Check PCB for damage or shorts
   - Verify all components are properly soldered
   - Inspect connectors for bent pins

2. **Continuity Testing**:
   - Test power rails (VCC, GND) for shorts
   - Verify motor driver connections
   - Check BambuBus connector wiring

### Power Connection

**IMPORTANT**: Verify voltage compatibility before powering on!

1. Identify power input connector on PCB
2. Check board marking for voltage (12V or 24V)
3. Connect appropriate power supply
4. **Do NOT exceed rated voltage** - risk of permanent damage

**Recommended Power Supply**:
- **Voltage**: As marked on PCB (typically 12V or 24V)
- **Current**: Minimum 3A (depends on motor load)
- **Type**: Regulated DC power supply with short-circuit protection

### Motor Connections

Connect stepper motors to each channel:

1. **Identify Motor Wires**:
   - Typical 4-wire bipolar stepper (A+, A-, B+, B-)
   - Consult motor datasheet for wire colors

2. **Connect to Board**:
   - Match motor wiring to driver outputs
   - Secure connections with screw terminals or connectors
   - Double-check polarity before powering on

3. **Test Motor Direction**:
   - Initial direction is stored in flash
   - Can be reversed via configuration if needed

### Filament Path Setup

1. **PTFE Tubing**:
   - Cut tubes to appropriate lengths for your installation
   - Use push-fit connectors for secure attachment
   - Ensure smooth internal bore (no obstructions)

2. **Sensor Mounting**:
   - Install drive gears on motor shafts
   - Position magnets on rotating elements
   - Verify AS5600 sensor clearance (0.5-3mm)

3. **Pressure Sensors**:
   - Connect pressure sensor outputs to ADC inputs
   - Verify voltage range matches ADC specifications (0-3.3V)

4. **Microswitch Installation** (if applicable):
   - **Single Mode**: One microswitch per channel for filament detection
   - **Dual Mode**: Inner and outer switches for position sensing
   - Connect to appropriate ADC input pins

### BambuBus Connection

1. **Locate BambuBus Port** on Bambu Lab printer:
   - Typically labeled "AMS" or "External Spool"
   - Consult printer manual for exact location

2. **Cable Connection**:
   - Use shielded cable for noise immunity
   - Keep cable length reasonable (<2 meters ideal)
   - Ensure proper pin alignment (DO NOT force connector)

3. **Verify Wiring**:
   - TX → RX (crossover)
   - RX → TX (crossover)
   - GND → GND (common ground essential)
   - +V → +V (if power over BambuBus)

---

## Firmware Installation

### Method 1: Using PlatformIO (Recommended)

#### 1. Clone the Repository
```bash
git clone https://github.com/MillionthOdin16/BMCU370.git
cd BMCU370
```

#### 2. Open in VS Code
```bash
code .
```

#### 3. Build Firmware
- Open PlatformIO sidebar (alien icon)
- Click "Build" under env:genericCH32V203C8T6
- Wait for compilation to complete

**Expected Output**:
```
Building .pio/build/genericCH32V203C8T6/firmware.bin
SUCCESS
```

#### 4. Connect Programmer
- Connect WCH-Link to computer via USB
- Connect WCH-Link to BMCU board:
  - **VCC** → 3.3V (if powering from programmer)
  - **GND** → GND
  - **SWDIO** → SWDIO
  - **SWCLK** → SWCLK

#### 5. Upload Firmware
- Click "Upload" in PlatformIO
- Monitor progress in terminal

**Expected Output**:
```
Uploading .pio/build/genericCH32V203C8T6/firmware.bin
Writing... [====] 100%
SUCCESS
```

### Method 2: Pre-compiled Binary

If you have a pre-compiled `.bin` file:

#### Using WCH-Link Utility
1. Open WCHISPTool (Windows) or wchisp (Linux/Mac)
2. Connect WCH-Link programmer
3. Select CH32V203C8T6 from device list
4. Browse to firmware `.bin` file
5. Click "Download" or "Flash"
6. Wait for verification complete

### Troubleshooting Upload Issues

**Error: "Device not found"**
- Verify WCH-Link drivers installed
- Check physical connections
- Try different USB port
- Ensure board has power (if not powered by programmer)

**Error: "Flash failed"**
- Check for write protection
- Verify correct chip selected (CH32V203C8T6)
- Try erasing chip before flashing
- Ensure stable power supply during flashing

**Error: "Verification failed"**
- Re-flash firmware
- Check for hardware issues (damaged flash)
- Try different programmer or cable

---

## Initial Configuration

### First Boot

1. **Power On**:
   - Apply power to BMCU board
   - Main LED should illuminate (red breathing if not connected to printer)

2. **LED Behavior**:
   - **Main LED Red Breathing**: Normal, waiting for printer connection
   - **Channel LEDs Off/Red**: Normal, no filament loaded
   - **Any Unusual Flashing**: Check wiring and power supply

### Serial Debug Monitor (Optional)

Connect to debug UART to view system status:

#### Using PlatformIO
```bash
pio device monitor --baud 115200
```

#### Using screen (Linux/Mac)
```bash
screen /dev/ttyUSB0 115200
```

#### Using PuTTY (Windows)
- Port: COM# (check Device Manager)
- Baud: 115200
- Data bits: 8
- Stop bits: 1
- Parity: None

**Expected Output**:
```
BMCU 370 C Hall Version
BambuBus Protocol: 5
Initializing AS5600 sensors...
Channel 1: OK
Channel 2: OK
Channel 3: OK
Channel 4: OK
Waiting for BambuBus connection...
```

### Sensor Verification

Check AS5600 Hall sensors are functioning:

1. **Monitor Debug Output**:
   - Look for "AS5600 online" messages
   - Verify all 4 channels detected

2. **Manual Test**:
   - Rotate each filament drive gear by hand
   - Observe angle values changing (0-4095)
   - Verify smooth, continuous readings (no jumps)

3. **Magnet Status**:
   - **Normal**: Magnet properly positioned
   - **High/Low**: Magnet too close or too far
   - **Offline**: Sensor communication failure or no magnet

**Fix Magnet Issues**:
- Adjust magnet distance (target: 1mm)
- Ensure magnet is diametric type
- Verify sensor I2C connections

### ADC Calibration

Pressure sensor readings should be in normal ranges:

1. **Without Filament**: ~1.45V (low pressure)
2. **With Filament**: ~1.5-1.7V (normal)
3. **Filament Blocked**: >1.85V (high pressure)

**If readings are off**:
- Check ADC reference voltage (should be 3.3V)
- Verify pressure sensor connections
- Adjust voltage thresholds in code if necessary

---

## Testing and Verification

### Pre-Integration Tests

Before connecting to printer, verify all systems:

#### 1. LED System Test
- Main LED should show red breathing (no printer connection)
- Channel LEDs should be dark or showing red (no filament)

#### 2. Hall Sensor Test
```
Expected: Real-time angle updates for all 4 channels
Action: Manually rotate each drive gear
Result: Angle values should change smoothly (0-4095)
```

#### 3. Motor Test (Manual)
**WARNING**: Ensure no filament loaded during motor tests!

Test motor response:
- Motors should NOT run without BambuBus commands
- If motors activate unexpectedly, immediately power off and check wiring

#### 4. Filament Detection Test
- Insert filament into each channel
- Verify channel LED color changes from red to filament color
- Check serial monitor for "Channel X online" messages

### Load Test

With filament loaded:

1. **Insert Filament**:
   - Feed filament through PTFE tube
   - Push until microswitch triggers
   - Verify LED shows filament color

2. **Check Pressure**:
   - Monitor ADC values via serial debug
   - Should be in normal range (1.45-1.85V)
   - If high, check for obstructions in filament path

3. **Remove Filament**:
   - Pull filament out
   - Verify LED returns to red (offline state)
   - Check serial monitor for "Channel X offline"

---

## Integration with Bambu Lab Printer

### Printer Configuration

#### 1. Physical Connection
- Power off both BMCU and printer
- Connect BambuBus cable
- Verify secure connection on both ends
- Power on printer first, then BMCU

#### 2. Printer Settings
Access printer network interface or touchscreen:

**P1P/P1S**:
1. Navigate to Settings → External Spool
2. Select "AMS" mode
3. Choose number of spools: 4

**X1C**:
1. Navigate to Settings → AMS
2. Add external AMS unit
3. System should auto-detect BMCU

**Expected Behavior**:
- Printer recognizes BMCU as AMS device
- Shows 4 available filament slots
- Allows filament selection in slicer

#### 3. Verify Communication
- Main LED on BMCU should change to white breathing
- Printer should display filament slots
- Check debug serial for BambuBus messages

### Filament Profile Setup

#### In Bambu Studio Slicer

1. **Open Filament Settings**:
   - Filament → Manage Filaments
   - Select or create filament profile

2. **Configure Filament**:
   - **Name**: Material type (e.g., "PLA Basic Red")
   - **Color**: RGB values (e.g., 255, 0, 0 for red)
   - **Temperature**: Nozzle temp range (e.g., 200-220°C for PLA)
   - **Type**: Material type (PLA, PETG, ABS, etc.)

3. **Assign to AMS Slot**:
   - In print preparation, assign filament to BMCU channel
   - Slicer will send metadata to BMCU
   - BMCU stores in flash and updates LED color

#### Manual Profile Entry (if needed)

If slicer doesn't sync properly:
- Filament metadata defaults to "PETG" white (255,255,255)
- Temperature range: 220-240°C
- ID: "GFG00"

### First Print Test

#### 1. Simple Test Print
Recommended: Single-color cube or benchy

**Process**:
- Slice model with single filament
- Send to printer
- Observe BMCU behavior during print

**Expected Behavior**:
- Printer requests filament from BMCU channel
- BMCU motor feeds filament
- LED shows green during feeding
- Pressure stays in normal range
- Filament reaches extruder smoothly

#### 2. Multi-Color Test
After single-color success, try color change:

**Process**:
- Slice model with 2+ colors/materials
- Assign to different BMCU channels
- Start print

**Expected Behavior**:
- BMCU retracts active filament when switching
- LED shows blue during retraction
- New filament feeds after retraction complete
- Smooth transition with minimal waste

### Troubleshooting Integration

**Issue: Printer doesn't detect BMCU**
- Verify BambuBus cable connection
- Check cable wiring (TX/RX crossover)
- Ensure common ground between devices
- Try power cycling both devices

**Issue: Filament won't feed**
- Check motor connections
- Verify Hall sensor detecting rotation
- Monitor pressure sensor (may be too high)
- Ensure filament path is clear

**Issue: Filament color incorrect on display**
- Re-sync filament profiles in slicer
- Manually check RGB values in BMCU flash
- Reset BMCU to defaults and reconfigure

**Issue: Frequent disconnects**
- Check BambuBus cable quality
- Reduce cable length if possible
- Add ferrite choke for noise reduction
- Verify stable power supply to BMCU

---

## Next Steps

After successful setup:

1. **Read Technical Documentation**: Understand system architecture and components
2. **Review Troubleshooting Guide**: Familiarize yourself with common issues
3. **Join Community**: Connect with other BMCU users for tips and support
4. **Optimize Settings**: Fine-tune PID parameters for your specific setup

### Useful Resources

- **Technical Documentation**: `docs/TECHNICAL_DOCUMENTATION.md`
- **Developer Guide**: `docs/DEVELOPER_GUIDE.md`
- **Troubleshooting**: `docs/TROUBLESHOOTING.md`
- **English Wiki**: https://wiki.yuekai.fr/
- **Chinese Wiki**: https://bmcu.wanzii.cn/

### Community Support

- **GitHub Issues**: Report bugs or ask questions
- **Discord/Forums**: Join BMCU user communities
- **Wiki**: Contribute your findings and improvements

---

**Document Version**: 1.0  
**Last Updated**: December 2024  
**Compatible Firmware**: BMCU-C 370 Hall V0.1-0020
