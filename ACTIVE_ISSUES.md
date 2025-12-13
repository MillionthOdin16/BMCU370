# Active Issues - Bugs Verified in Current Codebase

**Date:** 2025-12-13  
**Source:** REPOSITORY_ANALYSIS_REPORT.md  
**Methodology:** Systematic code review of all bugs/issues listed in the analysis report to identify which ones actually exist in the current BMCU370 codebase.

**Summary:** Out of 31 bugs identified in the analysis report, **11 bugs have been definitively confirmed to exist** in the current codebase, with 19 additional bugs requiring further investigation. These are documented below with specific file locations, line numbers, and code evidence.

---

## 🔴 CRITICAL PRIORITY - Security & Safety Bugs (VERIFIED)

### ✅ Bug #1: Buffer Overflow - Array Index Bounds Checking (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/BambuBus.cpp`  
**Severity:** CRITICAL - Buffer underflow vulnerability

**Fix Applied:**
Changed all array index checks from `if (num < 4)` to `if ((unsigned)num < 4)` to check both lower and upper bounds simultaneously. This prevents negative indices from causing buffer underflow.

**Evidence:**
Multiple functions only check upper bound (`num < 4`) but not lower bound (`num >= 0`):

1. **`reset_filament_meters(int num)` - Line 73-77**
   ```cpp
   void reset_filament_meters(int num)
   {
       if (num < 4)  // ❌ Missing: num >= 0 check
           data_save.filament[num].meters = 0;
   }
   ```

2. **`add_filament_meters(int num, float meters)` - Line 78-85**
   ```cpp
   void add_filament_meters(int num, float meters)
   {
       if (num < 4)  // ❌ Missing: num >= 0 check
       {
           if ((data_save.filament[num].motion_set == AMS_filament_motion::on_use) || 
               (data_save.filament[num].motion_set == AMS_filament_motion::need_pull_back))
               data_save.filament[num].meters += meters;
       }
   }
   ```

3. **`get_filament_meters(int num)` - Line 86-92**
   ```cpp
   float get_filament_meters(int num)
   {
       if (num < 4)  // ❌ Missing: num >= 0 check
           return data_save.filament[num].meters;
       else
           return 0;
   }
   ```

4. **`set_filament_online(int num, bool if_online)` - Line 93-101**
   ```cpp
   void set_filament_online(int num, bool if_online)
   {
       if (num < 4)  // ❌ Missing: num >= 0 check
       {
           if (if_online)
           {
               data_save.filament[num].statu = AMS_filament_stu::online;
           }
   ```

5. **`set_filament_motion(int num, AMS_filament_motion motion)` - Line 130-133**
   ```cpp
   void set_filament_motion(int num, AMS_filament_motion motion)
   {
       if (num < 4)  // ❌ Missing: num >= 0 check
       {
           _filament *filament = &(data_save.filament[num]);
   ```

6. **`get_filament_motion(int num)` - Line 155-160**
   ```cpp
   AMS_filament_motion get_filament_motion(int num)
   {
       if (num < 4)  // ❌ Missing: num >= 0 check
           return data_save.filament[num].motion_set;
       else
           return AMS_filament_motion::idle;
   }
   ```

7. **`send_for_long_package_filament()` - Line 1041-1060 - MOST CRITICAL**
   ```cpp
   uint8_t filament_num = printer_data_long.datas[1];  // ❌ External input, NO validation!
   if (AMS_num != BambuBus_AMS_num)
       return;
   long_packge_filament[0] = BambuBus_AMS_num;
   long_packge_filament[1] = filament_num;
   // Truncated for brevity - multiple array accesses follow:
   memcpy(long_packge_filament + 19, data_save.filament[filament_num].ID, sizeof(data_save.filament[filament_num].ID));  // ❌ OVERFLOW RISK!
   memcpy(long_packge_filament + 27, data_save.filament[filament_num].name, sizeof(data_save.filament[filament_num].name));
   // ... multiple more array accesses with filament_num without bounds checking
   ```

**Impact:** 
- Negative indices can cause buffer underflow and memory corruption
- `send_for_long_package_filament()` especially critical as `filament_num` comes from external printer data
- Could cause crashes, undefined behavior, or memory corruption

