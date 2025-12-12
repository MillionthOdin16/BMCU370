# Troubleshooting Guide - BMCU-C 370 Hall Version

## Introduction

This guide helps diagnose and resolve common issues with BMCU-C 370 Hall version. Before troubleshooting, ensure you have:
- Firmware version 0020 or compatible
- Proper assembly according to build guide
- Correct wiring and connections

## Quick Diagnostic Chart

| Symptom | Likely Cause | Quick Fix |
|---------|-------------|-----------|
| Mainboard LED red | No printer connection | Check cable, restart printer |
| Top LED red | Hall sensor issue | Check magnet position |
| Side LED stays black | Filament not detected | Push filament further |
| Motor doesn't run | Wiring or command issue | Check connections |
| Stuck at 99% | Buffer pressure issue | Adjust buffer spring |
| Grinding noise | Gears too tight | Adjust gear mesh |
| Filament chewing | Too much pressure | Reduce motor PWM or adjust gears |

## Communication Issues

### Printer Doesn't Detect BMCU

**Symptoms:**
- Mainboard LED stays red
- Printer shows no AMS/AMS Lite
- No communication

**Diagnostic Steps:**

1. **Check Physical Connections**
   ```
   ✓ 6-pin cable firmly connected both ends
   ✓ Cable not damaged or pinched
   ✓ Connector orientation correct
   ✓ No bent pins in connector
   ```

2. **Verify Power**
   - Check 24V present on connector
   - Measure between 24V and GND pins
   - Should read ~24V DC
   - If no power: Check printer's AMS port fuse

3. **Check RS-485 Wiring**
   - Verify A and B lines not swapped
   - Check for short circuits
   - Ensure differential pair integrity
   - Test with multimeter continuity mode

4. **Firmware Version**
   - Ensure firmware is flashed correctly
   - Try re-flashing firmware
   - Verify flash was successful (check debug output)

**Solutions:**
- Replace cable if damaged
- Re-seat connections
- Flash latest firmware
- Check printer firmware compatibility

### Intermittent Connection Loss

**Symptoms:**
- Mainboard LED switches between blue and red
- Print pauses unexpectedly
- "AMS connection lost" errors

**Causes:**
- Loose connector
- Cable interference
- Power supply instability
- Firmware communication bug

**Solutions:**

1. **Secure Connections**
   - Add cable strain relief
   - Use zip ties to prevent movement
   - Ensure connectors fully seated

2. **Reduce Interference**
   - Route cable away from motor wires
   - Add ferrite beads if available
   - Keep away from high-power lines

3. **Stabilize Power**
   - Check 24V rail stability
   - Ensure printer power supply adequate
   - Add capacitors if needed (advanced)

### BambuBus Protocol Errors

**Debug with Serial Monitor:**

Enable debug output and check for:
- `BambuBus_offline` - Lost connection
- `BambuBus_online` - Connection established
- `Run_To_AMS_lite` - Recognized correctly
- `Run_To_AMS` - Recognized as full AMS (unusual)

**If seeing constant offline:**
- Check baud rate configuration
- Verify CRC calculations
- Test with logic analyzer (advanced)

## Hall Sensor Issues

### Top LED Shows Red (Magnet Not Detected)

**Symptoms:**
- Top LED (fiber optic) is red instead of blue
- Channel non-functional
- Error reported to printer

**Root Causes:**

1. **Magnet Missing or Misaligned**
   - Radial magnet not installed on motor shaft
   - Magnet fell off during assembly
   - Magnet orientation incorrect
   - Distance too far from sensor

2. **AS5600 Sensor Issue**
   - Sensor not soldered correctly
   - Cold solder joint
   - Sensor damaged
   - I2C connection broken

**Diagnostic Procedure:**

1. **Visual Inspection**
   ```
   ✓ Radial magnet visible on motor shaft
   ✓ Magnet centered over AS5600 sensor
   ✓ Distance approximately 1-2mm
   ✓ No physical obstruction
   ```

2. **Test Magnet Polarity**
   - Use another magnet or compass
   - Verify radial magnetization (not axial)
   - Check field strength adequate

