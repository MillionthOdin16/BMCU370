# Active Issues - Current Status

**Last Updated:** 2025-12-13
**Status:** All critical security bugs from initial audit (Dec 13) have been RESOLVED.
**Recent Fix:** A1 Mini filament retraction bug fixed (Dec 13, evening)

---

## 📋 **Current Status Summary**

### ✅ All Security Bugs RESOLVED (December 13, 2025 morning)
- **10 critical security vulnerabilities** identified in comprehensive audit
- All bugs fixed and verified in commit batch
- See [archived audit reports](docs/archive/) for details

### ✅ A1 Mini Retraction Bug RESOLVED (December 13, 2025 evening)
- **NEW bug discovered and fixed**: A1 Mini filament retraction failure
- Root cause: Motor start command accidentally removed during refactoring
- Status: **FIXED** in commits 86e90d1, 1b182f9, e2f7cd9

---

## 🐛 **Recently Fixed Issues**

### Bug #32: A1 Mini Filament Retraction Failure (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13 evening**
**Severity:** CRITICAL - Printer unusable
**Commits:** 86e90d1, 1b182f9, e2f7cd9

**Problem:**
After v21 firmware, A1 Mini would fail to retract filament after printing, showing error "Failed to pull out filament from toolhead [1200-8015]". BMCU motor did not respond to retraction commands.

**Root Cause:**
During refactoring to fix over-retraction, the `MOTOR_CONTROL[num].set_motion()` call for AMS Lite was accidentally removed, preventing motor from starting when retraction command received.

**Fix Applied:**
1. **Restored motor start command** for AMS Lite in `need_pull_back` case (line 605)
2. **Clarified control architecture**: A1 Mini printer controls WHEN to stop (via BambuBus), but BMCU must START motor when commanded
3. **Verified against working BMCU370t firmware** to ensure correct behavior

**Files Modified:**
- `src/Motion_control.cpp` lines 603-609

**Impact:**
A1 Mini filament retraction now works correctly:
- Motor starts when retract command received
- Printer controls stopping via BambuBus idle command
- Filament pulls back ~80mm (clears toolhead)
- Filament remains in AMS (not completely unloaded)

---

### Bug #33: Jerky Filament Feeding on A1 Mini (RESOLVED)
**Status:** ✅ **RESOLVED - Fixed 2025-12-13 evening**
**Severity:** MEDIUM - Feeding quality issue
**Commit:** e2f7cd9

**Problem:**
Filament feeding would stutter or stall when encountering resistance during loading.

**Root Cause:**
When pressure reached 1.7V threshold during feeding, motor speed was set to 0 (full stop) instead of 10 mm/s (gentle slowdown), causing jerky start-stop cycling.

**Fix Applied:**
Changed `speed_set = 0` to `speed_set = 10` at line 408 to match working BMCU370t firmware.

**Files Modified:**
- `src/Motion_control.cpp` line 408

**Impact:**
- Smooth feeding: 30 mm/s → 10 mm/s transition (not 30 → 0 → 30)
- Better handling of resistance
- Matches original working firmware behavior

---

## 🔍 **Previously Resolved Critical Security Bugs** (December 13, 2025 morning)

All 10 critical security vulnerabilities from the comprehensive audit have been resolved. See [archived audit reports](docs/archive/) for complete details:

### Summary of Fixed Security Issues:
1. ✅ **Bug #1**: Buffer overflow - array index bounds checking (7 functions)
2. ✅ **Bug #2**: Race conditions in motion control
3. ✅ **Bug #3**: Negative meter value validation
4. ✅ **Bug #19**: AS5600 memory leak (delete vs delete[])
5. ✅ **Bug #22**: Filament data buffer overflows (2 functions)
6. ✅ **Bug #23**: Flash write verification disabled
7. ✅ **Bug #24**: DMA error handling missing
8. ✅ **Bug #25**: CRC failure logging
9. ✅ **Bug #27**: BambuBus buffer overflow
10. ✅ **Bug #31**: LED array bounds checking

**All fixes verified with successful build:**
- RAM: 46.5% (9532/20480 bytes)
- Flash: 57.8% (37884/65536 bytes)

---

## 📝 **Known Non-Critical Issues**

### Minor Issues (No Action Required)
- Transparent filament detection may be unreliable (hardware limitation)
- Some gear noise during operation (normal for 370 motors)

---

## 📚 **Documentation**

### Current Active Documentation
- **[README.md](README.md)** - Project overview and quick links
- **[CHANGELOG.md](CHANGELOG.md)** - Version history and changes
- **[docs/](docs/)** - Organized user and developer documentation

### Archived Reports
- **[docs/archive/](docs/archive/)** - Historical audit reports from December 13, 2025 security review

---

## 🔗 **Related Information**

**Firmware Version:** V0.1-0021+
**Last Major Update:** December 13, 2025
**Total Bugs Fixed:** 12 (10 security + 2 retraction)
**Build Status:** ✅ Passing
**Verification:** ✅ Tested on A1 Mini with BMCU 370c (AMS Lite emulation)

---

## 📊 **Testing Status**

### Verified Working
- ✅ A1 Mini filament loading
- ✅ A1 Mini filament retraction after print
- ✅ Smooth feeding under resistance
- ✅ All security fixes validated
- ✅ Build compiles successfully

### Pending User Testing
- Regular AMS (P-series/X-series) retraction behavior
- Multi-color print transitions
- Long-duration prints (>24 hours)

---

**For bug reports or feature requests, please open an issue on GitHub.**
