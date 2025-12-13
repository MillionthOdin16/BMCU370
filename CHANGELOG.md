# BMCU-C 370 Hall Version - Changelog

## Version History

This changelog documents the development history of the BMCU-C 370 Hall version firmware.

---

## [V0.1-0021] - 2025-12-13

### Fixed
- **Bug #27 - CRITICAL: BambuBus Buffer Overflow:** Fixed critical remote buffer overflow vulnerability in UART interrupt handler by checking bounds BEFORE write operation
- **Bug #22 - CRITICAL: Filament Data Buffer Overflows:** Fixed multiple buffer overflow vulnerabilities in `send_for_set_filament()` and `send_for_set_filament_type2()` by adding bounds checking for external input
- **Bug #1 - CRITICAL: Array Index Bounds Checking:** Fixed buffer underflow vulnerability in 7 functions by using `(unsigned)num < 4` to check both lower and upper bounds
- **Bug #19: AS5600 Memory Leak:** Fixed memory leak in AS5600 destructor by changing `delete` to `delete[]` for proper array deallocation
- **Bug #23: Flash Write Verification:** Re-enabled flash write verification to detect silent data corruption
- **Bug #2: Race Conditions in Motion Control:** Fixed potential race conditions in `MC_PULL_ONLINE_read()` by adding null pointer validation and atomic-like updates
- **Bug #31: LED Array Bounds Checking:** Fixed buffer overflow potential in `Set_MC_RGB()` by adding channel parameter bounds checking
- **Bug #3: Negative Meter Value Validation:** Added validation to prevent negative filament meter values in `add_filament_meters()`
- **Bug #24: DMA Error Handling:** Added DMA transfer error interrupt configuration and flag checking to prevent incorrect ADC readings
- **Bug #25: CRC Failure Logging:** Added debug logging and error counter for CRC8 validation failures to improve debugging

### Security
- **10 security vulnerabilities fixed:** 5 critical, 1 high priority, 3 medium priority, 1 low priority
- All buffer overflow and memory safety issues from ACTIVE_ISSUES.md have been resolved
- Remote exploit vulnerabilities in BambuBus communication protocol have been patched

### Changed
- Flash write operations now properly verify written data and return failure status on corruption
- ADC DMA operations now check for transfer errors before processing data
- Motion control now uses temporary buffers for atomic-like updates to sensor data

### Technical Details
- Modified files: `BambuBus.cpp`, `many_soft_AS5600.cpp`, `Flash_saves.cpp`, `Motion_control.cpp`, `main.cpp`, `ADC_DMA.cpp`
- Build verified successfully with PlatformIO on CH32V203C8T6 platform
- Memory usage: RAM 46.5% (9532/20480 bytes), Flash 57.8% (37884/65536 bytes)

### Documentation
- Updated `ACTIVE_ISSUES.md` to mark all 10 resolved bugs as FIXED
- All critical and high priority security bugs have been addressed

---

## [V0.1-0020] - 2025-07-17

### Fixed
- **Lighting Logic Error:** Fixed LED logic errors that caused some states to not illuminate properly
- **Channel Unexpected Online:** Fixed issue where channels would come online unexpectedly
- **Anti-Disconnection:** Corrected anti-disconnection feature that was not working properly
- **Flickering Issue:** Rewrote lighting system to fix flickering problems and reduce refresh frequency

### Changed
- **Channel Error Handling:** When channel error occurs, system now attempts to update red LED every 3 seconds to ensure newly inserted channels after BMCU enters working state will light up

### Technical Details
- LED refresh frequency reduced to prevent flicker
- Error state LED updates implemented with 3-second interval
- Improved channel online detection logic

---

## [V0.1-0019-Modified] - 2025-07-06

### Added
- **Dual Microswitch Hall Version:** Firmware now compatible with dual microswitch Hall sensor variant

