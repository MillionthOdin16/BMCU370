# Quick Start Guide - BMCU-C 370 Hall Version

## Welcome to BMCU-C!

This guide will help you get started with your BMCU-C 370 Hall Effect version filament management system.

## Before You Begin

### Prerequisites
- ✅ Completed BMCU-C assembly (see [Installation Guide](installation.md) if not done)
- ✅ Bambu Lab A1 or A1 mini printer
- ✅ Firmware flashed on BMCU-C mainboard (version 0020 recommended)
- ✅ 24V power supply (typically from printer's AMS hub connection)
- ✅ Filament spools ready to load

### Important Safety Notices

⚠️ **WARNING:** This is a DIY project for personal use only. Not endorsed by Bambu Lab.

⚠️ **CAUTION:** Ensure correct polarity when connecting power. Reversed polarity may damage components.

⚠️ **NOTE:** Future printer firmware updates may affect BMCU compatibility.

## What You Need to Know

### BMCU-C vs Official AMS Lite

**Similarities:**
- Four filament channels
- Compatible with A-series printers
- Multi-material printing support
- Automatic filament loading/unloading

**Differences:**
- ❌ No NFC/RFID filament recognition
- ✅ Can be placed in dry box (separate from unit)
- ✅ Bi-directional buffer (less hub ejection)
- ✅ Smart buffer features (auto feed/retract)
- ✅ Open source and customizable

**Recommendation:** Use official AMS Lite when possible for best experience. BMCU-C is for DIY enthusiasts and learning.

## Initial Setup

### Step 1: Power Connection

1. **Locate the 6-pin AMS Hub connector** on your BMCU-C mainboard
2. **Connect to printer's AMS port** using the provided cable
   - Pin layout: A, B, GND, 24V (ensure correct orientation)
3. **Verify power:**
   - Mainboard LED should light up
   - LED color indicates status:
     - 🔴 Red = No communication (normal before printer connection)
     - 🔵 Blue = Communication established

### Step 2: Verify Hall Sensors

**For each channel (1-4):**

1. **Check top LED (fiber optic):**
   - 🔴 Red = Magnet not detected or incorrect position
   - 🔵 Blue = Normal (magnet detected correctly)

2. **If red:**
   - Check radial magnet installation on motor shaft
   - Verify AS5600 sensor soldering and alignment
   - Distance should be ~1-2mm from magnet

### Step 3: Buffer Calibration

**For each channel:**

1. **Press buffer slider in:**
   - Side LED should turn 🔵 Blue
   - If red appears, flip the slider magnet 180°

2. **Pull buffer slider out:**
   - Side LED should turn 🔴 Red
   - If blue appears, flip the slider magnet 180°

3. **Release to middle position:**
   - LED should turn off (black) or show filament status

### Step 4: Printer Configuration

1. **Power on your Bambu Lab printer**
2. **Navigate to Settings → AMS**
3. **The printer should detect BMCU-C as "AMS Lite"**
4. **Verify all four channels are recognized**

**If not detected:**
- Check cable connections
- Verify 24V power is present
- Restart printer
- Check BambuBus communication (see [Troubleshooting](troubleshooting.md))

## Loading Filament

### Method 1: Manual Loading (Recommended for First Time)

1. **Select a channel** on printer screen
2. **Insert filament** into the top of the chosen BMCU-C channel
3. **Push until you feel resistance** at the gears
4. **Channel side LED should turn white** (filament detected)
5. **Press "Load Filament"** on printer
6. **Wait for filament** to reach the printer's extruder
7. **Filament color should appear** on printer screen

### Method 2: Smart Buffer Loading

1. **Insert filament** into channel
2. **Press buffer slider in firmly**
3. **BMCU-C automatically feeds filament** (if firmware supports)
4. **Release when filament is loaded**

### Loading Multiple Filaments

**Best Practice:**
- Load all four channels before starting a print
- Ensure each filament color is correctly set in slicer
- Verify filament colors match on printer screen

## First Print

### Preparing Your Slicer

1. **Open Bambu Studio or OrcaSlicer**
2. **Enable AMS** in printer settings
3. **Assign filaments** to slots 1-4 matching your BMCU-C
4. **Create or open a multi-color model**
5. **Slice and send to printer**

### During the Print

**Normal Operation:**
- Mainboard LED: 🔵 Blue
- Active channel top LED: White
- Inactive channels: Blue
- Side LEDs: White when filament is present

**Monitor for:**
- Buffer pressure (no consistent red side LED)
- Smooth filament transitions
- No grinding or clicking sounds

### After First Print

**Success indicators:**
- Print completed to 100%
- Filament retracted properly
- All channels return to standby (blue top LEDs)

**If stuck at 99%:**
- See [Troubleshooting - Stuck at 99%](troubleshooting.md#stuck-at-99)

## LED Status Reference

### Quick Status Guide

| LED Location | Color | Meaning | Action |
|-------------|-------|---------|--------|
| Mainboard | Blue | Online, normal | None |
| Mainboard | Red | Offline/error | Check connection |
| Top (fiber) | Blue | Standby | Normal |
| Top (fiber) | White | Channel active | Normal during print |
| Top (fiber) | Red | Hall sensor error | Check magnet |
| Side | Off/Black | No filament | Insert filament if needed |
| Side | White | Filament loaded | Normal |
| Side | Blue | Buffer pressed | Normal when pressed |
| Side | Red | Buffer extended | Normal when pulled |

## Basic Troubleshooting

### Filament Not Detected

**Symptoms:** Side LED stays black when filament inserted

**Solutions:**
1. Ensure filament is pushed past the gears
2. Check ADC voltage (should be > 1.65V when loaded)
3. Verify wiring to sub-board
4. Test with different filament (opaque, not transparent)

### Channel Shows Red Top LED

**Symptoms:** Top LED is red instead of blue

**Solutions:**
1. Check radial magnet installation on motor
2. Verify AS5600 sensor is soldered correctly
3. Ensure magnet distance is 1-2mm from sensor
4. Test magnet polarity

### Printer Doesn't Detect BMCU

**Symptoms:** AMS Lite doesn't appear in printer settings

**Solutions:**
1. Check 6-pin cable connection
2. Verify 24V power present
3. Restart printer
4. Check BambuBus A/B wiring polarity
5. Update BMCU firmware if needed

### Motor Doesn't Run

**Symptoms:** No motor movement during load command

**Solutions:**
1. Check motor power connections
2. Verify PWM signal from mainboard
3. Test motor independently
4. Check for mechanical binding

## Maintenance Schedule

### Daily (If Printing)
- ✓ Check for filament tangles
- ✓ Verify LED status normal
- ✓ Listen for unusual sounds

### Weekly
- ✓ Clean filament path
- ✓ Check gear mesh (no excessive wear)
- ✓ Verify buffer movement smooth

### Monthly
- ✓ Clean BMG gears (remove any buildup)
- ✓ Check all electrical connections
- ✓ Verify magnet positions
- ✓ Update firmware if available

### As Needed
- Clean metal gear shavings (if using metal gears)
- Re-calibrate buffer if behavior changes
- Tighten any loose screws

## Getting Help

### Documentation Resources
- [Hardware Overview](hardware-overview.md) - Detailed component information
- [Troubleshooting Guide](troubleshooting.md) - Common issues and solutions
- [Maintenance Guide](maintenance.md) - Detailed maintenance procedures

### Community Resources
- **English Wiki:** https://wiki.yuekai.fr/en/BMCU
- **Chinese Wiki:** https://bmcu.wanzii.cn/
- **Gitee Repository:** https://gitee.com/at_4061N/BMCU
- **GitHub:** https://github.com/Xing-C/BMCU370x

### Before Asking for Help

Please gather:
1. BMCU-C version and firmware version (0020, etc.)
2. Printer model and firmware version
3. LED status description
4. Error symptoms
5. Steps already tried

## Tips for Success

### Filament Selection
- ✅ Use quality filament with consistent diameter
- ✅ Avoid extremely flexible materials initially
- ✅ Opaque colors detect better than transparent
- ✅ Keep filament dry

### Environmental
- ✅ Use dry box for hygroscopic materials
- ✅ Avoid dusty environments
- ✅ Maintain reasonable room temperature (15-30°C)

### Operational
- ✅ Don't exceed 100% print speed initially
- ✅ Allow BMCU to warm up before printing
- ✅ Test single-channel before multi-color
- ✅ Keep filament paths clear

### Upgrades to Consider
- Metal worm gears (difficult to install, reduces wear)
- Better pneumatic fittings
- Improved cable management
- Dry box integration

## Next Steps

Now that you have BMCU-C running:

1. **Test each channel individually** with single-color prints
2. **Try a simple two-color print** to verify filament changes
3. **Gradually increase complexity** as you gain confidence
4. **Read the full documentation** to understand the system better
5. **Join the community** to share experiences and improvements

## Advanced Topics

For advanced users, explore:
- [Developer Guide](../developer-guide/architecture.md) - System architecture
- [BambuBus Protocol](../developer-guide/bambubus-protocol.md) - Communication details
- [Code Reference](../developer-guide/code-reference.md) - Firmware functions

---

**Happy Printing!** 🎨🖨️

If you encounter issues, remember: the BMCU community is active and helpful. Don't hesitate to seek assistance and share your experiences!
