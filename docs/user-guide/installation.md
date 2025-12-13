# Installation and Assembly Guide - BMCU-C 370 Hall Version

## Overview

This guide covers the complete assembly process for the BMCU-C 370 Hall version. Follow these steps carefully for best results.

**Estimated Time:** 4-8 hours for first-time builders
**Skill Level:** Intermediate (requires soldering and mechanical assembly)
**Tools Required:** See Tools section below

---

## Before You Begin

### Prerequisites

✅ **All parts gathered** - See [Bill of Materials](../hardware/bom.md) for complete list
✅ **PCBs received** - Mainboard and 4 sub-boards
✅ **3D printed parts** ready - All printed and cleaned
✅ **Tools prepared** - See list below
✅ **Workspace set up** - Clean, well-lit, ESD-safe

### Assembly Strategy

**Recommended Approach:**
1. Build and test ONE channel completely first
2. Verify firmware upload and basic operation
3. Scale to remaining three channels
4. Final integration and testing

**Why this works:**
- Catch errors early with minimal rework
- Learn the process before mass assembly
- Verify PCBs and components are good
- Build confidence before full commitment

---

## Required Tools

### Soldering Equipment

- **Soldering iron:** Temperature-controlled, 300-350°C
- **Solder:** 0.6-0.8mm diameter, leaded or lead-free
- **Flux:** Rosin flux pen or paste
- **Solder wick:** For removing solder/fixing mistakes
- **Tweezers:** Fine-point for SMD work
- **Magnification:** Loupe or microscope (5-10x)

### Hand Tools

- **Screwdriver set:** Phillips head, various sizes
- **Hex keys:** Metric set (2mm, 2.5mm, 3mm)
- **Pliers:** Needle-nose
- **Wire strippers:** For cables
- **Cutter:** Flush-cut for trimming
- **Hobby knife:** For cleaning 3D prints

### Testing Equipment

- **Multimeter:** For continuity and voltage checks
- **USB-to-TTL adapter:** OR CH340-equipped mainboard
- **Power supply:** 24V DC (can use printer initially)
- **Computer:** With PlatformIO installed

### Optional but Helpful

- Hot air station (for SMD rework)
- Helping hands/PCB holder
- Anti-static mat and wrist strap
- Label maker (for channel identification)

---

## Assembly Steps

### Phase 1: PCB Assembly (if not pre-assembled)

⚠️ **Skip if using assembled PCBs from JLCPCB or similar**

#### Mainboard Soldering

**Order of Assembly:**
1. **SMD components first** (smallest to largest)
   - Resistors and capacitors
   - IC chips (CH32V203, TP75176E, etc.)
   - LEDs
2. **Through-hole components**
   - Connectors (PH2.0, headers)
   - Larger capacitors
3. **Mechanical parts**
   - USB connector (if applicable)
   - Mounting holes/standoffs

**Critical SMD Components:**
- CH32V203C8T6: Ensure pin 1 alignment, no bridges
- TP75176E-SR: Verify orientation
- WS2812B (mainboard): Check polarity marking

**Soldering Tips:**
- Use flux liberally for SMD work
- Solder temperature: 320-350°C for lead-free
- Check for bridges between pins with magnification
- Test continuity on power rails (no shorts!)

#### Sub-board Soldering (×4)

**Critical Components:**

1. **AS5600 Hall Sensor**
   - **Package:** SOIC-8 or SSOP-8
   - **Orientation:** Pin 1 marking must align
   - **Tip:** Use flux, drag soldering technique
   - **Test:** Continuity from AS5600 pins to MCU connection

2. **WS2812B LEDs (2 per board)**
   - **Package:** 4020 side-mount
   - **Polarity:** CRITICAL - check datasheet
   - **Marking:** Small notch/dot indicates pin 1
   - **Orientation:** Side-emitting must face outward
   - **Common error:** Reversed LEDs won't work

3. **Passive Components**
   - Resistors, capacitors as per schematic
   - Check values with multimeter if unsure

**Soldering Order:**
1. AS5600 sensor
2. Small resistors and capacitors
3. WS2812B LEDs (very carefully!)
4. Connectors

