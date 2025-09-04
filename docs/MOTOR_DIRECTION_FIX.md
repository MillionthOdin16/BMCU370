# Motor Direction Reversal Fix

## Issue Description

The BMCU370 firmware has a known issue where certain channels (typically channels 1 and 2, sometimes channel 3) have reversed motor direction despite correct wiring. This causes the filament to be driven away from the tool head when the slider is pressed, instead of toward it.

## Root Cause

The issue occurs during automatic motor direction detection in the `MOTOR_get_dir()` function. The detection algorithm:

1. Applies PWM to each motor channel
2. Measures angle displacement using AS5600 Hall sensors
3. Determines direction based on positive/negative angle change

However, due to hardware variations between channels:
- Different physical mounting orientations of AS5600 sensors
- Inconsistent gear orientations across channels  
- PCB layout differences
- **Inconsistent magnet polarity orientation across channels**

The direction interpretation can be incorrect for certain channels.

### Magnet Polarity Impact

**Critical Factor**: The AS5600 hall sensors rely on diametrically magnetized magnets, and the polarity orientation directly affects direction detection:

- **Consistent Polarity**: When all magnets are installed with the same pole (North or South) facing the sensor, angle readings increase/decrease consistently for the same rotation direction across all channels
- **Inconsistent Polarity**: When magnets are installed with different pole orientations, the same motor rotation direction will produce opposite angle changes on different channels
- **Assembly Sensitivity**: Without specific polarity marking in assembly instructions, magnets may be installed randomly, causing unpredictable direction reversals

This explains why the issue appears on certain channels (1, 2, and sometimes 3) but not others - it depends on how the magnets were oriented during assembly.

## Solution

### Automatic Direction Correction (Primary Fix)

The firmware now includes automatic direction correction for affected channels:

```c
// In config.h
#define MOTOR_DIR_CORRECTION_CH0   false     // Channel 0 (typically correct)
#define MOTOR_DIR_CORRECTION_CH1   true      // Channel 1 (commonly reversed)
#define MOTOR_DIR_CORRECTION_CH2   true      // Channel 2 (commonly reversed)  
#define MOTOR_DIR_CORRECTION_CH3   false     // Channel 3 (typically correct)
```

This correction is applied automatically after direction detection but before the values are saved to flash memory.

### Configuration Options

#### For Standard Hardware (Default)
The default configuration should work for most BMCU370 units:
- Channels 1 and 2: Direction correction enabled
- Channels 0 and 3: No correction needed

#### For Units with Different Channel Issues
If your unit has a different combination of affected channels:

1. **Channel 3 affected instead**: Set `MOTOR_DIR_CORRECTION_CH3` to `true` and others as needed
2. **Different combination**: Adjust the correction flags in `config.h` accordingly

#### Manual Override (Advanced)
For persistent issues or non-standard hardware, you can manually set directions:

```c
// In MOTOR_init(), uncomment and modify:
set_motor_directions(1, 1, 1, 1); // 1=forward, -1=reverse for channels 0,1,2,3
```

## Testing Your Fix

### Expected Behavior After Fix
1. **All channels should feed filament toward the tool head** when the slider is pressed
2. **Consistent behavior** across all channels regardless of which module is installed
3. **No need to reverse motor wires** in hardware

### Verification Steps
1. Flash the updated firmware
2. Test each channel with filament insertion
3. Verify that pressing the slider feeds filament toward tool head on all channels
4. Swap modules between channels to confirm consistent behavior

## Implementation Details

### Code Changes Made

1. **config.h**: Added motor direction correction configuration flags
2. **Motion_control.cpp**: Added correction logic in `MOTOR_get_dir()` function
3. **Documentation**: Added this guide and improved code comments

### How It Works

```c
// After direction detection
for (int index = 0; index < 4; index++) {
    // Apply direction correction if needed for this channel
    if (motor_dir_correction[index] && dir[index] != 0) {
        dir[index] = -dir[index]; // Invert the detected direction
    }
    Motion_control_data_save.Motion_control_dir[index] = dir[index];
}
```

The correction is applied:
- Only to channels where correction is enabled
- Only when a direction was successfully detected (non-zero)
- Before saving to flash memory
- Automatically on every direction detection cycle

## Troubleshooting

### If Issues Persist
1. **Double-check configuration**: Verify the correction flags match your hardware
2. **Clear saved directions**: Reset flash memory to force re-detection
3. **Use manual override**: Set directions explicitly using `set_motor_directions()`
4. **Check wiring**: Ensure motor wires are connected correctly
5. **Verify magnet polarity**: Ensure all magnets have consistent pole orientation

### Magnet Polarity Verification
If you suspect inconsistent magnet polarity is causing direction issues:

1. **Visual Inspection**: If magnets are marked, verify all have the same pole facing the sensor
2. **Magnetic Field Test**: Use a compass or magnetic field indicator to check polarity
3. **Software Test**: Temporarily disable direction correction for all channels in `config.h`:
   ```c
   #define MOTOR_DIR_CORRECTION_CH0   false
   #define MOTOR_DIR_CORRECTION_CH1   false  
   #define MOTOR_DIR_CORRECTION_CH2   false
   #define MOTOR_DIR_CORRECTION_CH3   false
   ```
4. **Individual Channel Testing**: Test each channel separately to identify which have reversed direction
5. **Custom Correction**: Update correction flags to match your specific hardware configuration

### For Different Hardware Variants
If you have a hardware variant with different affected channels:
1. Test each channel individually
2. Identify which channels are reversed
3. Update the correction flags in `config.h` accordingly
4. Rebuild and test

### Hardware Assembly Recommendations
For future assemblies to prevent direction issues:
- **Mark magnet polarity** during manufacturing/assembly
- **Use consistent installation procedure** for all channels
- **Include polarity verification** in quality control testing
- **Document magnet orientation** in assembly instructions

## Compatibility

- **Backward Compatible**: Existing installations will automatically benefit from the fix
- **No Hardware Changes**: Fix is purely firmware-based
- **No Manual Intervention**: Works automatically after firmware update
- **Configurable**: Can be adjusted for different hardware variants

## Related Issues

This fix addresses:
- Issue #9: Filament drive direction reversed on channels 1 and 2
- Units requiring motor wire resoldering as workaround
- Inconsistent behavior when swapping modules between channels

The fix eliminates the need for hardware workarounds while maintaining compatibility with the existing BambuBus protocol and AMS functionality.