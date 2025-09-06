# Changelog

All notable changes to the BMCU370 firmware will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **Comprehensive .editorconfig** for consistent code formatting across editors
- **Detailed CONTRIBUTING.md** with development guidelines and standards
- **Organized documentation structure** with logical subdirectories:
  - `docs/firmware/` - Firmware-specific documentation
  - `docs/hardware/` - Hardware specifications and setup
  - `docs/assembly/` - Physical assembly documentation
  - `docs/tools/` - Development and programming tools
- **Documentation navigation** with comprehensive docs/README.md

### Enhanced
- **Improved .gitignore** with comprehensive exclusions for all build artifacts and IDE files
- **README structure** with better organization and updated documentation links
- **Repository organization** following best practices for embedded firmware projects

### Removed
- Outdated "Motor reversal, please see.txt" file (functionality now handled by automatic direction detection)

### Changed
- **Documentation organization**: Moved all docs to logical subdirectories
- **README links**: Updated all documentation references to new organized structure

## [2.0.0] - 2024-09-04

### Added
- **Complete firmware rewrite** with modular architecture
- **Automated GitHub Actions workflow** for CI/CD and release management
- **PlatformIO build system** with dependency management
- **Comprehensive configuration system** via `config.h`
- **Enhanced BambuBus protocol implementation** with improved error handling
- **Debug logging system** with configurable levels
- **Extensive documentation**:
  - Complete README with hardware specifications
  - API documentation for BambuBus protocol
  - Hardware setup and configuration guide
  - Build and development instructions
- **Version management system** for AMS compatibility
- **Flash memory management** for persistent configuration
- **64-bit timestamp utilities** for improved timing accuracy
- **Automated release asset generation** with build information

### Enhanced
- **LED control system** with configurable channel counts
- **Motion control algorithms** with voltage-based thresholds
- **AS5600 Hall sensor integration** with improved position sensing
- **ADC with DMA support** for efficient sensor reading
- **Error handling and recovery** mechanisms
- **Memory usage optimization** (57.6% flash, 46.5% RAM)
- **Cross-platform build support** via PlatformIO

### Fixed
- Lighting logic errors causing some states not to illuminate
- Unexpected channel online issues
- Anti-disconnect logic effectiveness
- RGB flickering issues with reduced refresh rate
- Channel error recovery with 3-second red light retry

### Changed
- **Build system**: Migrated from custom build to PlatformIO
- **Project structure**: Organized into logical modules
- **Configuration**: Centralized hardware parameters
- **Documentation**: Complete rewrite with comprehensive guides
- **Version reporting**: Separated AMS vs AMS Lite firmware versions

### Technical Specifications
- **Target Hardware**: CH32V203C8T6 (RISC-V, 144MHz)
- **Memory Usage**: 37,748 bytes flash (57.6%), 9,524 bytes RAM (46.5%)
- **AMS Firmware Version**: 00.00.06.49
- **AMS Lite Firmware Version**: 00.01.02.03
- **BambuBus Protocol**: Version 5
- **Supported Printers**: Bambu Lab X1, A1, P1 series

### Migration Notes
- Hardware parameters now configured via `config.h`
- Debug logging can be enabled via `Debug_log_on` definition
- Build process requires PlatformIO instead of custom toolchain
- Flash address remains `0x08000000` for compatibility

## [0020-1] - 2024-08-16 (Original krrr/BMCU370 Release)

### Reference Release
This was the last release from the original [krrr/BMCU370](https://github.com/krrr/BMCU370) repository.

### Features (from original)
- Basic AMS communication protocol
- Multi-channel filament management
- RGB LED status indicators
- Hall sensor integration
- Basic motion control

### Known Issues (addressed in 2.0.0)
- Lighting logic errors
- Channel online detection issues
- Ineffective anti-disconnect logic
- RGB flickering problems
- Limited error recovery

---

## Release Process

### Creating a New Release

1. Update version numbers in relevant files
2. Update this CHANGELOG.md with new changes
3. Create a new tag: `git tag -a vX.Y.Z -m "Version X.Y.Z"`
4. Push the tag: `git push origin vX.Y.Z`
5. GitHub Actions will automatically:
   - Build the firmware
   - Create a GitHub release
   - Upload firmware binaries and build information

### Version Scheme

- **Major (X)**: Breaking changes, architecture changes
- **Minor (Y)**: New features, enhancements
- **Patch (Z)**: Bug fixes, small improvements

### Build Information

Each release includes:
- `BMCU370-firmware-vX.Y.Z.bin` - Main firmware binary
- `BMCU370-firmware-vX.Y.Z.elf` - Debug symbols
- `BUILD-INFO.txt` - Comprehensive build information

---

## Support and Compatibility

### Printer Compatibility
- **Bambu Lab X1 Series**: Full compatibility
- **Bambu Lab A1 Series**: Full compatibility  
- **Bambu Lab P1 Series**: Full compatibility

### Firmware Version Compatibility
The firmware reports different versions based on detected device type:
- **AMS (8-channel)**: Reports version configured in `AMS_FIRMWARE_VERSION_*`
- **AMS Lite**: Reports version configured in `AMS_LITE_FIRMWARE_VERSION_*`

### Hardware Requirements
- **Microcontroller**: CH32V203C8T6 or compatible
- **Programmer**: WCH-Link or ST-Link compatible
- **Sensors**: AS5600 Hall sensors for position feedback
- **LEDs**: NeoPixel-compatible RGB strips

---

For complete installation and usage instructions, see the [README.md](README.md).