**Quality Check After Soldering:**
```
✓ No solder bridges between pins
✓ All joints shiny and well-wetted
✓ No cold solder joints (dull, grainy)
✓ AS5600 pins have continuity to connector
✓ Power and ground no shorts (>10kΩ)
✓ WS2812B oriented correctly
```

---

### Phase 2: Motor and Gear Assembly

#### Step 1: Install Radial Magnet on Motor Shaft

**Process:**
1. **Clean motor shaft** with isopropyl alcohol
2. **Slide 6mm radial magnet** onto shaft
3. **Position:** 1-2mm from where AS5600 will be
4. **Secure with adhesive** (cyanoacrylate or epoxy)
5. **Let cure fully** before proceeding

**Critical:**
- Magnet must be RADIAL magnetization
- Test with compass - should rotate as shaft turns
- Centered on shaft, not tilted

#### Step 2: Install BMG Gears

**Plastic Gears (Recommended):**

1. **Worm gear on motor shaft:**
   - May need gentle pressure to fit
   - Align with motor shaft flat (if present)
   - Position so it meshes properly with spur gear
   - Secure with set screw (tighten gently!)

2. **Spur gear on filament shaft:**
   - Press onto 3D printed shaft
   - Ensure smooth rotation
   - Align with worm gear

3. **Adjust mesh:**
   - Gears should just touch
   - Too tight = grinding, motor strain
   - Too loose = slipping, clicking
   - Test by hand rotation (should be smooth)

**Metal Gears (Optional):**

If using metal worm gears:
- **Challenge:** 2mm bore often 1.95-2.0mm
- **Method:** Heat worm gear with hot air (~150°C)
- **Install:** Quickly press onto shaft while hot
- **Cool:** Hold position until cool
- **Check:** Ensure tight fit, won't slip

#### Step 3: Install Motor in Housing

1. **Place motor** in 3D printed motor mount
2. **Attach sub-board** so AS5600 aligns with magnet
3. **Verify distance:** 1-2mm gap (adjust mount if needed)
4. **Secure with screws** (M2 or M3 as designed)

**Alignment Check:**
- AS5600 centered over magnet
- Magnet rotates freely without hitting sensor
- Gap: 1-2mm (use caliper if available)

---

### Phase 3: Buffer System Assembly

#### Buffer Slider

1. **Install radial magnet in slider:**
   - Press fit or glue into designated pocket
   - **Test polarity:**
     - Press slider in → should show BLUE on LED
     - Pull slider out → should show RED on LED
   - If wrong, flip magnet 180°

2. **Install spring:**
   - 0.5mm × 6mm × 10mm spring (standard)
   - Or heavier duty if needed
   - Ensure free compression and return

3. **Test movement:**
   - Slider should move smoothly
   - Spring should return slider to neutral
   - No binding or catching

#### Pneumatic Fitting

1. **Thread PC4 fitting** into slider or housing
   - M5 or M6 thread (depending on design)
   - Hand-tight, don't over-tighten plastic
   - Test with PTFE tube insertion/removal

---

### Phase 4: Electronics Integration

#### Sub-board to Housing

1. **Mount sub-board** in channel housing
2. **Position AS5600** aligned with motor magnet
3. **Secure with M2 screws** (typically 2-4 screws)
4. **Connect cables:**
   - Motor power (2-pin)
   - AS5600 I2C (SDA, SCL, VCC, GND)
   - Buffer Hall sensor
   - Filament detection sensor
   - WS2812B data line

**Cable Management:**
- Keep cables neat and secured
- Avoid pinching in moving parts
- Test motor rotation doesn't catch cables

#### Connect to Mainboard

**Channel Connection Cables:**
- **Channels 1-2:** 5cm cables (if using recommended)
- **Channels 3-4:** 8cm cables
- **Pin mapping:** Verify against schematic

**Important:**
- PH2.0 connectors may have retention
- Ensure fully seated
- Check polarity (usually keyed)

**Power Distribution:**
- All motors share 24V bus
- GND common to all channels
- No mixing power and signal grounds

---

### Phase 5: Firmware Upload

#### First-Time Flash

**Using Type-C mainboard:**
1. Connect USB-C cable to PC
2. Install CH340 drivers if needed (Windows)
3. Open PlatformIO project
4. Click "Upload"
5. Wait for completion

