# BMCU370t Code Improvements Analysis

**Focus:** Code-only improvements (excluding documentation/MD files)
**Date:** 2025-12-13

---

## Executive Summary

BMCU370t contains **significant code-level improvements** over BMCU370, particularly:
1. **Centralized configuration system** (`config.h`)
2. **Automatic motor direction learning** (eliminates manual calibration)
3. **Edge detection for filament presence** (auto-start feeding)
4. **Enhanced input validation** with debug error messages
5. **Improved debugging infrastructure** with float support
6. **Better code organization** using `MAX_FILAMENT_CHANNELS` constant
7. **Version extraction automation** (Python script)
8. **Const-correctness improvements**

---

## 1. NEW: Centralized Configuration System (`src/config.h`)

**Impact:** Major architectural improvement - 199 lines of well-documented configuration

### Features:
- **Hardware configuration:** LED pins, counts, brightness, system clock
- **Communication settings:** UART baud rate, protocol version
- **Firmware versions:** Separate AMS and AMS Lite version definitions
- **Motion control tuning:** Voltage thresholds, timing constants, distances
- **Flash memory layout:** Addresses and magic numbers centralized
- **Sensor configuration:** I2C pins, mathematical constants
- **Default filament properties:** Colors, temperatures, material names
- **Channel definitions:** `MAX_FILAMENT_CHANNELS = 4` (scalability)
- **Motor direction learning:** Auto-detection configuration constants

### Code Quality Benefits:
```c
// BEFORE (BMCU370) - Magic numbers scattered:
float PULL_voltage_up = 1.85f;
uint64_t Assist_send_time = 1200;
#define use_flash_addr ((uint32_t)0x0800F000)

// AFTER (BMCU370t) - Centralized and documented:
#define PULL_VOLTAGE_HIGH       1.85f       ///< High pressure threshold (red LED)
#define ASSIST_SEND_TIME_MS     1200        ///< Filament send assist duration
#define FLASH_SAVE_ADDRESS      0x0800F000UL ///< Flash memory address
```

**Maintainability:** Changing hardware now requires editing ONE file instead of searching through multiple source files.

---

## 2. NEW: Automatic Motor Direction Learning

**Impact:** HUGE improvement - eliminates need for manual hardware disassembly and calibration

### New Data Structures:

```c
struct DirectionLearningState
{
    bool learning_active;          // Currently learning direction
    bool learning_complete;        // Learning completed successfully
    uint64_t learning_start_time;  // When learning started (ms)
    uint64_t last_sample_time;     // Last sample time
    float initial_position;        // Initial filament position
    float total_movement;          // Total measured movement
    float accumulated_noise;       // Accumulated sensor noise
    int command_direction;         // Direction commanded (+1 or -1)
    int sample_count;              // Valid samples collected
    int positive_samples;          // Positive correlation samples
    int negative_samples;          // Negative correlation samples
    float confidence_score;        // Learning confidence (0.0-1.0)
    bool has_valid_data;          // Whether valid data received
    int error_count;              // Invalid/noisy samples rejected
} direction_learning[MAX_FILAMENT_CHANNELS];

struct LoadingDirectionState
{
    bool detection_active;         // Currently detecting loading direction
    bool detection_complete;       // Detection completed
    uint64_t detection_start_time; // Detection start time
    uint64_t stable_time;          // Time when presence became stable
    bool initial_presence;         // Initial presence sensor state
    bool presence_lost;            // Whether presence was lost during test
    int test_direction;            // Direction being tested (+1 or -1)
    int confirmed_loading_direction; // Confirmed loading direction
    bool presence_stable_phase;    // In stable monitoring phase
} loading_detection[MAX_FILAMENT_CHANNELS];
```

### Configuration Options (`config.h`):

