# Contributing to BMCU370

Thank you for your interest in contributing to the BMCU370 firmware project! This document provides guidelines and information for contributors.

## 🚀 Quick Start

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/BMCU370.git
   cd BMCU370
   ```
3. **Install PlatformIO**:
   ```bash
   pip install platformio
   ```
4. **Build the project** to ensure everything works:
   ```bash
   pio run
   ```

## 📝 Development Workflow

### 1. Setting Up Your Environment

- **Recommended IDE**: VS Code with PlatformIO extension
- **Hardware**: CH32V203C8T6 development board (optional for compilation)
- **Programmer**: WCH-Link or ST-Link (required for testing on hardware)

### 2. Making Changes

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** following our coding standards (see below)

3. **Test your changes**:
   ```bash
   # Build the firmware
   pio run
   
   # Check memory usage
   pio run --target size
   
   # Clean build (if needed)
   pio run --target clean
   ```

4. **Commit your changes**:
   ```bash
   git commit -m "feat: add your feature description"
   ```

### 3. Submitting Changes

1. **Push your branch**:
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Create a Pull Request** on GitHub with:
   - Clear description of changes
   - Reference to any related issues
   - Screenshots/videos for UI changes
   - Test results if applicable

## 📋 Coding Standards

### C/C++ Code Style

- **Indentation**: 2 spaces (no tabs)
- **Line length**: 120 characters maximum
- **Naming conventions**:
  - Functions: `camelCase` or `snake_case` (match existing style)
  - Variables: `camelCase` or `snake_case`
  - Constants: `UPPER_SNAKE_CASE`
  - Classes: `PascalCase`

### File Organization

- **Headers**: Include guards using `#ifndef`/`#define`/`#endif`
- **Includes**: Group and order logically (system, libraries, local)
- **Documentation**: Use clear comments for complex logic

### Memory Constraints

The CH32V203C8T6 has limited resources:
- **Flash**: 64KB (currently ~62.5% used)
- **RAM**: 20KB (currently ~48.8% used)

Always check memory usage after changes:
```bash
pio run --target size
```

## 🧪 Testing

### Build Testing

All changes must compile successfully:
```bash
pio run
```

### Hardware Testing

If you have hardware access:
1. Flash the firmware: `pio run --target upload`
2. Test basic functionality
3. Verify LED responses
4. Check BambuBus communication (if connected to printer)

### Automated Testing

- **CI/CD**: All PRs trigger automated builds
- **Dev Builds**: Development builds are created automatically
- **Artifacts**: Download test builds from GitHub Actions

## 📚 Documentation

### Updating Documentation

When making changes that affect:
- **API**: Update `docs/firmware/API.md`
- **Hardware**: Update `docs/hardware/HARDWARE.md`
- **Build process**: Update `docs/firmware/CI-CD.md`
- **New features**: Update main `README.md`

### Documentation Standards

- Use **Markdown** for all text documentation
- Follow existing structure and formatting
- Include code examples where helpful
- Update links when moving files

## 🐛 Issue Reporting

### Bug Reports

Include the following information:
- **Hardware**: BMCU370 version, printer model
- **Firmware**: Version number and build information
- **Symptoms**: Detailed description of the issue
- **Logs**: Debug output (if available)
- **Steps to reproduce**: Clear reproduction steps

### Feature Requests

- **Use case**: Explain why the feature is needed
- **Implementation ideas**: Suggest how it might work
- **Hardware requirements**: Any new hardware needs
- **Compatibility**: Impact on existing setups

## 🔧 Firmware Version Management

### Critical Information

⚠️ **Warning**: Firmware version changes affect printer compatibility!

- **Current AMS version**: 00.00.06.49
- **Current AMS Lite version**: 00.01.02.03
- **Configuration**: See `src/config.h`

Only modify firmware versions if you understand compatibility requirements.

### Release Process

1. **Version tags**: Use semantic versioning (`v1.2.3`)
2. **Automated builds**: Releases are built automatically
3. **Testing**: Ensure compatibility before release
4. **Documentation**: Update CHANGELOG.md

## 🎯 Priority Areas

### High Impact Contributions

- **Protocol improvements**: BambuBus enhancements
- **Motor control**: Precision and reliability improvements
- **Sensor integration**: Better Hall sensor handling
- **Power management**: Efficiency optimizations

### Easy Starting Points

- **Documentation**: Improve existing docs
- **Code cleanup**: Refactor without changing functionality
- **Testing**: Add validation scripts
- **Examples**: Usage examples and tutorials

## 📞 Getting Help

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and ideas
- **Wiki**: Check existing documentation first
- **Community**: Bambu Lab printer communities

## 📜 License

By contributing, you agree that your contributions will be licensed under the same license as the project (see [LICENSE](LICENSE)).

---

Thank you for contributing to BMCU370! 🎉