**Using external USB-to-TTL:**
1. Connect: GND, TX→RX, RX→TX, 3.3V
2. Enter bootloader mode:
   - Hold BOOT0 button
   - Press RESET button
   - Release RESET, then BOOT0
3. Upload via PlatformIO
4. Power cycle to run

**Verification:**
- Mainboard LED should light up
- Check serial output if debug enabled
- LEDs should respond to power-on

#### Initial LED Test

**Expected behavior after flash:**
- **Mainboard LED:** Red (no printer connection yet)
- **Channel top LEDs:** Red if magnet missing, Blue if OK
- **Channel side LEDs:** Black (no filament)

**If LEDs wrong:**
- Check WS2812B orientation
- Verify data pin connections
- Check power (5V or 3.3V depending on LED variant)

---

### Phase 6: Single Channel Testing

#### Test 1: AS5600 Sensor

**Enable debug output** and check serial:
```cpp
// Should see angle readings
DEBUG_MY("Angle: ");
DEBUG_float(MC_AS5600.raw_angle[0], 0);
```

**Expected:**
- Angle changes when motor shaft rotated manually
- Range: 0-4095 (12-bit)
- Smooth progression, no jumps

**If not working:**
- Check magnet present and radial
- Verify I2C connections (SDA, SCL, VCC, GND)
- Ensure AS5600 soldered correctly
- Test with different magnet distance

#### Test 2: Motor Control

**Manual test:**
1. Apply 24V power
2. Send PWM command (via firmware)
3. Motor should rotate

**Check:**
- Direction correct for feed/retract
- No excessive noise
- Gears mesh properly
- No mechanical binding

#### Test 3: Buffer Detection

**Test procedure:**
1. Press buffer slider in → Side LED turns BLUE
2. Release → LED turns BLACK or WHITE (if filament)
3. Pull slider out → LED turns RED

**If polarity wrong:**
- Remove slider, flip magnet 180°
- Re-test

#### Test 4: Filament Detection

**Insert filament:**
1. Push past gears into detection zone
2. Side LED should turn WHITE
3. Printer interface should show filament online

**If not detected:**
- Check sensor alignment
- Verify ADC connection
- Test with opaque filament (not transparent)
- Check voltage threshold in code

---

### Phase 7: Scale to Four Channels

**After confirming one channel works:**

1. **Assemble channels 2-4** using same procedure
2. **Test each individually** before integration
3. **Verify unique I2C addressing:**
   - Channel 1: SCL=PB15, SDA=PD0
   - Channel 2: SCL=PB14, SDA=PC15
   - Channel 3: SCL=PB13, SDA=PC14
   - Channel 4: SCL=PB12, SDA=PC13

4. **Check all LEDs** show correct colors
5. **Verify no I2C conflicts** (all sensors online)

---

### Phase 8: Final Assembly

#### Base Integration

1. **Mount four channels** in base structure
2. **Secure with M3 screws** as designed
3. **Connect all cables** to mainboard
4. **Install MX3.0 mirror cable** for base connection

#### Cable Management

- Route cables neatly
- Use zip ties or cable channels
- Prevent movement during operation
- Avoid sharp bends in PTFE tubes

#### Fiber Optic Installation

1. **Cut fiber** to length for each channel
2. **Insert into light guide** hole
3. **Position near WS2812B** LED on sub-board
4. **Secure** so it doesn't fall out
5. **Test** - should see LED color at fiber end

---

### Phase 9: Printer Integration

#### Connect to Printer

1. **Power off printer**
2. **Locate AMS port** on back of printer
3. **Connect 6-pin cable:**
   - BMCU mainboard → Printer AMS port
   - Verify polarity (usually keyed)
4. **Power on printer**

**Expected:**
- Mainboard LED turns BLUE (online)
- Printer detects "AMS Lite" in settings
- All four channels visible in interface

#### Initial Configuration

1. **Open printer settings** → AMS
2. **Verify four channels** detected
3. **Load test filament** in channel 1
4. **Trigger load** from printer menu
5. **Observe:**
   - Motor runs
   - Filament advances
   - Buffer responds to pressure
   - LED turns white (active)

---

## Testing and Calibration

### Functional Tests

**Test 1: Load Filament**
- Insert filament in each channel
- Trigger load via printer
- Should feed to extruder smoothly

