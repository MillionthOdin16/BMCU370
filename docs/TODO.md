# BMCU 370 C Documentation TODO and Notes

This document tracks items that need further investigation, clarification, or resolution. It serves as a reference for future documentation updates and code verification.

## Last Updated
December 2024

---

## Hardware Specifications - Items Needing Verification

### 1. Power Supply Voltage
- **Issue**: Documentation mentions both 12V and 24V compatibility
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**: 
  - Check actual board schematic for voltage regulator specifications
  - Verify if board has voltage selection jumper
  - Document exact power requirements for different board revisions
- **Source**: Inference from general BMCU documentation and typical AMS power requirements

### 2. Exact Motor Driver Type
- **Issue**: Motor driver chip model not specified in code
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**:
  - Identify motor driver IC from PCB or schematic
  - Document current limits and specifications
  - Verify PWM frequency requirements
- **Source**: Code shows PWM control but doesn't specify driver chip

### 3. ADC Input Configuration
- **Issue**: Physical ADC pin assignments not explicitly documented in code
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**:
  - Map ADC channels to specific GPIO pins
  - Verify ADC reference voltage source (internal vs external)
  - Document any external circuitry (voltage dividers, filters)
- **Source**: ADC_DMA.cpp shows 8-channel usage but pin mapping unclear

### 4. BambuBus Physical Interface
- **Issue**: Exact pins for BambuBus UART not documented
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**:
  - Identify UART TX/RX pins used
  - Document connector pinout
  - Verify if any level shifting required
- **Source**: BambuBus.cpp shows UART usage but pin configuration in hidden init

---

## Software/Firmware - Items Needing Clarification

### 5. Debug UART Pin Assignment
- **Issue**: Debug serial port pins not specified
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**:
  - Document which UART peripheral used (USART1/2/3)
  - Identify TX/RX GPIO pins
  - Note if conflicts with other features
- **Source**: Debug_log.cpp initializes UART but pin config unclear

### 6. PID Tuning Parameters
- **Issue**: Actual PID coefficients not visible in reviewed code
- **Status**: ⚠️ NEEDS RESEARCH
- **Action Required**:
  - Review Motion_control.cpp fully to find default PID values
  - Document recommended starting values
  - Create tuning guide if values are configurable
- **Source**: MOTOR_PID class exists but instantiation parameters not seen in sample

### 7. Flash Save Timing
- **Issue**: When exactly does periodic flash save occur?
- **Status**: ⚠️ NEEDS CODE REVIEW
- **Action Required**:
  - Find where `Bambubus_save()` is called in main loop
  - Document save interval
  - Verify wear-leveling strategy (if any)
- **Source**: Code shows `set_need_to_save()` but save trigger not seen in reviewed sections

### 8. Microswitch Voltage Thresholds (Dual Mode)
- **Issue**: Documentation lists thresholds but exact values may vary by hardware
- **Status**: ⚠️ NEEDS HARDWARE VERIFICATION
- **Action Required**:
  - Test with actual hardware to confirm voltage levels
  - Document any resistor values affecting thresholds
  - Note if calibration needed for different switch types
- **Current Values** (from code):
  - <0.6V: Offline
  - 1.4-1.7V: Outer switch only
  - >1.7V: Both switches
  - <1.4V: Inner switch only
- **Source**: Motion_control.cpp lines 93-108

---

## Documentation Consistency - Items to Cross-Check

### 9. Version Number Discrepancy
- **Issue**: Changelog mentions "25-7月17日" (July 17, 2025) but document written in Dec 2024
- **Status**: ⚠️ DATE INCONSISTENCY
- **Action Required**:
  - Verify if date is typo (should be 2024?)
  - Or if changelog is from future/planned release
  - Clarify version dating scheme
- **Source**: README.md changelog section

### 10. Assist Feed Timing
- **Issue**: `Assist_send_time = 1200` but units not clear
- **Status**: ⚠️ NEEDS CLARIFICATION
- **Action Required**:
  - Verify if 1200ms or different unit
  - Document what this timing controls exactly
  - Test optimal value for different installations
- **Source**: Motion_control.cpp line 29

### 11. Retraction Distance for External Installation
- **Issue**: Documentation says 700mm for external but value not found in code
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**:
  - Verify if 700mm is recommended or actual default
  - Check if configurable via flash or compile-time constant
  - Document how to change value safely
- **Source**: Inference from Motion_control.cpp line 31 showing 200mm default

---

## External Resource Verification - Links to Investigate