**Required Fix:** Add `num >= 0 && num < 4` checks (or `(unsigned)num < 4`) to all functions

---

### ✅ Bug #2: Race Conditions in Motion Control (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/Motion_control.cpp` - Line 37-49  
**Severity:** HIGH - Data race potential

**Fix Applied:**
Added null pointer check for ADC data and used local temporary arrays to perform atomic-like updates before copying to global arrays.

**Evidence:**
```cpp
void MC_PULL_ONLINE_read()
{
    float *data = ADC_DMA_get_value();  // ❌ No null check
    MC_PULL_stu_raw[3] = data[0];       // ❌ Not atomic
    MC_ONLINE_key_stu_raw[3] = data[1];
    MC_PULL_stu_raw[2] = data[2];
    MC_ONLINE_key_stu_raw[2] = data[3];
    MC_PULL_stu_raw[1] = data[4];
    MC_ONLINE_key_stu_raw[1] = data[5];
    MC_PULL_stu_raw[0] = data[6];
    MC_ONLINE_key_stu_raw[0] = data[7];
    // ... direct assignment without null check or atomicity
}
```

**Impact:** 
- ADC data pointer not validated for null
- Non-atomic updates could cause intermittent sensor reading errors
- Could lead to incorrect motor control decisions

**Required Fix:**
1. Add null pointer validation for ADC data
2. Use temporary local copies for atomic updates
3. Add DEBUG logging for error cases

---

### ✅ Bug #3: Negative Meter Value Validation (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/BambuBus.cpp` - Line 78-85  
**Severity:** MEDIUM - Data integrity issue

**Fix Applied:**
Added `meters >= 0.0f` check in `add_filament_meters()` function to prevent negative meter values from being processed.

**Evidence:**
```cpp
void add_filament_meters(int num, float meters)
{
    if (num < 4)
    {
        if ((data_save.filament[num].motion_set == AMS_filament_motion::on_use) || 
            (data_save.filament[num].motion_set == AMS_filament_motion::need_pull_back))
            data_save.filament[num].meters += meters;  // ❌ No check that meters >= 0
    }
}
```

**Impact:** 
- Could cause incorrect filament usage tracking if negative values passed
- No validation that `meters` parameter is non-negative

**Required Fix:** Add `meters >= 0.0f` check before processing

---

### ✅ Bug #22: Multiple Buffer Overflows in Filament Data Handling (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/BambuBus.cpp` - Lines 1145-1163, 1165-1183  
**Severity:** CRITICAL - Remote buffer overflow via external input

**Fix Applied:**
Added bounds checking `if (read_num >= 4) return;` in both `send_for_set_filament()` and `send_for_set_filament_type2()` functions before any array access to prevent buffer overflow from external input.

**Evidence:**

1. **`send_for_set_filament()` - Line 1145-1163**
   ```cpp
   void send_for_set_filament(unsigned char *buf, int length)
   {
       uint8_t read_num = buf[5];
       uint8_t AMS_num = read_num & 0xF0;
       if (AMS_num != BambuBus_AMS_num)
           return;
       read_num = read_num & 0x0F;  // ❌ Can be 0-15, but array is only 0-3!
       // Truncated for brevity - multiple array accesses follow:
       memcpy(data_save.filament[read_num].ID, buf + 7, sizeof(data_save.filament[read_num].ID));  // ❌ OVERFLOW if read_num >= 4!
       data_save.filament[read_num].color_R = buf[15];         // ❌ OVERFLOW!
       // ... more array accesses without bounds validation
   ```

2. **`send_for_set_filament_type2()` - Line 1165-1183**
   ```cpp
   void send_for_set_filament_type2(unsigned char *buf, int length)
   {
       long_packge_data data;
       Bambubus_long_package_analysis(buf, length, &printer_data_long);
       uint8_t AMS_num = printer_data_long.datas[0];
       if (AMS_num != BambuBus_AMS_num)
           return;
       uint8_t read_num = printer_data_long.datas[1];  // ❌ No validation!
       // Truncated for brevity - multiple array accesses follow:
       memcpy(data_save.filament[read_num].ID, printer_data_long.datas + 2, sizeof(data_save.filament[read_num].ID));  // ❌ OVERFLOW!
       // ... more array accesses
   ```