**Test 2: Unload Filament**
- Trigger unload
- Should retract cleanly
- Buffer should decompress

**Test 3: Filament Change**
- Start print with color change
- Verify smooth transition
- Check purge tower for quality

**Test 4: Multi-Color Print**
- Simple 2-color test print
- Monitor all channels
- Check for errors or jams

### Calibration

**Buffer Spring Tension:**
- If always high pressure: Soften spring or adjust threshold
- If always low pressure: Stiffen spring or adjust threshold
- Ideal: Mostly neutral, responds to actual feed

**Motor PWM:**
- Default may be too aggressive
- Reduce if filament chewing occurs
- Increase if slipping/insufficient torque

**LED Brightness:**
- Default: 35/255 mainboard, 15/255 channels
- Adjust in `main.cpp` → `RGB_Set_Brightness()`
- Reduce if too bright or overheating

---

## Troubleshooting Assembly Issues

### AS5600 Red LED (Magnet Not Detected)

**Check:**
1. Magnet installed?
2. Magnet radial, not axial?
3. Distance 1-2mm from sensor?
4. AS5600 soldered correctly?

### WS2812B LEDs Not Working

**Check:**
1. Orientation correct? (polarity matters!)
2. First LED in chain critical
3. Data pin connected?
4. Power (5V or 3.3V) present?

### Motor Doesn't Run

**Check:**
1. 24V power present?
2. Motor connections correct?
3. PWM signal from MCU?
4. Mechanical binding?

### Gears Slipping

**Fix:**
1. Tighten gear mesh (push motor toward gears)
2. Check set screws tight
3. Verify gears not worn/damaged
4. Consider metal worm gear

---

## Safety and Best Practices

### During Assembly

✓ Use ESD protection for electronics
✓ Don't exceed 350°C soldering temperature
✓ Wear safety glasses when cutting/trimming
✓ Work in ventilated area (solder fumes)
✓ Keep workspace organized and clean

### Testing

✓ Start with low power/PWM
✓ Don't force mechanical parts
✓ Monitor for overheating
✓ Have fire extinguisher nearby (electronics)
✓ Disconnect power when making changes

### Operation

✓ Don't exceed 100% print speed initially
✓ Monitor first few prints closely
✓ Keep filament path clear
✓ Regularly check for wear/loosening
✓ Maintain dry box if using hygroscopic filaments

---

## Post-Assembly Checklist

Before declaring complete:

- [ ] All four channels assembled
- [ ] Firmware uploaded successfully
- [ ] All LEDs showing correct colors
- [ ] All AS5600 sensors reading angles
- [ ] All motors run smoothly
- [ ] Buffer sliders respond correctly
- [ ] Filament detection working (all channels)
- [ ] Printer recognizes BMCU as AMS Lite
- [ ] Test filament loaded successfully
- [ ] Test filament unloaded successfully
- [ ] No unusual noises or vibrations
- [ ] All cables secured and managed
- [ ] Passed single-color test print
- [ ] Passed multi-color test print
- [ ] Documentation read and understood

---

## Maintenance Schedule

**After Assembly:**
- First week: Daily visual inspection
- First month: Weekly check for loose screws
- Ongoing: Monthly gear inspection

**See [Troubleshooting Guide](troubleshooting.md) for ongoing maintenance details**

---

## Getting Help

**If you encounter issues:**

1. Check [Troubleshooting Guide](troubleshooting.md)
2. Review assembly steps for missed items
3. Test components individually
4. Post in community forums with details

**Include when asking for help:**
- Assembly step where stuck
- Photos of the problem area
- LED status description
- Any error messages
- What you've already tried

---

## Resources

- **Bill of Materials:** [Complete parts list](../hardware/bom.md)
- **Hardware Details:** [Hardware Overview](hardware-overview.md)
- **Firmware Setup:** [Development Setup](../developer-guide/development-setup.md)
- **Community Wiki:** https://bmcu.wanzii.cn/doc/build/370hall.html

---

**Congratulations on completing your BMCU-C assembly!**

With patience and attention to detail, you should now have a fully functional four-channel filament management system. Enjoy your multi-color printing! 🎨

---

**Document Version:** 1.0
**Last Updated:** 2025-12-12
**Compatible with:** BMCU-C 370 Hall V0.1-0020