```c
#define AUTO_DIRECTION_LEARNING_ENABLED    true     // Enable automatic learning
#define AUTO_DIRECTION_MIN_SAMPLES         3        // Min samples (3-10)
#define AUTO_DIRECTION_MIN_MOVEMENT_MM     2.0f     // Min movement (1.0-5.0mm)
#define AUTO_DIRECTION_TIMEOUT_MS          5000     // Timeout (3000-10000ms)
#define AUTO_DIRECTION_CONFIDENCE_THRESHOLD 0.7f    // Min confidence (0.6-0.9)
#define AUTO_DIRECTION_MAX_NOISE_MM        0.5f     // Max noise per sample
#define AUTO_DIRECTION_SAMPLE_INTERVAL_MS  100      // Sample interval
#define AUTO_DIRECTION_DEBUG_ENABLED       false    // Debug output

// Fallback static correction if auto-learning disabled/fails
#define MOTOR_DIR_CORRECTION_CH0   false
#define MOTOR_DIR_CORRECTION_CH1   true
#define MOTOR_DIR_CORRECTION_CH2   true
#define MOTOR_DIR_CORRECTION_CH3   false
```

### New Functions:

```c
void start_direction_learning(int channel, int commanded_direction);
void update_direction_learning(int channel, float movement_delta);
bool finalize_direction_learning(int channel);
bool get_direction_learning_status(int channel, float* confidence, int* samples, bool* complete);
void reset_direction_learning(int channel);
void reset_all_learned_directions();
```

### How It Works:

1. **During normal filament feeding**, the system correlates:
   - Motor command direction (+1 or -1)
   - Actual filament movement from Hall sensor

2. **Collects samples** until confidence threshold reached
3. **Validates noise levels** - rejects erratic samples
4. **Auto-corrects** motor direction if negative correlation detected
5. **Saves to flash** - persists across power cycles
6. **Tracks metadata** - whether direction was auto-learned vs static correction

### Benefits:
- **No hardware disassembly required**
- **More accurate** - learns under real operating conditions
- **Adaptive** - can re-learn if mechanical changes occur
- **Self-documenting** - tracks learning confidence and sample count

---

## 3. NEW: Edge Detection for Filament Presence

**Impact:** Automatic feeding when filament is inserted

### New State Variable:

```c
int MC_ONLINE_key_stu_prev[MAX_FILAMENT_CHANNELS] = {0, 0, 0, 0};
// Previous presence sensor state for edge detection
```

### Edge Detection Logic (src/Motion_control.cpp):

```c
void MC_PULL_ONLINE_read()
{
    // Store previous presence sensor states for edge detection
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        MC_ONLINE_key_stu_prev[i] = MC_ONLINE_key_stu[i];
    }

    // ... read ADC values and process ...

    // Detect presence sensor rising edge (filament insertion)
    if (MC_ONLINE_key_stu_prev[i] == 0 && MC_ONLINE_key_stu[i] == 1) {
        // Filament presence detected for the first time
        if (get_filament_motion(i) == AMS_filament_motion::idle) {
            DEBUG_MY("Auto-start feeding for channel ");
            DEBUG_float(i, 0);
            DEBUG_MY(" - presence detected\n");
            set_filament_motion(i, AMS_filament_motion::need_send_out);
        }
    }
}
```

### Benefits:
- **Better UX** - Automatic feeding when filament inserted
- **Edge-triggered** - Only triggers on state transition (0→1), not continuous presence
- **Prevents spurious triggers** - Only when idle, won't interfere with existing operations

---

## 4. Enhanced Input Validation with Debug Messages

**Impact:** Better error handling and debugging

### Example: `Set_MC_RGB()` in `src/main.cpp`

**BEFORE (BMCU370):**
```c
void Set_MC_RGB(uint8_t channel, int num, uint8_t R, uint8_t G, uint8_t B)
{
    // Bug #31 Fix: Add bounds check for channel parameter
    if (channel >= 4)
        return;  // Silent failure

    int set_colors[3] = {R, G, B};
    // ... rest of code ...
}
```

