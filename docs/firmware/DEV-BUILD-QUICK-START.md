# Quick Reference: Dev Build System

## 🚀 What's New

Every commit and pull request now automatically triggers a **Dev Build** with:
- ⚡ **Fast builds** (50-80% faster with caching)
- 📝 **Automatic changelog** since last build
- 💬 **PR comments** with build info
- 📦 **Build artifacts** available for download

## 🔄 How It Works

### On Every Push:
1. **Dev build triggers** automatically
2. **Changelog generated** since last tag/build
3. **Artifacts uploaded** to GitHub Actions
4. **Build completes** in ~2-5 minutes (vs 5-10 minutes without cache)

### On Pull Requests:
1. **PR build triggers** with special versioning
2. **Comment posted** with build status and changelog
3. **Artifacts available** for testing
4. **Changelog shows** exactly what changed

### On Version Tags:
1. **Release build triggers** (existing workflow, now faster)
2. **GitHub release created** automatically
3. **Enhanced changelog** between releases
4. **Production artifacts** with full metadata

## 📝 Changelog Features

**Automatically includes:**
- List of commits since last build
- File changes (Added/Modified/Deleted)
- Build metadata (version, commit, date)
- Links to artifacts

**Example changelog:**
```
# Changes since v2.0.0

**Build:** dev-20240906-123456 • a1b2c3d4 • 2024-09-06_12-34-56 UTC
**Type:** push

## Commits (3)
- Add motor calibration feature
- Fix LED flickering issue  
- Update protocol documentation

## File Changes
- **Modified:** src/Motion_control.cpp
- **Modified:** src/Adafruit_NeoPixel.cpp
- **Added:** docs/calibration.md
```

## 📦 Artifacts Generated

### Dev Builds (30-day retention):
- `bmcu370-firmware_dev-YYYYMMDD-HHMMSS_ch32v203c8t6-riscv32.bin`
- `bmcu370-firmware_dev-YYYYMMDD-HHMMSS_ch32v203c8t6-riscv32.elf`
- `DEV-BUILD-INFO.txt` - Development build information  
- `CHANGELOG-dev-YYYYMMDD-HHMMSS.md` - Changes since last build
- `SHA256SUMS.txt` - Checksums for verification

### Release Builds (permanent retention):
- `bmcu370-firmware_vX.Y.Z_ch32v203c8t6-riscv32.bin`
- `bmcu370-firmware_vX.Y.Z_ch32v203c8t6-riscv32.elf`
- `BUILD-INFO.txt` - Complete build information
- `RELEASE-CHANGELOG-vX.Y.Z.md` - Detailed release changelog
- `SHA256SUMS.txt` - Checksums for verification

## 🛠️ Usage Examples

### Development Workflow:
```bash
# Make changes to source files
git add src/main.cpp
git commit -m "Add new feature"
git push origin feature-branch

# Dev build automatically triggers
# Check GitHub Actions for build status
# Download artifacts from Actions page
```

### Pull Request Workflow:
```bash
# Create PR from feature branch
# Dev build triggers automatically  
# Bot comments on PR with build info
# Review changelog in PR comment
# Download artifacts for testing
```

### Release Workflow:
```bash
# Update CHANGELOG.md
git add CHANGELOG.md
git commit -m "Prepare v2.1.0 release"
git tag -a v2.1.0 -m "Version 2.1.0"
git push origin v2.1.0

# Release build triggers automatically
# GitHub release created with artifacts
# Enhanced changelog included in release
```

## 📍 Where to Find Things

- **Build Status**: GitHub Actions tab
- **Dev Artifacts**: Actions > Workflow Run > Artifacts section  
- **Release Artifacts**: Releases page + GitHub release
- **Build Logs**: Actions > Workflow Run > Job logs
- **PR Build Info**: PR comments (automatic)

## 🔧 Troubleshooting

### Build Failed?
1. Check build logs in GitHub Actions
2. Common issues: syntax errors, missing dependencies
3. Re-run workflow if transient failure

### Missing Artifacts?
- Dev builds: 30-day retention
- Release builds: Permanent retention
- Check if build completed successfully

### Cache Issues?
- Usually self-resolving on re-run
- Clear cache by modifying `platformio.ini`

## 💡 Tips

1. **Use descriptive commit messages** for better changelogs
2. **Check PR comments** for build status before merging
3. **Verify checksums** before flashing firmware
4. **Test dev builds** before creating releases
5. **Use semantic versioning** for releases (v1.2.3)

---

For complete documentation, see [`docs/CI-CD.md`](docs/CI-CD.md)