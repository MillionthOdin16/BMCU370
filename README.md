# BMCU370

BMCU星尘修改版最新（BMCU-C 370霍尔版 V0.1-0020）源码，原项目链接：[Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)。有一些个人小优化。

BMCU Xing-C modified version latest (BMCU-C Hall V0.1-0020) source code. Includes some minor personal optimizations.

## Overview

The BMCU 370 C is a 4-channel filament controller for Bambu Lab 3D printers, featuring Hall effect sensors (AS5600) for precise filament tracking and PID-controlled motor management. This version integrates seamlessly with Bambu Lab's AMS ecosystem via the BambuBus protocol.

### Key Features
- 🎯 **4-Channel Management** - Independent control of 4 filament channels
- 🧲 **Hall Effect Sensing** - AS5600 magnetic sensors for accurate position tracking
- 🎮 **PID Motor Control** - Smooth filament feeding and retraction
- 💡 **RGB LED Indicators** - Per-channel status visualization
- 💾 **Flash Storage** - Non-volatile filament profile storage
- 📡 **BambuBus Protocol** - Native Bambu Lab printer integration

## Documentation

Comprehensive documentation is available in the `docs/` directory:

📖 **[Getting Started Guide](docs/GETTING_STARTED.md)** - Setup, installation, and first use  
🔧 **[Technical Documentation](docs/TECHNICAL_DOCUMENTATION.md)** - Hardware specs and system architecture  
👨‍💻 **[Developer Guide](docs/DEVELOPER_GUIDE.md)** - Code structure, API reference, and contributing  
🔍 **[Troubleshooting Guide](docs/TROUBLESHOOTING.md)** - Common issues and solutions  
📋 **[TODO & Notes](docs/TODO.md)** - Items needing verification and future work

## Quick Start

### Hardware Requirements
- BMCU 370 C board with AS5600 Hall sensors
- Compatible Bambu Lab printer (P1P/P1S/X1C)
- 12V or 24V DC power supply (check board marking)
- WCH-Link programmer (for firmware updates)

### Software Requirements
- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- WCH-Link drivers

### Building Firmware

```bash
# Clone repository
git clone https://github.com/MillionthOdin16/BMCU370.git
cd BMCU370

# Build with PlatformIO
pio run

# Upload to device
pio run --target upload

# Monitor serial output (optional)
pio device monitor --baud 115200
```

## External Resources

### Official Wikis
- **English Wiki**: https://wiki.yuekai.fr/
- **中文Wiki**: https://bmcu.wanzii.cn/

### Related Projects
- **Original Project**: [Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)
- **Hardware Design**: https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu
- **Gitee Mirror**: https://gitee.com/at_4061N/BMCU

## Changelog / 更新日志

### Version 0020 (July 17, 2025) / 25-7月17日-0020

**Fixes / 修复**:
- Fixed lighting logic errors causing some states not to illuminate / 修复灯光逻辑错误，导致一些状态不亮灯
- Fixed unexpected channel coming online / 修复通道意外上线
- Corrected anti-disconnect feature (previously ineffective) / 修正防掉线，之前并未生效
- Rewrote lighting system, fixed flickering issues, reduced refresh frequency / 重写灯光系统，修复了闪烁问题，降低了刷新频率
- When channel has error, attempts to update red LED every 3 seconds, prevents dark channel when inserted after BMCU enters working state / 当通道错误时，每隔3秒尝试更新一次红色，避免BMCU进入工作状态后插入的通道不亮灯

### Version 0019 (July 6, 2025) / 25-7月6日-0019修改版

**Compatibility / 兼容性**:
- Also compatible with dual microswitch Hall version / 双微动霍尔版本也可用

**Features / 新功能**:
- P1X1 now supports 16-color mode (firmware dependent) / 根据刷入固件不同可以让P1X1支持16色了
- Fixed filament information save issue with P1X1 printer firmware 00.01.06.62+ and slicer 2.1.1.52+ / 修复了P1X1打印机固件升级后（目前最新00.01.06.62），或切片软件最新版(目前2.1.1.52)下，无法保存耗材丝信息的问题
- Modified online logic detection to prevent erroneous channel online status / 修改了在线逻辑判断，防止某些状态下出现错误的通道在线
- Modified motor control logic with different calls for high/low voltage states / 修改了电机控制逻辑，在高低电压位使用不同调用

**Improvements / 改进**:
- Main board LED: red breathing when not connected to printer, white breathing during normal operation / 主板灯光，未连接打印机时红色呼吸，正常工作时白色呼吸
- Further reduced buffer and main board LED brightness / 进一步降低缓冲灯光和主板灯光的亮度
- Retraction: removed A1 control / 退料部分，抛弃对A1进行控制

## License

This project is licensed under the GNU General Public License v2.0 - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please read the [Developer Guide](docs/DEVELOPER_GUIDE.md) for details on code style, testing, and the pull request process.

## Support

- **Documentation Issues**: Open an issue on GitHub
- **Hardware Questions**: Consult the [Troubleshooting Guide](docs/TROUBLESHOOTING.md)
- **Community**: Visit the official wikis for community support

---

**Firmware Version**: BMCU-C 370 Hall V0.1-0020  
**Platform**: CH32V203C8T6 (RISC-V 32-bit @ 144MHz)  
**Protocol**: BambuBus v5