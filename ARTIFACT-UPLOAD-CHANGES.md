# GitHub Actions Artifact Upload Changes

## Summary
Modified the GitHub Actions workflows to upload individual files as separate artifacts instead of bundling them into ZIP files.

## Changes Made

### build-firmware.yml (Release builds)
- **Before**: Single artifact containing all files in a ZIP
- **After**: Five separate artifacts:
  1. `bmcu370-firmware_<version>_<platform>-<arch>.bin` - Main firmware binary
  2. `bmcu370-firmware_<version>_<platform>-<arch>.elf` - Debug symbols 
  3. `BUILD-INFO_<version>.txt` - Build metadata
  4. `RELEASE-CHANGELOG-<version>.md` - Release changelog
  5. `SHA256SUMS_<version>.txt` - File checksums

### dev-build.yml (Development builds)
- **Before**: Single artifact containing all files in a ZIP
- **After**: Five separate artifacts:
  1. `bmcu370-firmware_<version>_<platform>-<arch>.bin` - Main firmware binary
  2. `bmcu370-firmware_<version>_<platform>-<arch>.elf` - Debug symbols
  3. `DEV-BUILD-INFO_<version>.txt` - Development build metadata
  4. `CHANGELOG-<version>.md` - Development changelog
  5. `SHA256SUMS_<version>.txt` - File checksums

## Benefits

1. **Easier downloads**: Users can download only the specific files they need without extracting ZIP archives
2. **Faster access**: No need to download and extract large ZIP files for single file access
3. **Better organization**: Each artifact has a descriptive name indicating its purpose
4. **Bandwidth savings**: Users downloading only the firmware binary save bandwidth by not downloading debug files

## Technical Details

- Set `compression-level: 0` to avoid double compression of already compressed binary files
- Maintained existing retention policies (90 days for releases, 30 days for dev builds)
- Preserved all existing file naming conventions and metadata
- No changes to the actual build process or generated files

## Compatibility

- The GitHub Release creation process remains unchanged for tagged releases
- All existing file names and content are preserved
- No breaking changes to the CI/CD pipeline