3. **Check AS5600 Soldering**
   - Inspect solder joints under magnification
   - Test continuity from sensor to MCU pins
   - Verify 3.3V power present at sensor
   - Check I2C pull-up resistors if needed

4. **Test I2C Communication**
   ```cpp
   // Add to setup() for testing
   MC_AS5600.updata_stu();
   for (int i = 0; i < 4; i++) {
       DEBUG_MY("Ch ");
       DEBUG_float(i, 0);
       DEBUG_MY(" magnet: ");
       DEBUG_float(MC_AS5600.magnet_stu[i], 0);
       DEBUG_MY("\n");
   }
   ```
   Expected output: `0` (normal) for each channel

**Solutions:**

- **Magnet too far:** Move sensor closer or add shim
- **Magnet too close:** May show "high" status, add spacer
- **Magnet missing:** Install 6mm x 2mm radial magnet
- **Sensor dead:** Replace AS5600 chip
- **Bad solder:** Reflow joints or replace sensor

### Hall Sensor Reading Unstable

**Symptoms:**
- Erratic angle readings
- Motor behavior inconsistent
- Occasional red LED flashes

**Causes:**
- Magnetic interference
- Vibration causing movement
- Electrical noise on I2C

**Solutions:**
- Secure magnet with adhesive
- Shield sensor from external magnets
- Add I2C pull-up resistors (4.7kΩ typical)
- Route I2C wires away from motors

## Filament Detection Issues

### Filament Not Detected (Side LED Stays Black)

**Symptoms:**
- Side LED remains off/black when filament inserted
- Printer shows "No filament" for channel
- Cannot load filament

**Diagnostic Steps:**

1. **Verify Filament Insertion**
   - Push filament past the gears
   - Feel for resistance at gear engagement
   - Ensure filament straight, not kinked

2. **Check ADC Voltage**
   ```cpp
   // Add to motion control for testing
   float *adc = ADC_DMA_get_value();
   DEBUG_MY("Ch1 detect: ");
   DEBUG_float(adc[7], 3);  // Channel 1
   DEBUG_MY("V\n");
   ```
   - Expected: > 1.65V when filament present
   - If < 1.65V: Detection circuit issue

3. **Test Detection Circuit**
   - Verify sensor power (3.3V or 5V depending on type)
   - Check sensor output voltage
   - Test sensor with multimeter

**Solutions:**

- **Transparent filament:** Use opaque filament for testing
- **Sensor misaligned:** Adjust sensor position
- **Sensor failed:** Replace detection sensor
- **Wiring issue:** Check continuity to ADC pin
- **Threshold too high:** Adjust `MC_ONLINE_key_stu_raw[i] > 1.65` in code

### False Positive Detection

**Symptoms:**
- Side LED shows white when no filament
- Especially affects Channel 1 (common issue)

**Known Issue:**
This is documented in community wiki as a common problem, particularly for Channel 1.

**Causes:**
- Sensor too sensitive
- Electrical noise
- Dust or debris in sensor path
- Sensor calibration drift

**Solutions:**

1. **Adjust Threshold**
   ```cpp
   // In Motion_control.cpp, modify:
   if (MC_ONLINE_key_stu_raw[i] > 1.85)  // Increase from 1.65
   ```

2. **Clean Sensor Path**
   - Blow out dust with compressed air
   - Wipe sensor lens if optical
   - Check for debris in filament path

3. **Add Hysteresis** (advanced)
   ```cpp
   // Implement threshold band to prevent flickering
   if (MC_ONLINE_key_stu_raw[i] > 1.8) {
       MC_ONLINE_key_stu[i] = 1;
   } else if (MC_ONLINE_key_stu_raw[i] < 1.5) {
       MC_ONLINE_key_stu[i] = 0;
   }
   // Keep previous state if in between
   ```

4. **Hardware Fix**
   - Add shielding around sensor
   - Improve grounding
   - Replace sensor with better quality

## Buffer System Issues

### Buffer LEDs Wrong Color When Pressed/Pulled

**Symptoms:**
- Press buffer → shows red instead of blue
- Pull buffer → shows blue instead of red

**Cause:** Slider magnet polarity reversed