### Fixed (from 0019 original)
- **P1X1 16-Color Support:** Different firmware flashes can enable 16-color support for P1X1 printer
- **Filament Info Save Issue:** Fixed inability to save filament information after P1X1 printer firmware upgrade (latest 00.01.06.62) or slicer update (2.1.1.52)
- **Online Logic:** Modified online logic determination to prevent incorrect channel online status in certain states
- **Motor Control:** Changed motor control logic to use different calls for high and low voltage positions

### Changed (modifications from v0013)
- **Mainboard Lighting:**
  - Not connected to printer: Red breathing
  - Normal operation: White breathing
- **LED Brightness:** Further reduced brightness for both buffer LEDs and mainboard LED
- **Filament Retraction:** Abandoned control over A1 during retraction

---

## [V0.1-0013] - Earlier Version

### Changes from Previous
- Introduced modified lighting effects with overheat prevention
- Enhanced buffer LED and mainboard LED brightness control
- Changes to retraction logic

### Known Issues from This Version
- Potential for getting stuck at 99% after printing if not installed properly
- Required careful installation of clutch mechanism

---

## Protocol Version History

### BambuBus Protocol Version 5 (Current)

**Flash Storage Structure:**
```cpp
#define Bambubus_version 5
```

**Changes from Previous Versions:**
- Structure alignment requirements enforced
- Improved filament data persistence
- Enhanced flash validation

**Compatibility:**
- Compatible with Bambu Lab A1/A1 mini firmware 00.01.06.62+
- Compatible with Bambu Studio 2.1.1.52+

---

## Hardware Compatibility

### BMCU-C (370 Hall Version)

**Supported Hardware:**
- ✅ 370 motors (24V 6000RPM)
- ✅ AS5600 Hall sensors (4 units)
- ✅ Bi-directional buffer system
- ✅ WS2812B RGB LEDs
- ✅ CH32V203C8T6 microcontroller

**Sub-board Requirements:**
- Hall sensor sub-board (different from photoelectric versions)
- Dual microswitch variant (optional, supported in 0019-modified+)

**Mainboard Compatibility:**
- Universal mainboard compatible with all BMCU versions
- Optional Type-C variants supported
- Xing-C modified boards supported

---

## Known Issues

### Current (V0.1-0020)

**Minor Issues:**
- Transparent filament detection may be unreliable (use opaque filament)
- Channel 1 may show false positive filament detection (documented community issue)
- Some gear noise during operation (normal for 370 motors)

**Limitations:**
- No NFC/RFID support (hardware limitation)
- Future Bambu Lab firmware updates may affect compatibility

### Fixed in Recent Versions

- ~~LED flickering~~ - Fixed in v0020
- ~~Stuck at 99%~~ - Greatly reduced with bi-directional buffer
- ~~Channel false online~~ - Fixed in v0019
- ~~Filament info not saving~~ - Fixed in v0019

---

## Upgrade Path

### From V0.1-0019 to V0.1-0020

**Changes:**
- Improved LED system (rewritten)
- Better error handling
- Enhanced channel detection

**Flash Data Compatibility:** ✅ Compatible (same protocol version 5)

**Upgrade Procedure:**
1. Backup current firmware (optional)
2. Flash new firmware via PlatformIO or ISP tool
3. Flash data preserved automatically
4. Test all channels after upgrade

### From Earlier Versions

**From v0013 or earlier:**
- Flash data structure may be incompatible
- Recommend backing up filament colors/settings
- May need to re-configure channels after flash
- Test thoroughly before production use

---

## Development Milestones

### BMCU Evolution

**BMCU-A (130/180 Motor)** → **BMCU-B (370 Motor + Photoelectric)** → **BMCU-C (370 Motor + Hall Sensor)**

**BMCU-C Improvements:**
- Bi-directional buffer (vs one-way in BMCU-B)
- Smart buffer features (auto-feed on press, auto-retract on pull)
- Hall sensors for more accurate detection
- Reduced hub ejection risk
- Better suited for multi-BMCU setups

