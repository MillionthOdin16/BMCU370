# CI/CD and Build System

This repository uses a comprehensive CI/CD system that automatically builds firmware on every commit and pull request, with enhanced caching and changelog generation.

## Build Workflows

### Dev Build Workflow (`dev-build.yml`)

**Triggers on:**
- Every push to any branch (when source files change)
- Pull request creation and updates
- Manual dispatch

**Features:**
- ⚡ **Fast builds** with comprehensive caching strategy
- 📝 **Automatic changelog generation** since last build
- 🔄 **Artifact management** with 30-day retention
- 💬 **PR comments** with build information and changelog
- 🏷️ **Smart versioning** for dev builds and PR builds

**Caching Strategy:**
- PlatformIO packages and platforms cache
- Build directory cache for incremental builds
- Platform tools cache

**Artifacts Generated:**
- `bmcu370-firmware_dev-YYYYMMDD-HHMMSS_ch32v203c8t6-riscv32.bin`
- `bmcu370-firmware_dev-YYYYMMDD-HHMMSS_ch32v203c8t6-riscv32.elf`
- `DEV-BUILD-INFO.txt` - Development build information
- `CHANGELOG-dev-YYYYMMDD-HHMMSS.md` - Changes since last build
- `SHA256SUMS.txt` - Checksums for verification

### Release Build Workflow (`build-firmware.yml`)

**Triggers on:**
- Version tags (e.g., `v1.0.0`, `v2.1.3`)
- Manual dispatch

**Enhanced Features:**
- 🚀 **Comprehensive caching** for faster release builds
- 📋 **Enhanced changelog generation** between releases
- 🏷️ **GitHub release creation** with detailed information
- 📦 **Production-ready artifacts** with complete metadata

**Artifacts Generated:**
- `bmcu370-firmware_vX.Y.Z_ch32v203c8t6-riscv32.bin`
- `bmcu370-firmware_vX.Y.Z_ch32v203c8t6-riscv32.elf`
- `BUILD-INFO.txt` - Complete build information
- `RELEASE-CHANGELOG-vX.Y.Z.md` - Detailed release changelog
- `SHA256SUMS.txt` - Checksums for verification

## Changelog Generation

Both workflows automatically generate detailed changelogs that include:

- **Commit history** since the last build/release
- **File changes** with status indicators (Added, Modified, Deleted, Renamed)
- **Build metadata** (version, commit hash, build date)
- **Compatibility information**

### Changelog Format

```markdown
# Changes since [last reference]

**Build:** dev-20240906-123456 • a1b2c3d4 • 2024-09-06_12-34-56 UTC
**Type:** push/pull_request

## Commits (N)
- commit message 1
- commit message 2

## File Changes
- **Added:** new-file.cpp
- **Modified:** existing-file.h
- **Deleted:** old-file.txt
```

## Caching Strategy

### PlatformIO Dependencies
- **Cache key:** OS + platformio.ini hash
- **Cached paths:**
  - `~/.platformio/.cache`
  - `~/.platformio/packages`
  - `~/.platformio/platforms`

### Build Artifacts
- **Cache key:** OS + build + commit SHA
- **Cached paths:**
  - `.pio/build`

### Benefits
- 🚀 **50-80% faster builds** on cache hits
- 📦 **Reduced bandwidth** usage
- 🔄 **Incremental compilation** support

## Development Workflow

### For Feature Development
1. Create a feature branch
2. Make changes to source files
3. Push commits - **Dev build automatically triggers**
4. Check build status and artifacts in GitHub Actions
5. Create pull request - **PR build automatically triggers**
6. Review build information in PR comments

### For Releases
1. Update version numbers and CHANGELOG.md
2. Create and push a version tag: `git tag -a v1.2.3 -m "Version 1.2.3"`
3. **Release build automatically triggers**
4. GitHub release is automatically created with artifacts

## Build Information

Each build includes comprehensive metadata:

### Development Builds
- Build type (push/pull_request)
- Commit hash and timestamp
- Change summary since last build
- Development build warnings

### Release Builds
- Version information
- Complete feature list
- Hardware compatibility
- Installation instructions

## Artifact Security

All artifacts include:
- **SHA256 checksums** for integrity verification
- **Build provenance** information
- **Digital signatures** via GitHub's attestation system

## Troubleshooting

### Cache Issues
If builds fail due to cache corruption:
1. Re-run the workflow (often resolves transient issues)
2. Clear cache by updating `platformio.ini`
3. Use manual dispatch with cache reset option

### Missing Artifacts
Dev build artifacts are retained for 30 days, release artifacts indefinitely.

### Build Failures
Check the build logs for:
- Compilation errors in source files
- PlatformIO configuration issues
- Cache corruption (re-run usually fixes)

## Best Practices

### Commit Messages
Use clear, descriptive commit messages for better changelogs:
```
Add motor control calibration feature
Fix RGB LED flickering issue  
Update BambuBus protocol to v5.1
```

### Branch Protection
Consider enabling branch protection rules:
- Require dev build to pass before merging
- Require PR reviews
- Automatically delete head branches

### Release Management
- Use semantic versioning (`vMAJOR.MINOR.PATCH`)
- Update CHANGELOG.md before creating tags
- Test release candidates with dev builds first