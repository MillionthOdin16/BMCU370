# BMCU370t Code Improvements Patch

## Overview

This patch file (`bmcu370t-improvements-v2.patch`) applies key code improvements from BMCU370t to BMCU370 while **preserving all existing bug fixes** in BMCU370.

**✓ SAFE TO APPLY** - This patch has been tested and applies cleanly to BMCU370 main branch.

---

## What This Patch Includes

### 1. **Centralized Configuration (config.h)** ⭐⭐⭐
- **NEW FILE**: `src/config.h` (199 lines)
- Centralizes all magic numbers and configuration constants
- Easy hardware reconfiguration - change ONE file instead of many
- Well-documented with inline comments

**Configuration categories:**
- Hardware (LED pins, brightness, clock speed)
- Communication (UART baud rate, protocol version)
- Firmware versions (AMS & AMS Lite)
- Motion control (voltages, timing, distances)
- Flash memory layout
- Sensor configuration
- Default filament properties
- Channel definitions (`MAX_FILAMENT_CHANNELS = 4`)

### 2. **Enhanced Debugging Infrastructure** ⭐⭐
- **NEW**: `DEBUG_float()` macro for floating-point debugging
- **NEW**: `Debug_log_write_float()` function
- Proper macro hygiene with `do {} while(0)` wrapping
- Eliminates common macro expansion bugs

**Usage examples:**
```c
DEBUG_MY("Speed: ");
DEBUG_float(speed_as5600[i], 2);  // Output: "Speed: 25.37"

DEBUG_MY("Position: ");
DEBUG_float(position, 3);          // Output: "Position: 123.456"
```

### 3. **Code Quality Improvements**
- Better macro definitions (no more naked semicolons)
- Uses config.h constants (`DEBUG_UART_BAUDRATE`)
- Includes proper headers (`stdio.h`, `string.h`)
- Cleaner code organization

---

## What This Patch DOES NOT Include

To ensure safety and preserve BMCU370's bug fixes, this patch **deliberately excludes**:

❌ Automatic motor direction learning (complex, needs more testing)
❌ Edge detection for auto-feeding (could change behavior)
❌ Changes to BambuBus protocol handling
❌ Changes to Motion_control logic
❌ Changes to main.cpp logic

**All existing BMCU370 bug fixes are preserved:**
- ✅ Bug #1: Unsigned cast bounds checking
- ✅ Bug #2: ADC null pointer check
- ✅ Bug #3: Negative meters validation
- ✅ Bug #25: CRC error tracking
- ✅ Bug #27: Buffer overflow checks

---

## How to Apply This Patch

### Prerequisites
- Clean BMCU370 repository (no uncommitted changes)
- On the `main` branch or equivalent
- Git installed

### Application Steps

#### Method 1: Using `git apply` (Recommended)

```bash
# 1. Ensure you're on the main branch
git checkout main

# 2. Ensure working directory is clean
git status

# 3. Test if patch applies cleanly (optional but recommended)
git apply --check bmcu370t-improvements-v2.patch

# 4. Apply the patch
git apply bmcu370t-improvements-v2.patch

# 5. Review the changes
git diff

# 6. Stage and commit
git add src/config.h src/Debug_log.h src/Debug_log.cpp
git commit -m "Add BMCU370t code improvements: config.h and DEBUG_float"
```

#### Method 2: Using `git am` (Creates commit automatically)

```bash
# 1. Ensure you're on the main branch
git checkout main

# 2. Apply patch with commit
git am < bmcu370t-improvements-v2.patch

# This creates a commit automatically with the patch's commit message
```

#### Method 3: Using `patch` command

```bash
# 1. Apply using patch tool
patch -p1 < bmcu370t-improvements-v2.patch

# 2. Commit manually
git add src/config.h src/Debug_log.h src/Debug_log.cpp
git commit -m "Add BMCU370t improvements"
```

---

## Verification After Applying

After applying the patch, verify the changes:

```bash
# 1. Check that config.h was created
ls -la src/config.h

# 2. Verify DEBUG_float macro exists
grep "DEBUG_float" src/Debug_log.h

# 3. Verify Debug_log_write_float function exists
grep "Debug_log_write_float" src/Debug_log.cpp

# 4. Build the firmware to ensure no compilation errors
# (Use your normal build process)
pio run
```

---

## Files Modified/Created

| File | Status | Lines Changed | Description |
|------|--------|---------------|-------------|
| `src/config.h` | **NEW** | +199 | Centralized configuration header |
| `src/Debug_log.h` | Modified | +25, -12 | Added DEBUG_float macro and improved macros |
| `src/Debug_log.cpp` | Modified | +37, +2 | Added DEBUG_float implementation and includes |

**Total:** 3 files changed, 261 insertions(+), 14 deletions(-)

---

## Using config.h in Your Code

After applying this patch, you can use config.h constants in your code:

### Before (BMCU370):
```c
float PULL_voltage_up = 1.85f;
uint64_t Assist_send_time = 1200;
#define use_flash_addr ((uint32_t)0x0800F000)
```