**AFTER (BMCU370t):**
```c
void Set_MC_RGB(uint8_t channel, int num, uint8_t R, uint8_t G, uint8_t B)
{
    // Input validation - bounds checking
    if (channel >= MAX_FILAMENT_CHANNELS) {
        DEBUG_MY("ERROR: Invalid channel in Set_MC_RGB\n");
        return;
    }

    if (num < 0 || num >= 2) { // Each channel has max 2 LEDs
        DEBUG_MY("ERROR: Invalid LED num in Set_MC_RGB\n");
        return;
    }

    const int set_colors[3] = {R, G, B};  // const correctness
    // ... rest of code ...
}
```

### Improvements:
- **Validates BOTH parameters** (channel and num)
- **Debug error messages** - easier troubleshooting
- **Uses constant** `MAX_FILAMENT_CHANNELS` instead of magic number `4`
- **Const correctness** - `set_colors` marked const
- **Comments explain why** - "Each channel has max 2 LEDs"

---

## 5. Enhanced Debugging Infrastructure

**Impact:** Better float debugging and cleaner macro definitions

### New Function: `Debug_log_write_float()` in `src/Debug_log.cpp`

```c
void Debug_log_write_float(const void *prefix, float value, int precision)
{
    char buffer[32];
    int prefix_len = strlen((const char*)prefix);
    memcpy(buffer, prefix, prefix_len);

    // Convert float to string with specified precision
    int int_part = (int)value;
    float frac_part = value - int_part;
    if (frac_part < 0) frac_part = -frac_part;

    int len = sprintf(buffer + prefix_len, "%d", int_part);
    if (precision > 0) {
        buffer[prefix_len + len] = '.';
        len++;
        for (int i = 0; i < precision; i++) {
            frac_part *= 10;
            int digit = (int)frac_part;
            buffer[prefix_len + len + i] = '0' + digit;
            frac_part -= digit;
        }
        len += precision;
    }

    Debug_log_write_num(buffer, prefix_len + len);
}
```

### Improved Macros in `src/Debug_log.h`:

**BEFORE (BMCU370):**
```c
#ifdef Debug_log_on
    #define DEBUG_init() Debug_log_init()
    #define DEBUG_MY(logs) Debug_log_write(logs)
#else
    #define DEBUG(logs) ; ;  // Wrong name, extra semicolons
#endif
```

**AFTER (BMCU370t):**
```c
#ifdef Debug_log_on
    #define DEBUG_init() Debug_log_init()
    #define DEBUG_MY(logs) Debug_log_write(logs)
    #define DEBUG_num(logs, num) Debug_log_write_num(logs, num)
    #define DEBUG_float(logs, precision) Debug_log_write_float("", logs, precision)
    #define DEBUG_time() Debug_log_time()
    #define DEBUG_get_time() Debug_log_count64()
    #define DEBUG_time_log() DEBUG_time()
#else
    #define DEBUG_init() do {} while(0)
    #define DEBUG_MY(logs) do {} while(0)
    #define DEBUG_num(logs, num) do {} while(0)
    #define DEBUG_float(logs, precision) do {} while(0)
    #define DEBUG_time() do {} while(0)
    #define DEBUG_get_time() (0ULL)
    #define DEBUG_time_log() do {} while(0)
#endif
```

### Improvements:
- **NEW:** `DEBUG_float()` macro for floating-point debugging
- **Proper macro hygiene** - `do {} while(0)` prevents common macro bugs
- **Consistent naming** - All macros properly defined when disabled
- **Includes config.h** - Uses `DEBUG_UART_BAUDRATE` from config

---

## 6. Better Code Organization with Constants

**Impact:** Scalability and maintainability

### Using `MAX_FILAMENT_CHANNELS` Throughout Codebase

**BEFORE (BMCU370):**
```c
float speed_as5600[4] = {0, 0, 0, 0};
int MC_PULL_stu[4] = {0, 0, 0, 0};
bool MC_STU_ERROR[4] = {false, false, false, false};

for (int i = 0; i < 4; i++) { /* ... */ }
```

