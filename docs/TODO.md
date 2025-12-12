# TODO - Documentation and Code Issues

This document tracks inconsistencies, questions, and items that need resolution or further investigation.

## Documentation Inconsistencies

### 1. Firmware Version Numbering

**Issue:** Multiple version references found
- README.md mentions "V0.1-0020"
- BambuBus.cpp defines `Bambubus_version 5`
- Chinese changelog mentions versions like "0019", "0020"

**Questions:**
- What is the relationship between "0020" and "version 5"?
- Is "0020" the build/release number while "5" is the protocol version?
- Should documentation clarify the difference?

**Resolution Needed:** Clarify version numbering scheme

---

### 2. Dual Microswitch Support

**Issue:** Code references dual microswitch mode but documentation unclear on availability
- `Motion_control.cpp` line 35: `#define is_two false`
- Conditional code for dual microswitch (lines 90-100)
- README and wiki mention "dual microswitch" as a variant

**Questions:**
- Is dual microswitch mode available in this firmware?
- Does it require hardware changes (different sub-board)?
- Should users enable this, and if so, how?

**Resolution Needed:** Document dual microswitch variant clearly, specify if this firmware supports it

---

### 3. PCB Variant Confusion

**Issue:** Multiple PCB versions mentioned without clear mapping to this code
- "Original version" mainboard
- "C-type version" (Type-C)
- "Side Type-C version by @XC"
- "Star Dust modified version" (Xing-C)
- Hall sensor version sub-board

**Questions:**
- Which PCB version(s) is this code compatible with?
- Are there any code changes needed for different PCBs?
- How does the "Star Dust modified version" differ from original?

**Resolution Needed:** Create compatibility matrix of PCB versions vs firmware versions

---

### 4. AS5600 I2C Address Conflict

**Issue:** Using 4 AS5600 sensors on software I2C, all same address
- All AS5600 chips use I2C address 0x36
- Code uses separate GPIO pairs for each sensor
- Software I2C implementation allows this

**Questions:**
- Is using separate I2C buses (software) the only way to support 4 sensors?
- Could hardware I2C be used with multiplexer?
- Are there performance implications?

**Resolution Needed:** Document why software I2C is required (address conflict)

---

### 5. P-Series Printer Support

**Issue:** Conflicting information about P-series compatibility
- Wiki says "requires additional configuration"
- Wiki mentions "multi-BMCU setups on P-series"
- No code in this repository specific to P-series

**Questions:**
- What modifications are needed for P-series?
- Is it firmware changes or hardware changes?
- Where is the P-series specific code/documentation?

**Resolution Needed:** Either document P-series support or clearly state it's not included in this version

---

### 6. Metal Gear Recommendations

**Issue:** Conflicting advice about metal gears
- Wiki says metal gears "do not provide better performance"
- Wiki also mentions metal worm gears are "recommended"
- Concerns about metal shavings and maintenance

**Questions:**
- Are all-metal gears (spur + worm) not recommended?
- Are plastic spur + metal worm the recommendation?
- What about long-term wear?

**Resolution Needed:** Clarify specific gear recommendations (plastic spur + metal worm?)

---

## Code Issues and Uncertainties

### 7. Commented-Out Code Sections

**Issue:** Several sections of commented code without explanation

**Examples:**
- `main.cpp` line 8: `// #define LED_PA11_NUM 8` (testing 8 LEDs?)
- `BambuBus.cpp` line 32: A1 control abandoned in changelog
- `Flash_saves.cpp` lines 65-81: Verification code commented out

**Questions:**
- Is the commented verification code (Flash_saves.cpp) intentionally disabled?
- Should it be removed or re-enabled?
- Are the 8-LED test configurations safe to remove?

**Resolution Needed:** Review commented code, decide to remove or document why disabled

---

### 8. Debug Flag Inconsistency

**Issue:** Debug logging controlled by compile-time flag
- `Debug_log.h` uses `#ifdef Debug_log_on`
- No clear documentation on how to enable for users
- May be confusing for developers