### 12. Online Resources Content
- **Status**: ⚠️ NOT ACCESSIBLE during documentation creation
- **Resources to Review**:
  - https://wiki.yuekai.fr/en/BMCU
  - https://wiki.yuekai.fr/en/BMCU/for_developers
  - https://wiki.yuekai.fr/en/BMCU/bmcu_branches
  - https://wiki.yuekai.fr/en/BMCU/troubleshooting
  - https://bmcu.wanzii.cn/doc/build/370hall.html
  - https://bmcu.wanzii.cn/doc/prepare/list.html
  - https://bmcu.wanzii.cn/doc/prepare/versions.html
  - https://bmcu.wanzii.cn/doc/debug/value-to-ground.html
  - https://bmcu.wanzii.cn/doc/other/download-center.html
  - https://bmcu.wanzii.cn/doc/debug/problems.html
  - https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu
  - https://gitee.com/at_4061N/BMCU

- **Action Required**:
  - Access each link when network available
  - Cross-reference information with current documentation
  - Update documentation to match official resources
  - Note any conflicts or discrepancies
  - Extract hardware schematics from oshwhub if available

### 13. Hardware Schematic
- **Issue**: PCB schematic not available in repository
- **Status**: ⚠️ NEEDS EXTERNAL RESOURCE
- **Action Required**:
  - Obtain schematic from https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu
  - Verify all pin assignments match code
  - Document any hardware revisions
  - Include schematic reference in documentation
- **Priority**: HIGH - Would resolve many hardware verification items above

---

## Feature Implementation - Uncertain Items

### 14. NFC Filament Detection
- **Issue**: Code references NFC but implementation unclear
- **Status**: ⚠️ FUTURE FEATURE?
- **Action Required**:
  - Determine if NFC hardware present on board
  - Check if feature is placeholder for future
  - Document NFC functionality if implemented
  - Remove references if not applicable to 370 C version
- **Source**: BambuBus.h shows `NFC_waiting` state and `NFC_detect` packet type

### 15. 16-Color Mode Support
- **Issue**: Changelog mentions 16-color support for P1X1 but details lacking
- **Status**: ⚠️ NEEDS DOCUMENTATION
- **Action Required**:
  - Document how 16-color mode differs from standard
  - List compatible firmware versions
  - Explain configuration steps if needed
  - Note limitations or special requirements
- **Source**: README.md changelog V0.1-0019

### 16. Humidity Sensor
- **Issue**: `AMS_humidity_wet` variable found but usage unclear
- **Status**: ⚠️ NEEDS INVESTIGATION
- **Action Required**:
  - Check if humidity sensor hardware exists
  - Verify if data sent to printer
  - Document sensor type and location if present
  - Note if feature is disabled/unused
- **Source**: BambuBus.cpp line 11

---

## Code vs. Documentation Inconsistencies

### 17. Flash Memory Layout
- **Issue**: Documentation shows approximate sizes, exact boundaries need verification
- **Status**: ⚠️ NEEDS PRECISION
- **Current Documentation**:
  - Program: 0x08000000 - 0x0800DFFF (~56 KB)
  - Motion config: 0x0800E000 - 0x0800EFFF (4 KB)
  - Filament data: 0x0800F000 - 0x0800FFFF (4 KB)
- **Action Required**:
  - Verify program never exceeds 0x0800DFFF
  - Check linker script for actual allocation
  - Document any reserved regions
  - Note if layout changes between firmware versions

### 18. LED Count Discrepancy
- **Issue**: Code comment mentions "测试用 8灯珠" (test 8 LEDs) but defines 2 per channel
- **Status**: ⚠️ NEEDS CLARIFICATION
- **Action Required**:
  - Verify actual LED count on production hardware
  - Check if 8-LED configuration available as option
  - Document both configurations if both valid
- **Source**: main.cpp lines 8-14

### 19. A1 Control Reference
- **Issue**: Changelog mentions "抛弃对A1进行控制" (abandon A1 control) but A1 not defined elsewhere
- **Status**: ⚠️ NEEDS CONTEXT
- **Action Required**:
  - Identify what A1 refers to (pin? component? feature?)
  - Document why control was removed
  - Note if affects any functionality
- **Source**: README.md changelog

---

## Testing and Validation - Items Needing Practical Verification

### 20. AS5600 Magnet Distance Tolerance
- **Issue**: Documentation states 0.5-3mm range from datasheet, but optimal value hardware-dependent
- **Status**: ⚠️ NEEDS FIELD TESTING
- **Action Required**:
  - Test with actual BMCU hardware
  - Document optimal distance for specific magnet type used
  - Note sensitivity to misalignment
  - Provide troubleshooting tips for field adjustment

### 21. Motor Current Requirements
- **Issue**: "Typical 0.5-1.0A" mentioned but not verified for actual motors used
- **Status**: ⚠️ NEEDS HARDWARE SPECS
- **Action Required**:
  - Identify actual motor model used
  - Document rated current
  - Recommend driver current setting
  - Note if different motors require different settings