### After (With config.h):
```c
#include "config.h"

const float PULL_voltage_up = PULL_VOLTAGE_HIGH;
const uint64_t Assist_send_time = ASSIST_SEND_TIME_MS;
// Use FLASH_SAVE_ADDRESS instead of use_flash_addr
```

### Modifying Configuration:

To change hardware settings, edit `src/config.h`:

```c
// Change LED brightness
#define BRIGHTNESS_MAIN_BOARD   35   // 0-255
#define BRIGHTNESS_CHANNEL      15   // 0-255

// Change filament detection voltages
#define PULL_VOLTAGE_HIGH       1.85f
#define PULL_VOLTAGE_LOW        1.45f

// Change retraction distances
#define P1X_OUT_FILAMENT_MM     200.0f   // Internal (mm)
#define P1X_OUT_FILAMENT_EXT_MM 700.0f   // External (mm)
```

---

## Using DEBUG_float

After applying this patch, you can debug floating-point values:

```c
#include "Debug_log.h"

// Debug with 2 decimal places
DEBUG_float(temperature, 2);  // Outputs: "25.37"

// Debug with label
DEBUG_MY("Temperature: ");
DEBUG_float(temperature, 2);
DEBUG_MY(" °C\n");
// Outputs: "Temperature: 25.37 °C"

// In production (when Debug_log_on is undefined)
// DEBUG_float() compiles to nothing - zero overhead
```

---

## Troubleshooting

### Patch Fails to Apply

If you get errors like "patch does not apply":

1. **Check your current branch:**
   ```bash
   git branch
   # Should be on main or equivalent
   ```

2. **Ensure working directory is clean:**
   ```bash
   git status
   # Should show "nothing to commit, working tree clean"
   ```

3. **Check if files were already modified:**
   ```bash
   git diff src/Debug_log.h src/Debug_log.cpp
   # Should show no differences
   ```

4. **Try with 3-way merge:**
   ```bash
   git apply -3 bmcu370t-improvements-v2.patch
   ```

### Compilation Errors After Applying

1. **"config.h: No such file or directory"**
   - Verify config.h was created: `ls src/config.h`
   - If missing, re-apply the patch

2. **Undefined reference to `Debug_log_write_float`**
   - Check Debug_log.cpp was updated: `grep "Debug_log_write_float" src/Debug_log.cpp`
   - Rebuild completely: `pio run --target clean && pio run`

3. **Macro redefinition errors**
   - Ensure you don't have duplicate definitions
   - Check that config.h is only included once per file

---

## Reverting the Patch

If you need to revert the changes:

### If Not Yet Committed:
```bash
git restore src/Debug_log.h src/Debug_log.cpp
rm src/config.h
```

### If Already Committed:
```bash
git revert HEAD
```

### Manual Revert:
```bash
git checkout HEAD~1 -- src/Debug_log.h src/Debug_log.cpp
rm src/config.h
git commit -m "Revert: Remove BMCU370t improvements"
```

---

## Future Improvements (Not in This Patch)

These improvements from BMCU370t are **not included** in this patch but could be considered for future updates:

1. **Automatic Motor Direction Learning** - Requires extensive testing
2. **Edge Detection Auto-Feeding** - Changes user-facing behavior
3. **MAX_FILAMENT_CHANNELS** constant usage throughout codebase
4. **Loop refactoring** in main.cpp
5. **Enhanced input validation** with debug messages
6. **Const correctness** improvements

These require more careful integration and testing to ensure they don't break existing functionality.

---

## Benefits of This Patch

✅ **Better Maintainability** - All config in one place
✅ **Easier Hardware Customization** - Edit config.h instead of hunting through code
✅ **Better Debugging** - Float value debugging support
✅ **Code Quality** - Proper macro hygiene, no common pitfalls
✅ **Zero Risk** - Preserves all existing bug fixes
✅ **Tested** - Applies cleanly and builds successfully
✅ **Documented** - config.h has inline documentation for all constants

---

## Support & Questions

If you encounter issues:

1. Check this README thoroughly
2. Review the verification steps
3. Check the troubleshooting section
4. Examine the patch file content: `cat bmcu370t-improvements-v2.patch`

---

## Version Information

- **Patch Version:** v2
- **Generated:** 2025-12-13
- **Based on:** BMCU370 main (commit d1408b7)
- **Source:** BMCU370t main branch improvements
- **Compatibility:** BMCU370 main branch

---

## Summary

This is a **safe, tested patch** that adds important code infrastructure improvements from BMCU370t to BMCU370 without risking any of the existing bug fixes. The main benefits are centralized configuration management and better debugging capabilities.

**Recommended:** Apply this patch to improve code maintainability and ease future development.

---

*For detailed analysis of all differences between BMCU370 and BMCU370t, see: `BMCU370_vs_BMCU370t_DIFF.md` and `BMCU370t_CODE_IMPROVEMENTS.md`*