**Questions:**
- Should this be a runtime configurable option?
- Should there be different debug levels?
- Is there a performance impact when enabled?

**Resolution Needed:** Document debug enabling procedure clearly

---

### 9. Watchdog Timer Disabled

**Issue:** Watchdog explicitly disabled
- `main.cpp` line 77-78: `WWDG_DeInit()` and clock disabled
- No explanation why

**Questions:**
- Is this temporary for development?
- Should it be re-enabled for production?
- Are there stability concerns without watchdog?

**Resolution Needed:** Document reason for disabling watchdog, consider re-enabling with proper timing

---

### 10. Flash Write Verification

**Issue:** Flash write verification code commented out
- `Flash_saves.cpp` lines 65-81
- Write operation returns true without verifying
- Risk of silent data corruption

**Questions:**
- Why was verification disabled?
- Should it be re-enabled?
- Are there flash write reliability issues?

**Resolution Needed:** Investigate flash reliability, consider re-enabling verification

---

### 11. Buffer Voltage Threshold Magic Numbers

**Issue:** Hardcoded voltage thresholds without calibration
- `PULL_voltage_up = 1.85f`
- `PULL_voltage_down = 1.45f`
- `PULL_VOLTAGE_SEND_MAX = 1.7f`
- Filament detection threshold `1.65V`

**Questions:**
- Are these values universal or hardware-dependent?
- Should they be configurable per-build?
- Is there a calibration procedure?

**Resolution Needed:** Document how these values were determined, provide calibration guide

---

### 12. Filament Retraction Distance

**Issue:** Hardcoded retraction distance
- `P1X_OUT_filament_meters = 200.0f` with comment "内置200mm 外置700mm"
- Suggests different values for different printer configurations

**Questions:**
- What does "internal 200mm external 700mm" mean?
- Should this be configurable?
- How does user know which value to use?

**Resolution Needed:** Document retraction distance configuration for different setups

---

## Missing Documentation

### 13. Installation/Assembly Guide

**Status:** Not present in this repository

**Needed:**
- Step-by-step assembly instructions
- Required tools list
- Wiring diagrams
- Testing procedures
- Calibration steps

**Priority:** High - essential for users

---

### 14. BOM (Bill of Materials)

**Status:** Not present in this repository

**Needed:**
- Complete parts list with specifications
- Vendor links or part numbers
- Quantities for each component
- Alternative parts
- Cost estimates

**Priority:** High - needed before building

---

### 15. Wiring Diagrams

**Status:** Not present in this repository

**Needed:**
- Mainboard to sub-board connections
- Motor wiring
- Power distribution
- Sensor connections
- Connector pinouts

**Priority:** High - essential for assembly

---

### 16. Firmware Update Procedure

**Status:** Minimal information

**Needed:**
- Detailed flashing instructions
- Bootloader entry methods
- Troubleshooting flash failures
- Firmware backup/restore
- Rollback procedure

**Priority:** Medium - users may need to update

---

### 17. Calibration Procedures

**Status:** Not documented

**Needed:**
- AS5600 magnet alignment
- Buffer pressure calibration
- Filament detection threshold adjustment
- Motor PWM tuning
- LED brightness optimization

**Priority:** Medium - may improve reliability

---

## Code Quality Improvements

### 18. Magic Numbers

**Issue:** Many hardcoded values throughout code

**Examples:**
- LED brightness values (35, 15)
- ADC channel mapping (0-7)
- Timeout values (3000ms)
- PWM limits
- Voltage thresholds

**Recommendation:** Define constants with descriptive names

---

### 19. Global Variables

**Issue:** Heavy use of global variables

**Examples:**
- `channel_colors[4][4]`
- `channel_runs_colors[4][2][3]`
- `MC_STU_ERROR[4]`
- `speed_as5600[4]`

**Recommendation:** Consider encapsulation in structs or classes

---

