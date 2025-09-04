# Automatic Motor Direction Detection

This document describes the new automatic motor direction detection system that eliminates the need for hardware disassembly when troubleshooting motor direction issues.

## Overview

The BMCU370 firmware now includes an intelligent direction learning system that automatically detects correct motor direction during normal filament feeding operations. This feature addresses the motor reversal issues on channels 1, 2, and sometimes channel 3, where filament is driven away from the tool head instead of toward it.

## How It Works

### Traditional Method (Fallback)
The original system performs startup motor calibration by:
1. Running each motor at fixed PWM (1000) for up to 2 seconds
2. Reading AS5600 hall sensor angle changes 
3. Applying static per-channel corrections based on known hardware differences
4. Saving the corrected direction to flash memory

### New Automatic Learning Method
The new system learns motor direction during actual filament feeding operations by:
1. **Triggering during normal operation**: When filament feeding begins (`need_send_out` command)
2. **Correlating commands with movement**: Comparing commanded motor direction with actual movement detected by AS5600 sensors
3. **Sampling multiple measurements**: Collecting at least 3 samples of 2mm+ movement to ensure accuracy
4. **Determining correlation**: If commanded direction matches actual movement, direction is correct; if opposite, direction is inverted
5. **Automatic saving**: Once learned, the correct direction is saved to flash memory and marked as "auto-learned"

## Key Benefits

✅ **No hardware disassembly required** - Detection happens during normal filament loading  
✅ **Real operating conditions** - Uses actual filament movement instead of no-load motor testing  
✅ **Higher accuracy** - Multiple samples with actual filament resistance provide more reliable detection  
✅ **User-friendly** - Completely automatic with no user intervention required  
✅ **Backward compatible** - Falls back to static corrections if auto-learning is disabled  

## Configuration

### Enable/Disable Auto-Learning
In `config.h`:
```c
#define AUTO_DIRECTION_LEARNING_ENABLED    true     // Enable automatic learning (recommended)
```

### Learning Parameters
```c
#define AUTO_DIRECTION_MIN_SAMPLES         3        // Minimum samples needed (increase for more confidence)
#define AUTO_DIRECTION_MIN_MOVEMENT_MM     2.0f     // Minimum movement per sample (mm)
#define AUTO_DIRECTION_TIMEOUT_MS          5000     // Max time to collect samples (ms)
```

### Fallback Static Corrections
If auto-learning is disabled, these static corrections are applied:
```c
#define MOTOR_DIR_CORRECTION_CH0   false     // Channel 0 (typically correct)
#define MOTOR_DIR_CORRECTION_CH1   true      // Channel 1 (commonly reversed)
#define MOTOR_DIR_CORRECTION_CH2   true      // Channel 2 (commonly reversed)  
#define MOTOR_DIR_CORRECTION_CH3   false     // Channel 3 (typically correct)
```

## Operation Flow

### First-Time Setup
1. Flash firmware with automatic direction detection enabled
2. Install filament in any channel following normal procedures
3. Initiate filament loading from printer interface
4. System automatically learns correct direction during feeding
5. Direction is saved to flash memory for future use

### Ongoing Operation
- Once learned, directions are remembered across power cycles
- No re-learning required unless flash memory is erased
- System can learn different channels independently

### Learning Process Details
```
1. Filament feed command received (need_send_out)
2. Start direction learning for channel
3. Begin motor movement with commanded direction
4. Monitor AS5600 sensor for actual movement direction
5. Collect samples when movement ≥ 2mm occurs
6. After 3+ samples, determine correlation:
   - Positive correlation → Direction is correct (save +1)
   - Negative correlation → Direction is inverted (save -1)
7. Save learned direction to flash memory
8. Continue normal operation with correct direction
```

## Troubleshooting

### Learning Fails to Complete
**Symptoms**: Direction remains unlearned after multiple feeding attempts

**Possible Causes**:
- Insufficient filament movement (< 2mm per sample)
- AS5600 sensor not detecting movement properly
- Timeout reached before collecting enough samples

**Solutions**:
1. Ensure filament moves freely during feeding
2. Check AS5600 sensor connectivity and magnet alignment
3. Increase timeout value in configuration
4. Fall back to manual direction setting if needed

### Inconsistent Direction Detection
**Symptoms**: Direction changes between learning sessions

**Possible Causes**:
- Mechanical issues causing inconsistent movement
- Sensor noise or connectivity problems
- Insufficient sample count

**Solutions**:
1. Increase `AUTO_DIRECTION_MIN_SAMPLES` for more confidence
2. Check mechanical components for binding or slippage
3. Verify AS5600 sensor mounting and magnet positioning

### Disable Auto-Learning
If automatic learning causes issues, disable it:
```c
#define AUTO_DIRECTION_LEARNING_ENABLED    false
```
This reverts to the original static correction method.

## Technical Implementation

### Data Storage
Direction learning status is stored in flash memory:
```c
struct Motion_control_save_struct {
    int Motion_control_dir[4];     // Direction for each channel (+1/-1)
    bool auto_learned[4];          // Whether auto-learned vs static correction
    int check = 0x40614061;        // Validation checksum
};
```

### Learning State Tracking
Each channel maintains learning state:
```c
struct DirectionLearningState {
    bool learning_active;          // Currently learning
    bool learning_complete;        // Learning successful
    uint64_t learning_start_time;  // Start timestamp
    float total_movement;          // Accumulated movement
    int command_direction;         // Commanded direction
    int sample_count;              // Samples collected
    int positive_samples;          // Matching samples
    int negative_samples;          // Inverted samples
};
```

### Integration Points
- **Trigger**: `motor_motion_switch()` when `need_send_out` state begins
- **Update**: `AS5600_distance_updata()` during movement measurement  
- **Storage**: `Motion_control_save()` when learning completes

## Future Enhancements

### Potential Improvements
1. **Confidence scoring**: Track learning confidence and re-learn if low
2. **Drift detection**: Monitor for direction changes over time
3. **Multi-condition learning**: Learn under different load conditions
4. **Diagnostic reporting**: Provide learning status feedback to user interface

### Advanced Configuration Options
Consider adding:
- Per-channel learning sensitivity
- Minimum confidence thresholds
- Automatic re-learning triggers
- Learning history logging

## Conclusion

The automatic direction detection system provides a robust, user-friendly solution to motor direction issues without requiring hardware modifications or disassembly. It leverages the existing sensor infrastructure to provide more accurate direction detection under real operating conditions.

For most users, simply enabling the feature and following normal filament loading procedures will automatically resolve direction issues permanently.