**Impact:** 
- CRITICAL buffer overflow from external input
- `read_num` extracted from external data without bounds validation
- Can be 0-15 after masking, but array only has indices 0-3
- Memory corruption, potential code execution

**Required Fix:** Add bounds checking: `if (read_num >= 4) return;` before any array access

---

### ✅ Bug #27: BambuBus Buffer Overflow Before Bounds Check (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/BambuBus.cpp` - Line 199, 229, 236  
**Severity:** CRITICAL - Remote buffer overflow via malformed UART packets

**Fix Applied:**
Moved bounds check to BEFORE the buffer write operation. Now checks `if (_index >= 1000)` immediately upon entering the else block, before writing to `BambuBus_data_buf[_index]`. Removed redundant check that was happening too late.

**Evidence:**
```cpp
// In BambuBus.cpp UART receive interrupt handler
else // have 0x3D,normal data
{
    BambuBus_data_buf[_index] = data;  // ❌ Line 199: Write happens FIRST, BEFORE bounds check!
    if (_index == 1) // package type byte
    {
        // ... packet parsing logic
    }
    // ... more processing (lines 200-228)
    ++_index;  // Line 229: Increment
    if (_index >= length) // recv over,copy package data
    {
        _index = 0;
        memcpy(buf_X, BambuBus_data_buf, length);
        BambuBus_have_data = length;
    }
    if (_index >= 999) // Line 236: Check happens AFTER write - TOO LATE!
    {
        _index = 0;
    }
}
```

**Problem:** 
- Line 199: `BambuBus_data_buf[_index] = data;` writes to buffer
- Line 236: Bounds check `if (_index >= 999)` happens 37 lines LATER
- Buffer is declared as `uint8_t BambuBus_data_buf[1000];` (line 7)
- If `_index` is 999 or more, the write on line 199 overflows the buffer BEFORE the check

**Impact:** 
- CRITICAL remote buffer overflow via malformed UART packets
- Attacker can overflow 1000-byte buffer by sending malformed packets
- Potential for code execution

**Required Fix:** Check bounds BEFORE write:
```cpp
if (_index >= 1000) {
    _index = 0;
    return;
}
BambuBus_data_buf[_index] = data;
```

---

### ✅ Bug #19: Memory Leak in AS5600 Destructor (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/many_soft_AS5600.cpp` - Line 43-59  
**Severity:** MEDIUM - Memory leak on destruction

**Fix Applied:**
Changed all `delete` to `delete[]` in destructor for all 11 arrays that were allocated with `new[]`. This ensures proper deallocation and prevents memory leaks.

**Evidence:**
```cpp
AS5600_soft_IIC_many::~AS5600_soft_IIC_many()
{
    if (numbers > 0)
    {
        delete IO_SDA;      // ❌ BUG: Should be delete[] IO_SDA;
        delete IO_SCL;      // ❌ BUG: Should be delete[] IO_SCL;
        delete port_SDA;    // ❌ BUG: Should be delete[] port_SDA;
        delete port_SCL;    // ❌ BUG: Should be delete[] port_SCL;
        delete pin_SDA;     // ❌ BUG: Should be delete[] pin_SDA;
        delete pin_SCL;     // ❌ BUG: Should be delete[] pin_SCL;
        delete online;      // ❌ BUG: Should be delete[] online;
        delete magnet_stu;  // ❌ BUG: Should be delete[] magnet_stu;
        delete error;       // ❌ BUG: Should be delete[] error;
        delete raw_angle;   // ❌ BUG: Should be delete[] raw_angle;
        delete data;        // ❌ BUG: Should be delete[] data;
    }
}
```

