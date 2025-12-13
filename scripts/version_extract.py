#!/usr/bin/env python3
"""
Version extraction script for BMCU370 firmware builds

This script extracts version information from git tags and environment variables
to potentially update firmware version constants during build.
"""

import os
import subprocess
import re
from pathlib import Path

def get_git_version():
    """Get version from git tag or commit"""
    try:
        # Try to get the latest tag
        result = subprocess.run(['git', 'describe', '--tags', '--abbrev=0'], 
                              capture_output=True, text=True, check=True)
        tag = result.stdout.strip()
        
        # Parse semantic version (v1.2.3 -> 1.2.3)
        version_match = re.match(r'v?(\d+)\.(\d+)\.(\d+)', tag)
        if version_match:
            major, minor, patch = version_match.groups()
            return {
                'major': int(major),
                'minor': int(minor), 
                'patch': int(patch),
                'build': 0,  # Default build number
                'tag': tag
            }
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass
    
    try:
        # Fall back to commit count as build version
        result = subprocess.run(['git', 'rev-list', '--count', 'HEAD'], 
                              capture_output=True, text=True, check=True)
        build = int(result.stdout.strip())
        
        return {
            'major': 0,
            'minor': 0,
            'patch': 0,
            'build': build,
            'tag': f'dev-{build}'
        }
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass
    
    # Ultimate fallback
    return {
        'major': 0,
        'minor': 0, 
        'patch': 0,
        'build': 0,
        'tag': 'unknown'
    }

def get_build_info():
    """Get comprehensive build information"""
    version = get_git_version()
    
    # Get current firmware versions from config.h
    config_path = Path('src/config.h')
    ams_version = {'major': 0, 'minor': 0, 'patch': 6, 'build': 49}
    ams_lite_version = {'major': 0, 'minor': 1, 'patch': 2, 'build': 3}
    
    if config_path.exists():
        with open(config_path, 'r') as f:
            content = f.read()
            
        # Extract AMS versions
        ams_patterns = {
            'major': r'#define\s+AMS_FIRMWARE_VERSION_MAJOR\s+0x([0-9A-Fa-f]+)',
            'minor': r'#define\s+AMS_FIRMWARE_VERSION_MINOR\s+0x([0-9A-Fa-f]+)', 
            'patch': r'#define\s+AMS_FIRMWARE_VERSION_PATCH\s+0x([0-9A-Fa-f]+)',
            'build': r'#define\s+AMS_FIRMWARE_VERSION_BUILD\s+0x([0-9A-Fa-f]+)'
        }
        
        for key, pattern in ams_patterns.items():
            match = re.search(pattern, content)
            if match:
                ams_version[key] = int(match.group(1), 16)
        
        # Extract AMS Lite versions  
        ams_lite_patterns = {
            'major': r'#define\s+AMS_LITE_FIRMWARE_VERSION_MAJOR\s+0x([0-9A-Fa-f]+)',
            'minor': r'#define\s+AMS_LITE_FIRMWARE_VERSION_MINOR\s+0x([0-9A-Fa-f]+)',
            'patch': r'#define\s+AMS_LITE_FIRMWARE_VERSION_PATCH\s+0x([0-9A-Fa-f]+)',  
            'build': r'#define\s+AMS_LITE_FIRMWARE_VERSION_BUILD\s+0x([0-9A-Fa-f]+)'
        }
        
        for key, pattern in ams_lite_patterns.items():
            match = re.search(pattern, content)
            if match:
                ams_lite_version[key] = int(match.group(1), 16)
    
    return {
        'git_version': version,
        'ams_version': ams_version,
        'ams_lite_version': ams_lite_version,
        'build_date': subprocess.run(['date', '-u'], capture_output=True, text=True).stdout.strip()
    }

def format_version(version_dict):
    """Format version dict as string"""
    return f"{version_dict['major']:02d}.{version_dict['minor']:02d}.{version_dict['patch']:02d}.{version_dict['build']:02d}"

if __name__ == '__main__':
    import json
    import sys
    
    build_info = get_build_info()
    
    if len(sys.argv) > 1:
        if sys.argv[1] == '--json':
            print(json.dumps(build_info, indent=2))
        elif sys.argv[1] == '--ams-version':
            print(format_version(build_info['ams_version']))
        elif sys.argv[1] == '--ams-lite-version':
            print(format_version(build_info['ams_lite_version']))
        elif sys.argv[1] == '--git-tag':
            print(build_info['git_version']['tag'])
        else:
            print("Usage: version_extract.py [--json|--ams-version|--ams-lite-version|--git-tag]")
            sys.exit(1)
    else:
        print("BMCU370 Build Information")
        print("=" * 40) 
        print(f"Git Version: {build_info['git_version']['tag']}")
        print(f"AMS Firmware Version: {format_version(build_info['ams_version'])}")
        print(f"AMS Lite Firmware Version: {format_version(build_info['ams_lite_version'])}")
        print(f"Build Date: {build_info['build_date']}")