### 22. BambuBus Cable Length Limitations
- **Issue**: "2 meters ideal" stated but not tested
- **Status**: ⚠️ NEEDS TESTING
- **Action Required**:
  - Test communication reliability at various lengths
  - Document maximum length before issues
  - Note if shielded cable extends range
  - Provide cable specification recommendations

---

## Translation and Localization

### 23. Chinese Comments in Code
- **Issue**: Many code comments in Chinese, may need English translation
- **Status**: ℹ️ LOW PRIORITY (code is bilingual project)
- **Action Required**:
  - Consider adding English translations alongside Chinese
  - Or document that Chinese comments are intentional
  - Note in contribution guidelines about comment language
- **Source**: Throughout codebase

### 24. Error Messages Language
- **Issue**: Debug output language not specified
- **Status**: ⚠️ NEEDS VERIFICATION
- **Action Required**:
  - Check if debug messages in English or Chinese
  - Document expected language
  - Note if user-configurable
- **Source**: Debug_log.cpp usage throughout

---

## Safety and Warnings

### 25. Overclock Stability
- **Issue**: 144 MHz overclock (2× standard 72 MHz) documented but stability not characterized
- **Status**: ⚠️ NEEDS TESTING DATA
- **Action Required**:
  - Document temperature range limitations
  - Note if some chips may be unstable at this speed
  - Provide fallback to 72 MHz if needed
  - Warn about warranty implications
- **Source**: platformio.ini build flag

### 26. Flash Wear Lifetime
- **Issue**: ~10,000 write cycles mentioned but actual lifetime depends on save frequency
- **Status**: ⚠️ NEEDS CALCULATION
- **Action Required**:
  - Document typical save frequency in normal use
  - Calculate expected lifetime (years)
  - Recommend backup procedures
  - Note warning signs of wear
- **Source**: Flash_saves.cpp and general flash memory specs

---

## Future Documentation Needs

### 27. Hardware Assembly Guide
- **Status**: 📋 NEEDED
- **Action Required**:
  - Create step-by-step assembly instructions with photos
  - Document PCB assembly (if sold as kit)
  - Provide mechanical assembly diagrams
  - Include cable routing recommendations

### 28. Calibration Procedures
- **Status**: 📋 NEEDED
- **Action Required**:
  - Document AS5600 magnet alignment procedure
  - Pressure sensor calibration (if applicable)
  - Motor direction verification steps
  - End-to-end system test protocol

### 29. Advanced Configuration Guide
- **Status**: 📋 NEEDED
- **Action Required**:
  - PID tuning tutorial
  - Custom threshold adjustment
  - Performance optimization tips
  - Power consumption optimization

### 30. FAQ Document
- **Status**: 📋 NEEDED
- **Action Required**:
  - Compile common questions from community
  - Create quick-reference FAQ
  - Link to relevant documentation sections
  - Keep updated based on user feedback

---

## Notes for Future Maintainers

### Code Review Limitations
This documentation was created based on:
- Limited view of source files (selected sections reviewed)
- README.md and platformio.ini
- Inference from code structure and comments
- General knowledge of embedded systems and 3D printing

**Not reviewed in full**:
- Complete BambuBus.cpp implementation (1285 lines)
- Complete Motion_control.cpp implementation (1052 lines)
- Complete Adafruit_NeoPixel.cpp implementation (3811 lines)
- Other supporting libraries and dependencies

### Recommended Next Steps
1. **Access external resources** listed in item #12
2. **Obtain hardware schematic** from OSHWHub
3. **Test with actual hardware** to verify all specifications
4. **Review complete source files** not fully examined
5. **Engage with community** to validate documentation accuracy
6. **Create visual aids** (diagrams, flowcharts, photos)
7. **Translate** between English and Chinese for wider accessibility

### Documentation Philosophy
- **Accuracy over completeness**: Better to mark items as "needs verification" than document incorrect information
- **User-focused**: Written for makers and programmers as specified
- **Living document**: Should be updated as more information becomes available
- **Best practices**: Follows documentation standards for embedded systems

---

## Tracking Changes to This TODO

When items are resolved:
1. ✅ Mark item as RESOLVED
2. Update relevant documentation
3. Add resolution date
4. Link to updated document sections
5. Keep resolved items for reference (don't delete)

### Resolution Template
```
### [Item Number] [Item Title]
- **Issue**: [Original issue description]
- **Status**: ✅ RESOLVED [Date]
- **Resolution**: [How it was resolved]
- **Updated Documentation**: [Links to updated files/sections]
- **Verified By**: [Who verified the information]
```

---

**Document Version**: 1.0  
**Created**: December 2024  
**Last Updated**: December 2024  
**Items Tracked**: 30 items
- ⚠️ Needs Verification: 21
- 📋 Future Work: 4
- ℹ️ Low Priority: 1
- 🔍 Informational: 4
