# BMCU-C 370 Hall Version

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Community-orange.svg)](https://platformio.org/)
[![RISC-V](https://img.shields.io/badge/RISC--V-CH32V203-green.svg)](http://www.wch-ic.com/)

**BMCU Xing-C Modified Version** - Latest firmware for BMCU-C 370 Hall Effect version (V0.1-0020)

🇨🇳 **中文版本在下方 | Chinese version below**

---

## Overview

BMCU-C is an open-source, four-channel filament management system for Bambu Lab 3D printers, featuring 370 motors and Hall effect sensors. This version represents the latest evolution of the BMCU project, offering improved reliability and smart features compared to earlier variants.

**Key Features:**
- 🎯 Four-channel filament management compatible with A-series printers
- 🧲 AS5600 Hall effect sensors for accurate position detection
- 🔄 Bi-directional buffer system preventing hub ejection
- 💡 Smart auto-feed and auto-retract on buffer manipulation
- 🌈 RGB LED status indicators for each channel
- ⚡ 370 motors (24V 6000RPM) for fast loading/unloading
- 🔌 BambuBus protocol for seamless printer integration

**Version Information:**
- **Firmware Version:** V0.1-0020 (2025-07-17)
- **Protocol Version:** 5
- **Microcontroller:** CH32V203C8T6 (RISC-V 32-bit @ 144MHz)
- **Framework:** Arduino on PlatformIO

---

## 📚 Documentation

**Comprehensive documentation is available in the [`/docs`](docs/) directory:**

### For Users
- **[Quick Start Guide](docs/user-guide/quick-start.md)** - Get up and running quickly
- **[Hardware Overview](docs/user-guide/hardware-overview.md)** - Understand the components
- **[Troubleshooting](docs/user-guide/troubleshooting.md)** - Solve common issues

### For Developers
- **[Development Setup](docs/developer-guide/development-setup.md)** - Build environment configuration
- **[Architecture Overview](docs/developer-guide/architecture.md)** - System design and structure
- **[BambuBus Protocol](docs/developer-guide/bambubus-protocol.md)** - Communication protocol details

### Additional Resources
- **[Changelog](CHANGELOG.md)** - Version history and updates
- **[TODO List](docs/TODO.md)** - Known issues and future improvements

---

## 🚀 Quick Start

### Prerequisites
- Bambu Lab A1 or A1 mini printer
- BMCU-C hardware assembled (see community wiki for build guides)
- PlatformIO IDE or Core installed

### Building the Firmware

```bash
# Clone the repository
git clone https://github.com/MillionthOdin16/BMCU370.git
cd BMCU370

# Build with PlatformIO
pio run

# Upload to device
pio run --target upload
```

### Configuration

The firmware is pre-configured for BMCU-C 370 Hall version. Key settings are in `platformio.ini`:

```ini
[env:genericCH32V203C8T6]
platform = https://github.com/Community-PIO-CH32V/platform-ch32v.git
board = genericCH32V203C8T6
framework = arduino
lib_deps = robtillaart/CRC@^1.0.3
build_flags = -D SYSCLK_FREQ_144MHz_HSI=144000000
```

---

## 🔧 Features

### Latest Updates (V0.1-0020)

✅ **Fixed Issues:**
- LED logic errors causing incorrect illumination
- Unexpected channel online detection
- Anti-disconnection feature now works properly
- LED flickering eliminated with rewritten lighting system

📈 **Improvements:**
- Reduced LED refresh frequency
- Error state LEDs update every 3 seconds
- Better channel insertion detection

### BMCU-C Advantages

| Feature | BMCU-A (130) | BMCU-B (370) | BMCU-C (370 Hall) |
|---------|--------------|--------------|-------------------|
| Motor Type | 130/180 | 370 | 370 |
| Detection | Photoelectric | Photoelectric | Hall Sensor |
| Buffer | One-way | One-way | **Bi-directional** |
| Smart Features | No | No | **Yes** |
| Hub Ejection Risk | Low | Medium | **Very Low** |
| Assembly Difficulty | Complex | Medium | Medium |

---

## 🌐 Links and Resources

### Official Resources
- **Original Project:** [Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)
- **BMCU Wiki (English):** https://wiki.yuekai.fr/en/BMCU
- **BMCU Wiki (Chinese):** https://bmcu.wanzii.cn/
- **Gitee Repository:** https://gitee.com/at_4061N/BMCU
- **Hardware Design:** https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu

### Community
- **Documentation:** See [/docs](docs/) directory
- **Issues:** GitHub Issues for bug reports
- **Discussions:** Community forums (see wiki)

---

## ⚠️ Important Notices

**⚠️ DIY Project Notice:**
- This is an **unofficial, community-driven** project
- For **personal use and learning** purposes only
- **Commercial use is prohibited** under GPL 2.0 license
- Not endorsed or supported by Bambu Lab

**⚠️ Compatibility Warning:**
- Future Bambu Lab firmware updates **may break compatibility**
- BMCU may stop working after printer firmware updates
- **Always use official AMS Lite when possible** for best experience

**⚠️ Limitations:**
- ❌ No NFC/RFID support (cannot read Bambu filament tags)
- ❌ Transparent filament detection may be unreliable
- ❌ Some operational noise from 370 motors

---

## 🛠️ Technical Specifications

### Hardware
- **MCU:** CH32V203C8T6 (RISC-V, 32-bit, 144 MHz)
- **Motors:** 4× 370 DC motors (24V, 6000 RPM)
- **Sensors:** 4× AS5600 Hall sensors (I2C, 12-bit)
- **LEDs:** 5× WS2812B RGB LED strips
- **Power:** 24V DC from printer (via 6-pin connector)
- **Communication:** RS-485 (BambuBus protocol @ 1.25 Mbaud)

### Software
- **Framework:** Arduino (CH32V support)
- **Platform:** PlatformIO
- **Language:** C++ with Arduino libraries
- **Flash Storage:** Persistent filament data @ 0x0800F000
- **Debug:** Optional USART3 debug output

---

## 📖 Project Structure

```
BMCU370/
├── src/                    # Source code
│   ├── main.cpp           # Main program loop
│   ├── BambuBus.cpp       # Communication protocol
│   ├── Motion_control.cpp # Motor and sensor control
│   ├── many_soft_AS5600.cpp # Hall sensor driver
│   └── ...                # Other modules
├── docs/                  # Comprehensive documentation
│   ├── user-guide/        # User documentation
│   ├── developer-guide/   # Developer documentation
│   └── hardware/          # Hardware specifications
├── platformio.ini         # PlatformIO configuration
├── LICENSE               # GPL 2.0 license
├── README.md             # This file
└── CHANGELOG.md          # Version history
```

---

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

**See also:** [Development Setup Guide](docs/developer-guide/development-setup.md)

---

## 📄 License

This project is licensed under the **GNU General Public License v2.0**.

**Key Points:**
- ✅ Free to use for personal and educational purposes
- ✅ Source code must remain open
- ✅ Modifications must be shared under GPL 2.0
- ❌ Commercial use prohibited
- ❌ No warranty provided

See [LICENSE](LICENSE) for full terms.

---

## 🙏 Credits

### Original Developers
- **4061N (@at_4061N):** Original BMCU creator
- **Xing-C (@XC):** 370X variant, PCB improvements
- **ABin:** Hall sensor implementation co-developer

### Community Contributors
- Testing and feedback
- Documentation and translations
- Hardware variant development
- This project stands on the shoulders of the entire BMCU community

---

## 🌏 中文版本 | Chinese Version

### 项目简介

BMCU星尘修改版最新（BMCU-C 370霍尔版 V0.1-0020）源码，原项目链接：[Xing-C/BMCU370x](https://github.com/Xing-C/BMCU370x)。包含一些个人小优化。

### 主要特点
- 四通道耗材管理系统
- AS5600霍尔传感器精确检测
- 双向缓冲系统
- 智能自动进退料
- RGB LED状态指示
- 370电机快速装卸料

### 资源链接
- **英文Wiki:** https://wiki.yuekai.fr/en/BMCU
- **中文Wiki:** https://bmcu.wanzii.cn/
- **Gitee仓库:** https://gitee.com/at_4061N/BMCU
- **硬件设计:** https://oshwhub.com/bamboo-shoot-xmcu-pcb-team/bmcu

### 更新日志

**25-7月17日-0020：**
- 修复灯光逻辑错误，导致一些状态不亮灯
- 修复通道意外上线
- 修正防掉线，之前并未生效
- 重写灯光系统，修复了闪烁问题，降低了刷新频率
- 当通道错误时，每隔3秒尝试更新一次红色，避免BMCU进入工作状态后插入的通道不亮灯

**25-7月6日-0019修改版：**
- 双微动霍尔版本也可用
- 根据刷入固件不同可以让P1X1支持16色了
- 修复了P1X1打印机固件升级后无法保存耗材丝信息的问题
- 修改了在线逻辑判断
- 修改了电机控制逻辑
- 主板灯光优化
- 进一步降低LED亮度

### 注意事项
⚠️ 本项目遵循GPL 2.0开源协议，仅供个人学习使用，禁止商用  
⚠️ BMCU不是拓竹官方产品，未来固件更新可能影响兼容性  
⚠️ 建议优先选择官方AMS Lite以获得最佳体验

---

## 📞 Support

For support and questions:
- 📖 Check the [documentation](docs/)
- 🐛 Report bugs via GitHub Issues
- 💬 Join community discussions (see wiki)
- 📧 Contact via repository (for major issues)

---

**Happy Printing! 🎨🖨️**