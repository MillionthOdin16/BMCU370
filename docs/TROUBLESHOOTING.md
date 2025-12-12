# BMCU 370 C Troubleshooting Guide

## Table of Contents
1. [General Issues](#general-issues)
2. [Hardware Problems](#hardware-problems)
3. [Communication Issues](#communication-issues)
4. [Motor and Motion Control](#motor-and-motion-control)
5. [Sensor Issues](#sensor-issues)
6. [LED Problems](#led-problems)
7. [Firmware and Software](#firmware-and-software)
8. [Advanced Diagnostics](#advanced-diagnostics)

---

## General Issues

### Device Won't Power On

**Symptoms**:
- No LEDs illuminate
- No response from any component

**Possible Causes & Solutions**:

1. **No Power Supply**
   - Check power adapter is plugged in and switched on
   - Verify power cable connections
   - Test power supply output voltage with multimeter
   - Expected: 12V or 24V DC (check board marking)

2. **Incorrect Voltage**
   - Verify power supply matches board requirements
   - **CRITICAL**: Using wrong voltage can damage board permanently
   - Check board markings for voltage specification

3. **Power Supply Insufficient**
   - Minimum 3A recommended (4A+ for heavy loads)
   - Test with known-good power supply
   - Check for voltage drop under load

4. **Board Damage**
   - Inspect for visible damage (burned components, traces)
   - Check for shorts between power rails with multimeter
   - Look for cold solder joints or broken traces

5. **Fuse Blown** (if present)
   - Check board schematic for fuse location
   - Test continuity with multimeter
   - Replace with same rating if blown

### Main LED Shows Red Breathing (No Printer Connection)

**Status**: **NORMAL** - This is expected behavior when not connected to printer.

**What It Means**:
- BMCU is powered and firmware is running
- Waiting for BambuBus connection from printer

**To Resolve**:
1. Connect BambuBus cable to printer
2. Ensure printer is powered on
3. Check printer recognizes external AMS
4. LED should change to white breathing when connected

### All LEDs Flashing Rapidly

**Symptoms**:
- Erratic LED behavior
- Random colors or patterns

**Possible Causes**:

1. **Firmware Corruption**
   - Re-flash firmware using WCH-Link programmer
   - Verify flash operation completed successfully

2. **Power Supply Noise**
   - Add capacitor to power input (100µF + 0.1µF)
   - Use regulated power supply with low ripple
   - Add ferrite bead to power cable

3. **Hardware Fault**
   - Check microcontroller for damage
   - Verify crystal oscillator is functioning
   - Test with minimal connections (no motors)

---

## Hardware Problems

### Hall Sensors Not Detecting (AS5600)

**Symptoms**:
- Sensor shows "offline" in debug output
- Angle reading always 0 or not changing
- Magnet status shows "offline"

**Diagnostics**:
```cpp
// Check via serial debug
AS5600 Channel 0: offline
AS5600 Channel 0 magnet: offline
```

**Solutions**:

1. **Magnet Not Present or Misaligned**
   - Verify magnet is installed on rotating element
   - Check magnet type (must be diametric, not axial)
   - Adjust distance: optimal 0.5-3mm from sensor surface
   - Use AS5600 datasheet as reference

2. **I2C Communication Failure**
   - Check SCL/SDA connections for channel
   - Verify pull-up resistors present (typically 4.7kΩ)
   - Test with different sensor (could be defective)
   - Check for shorts on I2C lines

3. **Magnet Too Weak or Wrong Type**
   - AS5600 requires diametric magnetization
   - Recommended: 6mm diameter × 2.5mm height
   - Field strength: 30-60 mT at sensor
   - Test with known-good magnet

4. **Software Issue**
   - Verify GPIO pins configured correctly in code
   - Check `AS5600_SCL[]` and `AS5600_SDA[]` arrays match hardware
   - Ensure `MC_AS5600.init()` called in setup

**Pin Configuration to Verify**:
```cpp
// In Motion_control.cpp
uint32_t AS5600_SCL[] = {PB15, PB14, PB13, PB12};
uint32_t AS5600_SDA[] = {PD0, PC15, PC14, PC13};
```

### Pressure Sensor Readings Incorrect

**Symptoms**:
- Constant high pressure (>1.85V)
- Constant low pressure (<1.45V)
- No change when filament loaded/unloaded

**Diagnostics**:
```cpp
// Monitor ADC values via serial debug
Channel 0 pressure: 3.30V  // Wrong - maxed out
Channel 1 pressure: 0.00V  // Wrong - zero
Channel 2 pressure: 1.65V  // Normal
```

**Solutions**:

1. **Sensor Disconnected**
   - Check ADC input connections
   - Verify sensor power supply (typically 3.3V or 5V)
   - Test sensor output with multimeter

2. **Wrong Voltage Range**
   - ADC expects 0-3.3V input
   - If sensor outputs 0-5V, add voltage divider
   - Formula: Vout = Vin × R2/(R1+R2)
   - Example: 10kΩ and 20kΩ for 5V→3.3V

3. **Mechanical Issue**
   - Inspect pressure sensor mechanism
   - Check for jammed components
   - Verify spring or lever moves freely
   - Clean debris from sensor area

4. **Calibration Needed**
   - Adjust thresholds in code if needed:
```cpp
// In Motion_control.cpp
float PULL_voltage_up = 1.85f;   // High pressure threshold
float PULL_voltage_down = 1.45f; // Low pressure threshold
```

### Microswitch Detection Not Working

**Symptoms**:
- Filament inserted but shows "offline"
- Shows "online" when no filament present
- Erratic online/offline status

**Diagnostics**:
```cpp
// Check detection voltage via serial debug
Channel 0 detection: 0.45V  // Should be >1.65V when online
```

**Solutions**:

1. **Microswitch Stuck or Broken**
   - Test switch continuity with multimeter
   - Check for mechanical obstruction
   - Verify switch actuation force matches filament
   - Replace if clicking but no electrical change

2. **Wrong Operating Mode**
   - Single vs. dual microswitch mode
   - Check `is_two` flag in Motion_control.cpp:
```cpp
#define is_two false  // Change to true for dual microswitch
```

3. **Wiring Issue**
   - Verify switch connects to correct ADC channel
   - Check pull-up/pull-down resistor values
   - Ensure common ground between switch and MCU

4. **Voltage Levels Wrong**
   - Measure switch output voltage
   - Should be >1.65V for "online" (single mode)
   - Adjust detection threshold if needed

### Motors Not Running

**Symptoms**:
- No motor movement when filament requested
- Motors hum but don't rotate
- Inconsistent motor behavior

**Solutions**:

1. **Motor Driver Not Enabled**
   - Check enable pins on motor drivers
   - Verify driver power supply
   - Test with multimeter on driver outputs

2. **Motor Wiring Incorrect**
   - Verify A+/A-/B+/B- connections
   - Try swapping A and B coils (may just reverse direction)
   - Check for open circuit in motor windings

3. **Current Limit Too Low**
   - Adjust motor driver current limit potentiometer
   - Typical: 0.5-1.0A for small steppers
   - Use formula: Vref = Current × 8 × Rsense

4. **PWM Not Generated**
   - Check PWM output pins with oscilloscope
   - Verify `Motion_control_set_PWM()` called
   - Test with manual PWM command

5. **Direction Saved Incorrectly**
   - Motor direction stored in flash
   - If wrong, filament may feed backward
   - Clear flash and reconfigure:
```cpp
// Add to setup() temporarily
Motion_control_data_save.Motion_control_dir[0] = 1;  // Try opposite
Motion_control_save();
```

---

## Communication Issues

### Printer Doesn't Detect BMCU

**Symptoms**:
- Printer shows no external AMS
- Main LED stays red (not white)
- No BambuBus communication

**Solutions**:

1. **Cable Not Connected**
   - Verify BambuBus cable plugged into both ends
   - Check for bent pins in connectors
   - Ensure cable fully seated

2. **Wrong Cable Pinout**
   - Verify TX/RX crossover (TX→RX, RX→TX)
   - Check ground connection
   - Confirm cable isn't straight-through type

3. **UART Configuration Mismatch**
   - BambuBus typically uses specific baud rate
   - Verify UART initialization in BambuBus.cpp
   - Check printer firmware version compatibility

4. **Printer Setting Disabled**
   - Access printer menu
   - Enable "External Spool" or "AMS" mode
   - Restart printer after enabling

5. **Cable Too Long**
   - Maximum recommended: 2 meters
   - Longer cables may need shielding
   - Try shorter cable for testing

### Frequent BambuBus Disconnects

**Symptoms**:
- Intermittent connection
- Main LED alternating red/white
- Print failures mid-job

**Solutions**:

1. **Cable Quality Issues**
   - Use shielded cable for noise immunity
   - Check for damaged insulation
   - Replace with higher-quality cable

2. **Electrical Noise**
   - Add ferrite choke to BambuBus cable
   - Keep cable away from motor wires
   - Ensure good grounding between devices

3. **Power Supply Instability**
   - Check for voltage drops during motor operation
   - Add bulk capacitance (1000µF) to power input
   - Use linear regulator for MCU power if possible

4. **CRC Errors**
   - Monitor debug output for CRC failures
   - May indicate data corruption on line
   - Check for loose connections

### Filament Info Not Saving to Printer

**Symptoms**:
- Printer shows wrong filament color
- Temperature settings not updated
- Filament type displays incorrectly

**Compatible Firmware Versions**:
- P1X1 firmware: 00.01.06.62 or later
- Bambu Studio: 2.1.1.52 or later

**Solutions**:

1. **Outdated Firmware**
   - Update printer firmware via Bambu Studio
   - Update Bambu Studio to latest version
   - BMCU firmware V0.1-0019+ required for fix

2. **BambuBus Protocol Mismatch**
   - Verify `Bambubus_version` in code (should be 5)
   - Check flash data structure version matches

3. **Flash Data Corrupted**
   - Clear BMCU flash and reconfigure
   - Re-send filament profiles from slicer

4. **P1X1 16-Color Mode**
   - Requires specific printer firmware
   - May need slicer configuration
   - Not all colors may be supported

---

## Motor and Motion Control

### Filament Won't Feed

**Symptoms**:
- Motor runs but filament doesn't move
- Filament gets stuck during feed

**Solutions**:

1. **Pressure Too High**
   - Check pressure sensor reading (should be <1.85V)
   - Inspect filament path for obstructions
   - Verify PTFE tube not kinked
   - Clean drive gear teeth

2. **Insufficient Motor Torque**
   - Increase motor current limit
   - Check motor driver heat dissipation
   - Verify motor isn't overheating

3. **Drive Gear Slipping**
   - Tighten drive gear set screw
   - Inspect gear teeth for wear
   - Adjust filament pressure (if adjustable)

4. **Wrong Motor Direction**
   - Filament retracting when should feed
   - Flip motor direction in config
   - Or swap motor coil wiring

5. **PID Parameters Wrong**
   - May be oscillating or hunting
   - Observe motor behavior (smooth vs. jerky)
   - Adjust PID gains if needed (advanced)

### Filament Won't Retract

**Symptoms**:
- Motor tries to retract but filament stuck
- Partial retraction then stops

**Solutions**:

1. **Filament Jammed in Extruder**
   - Manually release filament from printer
   - Check for partial clogs
   - Verify retraction distance setting

2. **Retraction Distance Too Long**
   - Default: 200mm (internal) or 700mm (external)
   - Adjust in Motion_control.cpp:
```cpp
float_t P1X_OUT_filament_meters = 200.0f;  // Reduce if needed
```

3. **Motor Overheating**
   - Allow cool-down period
   - Reduce motor current if frequently overheating
   - Improve cooling/ventilation

4. **Hall Sensor Not Tracking**
   - Check AS5600 angle updates during retraction
   - Verify magnet hasn't shifted position
   - Ensure sensor reading correctly

### Erratic Motor Speed

**Symptoms**:
- Motor speed varies unexpectedly
- Jerky motion during feed/retract
- Speed doesn't match commanded value

**Solutions**:

1. **PID Tuning Issue**
   - Integral term may be winding up
   - Derivative term causing oscillation
   - May need expert tuning (see Developer Guide)

2. **Hall Sensor Noise**
   - Check for magnetic interference
   - Verify sensor readings smooth in debug output
   - Shield sensor wires if necessary

3. **Power Supply Insufficient**
   - Voltage sag under load
   - Test with oscilloscope on motor supply
   - Upgrade to higher-current PSU

4. **Software Bug**
   - Update to latest firmware
   - Check GitHub issues for known problems
   - Report if issue persists

---

## Sensor Issues

### AS5600 Shows "Magnet High" or "Magnet Low"

**Symptoms**:
- Magnet status not "normal"
- May still read angles but not reliably

**Solutions**:

1. **Magnet Too Close** (High):
   - Increase distance between magnet and sensor
   - Target: ~1mm gap
   - Maximum field strength: ~60mT

2. **Magnet Too Far** (Low):
   - Decrease distance
   - Minimum field strength: ~30mT
   - Check magnet hasn't fallen off

3. **Wrong Magnet Type**:
   - Must be diametrically magnetized
   - Axial magnets won't work correctly
   - Order correct type from AS5600 supplier

**Reference**: AS5600 datasheet section on magnet requirements

### Angle Readings Jumping or Unstable

**Symptoms**:
- Angle values change erratically
- Speed calculations wildly varying
- Motor control unstable

**Solutions**:

1. **Mechanical Wobble**
   - Check magnet securely mounted
   - Verify rotation is concentric
   - Tighten any loose components

2. **Electromagnetic Interference**
   - Move sensor away from motor windings
   - Shield sensor with grounded metal case
   - Add capacitors to sensor power (0.1µF + 10µF)

3. **I2C Communication Errors**
   - Check for noise on SCL/SDA lines
   - Reduce I2C clock speed if needed
   - Ensure proper pull-up resistors

---

## LED Problems

### All LEDs Off

**Symptoms**:
- No LEDs illuminate even when powered

**Solutions**:

1. **LED Power Issue**
   - Verify LED strip power connections
   - Check 5V supply to LEDs (if separate from MCU)
   - Test with multimeter

2. **Data Signal Not Reaching LEDs**
   - Check data pin connections (PA11, PA8, PB1, PB0, PD1)
   - Verify no broken traces on PCB
   - Test data line with oscilloscope

3. **First LED Damaged**
   - WS2812B strips fail if first LED breaks
   - Test by bypassing first LED
   - Replace LED strip if confirmed

4. **Brightness Set Too Low**
   - Check brightness values in code:
```cpp
strip_PD1.setBrightness(35);      // Main board
strip_channel[0].setBrightness(15); // Channels
```

### LEDs Show Wrong Colors

**Symptoms**:
- Red appears green, blue appears red, etc.
- Filament color doesn't match display

**Solutions**:

1. **Color Order Wrong**
   - WS2812B typically GRB, not RGB
   - Verify strip type matches code:
```cpp
Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
```

2. **Filament Metadata Incorrect**
   - Check RGB values stored in flash
   - Re-send from slicer
   - Manually verify in debug output

3. **LED Strip Type Mismatch**
   - Using SK6812 instead of WS2812B
   - Different strips have different orders
   - Change to `NEO_RGB` if needed

### LEDs Flickering

**Symptoms**:
- Rapid on/off flashing
- Unstable colors

**Solutions**:

1. **Update Rate Too High**
   - V0.1-0020 reduced refresh rate
   - Avoid calling `RGB_show_data()` too frequently
   - Implement minimum time between updates:
```cpp
static uint64_t last_led_update = 0;
if (DEBUG_get_time() - last_led_update >= 50000) {  // 50ms min
    RGB_show_data();
    last_led_update = DEBUG_get_time();
}
```

2. **UART Interference**
   - LED updates can disrupt BambuBus communication
   - Timing issue between bit-banging and UART
   - Current firmware addresses this (V0.1-0020)

3. **Power Supply Noise**
   - Add capacitor near LED power input
   - Use separate 5V regulator for LEDs if possible
   - Ensure good ground connection

### Channel LED Stays Red (Error Indication)

**Symptoms**:
- Channel LED red even when filament loaded
- Doesn't show filament color

**Expected Behavior** (V0.1-0020+):
- Red LED updates every 3 seconds if channel has error
- Ensures visibility if filament inserted late

**Solutions**:

1. **Filament Not Detected**
   - Check microswitch triggering
   - Verify filament fully inserted
   - Test detection voltage (should be >1.65V)

2. **Hall Sensor Offline**
   - Check AS5600 status for that channel
   - Verify magnet present and aligned
   - Fix per Hall Sensor section above

3. **Motor Error**
   - Check if motor is functioning
   - Verify motor driver enabled
   - Test motor connections

---

## Firmware and Software

### Can't Upload Firmware

**Symptoms**:
- Upload fails with error message
- Programmer not recognized

**Solutions**:

1. **Driver Not Installed**
   - Install WCH-Link drivers
   - Restart computer after installation
   - Check Device Manager (Windows) for COM port

2. **Wrong Chip Selected**
   - Verify CH32V203C8T6 selected
   - Not CH32V203C6T6 or other variant
   - Check board marking to confirm

3. **Programmer Not Connected**
   - Verify all 4 pins connected (VCC, GND, SWDIO, SWCLK)
   - Check cable continuity
   - Try different USB port

4. **Board Write-Protected**
   - Some chips have read protection
   - Use WCH tools to unlock
   - May erase flash content

### Compilation Errors

**Common Error**: "Platform not found"
```
PlatformIO: Platform 'https://github.com/Community-PIO-CH32V/platform-ch32v.git' not found
```

**Solution**:
```bash
pio platform install https://github.com/Community-PIO-CH32V/platform-ch32v.git
```

**Common Error**: "Library not found"
```
Error: Could not find library 'CRC' version ^1.0.3
```

**Solution**:
```bash
pio lib install "robtillaart/CRC@^1.0.3"
```

### Flash Memory Full

**Symptoms**:
- Code won't fit in 64KB flash
- Upload succeeds but device doesn't boot

**Solutions**:

1. **Optimize Code Size**
   - Remove unused features
   - Disable debug logging in production
   - Use compiler optimization flags

2. **Check Binary Size**
```bash
pio run --target size
```
Should be <64KB total.

3. **Remove Large Buffers**
   - Check for large arrays
   - Move to flash storage if read-only
   - Reduce buffer sizes if possible

### Lost Flash Configuration

**Symptoms**:
- Filament data reset to defaults
- Motor directions reset
- After power cycle, settings gone

**Causes**:
- Flash wear exceeded (~10,000 writes)
- Power loss during flash write
- Flash page corruption

**Solutions**:

1. **Restore from Backup**
   - Re-enter filament profiles via slicer
   - Reconfigure motor directions if needed

2. **Reduce Flash Writes**
   - Firmware already uses `set_need_to_save()` pattern
   - Don't add additional save calls
   - Periodical saves better than immediate

3. **Test Flash Integrity**
   - Read flash at boot and verify checksum
   - If corruption frequent, may be hardware issue

---

## Advanced Diagnostics

### Using Serial Debug Output

Enable full diagnostics:

1. **Ensure Debug Enabled**:
In `Debug_log.h`:
```cpp
#define Debug_log_on  // Must be defined
```

2. **Connect Serial Adapter**:
- TX pin → Serial RX
- RX pin → Serial TX  
- GND → GND

3. **Monitor at 115200 Baud**:
```bash
pio device monitor --baud 115200
```

4. **Interpret Output**:
```
[Timestamp] BambuBus: Packet received, type=3
[Timestamp] Channel 0: Filament online
[Timestamp] AS5600[0]: Angle=2048, Status=normal
[Timestamp] Pressure[0]: 1.65V
[Timestamp] Motion[0]: PID output=450, Speed=12.5
```

### Reading Flash Data Manually

Use debugger or custom code:

```cpp
void dump_flash() {
    flash_save_struct *ptr = (flash_save_struct*)(0x0800F000);
    
    DEBUG_MY("Flash validation: ");
    DEBUG_num(&ptr->check, 1);
    DEBUG_MY(" (should be 0x40614061)\n");
    
    DEBUG_MY("Version: ");
    DEBUG_num(&ptr->version, 1);
    DEBUG_MY("\n");
    
    for (int i = 0; i < 4; i++) {
        DEBUG_MY("Channel ");
        DEBUG_num(&i, 1);
        DEBUG_MY(": ");
        DEBUG_MY(ptr->filament[i].name);
        DEBUG_MY(" RGB=(");
        DEBUG_num(&ptr->filament[i].color_R, 1);
        DEBUG_MY(",");
        DEBUG_num(&ptr->filament[i].color_G, 1);
        DEBUG_MY(",");
        DEBUG_num(&ptr->filament[i].color_B, 1);
        DEBUG_MY(")\n");
    }
}
```

### Oscilloscope Debugging

**PWM Signals**:
- Connect to motor driver inputs
- Verify frequency and duty cycle
- Check for glitches or noise

**I2C Signals**:
- Monitor SCL and SDA during AS5600 read
- Verify clock stretching if present
- Check ACK/NACK bits

**UART Signals**:
- Monitor BambuBus TX/RX lines
- Verify baud rate accurate
- Check for framing errors

### Common Signal Issues

**PWM Not Present**:
- Check timer initialization
- Verify GPIO alternate function
- Confirm PWM generation code executing

**I2C Clock Stretching**:
- AS5600 may hold SCL low if not ready
- Increase timeout in code if needed
- Verify sensor isn't stuck

**UART Baud Rate Mismatch**:
- Use oscilloscope to measure actual baud
- Adjust crystal frequency if necessary
- Check for clock source issues

---

## Getting Further Help

### Before Asking for Help

Collect the following information:

1. **Hardware Version**:
   - PCB markings/version number
   - Component part numbers
   - Modifications made

2. **Firmware Version**:
   - Git commit hash or version string
   - Build date
   - Modified code? (yes/no)

3. **Serial Debug Output**:
   - Copy/paste relevant sections
   - Include boot messages
   - Show error messages

4. **Steps to Reproduce**:
   - What were you doing when issue occurred?
   - Does it happen consistently?
   - What have you tried already?

### Community Resources

- **GitHub Issues**: https://github.com/MillionthOdin16/BMCU370/issues
- **English Wiki**: https://wiki.yuekai.fr/en/BMCU
- **Chinese Wiki**: https://bmcu.wanzii.cn/
- **Original Project**: https://github.com/Xing-C/BMCU370x

### Reporting Bugs

Good bug report includes:

```markdown
**Hardware**: BMCU 370 C Hall Version V0.1
**Firmware**: V0.1-0020
**Printer**: Bambu Lab P1S (firmware 01.02.03.04)

**Description**: 
Channel 2 motor won't run during prints

**Steps to Reproduce**:
1. Load filament into channel 2
2. Start print using channel 2
3. Motor doesn't activate

**Expected**: Motor should feed filament
**Actual**: Motor silent, no movement

**Debug Output**:
[Paste relevant serial output]

**Already Tried**:
- Tested motor manually (works)
- Swapped with channel 1 (same result)
- Checked Hall sensor (reads correctly)
```

---

**Document Version**: 1.0  
**Last Updated**: December 2024  
**Compatible Firmware**: BMCU-C 370 Hall V0.1-0020
