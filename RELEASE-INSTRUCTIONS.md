# Release Instructions for BMCU370 v2.0.0

## Release Status
- **Version**: v2.0.0
- **Tag Created**: ✅ Local tag created (`git tag -a v2.0.0`)
- **Build Tested**: ✅ Firmware builds successfully
- **GitHub Actions**: ✅ Workflow ready for tag push
- **Release Notes**: ✅ Comprehensive release notes prepared

## To Complete the Release

### Step 1: Push the Release Tag
Execute the following command to push the tag and trigger the automated release:

```bash
git push origin v2.0.0
```

This will:
- Push the v2.0.0 tag to GitHub
- Trigger the GitHub Actions workflow (.github/workflows/build-firmware.yml)
- Automatically build the firmware
- Create a GitHub release with assets

### Step 2: Verify Release Creation

After pushing the tag, check:
1. **GitHub Actions**: Visit the Actions tab to see the build progress
2. **Release Page**: Check https://github.com/MillionthOdin16/BMCU370/releases
3. **Assets**: Verify the following files are generated:
   - `BMCU370-firmware-v2.0.0.bin` (main firmware binary)
   - `BMCU370-firmware-v2.0.0.elf` (debug symbols)
   - `BUILD-INFO.txt` (build information)

### Step 3: Test Release Artifacts

Download and verify:
- Firmware binary size should be ~37,748 bytes
- BUILD-INFO.txt should contain proper version information
- ELF file should be available for debugging

## Expected Release Content

### Generated Assets
- **firmware.bin**: Ready-to-flash binary for CH32V203C8T6
- **firmware.elf**: Debug symbols and development binary  
- **BUILD-INFO.txt**: Comprehensive build information including:
  - Commit SHA: 8a38bc90133e0a54a2f7b01808edac824824706d
  - Build date and PlatformIO version
  - Hardware specifications
  - Firmware version details
  - Flash instructions

### Release Description
The GitHub Actions workflow will automatically generate a release with:
- Complete feature list and improvements
- Installation instructions
- Compatibility information
- Technical specifications

## Firmware Specifications

### Build Information
- **Flash Usage**: 57.6% (37,748 bytes of 65,536 bytes)
- **RAM Usage**: 46.5% (9,524 bytes of 20,480 bytes)
- **Target**: CH32V203C8T6 microcontroller
- **Framework**: Arduino with CH32V platform

### Version Information
- **AMS Firmware**: 00.00.06.49
- **AMS Lite Firmware**: 00.01.02.03
- **BambuBus Protocol**: Version 5

### Compatibility
- Bambu Lab X1 series printers
- Bambu Lab A1 series printers
- Bambu Lab P1 series printers

## Troubleshooting

### If GitHub Actions Fails
1. Check workflow logs in GitHub Actions tab
2. Verify PlatformIO can access the CH32V platform
3. Ensure all dependencies are available
4. Check for any missing files or permissions issues

### If Build Fails
The firmware has been pre-tested and builds successfully. If issues occur:
1. Compare with local build results
2. Check platform-specific dependencies
3. Verify workflow environment setup

### Manual Build Process
If automated build fails, manual process:
```bash
pip install platformio
pio run
cp .pio/build/genericCH32V203C8T6/firmware.bin ./BMCU370-firmware-v2.0.0.bin
```

## Release Notes Location

The complete release notes are prepared and available in:
- Repository: `/tmp/release-notes.md` (temporary location)
- Content: Comprehensive changelog with all improvements since 0020-1

## Post-Release Actions

After successful release:
1. Update repository README with latest release link
2. Announce release in relevant forums/communities  
3. Update documentation references to v2.0.0
4. Monitor for any compatibility issues with different printer models

---

**Note**: This release represents a major milestone with complete firmware rewrite, enhanced documentation, and automated build system. The original firmware from krrr/BMCU370 has been significantly enhanced with modern development practices and comprehensive feature additions.