### Community Contributions

**Key Contributors:**
- **4061N (@at_4061N):** Original BMCU developer
- **Xing-C (@XC):** 370X microswitch version, PCB improvements
- **ABin:** Dual microswitch Hall version co-developer
- **Community:** Testing, feedback, translations, improvements

---

## Future Roadmap

### Planned Enhancements (Community Discussion)

**Under Consideration:**
- [ ] Improved transparent filament detection
- [ ] Configurable buffer thresholds via UI
- [ ] Enhanced error reporting
- [ ] Multiple BMCU coordination
- [ ] P-series printer optimizations

**Hardware Dependent (Unlikely):**
- ❌ NFC/RFID support (requires hardware redesign)
- ❌ WiFi connectivity (major hardware change)

### Community Requests

See GitHub/Gitee issues for feature requests and discussions.

---

## Compatibility Matrix

| Firmware Version | Protocol Ver | Printer Firmware | Slicer Version | Notes |
|-----------------|--------------|------------------|----------------|-------|
| V0.1-0020 | 5 | 00.01.06.62+ | 2.1.1.52+ | Current, recommended |
| V0.1-0019 | 5 | 00.01.06.62+ | 2.1.1.52+ | Stable |
| V0.1-0013 | ? | Earlier | Earlier | Older, upgrade recommended |

**Printer Compatibility:**
- ✅ Bambu Lab A1
- ✅ Bambu Lab A1 mini
- 🔶 P-series (requires modification)

---

## Breaking Changes

### None in Recent Versions

V0.1-0019 to V0.1-0020 is backward compatible with flash data and hardware.

### Historical Breaking Changes

**BMCU-B to BMCU-C:**
- Requires different sub-board (Hall sensor instead of photoelectric)
- Mainboard remains compatible
- Firmware incompatible (different sensor types)

---

## Release Notes Format

Each release should document:
- 🆕 New Features
- 🔧 Fixed Issues
- 🔄 Changed Behavior
- ⚠️ Breaking Changes
- 📝 Known Issues

---

## Version Numbering Scheme

**Format:** `V[major].[minor]-[build]`

**Example:** V0.1-0020
- Major: 0 (pre-release/beta)
- Minor: 1 (feature set)
- Build: 0020 (incremental build number)

**Protocol Version:** Separate numbering (currently 5)

---

## How to Contribute

### Reporting Issues

1. Check existing issues/changelog
2. Test with latest firmware
3. Provide detailed information:
   - Firmware version
   - Hardware variant
   - Steps to reproduce
   - Expected vs actual behavior
   - Debug logs if possible

### Suggesting Features

1. Search existing requests
2. Describe use case clearly
3. Consider hardware limitations
4. Provide implementation ideas if possible

### Contributing Code

1. Fork repository
2. Create feature branch
3. Follow existing code style
4. Test thoroughly
5. Submit pull request with clear description
6. Update documentation as needed

---

## Documentation Changes

### Documentation V1.0 - 2025-12-12

**Added:**
- Comprehensive documentation structure
- User guides (Quick Start, Hardware Overview, Troubleshooting)
- Developer guides (Development Setup, Architecture, BambuBus Protocol)
- Hardware documentation (planned)
- TODO document for tracking issues

**Format:** Markdown in `/docs` directory

---

## License

This firmware is licensed under GPL 2.0. See LICENSE file for details.

**Important:** For personal use and learning only. Commercial use is prohibited.

---

## Resources

- **Original Project:** [Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)
- **Community Wiki (EN):** https://wiki.yuekai.fr/en/BMCU
- **Community Wiki (CN):** https://bmcu.wanzii.cn/
- **Gitee Repository:** https://gitee.com/at_4061N/BMCU
- **Hardware Design:** https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu

---

**Note:** This changelog is maintained by the community. Dates and version numbers are based on available information and may not be exact. Always refer to source code version defines for authoritative version information.
