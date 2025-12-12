# BMCU-C 370 Hall Version - Comprehensive Documentation

This directory contains comprehensive technical and user documentation for the BMCU-C 370 Hall Effect version.

## Documentation Structure

### User Documentation
- [Quick Start Guide](user-guide/quick-start.md) - Getting started with BMCU-C 370
- [Hardware Overview](user-guide/hardware-overview.md) - Understanding the hardware components
- [Installation Guide](user-guide/installation.md) - Step-by-step installation instructions
- [Troubleshooting](user-guide/troubleshooting.md) - Common issues and solutions
- [Maintenance Guide](user-guide/maintenance.md) - Keeping your BMCU-C running smoothly

### Developer Documentation
- [Development Environment Setup](developer-guide/development-setup.md) - Setting up your development environment
- [Architecture Overview](developer-guide/architecture.md) - System architecture and design
- [BambuBus Protocol](developer-guide/bambubus-protocol.md) - Communication protocol details
- [Hall Sensor System](developer-guide/hall-sensor.md) - AS5600 Hall sensor implementation
- [Motor Control](developer-guide/motor-control.md) - Motion control system
- [RGB LED System](developer-guide/rgb-leds.md) - LED control and status indication
- [Code Reference](developer-guide/code-reference.md) - API and function reference

### Hardware Documentation
- [PCB Specifications](hardware/pcb-specs.md) - PCB design and specifications
- [Bill of Materials](hardware/bom.md) - Complete parts list
- [Wiring Diagrams](hardware/wiring.md) - Connection diagrams
- [Sensor Specifications](hardware/sensors.md) - Hall sensor and ADC details

## About This Project

BMCU-C is the latest generation of the BMCU (Bambu Multi-Color Unit) project, featuring 370 motors and Hall effect sensors for improved reliability and performance.

**Version:** BMCU-C 370 Hall V0.1-0020  
**Microcontroller:** CH32V203C8T6 (RISC-V)  
**Framework:** Arduino on PlatformIO  
**License:** GPL 2.0 (Personal use and learning only)

## Key Features

- **Four-channel filament management** compatible with Bambu Lab A-series printers
- **Hall effect sensors (AS5600)** for accurate filament detection and buffer monitoring
- **Bi-directional buffer system** prevents AMS Lite Hub ejection
- **Smart filament handling** with automatic feed and retraction
- **RGB LED status indicators** for each channel and system status
- **370 motors (24V 6000RPM)** for faster filament loading/unloading
- **BambuBus protocol** for seamless printer integration

## Quick Links

- **Original Project:** [Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)
- **Community Wiki (English):** https://wiki.yuekai.fr/en/BMCU
- **Community Wiki (Chinese):** https://bmcu.wanzii.cn/
- **Gitee Repository:** https://gitee.com/at_4061N/BMCU
- **Hardware Design:** https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu

## Important Notices

⚠️ **This is an open-source DIY project for personal learning purposes only. Commercial use is prohibited.**

⚠️ **BMCU is not an official Bambu Lab product. Future firmware updates may affect compatibility.**

✅ **Always recommend official Bambu Lab AMS Lite when possible for the best experience.**

## Getting Started

1. Read the [Hardware Overview](user-guide/hardware-overview.md) to understand the components
2. Review the [Bill of Materials](hardware/bom.md) to gather required parts
3. Follow the [Installation Guide](user-guide/installation.md) for assembly
4. Use the [Troubleshooting Guide](user-guide/troubleshooting.md) if you encounter issues

For developers:
1. Set up your [Development Environment](developer-guide/development-setup.md)
2. Read the [Architecture Overview](developer-guide/architecture.md)
3. Explore the [Code Reference](developer-guide/code-reference.md)

## Support and Community

- Join the community discussions (check wiki links above)
- Report issues on GitHub
- Contribute improvements via pull requests

## Changelog

See [CHANGELOG.md](../CHANGELOG.md) for version history and updates.