### 20. Error Handling

**Issue:** Limited error handling

**Examples:**
- Flash write failure not propagated
- I2C communication failures not always handled
- No retry logic for transient errors

**Recommendation:** Add comprehensive error handling and recovery

---

## Testing Gaps

### 21. No Automated Tests

**Issue:** No unit tests or integration tests present

**Needed:**
- Unit tests for critical functions (CRC, flash, etc.)
- Integration tests for protocol handling
- Hardware-in-the-loop tests
- Regression test suite

**Priority:** Low - DIY project, but would improve quality

---

### 22. No Continuous Integration

**Issue:** No CI/CD pipeline

**Potential Benefits:**
- Automatic build verification
- Code quality checks
- Documentation generation
- Release automation

**Priority:** Low - nice to have for community project

---

## Community Coordination

### 23. Multiple Forks and Versions

**Issue:** Several developers have created variants
- Original 4061N version
- Xing-C modifications
- MillionthOdin16 fork
- Other community versions

**Questions:**
- How do changes get merged upstream?
- Is there a canonical version?
- How to handle incompatible changes?

**Resolution Needed:** Establish contribution guidelines and merge process

---

### 24. Language Barriers

**Issue:** Mixed Chinese and English in code and documentation
- Comments in Chinese
- Documentation in both languages
- Potential for mistranslation

**Recommendation:**
- Maintain both language versions
- Use community for translation verification
- Ensure technical accuracy across languages

---

## Hardware Questions

### 25. Power Supply Requirements

**Issue:** Power budget not clearly documented

**Needed:**
- Total current draw calculation
- Peak vs continuous current
- Voltage stability requirements
- Recommended PSU specifications

**Priority:** Medium - important for reliability

---

### 26. Thermal Management

**Issue:** No thermal analysis or cooling recommendations

**Questions:**
- Do components overheat during extended prints?
- Are heatsinks needed?
- Is active cooling required?

**Resolution Needed:** Testing and documentation

---

## Future Enhancements to Document

### 27. Potential Features

**Ideas mentioned in community:**
- NFC/RFID support (hardware limitation)
- Multiple BMCU cascading
- Humidity sensor integration
- Automatic filament runout detection
- WiFi connectivity for monitoring

**Action:** Document what's possible vs what's constrained by hardware

---

### 28. Compatibility Matrix

**Needed:** Clear table of compatibility
- Printer models vs BMCU versions
- Firmware versions
- PCB revisions
- Required modifications

---

## Documentation Standards

### 29. Consistent Terminology

**Issue:** Mixed terms used
- "Channel" vs "Slot"
- "Buffer" vs "Cache"
- "Filament" vs "Material"

**Recommendation:** Establish glossary and use consistently

---

### 30. Version Tracking

**Issue:** Documentation not version-controlled with code

**Recommendation:**
- Tag documentation with firmware version
- Update changelog for doc changes
- Maintain version compatibility matrix

---

## Priority Summary

**High Priority (Blocking users):**
- [ ] Installation/assembly guide (#13)
- [ ] Bill of materials (#14)
- [ ] Wiring diagrams (#15)
- [ ] PCB variant compatibility (#3)

**Medium Priority (Improves experience):**
- [ ] Firmware update procedure (#16)
- [ ] Calibration procedures (#17)
- [ ] Power supply requirements (#25)
- [ ] Threshold configuration guide (#11)

**Low Priority (Nice to have):**
- [ ] Automated testing (#21)
- [ ] Code refactoring (#18, #19)
- [ ] CI/CD pipeline (#22)

---

## Notes for Reviewers

**Please add any additional items discovered during review:**
- Inconsistencies between code and documentation
- Unclear explanations
- Missing information
- Technical inaccuracies

**Update this file as issues are resolved:**
- Mark items as completed
- Add resolution notes
- Link to commits that address issues

---

**Last Updated:** 2025-12-12
**Maintainer:** Documentation team
**Status:** Living document - please keep updated