**AFTER (BMCU370t):**
```c
float speed_as5600[MAX_FILAMENT_CHANNELS] = {0, 0, 0, 0};
int MC_PULL_stu[MAX_FILAMENT_CHANNELS] = {0, 0, 0, 0};
bool MC_STU_ERROR[MAX_FILAMENT_CHANNELS] = {false, false, false, false};

for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) { /* ... */ }
```

**Benefits:**
- **Single point of change** - Easy to support 8-channel variant
- **Self-documenting** - Clear what the array represents
- **Type safety** - Compile-time constant checking

### Loop Improvements in `src/main.cpp`:

**BEFORE:**
```c
void RGB_init() {
    strip_PD1.begin();
    strip_channel[0].begin();
    strip_channel[1].begin();
    strip_channel[2].begin();
    strip_channel[3].begin();
}

void RGB_show_data() {
    strip_PD1.show();
    strip_channel[0].show();
    strip_channel[1].show();
    strip_channel[2].show();
    strip_channel[3].show();
}
```

**AFTER:**
```c
void RGB_init() {
    strip_PD1.begin();
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        strip_channel[i].begin();
    }
}

void RGB_show_data() {
    strip_PD1.show();
    for (int i = 0; i < MAX_FILAMENT_CHANNELS; i++) {
        strip_channel[i].show();
    }
}
```

**Benefits:**
- **DRY principle** - Don't Repeat Yourself
- **Easier to maintain** - Add/remove channels in one place
- **Less error-prone** - Can't forget a channel

---

## 7. NEW: Version Extraction Script

**Impact:** Build automation and version management

### File: `scripts/version_extract.py` (135 lines)

**Features:**
- Extracts version from git tags
- Falls back to commit count if no tags
- Reads current firmware versions from `config.h`
- Provides multiple output formats:
  - JSON format for CI/CD integration
  - Individual version components
  - Human-readable format

**Usage:**
```bash
# Get JSON output
./scripts/version_extract.py --json

# Get AMS version string
./scripts/version_extract.py --ams-version
# Output: 00.00.06.49

# Get git tag
./scripts/version_extract.py --git-tag
# Output: v2.0.2 (or dev-123)
```

**Integration Points:**
- Can be integrated into CI/CD workflows
- Useful for firmware artifact naming
- Helps track build provenance

---

## 8. Const Correctness Improvements

**Impact:** Better memory safety and compiler optimization

### Example: Flash Read in `src/BambuBus.cpp`

**BEFORE (BMCU370):**
```c
bool Bambubus_read()
{
    flash_save_struct *ptr = (flash_save_struct *)(use_flash_addr);
    if ((ptr->check == 0x40614061) && (ptr->version == Bambubus_version))
    {
        memcpy(&data_save, ptr, sizeof(data_save));
        return true;
    }
    return false;
}
```

**AFTER (BMCU370t):**
```c
bool Bambubus_read()
{
    const flash_save_struct *ptr = (const flash_save_struct *)(FLASH_SAVE_ADDRESS);

    if ((ptr->check == FLASH_MAGIC_NUMBER) && (ptr->version == BAMBU_BUS_VERSION))
    {
        memcpy(&data_save, ptr, sizeof(data_save));
        return true;
    }
    return false;
}
```

**Improvements:**
- **const pointer** - Prevents accidental modification of flash data
- **Named constants** - Uses `FLASH_SAVE_ADDRESS` instead of magic number
- **Better formatting** - Extra whitespace for readability

---

## 9. Improved Motion Control Logic

**Impact:** Better filament handling

### Fixed Speed Setting in `src/Motion_control.cpp`

**BEFORE (BMCU370):**
```c
if (motion == filament_motion_enum::filament_motion_send)
{
    if (device_type == BambuBus_AMS_lite)
    {
        if (MC_PULL_stu_raw[CHx] < PULL_VOLTAGE_SEND_MAX)
            speed_set = 30;
        else
            speed_set = 0; // Changed from original 10 - potential issue!
    }
    else
    {
        speed_set = 50;
    }
}
```

