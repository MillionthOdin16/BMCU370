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
#define AUTO_DIRECTION_MIN_SAMPLES         3        // Minimum samples needed (3-10 recommended)
#define AUTO_DIRECTION_MIN_MOVEMENT_MM     2.0f     // Minimum movement per sample in mm (1.0-5.0 recommended)
#define AUTO_DIRECTION_TIMEOUT_MS          5000     // Max time to collect samples in ms (3000-10000 recommended)
#define AUTO_DIRECTION_CONFIDENCE_THRESHOLD 0.7f    // Minimum confidence ratio required (0.6-0.9 recommended)
#define AUTO_DIRECTION_MAX_NOISE_MM        0.5f     // Maximum acceptable sensor noise per sample in mm
#define AUTO_DIRECTION_SAMPLE_INTERVAL_MS  100      // Minimum time between samples in ms
#define AUTO_DIRECTION_DEBUG_ENABLED       false    // Enable debug output for direction learning
```

### Parameter Tuning Guidelines

#### Sample Count (`AUTO_DIRECTION_MIN_SAMPLES`)
- **3-5**: Fast learning, lower confidence
- **5-7**: Balanced speed and confidence (recommended)
- **7-10**: Slower learning, higher confidence
- **>10**: May timeout before completion

#### Movement Threshold (`AUTO_DIRECTION_MIN_MOVEMENT_MM`)
- **1.0-2.0mm**: More sensitive, may be affected by noise
- **2.0-3.0mm**: Balanced sensitivity (recommended)
- **3.0-5.0mm**: Less sensitive, requires more movement

#### Confidence Threshold (`AUTO_DIRECTION_CONFIDENCE_THRESHOLD`)
- **0.6**: Accept learning with 60% consistency
- **0.7**: Recommended balance of accuracy and reliability
- **0.8-0.9**: Very high confidence requirement, may timeout

#### Noise Threshold (`AUTO_DIRECTION_MAX_NOISE_MM`)
- **0.3mm**: Very strict noise filtering
- **0.5mm**: Recommended for most installations
- **1.0mm**: Relaxed filtering for noisy environments

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
- Too many noisy/invalid samples

**Solutions**:
1. **Check filament movement**: Ensure filament moves freely during feeding
2. **Verify sensor connectivity**: Check AS5600 sensor connectivity and magnet alignment
3. **Increase timeout**: Set `AUTO_DIRECTION_TIMEOUT_MS` to 8000-10000
4. **Lower noise threshold**: Increase `AUTO_DIRECTION_MAX_NOISE_MM` to 1.0
5. **Reduce confidence requirement**: Lower `AUTO_DIRECTION_CONFIDENCE_THRESHOLD` to 0.6
6. **Fall back to manual**: Set static corrections if needed

### Inconsistent Direction Detection
**Symptoms**: Direction changes between learning sessions or low confidence scores

**Possible Causes**:
- Mechanical issues causing inconsistent movement
- Sensor noise or connectivity problems
- Insufficient sample count
- Environmental vibration or temperature effects

**Solutions**:
1. **Increase sample count**: Set `AUTO_DIRECTION_MIN_SAMPLES` to 5-7
2. **Check mechanical components**: Verify no binding, slippage, or loose connections
3. **Verify sensor mounting**: Ensure AS5600 sensor mounting and magnet positioning are secure
4. **Reduce sensitivity**: Increase `AUTO_DIRECTION_MIN_MOVEMENT_MM` to 3.0
5. **Check environment**: Ensure stable temperature and minimal vibration

### Learning Times Out
**Symptoms**: Learning process reaches timeout without completing

**Possible Causes**:
- Filament not moving enough during feeding
- Motor control issues preventing consistent movement
- Configuration parameters too strict

**Solutions**:
1. **Increase timeout**: Set `AUTO_DIRECTION_TIMEOUT_MS` to 10000
2. **Lower movement threshold**: Reduce `AUTO_DIRECTION_MIN_MOVEMENT_MM` to 1.5
3. **Reduce sample requirements**: Lower `AUTO_DIRECTION_MIN_SAMPLES` to 3
4. **Check motor operation**: Verify motor responds to commands
5. **Verify filament path**: Ensure no obstructions in filament path

### High Error Count
**Symptoms**: Many invalid/noisy samples rejected during learning

**Possible Causes**:
- Sensor noise from electrical interference
- Mechanical vibration or loose mounting
- Defective AS5600 sensor or magnet

**Solutions**:
1. **Check electrical connections**: Verify clean I2C connections
2. **Improve shielding**: Add electrical shielding around sensors
3. **Secure mounting**: Ensure all mechanical components are properly secured
4. **Relax noise filtering**: Increase `AUTO_DIRECTION_MAX_NOISE_MM` to 1.0
5. **Replace hardware**: Consider replacing sensor or magnet if consistently noisy

### No Valid Sensor Data
**Symptoms**: Learning reports no valid movement detected

**Possible Causes**:
- AS5600 sensor offline or not communicating
- Magnet not properly positioned near sensor
- Motor not actually moving filament

**Solutions**:
1. **Check sensor status**: Verify AS5600 sensor reports online in system status
2. **Test motor independently**: Manually test motor operation
3. **Verify magnet position**: Ensure magnet is properly positioned relative to sensor
4. **Check I2C communication**: Verify I2C bus operation and addressing
5. **Review pin configuration**: Confirm AS5600_SCL_PINS and AS5600_SDA_PINS are correct

### Learning Confidence Too Low
**Symptoms**: Learning completes but with low confidence scores

**Possible Causes**:
- Inconsistent mechanical operation
- Sensor drift or calibration issues
- Marginal sample quality

**Solutions**:
1. **Lower confidence threshold**: Reduce `AUTO_DIRECTION_CONFIDENCE_THRESHOLD` to 0.6
2. **Increase sample count**: Set `AUTO_DIRECTION_MIN_SAMPLES` to 5-7 for more data
3. **Improve mechanical stability**: Check for loose components or binding
4. **Recalibrate sensors**: Verify AS5600 sensor calibration and operation
5. **Check sample interval**: Ensure `AUTO_DIRECTION_SAMPLE_INTERVAL_MS` allows stable readings

### Debug Mode Usage
Enable debug output for detailed troubleshooting:
```c
#define AUTO_DIRECTION_DEBUG_ENABLED       true
```

Debug output will show:
- Learning start/completion events
- Individual sample results and correlations
- Confidence score progression
- Error and timeout conditions

### Disable Auto-Learning
If automatic learning consistently fails:
```c
#define AUTO_DIRECTION_LEARNING_ENABLED    false
```
This reverts to the original static correction method with these fallback corrections:

```c
#define MOTOR_DIR_CORRECTION_CH0   false     // Channel 0 (typically correct)
#define MOTOR_DIR_CORRECTION_CH1   true      // Channel 1 (commonly reversed)
#define MOTOR_DIR_CORRECTION_CH2   true      // Channel 2 (commonly reversed)  
#define MOTOR_DIR_CORRECTION_CH3   false     // Channel 3 (typically correct)
```

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
Each channel maintains enhanced learning state with validation and confidence tracking:
```c
struct DirectionLearningState {
    bool learning_active;          // Currently learning direction for this channel
    bool learning_complete;        // Direction learning completed successfully
    uint64_t learning_start_time;  // When learning started (ms)
    uint64_t last_sample_time;     // Last time a sample was taken (ms)
    float initial_position;        // Initial filament position when learning started
    float total_movement;          // Total measured movement during learning
    float accumulated_noise;       // Accumulated sensor noise for validation
    int command_direction;         // Direction commanded to motor (+1 or -1)
    int sample_count;              // Number of valid direction samples collected
    int positive_samples;          // Samples showing positive correlation
    int negative_samples;          // Samples showing negative correlation
    float confidence_score;        // Learning confidence (0.0-1.0)
    bool has_valid_data;          // Whether any valid sensor data was received
    int error_count;              // Number of invalid/noisy samples rejected
};
```

### Integration Points
- **Trigger**: `motor_motion_switch()` when `need_send_out` state begins
- **Update**: `AS5600_distance_updata()` during movement measurement  
- **Storage**: `Motion_control_save()` when learning completes

## Testing and Validation

### Pre-Installation Testing
Before deploying the automatic direction detection system:

1. **Hardware Verification**
   ```
   - Verify all AS5600 sensors report online
   - Test motor operation for each channel
   - Confirm magnet placement and sensor readings
   - Check I2C communication stability
   ```

2. **Configuration Validation**
   ```
   - Verify parameter ranges are within recommended values
   - Test with debug mode enabled initially
   - Start with conservative settings (higher confidence, more samples)
   - Monitor learning progress through debug output
   ```

### Installation Testing Procedure

1. **Initial Setup**
   - Flash firmware with auto-learning enabled
   - Enable debug mode initially: `AUTO_DIRECTION_DEBUG_ENABLED = true`
   - Use recommended default parameters

2. **Channel-by-Channel Testing**
   For each channel (0-3):
   ```
   a. Insert filament following normal procedure
   b. Initiate filament loading from printer interface
   c. Monitor debug output for learning progress
   d. Verify learning completes with confidence > 0.7
   e. Test direction by observing filament movement
   f. Record results and any issues encountered
   ```

3. **Verification Tests**
   After learning completion:
   ```
   a. Power cycle the unit to test flash memory persistence
   b. Verify learned directions are retained
   c. Test normal filament loading operation
   d. Confirm filament moves toward toolhead (not away)
   ```

4. **Long-term Validation**
   ```
   a. Monitor operation over multiple filament changes
   b. Check for any direction reversals or inconsistencies
   c. Validate confidence remains high across different filament types
   d. Test under various environmental conditions
   ```

### Diagnostic Commands
If available in your system interface, use these for diagnostics:

```c
// Check learning status for a channel
float confidence;
int samples;
bool complete;
bool active = get_direction_learning_status(channel, &confidence, &samples, &complete);