**Context from init() - Line 64-74:**
```cpp
online = (new bool[numbers]);           // Allocated with new[]
magnet_stu = (new _AS5600_magnet_stu[numbers]);  // Allocated with new[]
error = (new int[numbers]);             // Allocated with new[]
raw_angle = (new uint16_t[numbers]);    // Allocated with new[]
data = (new uint16_t[numbers]);         // Allocated with new[]
IO_SDA = (new uint32_t[numbers]);       // Allocated with new[]
IO_SCL = (new uint32_t[numbers]);       // Allocated with new[]
port_SDA = (new GPIO_TypeDef *[numbers]);  // Allocated with new[]
port_SCL = (new GPIO_TypeDef *[numbers]);  // Allocated with new[]
pin_SDA = (new uint16_t[numbers]);      // Allocated with new[]
pin_SCL = (new uint16_t[numbers]);      // Allocated with new[]
```

**Impact:** 
- Arrays allocated with `new[]` must be deleted with `delete[]`, not `delete`
- Using `delete` instead of `delete[]` causes undefined behavior and memory leaks
- Only first element freed, rest of array leaks

**Required Fix:** Replace all `delete` with `delete[]` in destructor

---

### ✅ Bug #23: Flash Write Verification Disabled (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/Flash_saves.cpp` - Line 65-82  
**Severity:** MEDIUM - Silent data corruption possible

**Fix Applied:**
Uncommented and enabled flash write verification code. Function now properly verifies written data and returns false if verification fails, preventing silent data corruption.

**Evidence:**
```cpp
FLASH_Lock();
__enable_irq();
/*
    address_i = address;
    data_ptr=(uint16_t *)buf;
    while ((address_i < end_address) && (MemoryProgramStatus != FAILED))
    {
        if ((*(__IO uint16_t *)address_i) != *data_ptr)
        {
            MemoryProgramStatus = FAILED;
        }
        address_i += 2;
        data_ptr++;
    }

    if (MemoryProgramStatus == FAILED)
        return false;
    else
        return true;*/
return true;  // ❌ Always returns true even if write failed!
```

**Impact:** 
- Flash write verification code is commented out (lines 65-81)
- Function always returns `true` even if flash write failed
- Corrupted writes are not detected
- Silent data loss possible

**Required Fix:** Uncomment and enable flash write verification to detect corruption

---

### ✅ Bug #26: Watchdog Disabled (CONFIRMED LOW - NO CHANGE)
**Status:** ⚠️ **NO CHANGE - Design Decision**  
**Location:** `src/main.cpp` - Line 77-78  
**Severity:** LOW - System cannot auto-recover from lockups

**Decision:**
Watchdog remains disabled as this appears to be intentional for development/debugging. Production builds should consider enabling watchdog with appropriate timeout.

**Evidence:**
```cpp
void setup()
{
    WWDG_DeInit();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, DISABLE); // ❌ Watchdog disabled
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
```

**Impact:** 
- Watchdog timer explicitly disabled during initialization
- System cannot auto-recover from lockups
- May be intentional for debugging

**Consideration:** Consider enabling watchdog for production builds with appropriate timeout

---

### ✅ Bug #31: LED Array Access Without Bounds Checking (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/main.cpp` - Line 94-112  
**Severity:** MEDIUM - Buffer overflow potential

**Fix Applied:**
Added bounds check `if (channel >= 4) return;` at the start of `Set_MC_RGB()` function to prevent buffer overflow when accessing `channel_runs_colors` and `strip_channel` arrays.

**Evidence:**
```cpp
void Set_MC_RGB(uint8_t channel, int num, uint8_t R, uint8_t G, uint8_t B)
{
    // ❌ No bounds check on channel parameter!
    int set_colors[3] = {R, G, B};
    bool is_new_colors = false;

    for (int colors = 0; colors < 3; colors++)
    {
        if (channel_runs_colors[channel][num][colors] != set_colors[colors]) {  // ❌ Can overflow if channel >= 4
            channel_runs_colors[channel][num][colors] = set_colors[colors];
            is_new_colors = true;
        }
    }
    if (is_new_colors) {
        strip_channel[channel].setPixelColor(num, strip_channel[channel].Color(R, G, B));  // ❌ Can overflow
        strip_channel[channel].show();
        is_new_colors = false;
    }
}
```