**Solution:**
1. Remove slider from buffer chamber
2. Remove radial magnet from slider
3. Flip magnet 180 degrees
4. Reinstall magnet
5. Test again

Should now show:
- Blue when pressed in
- Red when pulled out

### Buffer Pressure Always High/Low

**Symptoms:**
- Side LED constantly red (high pressure)
- Side LED never changes from blue (low pressure)
- Affects filament feeding behavior

**Diagnostic:**

Check ADC reading:
```cpp
DEBUG_MY("Ch1 buffer: ");
DEBUG_float(MC_PULL_stu_raw[0], 3);
DEBUG_MY("V\n");
```

Expected range: 1.45V - 1.85V normal
- > 1.85V = High pressure (red)
- < 1.45V = Low pressure (blue)

**Solutions:**

1. **Mechanical:**
   - Adjust buffer spring tension
   - Check slider moves freely
   - Lubricate slider if sticky
   - Verify pneumatic tube not blocked

2. **Sensor Calibration:**
   ```cpp
   // Adjust thresholds in Motion_control.cpp
   float PULL_voltage_up = 1.90f;    // Increase if too sensitive
   float PULL_voltage_down = 1.40f;  // Decrease if needed
   ```

3. **Hardware:**
   - Check Hall sensor on slider assembly
   - Verify ADC input not noisy
   - Test with oscilloscope for AC ripple

## Motor and Motion Issues

### Motor Doesn't Run

**Symptoms:**
- No motor movement when commanded
- Silent operation, no sound
- Filament doesn't feed

**Diagnostic Steps:**

1. **Check Motor Power**
   - Verify 24V at motor terminals
   - Check motor driver connections
   - Test motor directly (disconnect from circuit)

2. **Check PWM Signal**
   - Measure PWM output from MCU
   - Should see varying voltage (0-3.3V PWM)
   - Frequency depends on motion control

3. **Test Motor Independently**
   - Connect motor directly to 24V (briefly)
   - Should rotate
   - If not: motor failed

**Solutions:**
- Replace motor if failed
- Check motor driver IC (if separate)
- Verify PWM pin not reassigned
- Check for mechanical binding

### Grinding or Clicking Noise

**Symptoms:**
- Clicking sound during filament feed
- Grinding noise from gears
- Filament not advancing smoothly

**Causes:**
- **Gears too tight:** Over-meshing causes excessive friction
- **Filament chewing:** Too much pressure on filament
- **Motor stalling:** Insufficient torque
- **Gear wear:** Teeth damaged or worn

**Solutions:**

1. **Adjust Gear Mesh**
   - Loosen gear mounting slightly
   - Adjust distance between gears
   - Ensure just touching, not pressed hard
   - Test by rotating manually (should be smooth)

2. **Reduce Motor PWM** (if filament chewing)
   ```cpp
   // In Motion_control.cpp, reduce PWM value
   Motion_control_set_PWM(channel, 180);  // From 255
   ```

3. **Check Filament Path**
   - Ensure filament enters straight
   - Remove any tangles or knots
   - Verify no obstructions

4. **Gear Maintenance**
   - Clean gears of debris
   - Remove metal shavings (if metal gears)
   - Replace if teeth worn
   - Consider plastic gears to reduce wear

### Excessive Noise

**Known Issue:** 370 motors are noisier than 130/180 motors due to higher torque and speed.

**Mitigation:**
- Use plastic gears instead of metal
- Add sound dampening material around motors
- Adjust PWM to reduce speed
- Ensure mechanical assembly tight (no vibration)
- Consider 130 version if noise unacceptable

### Filament Chewing (Gear Marks on Filament)

**Symptoms:**
- Filament surface damaged
- Plastic shavings visible
- Filament may jam in extruder

**Causes:**
- Gears too tight on filament
- Motor applying excessive force
- Filament not advancing (blocked downstream)
- Buffer pressure too high causing over-feed

**Solutions:**

1. **Reduce Pressure**
   - Loosen gear tension
   - Reduce motor PWM
   - Check buffer spring not too stiff

2. **Check Downstream Path**
   - Verify filament can exit freely
   - Check tube not kinked
   - Ensure printer extruder not jammed