**AFTER (BMCU370t):**
```c
if (motion == filament_motion_enum::filament_motion_send)
{
    if (device_type == BambuBus_AMS_lite)
    {
        if (MC_PULL_stu_raw[CHx] < PULL_VOLTAGE_SEND_MAX)
        {
            speed_set = 30;
        }
        else
        {
            speed_set = 10; // Restored original working value
        }
    }
    else
    {
        speed_set = 50;
    }
}
```

**Fix:** Restored `speed_set = 10` instead of `0`, fixing potential feeding stall issue.

---

## 10. Integration with Direction Learning

**Impact:** Automatic calibration during normal operation

### Update Direction Learning During Movement

```c
void _Motion_control_run(float T)
{
    // ... existing angle/speed calculation code ...

    distance_E = -(float)(now_distance - last_distance + cir_E) * AS5600_PI * 7.5 / 4096;
    as5600_distance_save[i] = now_distance;

    float speedx = distance_E / T * 1000;
    speed_as5600[i] = speedx;
    add_filament_meters(i, distance_E / 1000);

    // NEW: Update automatic direction learning with movement data
    if (AUTO_DIRECTION_LEARNING_ENABLED && fabs(distance_E) > 0.1) {
        update_direction_learning(i, distance_E);
    }
}
```

**Benefits:**
- **Zero-impact** - Learning happens during normal operation
- **Threshold-based** - Only updates with significant movement (>0.1mm)
- **Configurable** - Can disable via `AUTO_DIRECTION_LEARNING_ENABLED`

---

## Summary Table: Code Improvements in BMCU370t

| Improvement | Impact | Files Affected | Lines Changed |
|-------------|--------|----------------|---------------|
| Centralized config.h | High | All source files | +199 new |
| Auto direction learning | Very High | Motion_control.cpp/h | +400 approx |
| Edge detection (auto-feed) | Medium | Motion_control.cpp | +15 |
| Enhanced input validation | Medium | main.cpp | +10 |
| Float debugging support | Low | Debug_log.cpp/h | +40 |
| MAX_FILAMENT_CHANNELS const | Medium | All files | ~100 changes |
| Version extraction script | Low | scripts/ | +135 new |
| Const correctness | Low | BambuBus.cpp | +5 |
| Loop refactoring | Low | main.cpp | +20/-40 |
| Speed setting fix | Medium | Motion_control.cpp | +3 |

---

## Recommendations for BMCU370

### Should Adopt from BMCU370t:

1. ✅ **config.h** - Critical for maintainability
2. ✅ **Auto direction learning** - Major UX improvement
3. ✅ **Edge detection auto-feed** - Better user experience
4. ✅ **Enhanced validation with debug** - Easier debugging
5. ✅ **DEBUG_float() support** - Better diagnostics
6. ✅ **MAX_FILAMENT_CHANNELS** - Code scalability
7. ✅ **Const correctness** - Memory safety
8. ✅ **Speed setting fix** (speed_set = 10 not 0)

### Should Merge INTO BMCU370t:

1. ⚠️ **Bug #1 fix** - Unsigned cast for bounds checking
2. ⚠️ **Bug #2 fix** - ADC null pointer check
3. ⚠️ **Bug #3 fix** - Negative meters validation
4. ⚠️ **Bug #25** - CRC error tracking
5. ⚠️ **Bug #27** - Buffer overflow checks

---

## Conclusion

**BMCU370t has significantly better code architecture** with:
- Centralized configuration management
- Automatic motor direction learning (game-changer!)
- Edge-triggered auto-feeding
- Better debugging infrastructure
- Scalable design patterns

However, it's **missing critical safety checks** from BMCU370's bug fixes.

**The ideal firmware would merge BMCU370t's architectural improvements with BMCU370's bug fixes.**

---

*Analysis Date: 2025-12-13*
*Comparison: BMCU370 vs BMCU370t (code only, excluding documentation)*