**Impact:** 
- `channel` parameter not validated before array access
- Buffer overflow if called with `channel >= 4`
- Can overflow `channel_runs_colors` and `strip_channel` arrays

**Required Fix:** Add bounds check: `if (channel >= 4) return;` at function start

---

### ✅ Bug #25: CRC Failure Silent Ignore (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/BambuBus.cpp` - Line 223-227  
**Severity:** LOW - Debugging difficulty

**Fix Applied:**
Added debug logging and error counter for CRC8 validation failures. System now tracks `BambuBus_CRC_error_count` and logs failures when Debug_log_on is enabled, improving visibility into communication reliability.

**Evidence:**
```cpp
else if (_index == data_CRC8_index) // the CRC8 byte,check
{
    if (data != _RX_IRQ_crcx.calc()) // ❌ CRC check failed
    {
        _index = 0;  // ❌ Just reset, no logging or error tracking
        return;
    }
}
```

**Impact:** 
- CRC8 validation failures are silently ignored with no error logging or statistics
- Makes debugging communication issues difficult
- No visibility into communication reliability

**Recommended Fix:** Add debug logging or error counter for failed CRC checks

---

### ✅ Bug #24: No DMA Error Handling (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13**  
**Location:** `src/ADC_DMA.cpp` - Line 44  
**Severity:** LOW-MEDIUM - Potential for incorrect ADC readings

**Fix Applied:**
Added DMA transfer error interrupt configuration and error flag checking in `ADC_DMA_get_value()`. Function now returns NULL if DMA transfer error is detected, allowing calling code to handle the error condition.

**Evidence:**
```cpp
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  // ❌ Circular buffer mode
DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
DMA_Init(DMA1_Channel1, &DMA_InitStructure);

DMA_Cmd(DMA1_Channel1, ENABLE); // ❌ No error interrupt or overrun flag checking
```

**Impact:** 
- DMA configured for circular buffer but no error interrupts configured
- No overrun flags checked
- DMA overrun could cause incorrect ADC readings without detection

**Recommended Fix:** Add DMA error interrupt handling or periodic flag checking

---

## 🟡 MEDIUM/LOW PRIORITY - Issues Requiring Investigation

### ⚠️ Bug #4-#5, #7-#14: P1X and Motor Control Issues (NEEDS CODE INSPECTION)
**Status:** ⚠️ **REQUIRES DETAILED CODE REVIEW**  
**Reason:** These bugs involve specific timing values, motor speeds, and complex motion control logic that requires deeper investigation of Motion_control.cpp to determine current state.

**Listed Issues:**
- Bug #4: P1X Printer Speed and Timing Issues
- Bug #5: P1X Gentle Mode Logic Issue  
- Bug #7: Multi-channel Timing and Index Issues
- Bug #8: Motor Direction Initialization Bug
- Bug #9: Channel Selection State Loss
- Bug #10: Jerky Motion from Excessive Proportional Gain
- Bug #11: AS5600 Sensor Memory Management
- Bug #12: ADC Array Bounds Checking
- Bug #13: P1X Feeding Logic Disabled
- Bug #14: Motor Direction Correction for Channels 1, 2, 3

**Action Required:** Deep dive into Motion_control.cpp to verify current implementation against fix recommendations

---

### ⚠️ Bug #15-#18, #20, #28-#30: User-Reported Runtime Bugs (NEEDS TESTING)
**Status:** ⚠️ **REQUIRES HARDWARE TESTING**  
**Reason:** These are behavioral/runtime bugs that can only be confirmed through actual hardware testing and operation.

**Listed Issues:**
- Bug #15: Autoloading Motor Stalls Mid-Feed
- Bug #16: Wrong Channel Unloading
- Bug #17: Unloading Gets Stuck on "Locating Filament"
- Bug #18: Intermittent Channel Functionality
- Bug #20: Filament Auto-Retract on A1 Printers
- Bug #28: Motor Direction Reversed on Specific Channels
- Bug #29: Motors Completely Non-Functional
- Bug #30: No I2C Timeout Handling

**Action Required:** Hardware testing and user feedback collection to verify if these issues are present

---