3. **Monitor Buffer**
   - Adjust buffer to maintain normal pressure
   - Don't let pressure go too high consistently

## Print Quality Issues

### Stuck at 99% After Print

**Symptoms:**
- Print completes but stays at 99%
- Filament not retracting
- BMCU keeps trying to feed

**Known Issue:** Common with BMCU, especially older versions. BMCU-C with bi-directional buffer reduces this.

**Immediate Fix:**
1. Manually pull filament back slightly
2. Press buffer slider to relieve pressure
3. Cancel print if necessary
4. Unload filament via printer menu

**Root Causes:**
- Buffer pressure prevents retraction
- Five-pass connector pushed up
- Filament caught in tube
- Communication timing issue

**Long-term Solutions:**

1. **Buffer Adjustment**
   - Reduce buffer spring tension slightly
   - Ensure slider moves freely
   - Check pneumatic fitting not overtight

2. **Firmware Tuning**
   - Adjust retraction distance
   - Modify pressure thresholds
   - Update to latest firmware

3. **Mechanical**
   - Ensure tube not kinked
   - Check five-pass connector secure
   - Verify filament path clear

### Color Changes Incorrect

**Symptoms:**
- Wrong color printed
- Colors mixed during transition
- Purge tower issues

**Not BMCU Issue If:**
- Slicing software color assignment wrong
- Printer filament slots don't match BMCU
- Purge settings inadequate

**BMCU Issue If:**
- Filament data not saved correctly
- Channel reported to printer wrong
- Flash memory corruption

**Solutions:**

1. **Verify Slicer Settings**
   - Check filament assigned to correct slot (1-4)
   - Match physical BMCU channel loading

2. **Check Flash Data**
   - Filament info saved? (check after loading)
   - Try reloading filament data
   - Reflash firmware if data corrupt

3. **Increase Purge**
   - More purge volume in slicer
   - Slower color change for better transition

### Hub Ejection (Five-Pass Pushed Up)

**Symptoms:**
- Five-pass connector unseats from printer
- Filament path disrupted
- Feeding fails

**Cause:** 370 motor has high torque, can push against connector if buffer pressure high

**BMCU-C Advantage:** Bi-directional buffer significantly reduces this issue

**Solutions:**

1. **Buffer Pressure Management**
   - Keep buffer pressure in normal range
   - Don't let it stay at high consistently
   - Adjust spring if needed

2. **Mechanical Securing**
   - Add retention clip to five-pass connector
   - Use tape or zip tie as temporary fix
   - Ensure connector fully seated before print

3. **Reduce Feed Force**
   - Lower motor PWM
   - Adjust buffer response

## Electrical Issues

### LED Colors Wrong

**Symptoms:**
- LEDs show unexpected colors
- Brightness too high/low
- Some LEDs don't light

**Diagnostic:**

1. **Check Wiring**
   - Verify data line continuity
   - Ensure power (5V) and GND connected
   - Check WS2812B orientation (DIN → DOUT)

2. **Test LED Chain**
   - First LED in chain most critical
   - If first LED fails, entire chain fails
   - Check solder joints on first LED

3. **Voltage Levels**
   - WS2812B requires 5V (or 3.3V tolerant version)
   - Data signal should be 3.3V from MCU
   - May need level shifter (some WS2812B tolerate 3.3V)

**Solutions:**
- Reflow solder on LED
- Replace failed LED
- Check orientation (GND, DIN, 5V, DOUT)
- Reduce brightness if overheating: `setBrightness(10)`

### Overheating

**Symptoms:**
- Components hot to touch
- System resets unexpectedly
- LEDs fade or flicker

**Check:**
- Motor driver temperature
- Voltage regulator temperature
- LED current draw

**Solutions:**
- Reduce LED brightness (main.cpp)
- Add heatsinks to hot components
- Improve airflow around electronics
- Check for short circuits

### Power Supply Issues

**Symptoms:**
- System unstable
- Resets during motor activation
- Voltage drops visible

**Diagnostic:**
- Measure 24V under load (should not drop below 22V)
- Check for voltage drops during motor spin-up
- Verify printer PSU adequate