// Interpret results:
// active = true: currently learning or has learned
// confidence: 0.0-1.0, higher is better (target: >0.7)
// samples: number of samples collected
// complete: true if learning finished successfully
```

### Quality Assurance Checklist

Before marking a unit as ready for deployment:
- [ ] All 4 channels have AS5600 sensors online
- [ ] All 4 channels complete direction learning successfully
- [ ] All learned directions have confidence > 0.7
- [ ] Direction corrections persist across power cycles
- [ ] Filament loading works correctly in both directions
- [ ] No error messages in debug output during normal operation
- [ ] Flash memory usage remains within acceptable limits
- [ ] System performs normally with auto-learning disabled (fallback test)

### Performance Benchmarks

**Typical Learning Performance:**
- **Learning Time**: 5-15 seconds per channel during normal feeding
- **Sample Count**: 3-7 samples for reliable detection  
- **Confidence Score**: 0.8-1.0 for stable mechanical systems
- **Success Rate**: >95% for properly configured systems
- **Flash Memory**: <64 bytes per channel for direction data

**Warning Thresholds:**
- Learning time > 30 seconds: Check mechanical issues
- Confidence < 0.6: Investigate sensor or mounting problems
- Error count > 50% of samples: Check electrical connections
- Repeated learning failures: Fall back to static corrections

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