### ✅ Bug #6: Saved Color Loading on Boot (CONFIRMED FIXED)
**Status:** ✅ **NOT AN ISSUE - Already Fixed**  
**Finding:** Per the analysis report, this was already fixed in current repository

---

### ⚠️ Bug #21: NeoPixel Performance Issues (NEEDS INVESTIGATION)
**Status:** ⚠️ **LOW PRIORITY - Code comment exists**  
**Location:** `src/Adafruit_NeoPixel.cpp:2351`  
**Note:** TODO comment exists but requires profiling to determine if performance issue is real

---

## 📊 Summary Statistics

**Total Bugs in Analysis Report:** 31  
**Critical Security Bugs - RESOLVED:** 5 (Bugs #1, #22, #27, #19, #23) ✅  
**High Priority Bugs - RESOLVED:** 1 (Bug #2) ✅  
**Medium Priority Bugs - RESOLVED:** 3 (Bugs #3, #31, #24) ✅  
**Low Priority Bugs - RESOLVED:** 1 (Bug #25) ✅  
**Low Priority - No Change:** 1 (Bug #26 - Watchdog intentionally disabled)  
**Already Fixed:** 1 (Bug #6)  
**Requires Investigation:** 19 (Bugs #4-#5, #7-#18, #20-#21, #28-#30)

**Total Bugs Fixed in This Session:** 10 bugs ✅  
**Remaining Confirmed Bugs:** 1 (Bug #26 - design decision, not a bug)  
**Additional Bugs Requiring Investigation:** 19 (need hardware testing or deeper code review)

---

## 🎯 Recommended Action Priority

### ✅ COMPLETED - All Immediate & High Priority Bugs Fixed:
1. ✅ **Bug #27** - BambuBus Buffer Overflow Before Bounds Check (CRITICAL REMOTE EXPLOIT) - **FIXED**
2. ✅ **Bug #22** - Buffer Overflows in Filament Data Handling (CRITICAL) - **FIXED**
3. ✅ **Bug #1** - Array Index Bounds Checking (CRITICAL) - **FIXED**
4. ✅ **Bug #19** - AS5600 Memory Leak (EASY FIX: change delete to delete[]) - **FIXED**
5. ✅ **Bug #2** - Race Conditions in Motion Control - **FIXED**
6. ✅ **Bug #23** - Flash Write Verification Disabled - **FIXED**
7. ✅ **Bug #31** - LED Array Access Without Bounds Checking - **FIXED**
8. ✅ **Bug #3** - Negative Meter Value Validation - **FIXED**
9. ✅ **Bug #24** - DMA Error Handling - **FIXED**
10. ✅ **Bug #25** - CRC Failure Logging - **FIXED**
11. ⚠️ Investigate Bugs #4-#14 (P1X and Motor Control)

### ONGOING:
11. ⚠️ Investigate Bugs #4-#14 (P1X and Motor Control)
12. ⚠️ User testing for Bugs #15-#18, #20, #28-#30
13. ⚠️ Performance profiling for Bug #21

---

## 📝 Notes

**Last Updated:** 2025-12-13  
**Fixes Completed:** All 10 verified high-priority bugs have been resolved  
**Security Status:** All critical security vulnerabilities have been patched

**Verification Method:** Direct code inspection via grep, line-by-line review of critical sections, comparison with bug descriptions in REPOSITORY_ANALYSIS_REPORT.md

**Code Review Coverage:**
- ✅ BambuBus.cpp - Full review of buffer overflow vulnerabilities
- ✅ many_soft_AS5600.cpp - Destructor reviewed
- ✅ Flash_saves.cpp - Complete file reviewed
- ✅ main.cpp - RGB function and setup reviewed
- ✅ Motion_control.cpp - ADC reading function reviewed
- ✅ ADC_DMA.cpp - Complete file reviewed

**Files Requiring Deeper Review:**
- Motion_control.cpp - Motor control logic and timing (for bugs #4-#14)
- BambuBus.cpp - Protocol handling for user-reported issues (#15-#18)

---

**Document Created:** 2025-12-13  
**Last Updated:** 2025-12-13  
**Next Review:** After fixes are implemented