**Solutions:**
- Use higher capacity power supply
- Add bulk capacitors (1000µF+)
- Check wire gauge adequate (20 AWG minimum)
- Reduce simultaneous motor activation

## Software/Firmware Issues

### Firmware Won't Flash

**Symptoms:**
- Upload fails in PlatformIO
- Error messages during flash
- Device not recognized

**Solutions:**

1. **Check USB Connection**
   - Try different USB cable
   - Use different USB port
   - Install CH340 drivers (Windows)

2. **Enter Bootloader**
   - Hold BOOT0, press RESET, release BOOT0
   - Try manual bootloader entry
   - Check if device appears in device manager

3. **Use Alternative Tool**
   - WCH-LinkE hardware programmer
   - ISP Flash Tool from WCH
   - Try different computer

### Firmware Behaves Erratically

**Symptoms:**
- Random resets
- Strange LED behavior
- Unexpected motor actions

**Possible Causes:**
- Watchdog timer (disabled in code, but check)
- Memory corruption
- Stack overflow
- Interrupt priority issue

**Debugging:**
- Enable debug output
- Check for NULL pointer dereferences
- Verify array bounds
- Check stack usage

**Solutions:**
- Reflash firmware
- Reset to factory defaults (reload from flash)
- Check for code modifications that may cause issues

## Advanced Diagnostics

### Using Serial Debug Output

Enable in code:
```cpp
#define Debug_log_on
```

Connect USART3 (PB10=TX, PB11=RX) to USB-serial adapter.

**Useful Debug Points:**
```cpp
// Check AS5600 angles
DEBUG_MY("Angle[0]: ");
DEBUG_float(MC_AS5600.raw_angle[0], 0);
DEBUG_MY("\n");

// Check ADC values
float *adc = ADC_DMA_get_value();
DEBUG_MY("ADC[0]: ");
DEBUG_float(adc[0], 3);
DEBUG_MY("\n");

// Check filament state
DEBUG_MY("Online: ");
DEBUG_MY(get_filament_online(0) ? "Yes\n" : "No\n");
```

### Hardware Testing

**Continuity Tests:**
- All GND connections
- Power rails (24V, 5V, 3.3V)
- Data lines (I2C, ADC, PWM)

**Voltage Tests:**
- Measure all power rails under load
- Check ADC input ranges
- Verify PWM output levels

**Signal Tests:**
- Oscilloscope on RS-485 A/B
- Check WS2812B data signal timing
- Monitor I2C SCL/SDA with logic analyzer

## Getting Help

### Information to Provide

When asking for help, include:
1. **Hardware:** PCB version, motor type, sensor types
2. **Firmware:** Version number, any modifications
3. **Symptoms:** Detailed description, when it occurs
4. **LED Status:** All LED colors and patterns
5. **Debug Output:** Serial log if available
6. **Steps Taken:** What you've already tried

### Community Resources

- **BMCU Wiki:** https://wiki.yuekai.fr/en/BMCU
- **Chinese Wiki:** https://bmcu.wanzii.cn/
- **Gitee:** https://gitee.com/at_4061N/BMCU
- **GitHub:** https://github.com/Xing-C/BMCU370x

### Before Creating Issue

✓ Search existing issues first  
✓ Read troubleshooting guide  
✓ Try basic solutions  
✓ Gather diagnostic information  
✓ Prepare clear description with photos/videos if helpful

## Prevention and Maintenance

### Regular Checks

**Daily (If Printing):**
- LED status normal
- No unusual sounds
- Filament feeding smoothly

**Weekly:**
- Clean filament path
- Check gear condition
- Verify all connections secure

**Monthly:**
- Clean gears thoroughly
- Check magnet positions
- Test all channels
- Update firmware if available

### Best Practices

✅ Keep BMCU in clean, dust-free environment  
✅ Use quality filament with consistent diameter  
✅ Don't exceed 100% print speed initially  
✅ Monitor buffer pressure during prints  
✅ Keep firmware updated  
✅ Maintain good cable management  
✅ Document any modifications made  

---

**Remember:** BMCU-C is a DIY project. Some issues are normal and expected. Patience and systematic troubleshooting are key to success!
