#!/usr/bin/env python3

"""
Script to update UI files from export folder
This script removes old UI files and replaces them with the new export structure
"""

import os
import shutil
import glob
import re
from pathlib import Path

def main():
    # Get the script directory and change to it
    script_dir = Path(__file__).parent.absolute()
    os.chdir(script_dir)
    
    print("Starting UI update process...")
    
    # List of files to keep (never delete these) - matches shell version exactly
    keep_files = {
        "vitmitatronics.ino",
        "LovyanGFX_Driver.h", 
        "pins_config.h",
        "export",
        "ui_events.cpp",
        "update_ui.sh",
        "update_ui.py"  # Added to preserve this script
    }
    
    def should_keep_file(file_path):
        """Check if a file should be kept"""
        return file_path.name in keep_files
    
    # Remove old UI files (but keep the files we want to preserve)
    print("Removing old UI files...")
    for file_path in script_dir.iterdir():
        if file_path.is_file() and not should_keep_file(file_path):
            print(f"  Removing: {file_path.name}")
            file_path.unlink()
    
    # Copy new files from export folder
    print("Copying new UI files from export folder...")
    
    export_dir = script_dir / "export"
    
    if not export_dir.exists():
        print("Error: export folder not found!")
        return
    
    # Copy main UI files
    main_files = ["ui.h", "ui.c", "ui_helpers.h", "ui_helpers.c", "ui_events.h"]
    for file_name in main_files:
        src = export_dir / file_name
        if src.exists():
            shutil.copy2(src, script_dir / file_name)
            print(f"  Copied: {file_name}")
        else:
            print(f"  Warning: {file_name} not found in export folder")
    
    # Copy all screen files to main directory
    screens_dir = export_dir / "screens"
    if screens_dir.exists():
        for file_path in screens_dir.iterdir():
            if file_path.is_file():
                shutil.copy2(file_path, script_dir / file_path.name)
                print(f"  Copied screen file: {file_path.name}")
    else:
        print("  Warning: screens directory not found")
    
    # Copy all font files to main directory
    fonts_dir = export_dir / "fonts"
    if fonts_dir.exists():
        for file_path in fonts_dir.iterdir():
            if file_path.is_file():
                shutil.copy2(file_path, script_dir / file_path.name)
                print(f"  Copied font file: {file_path.name}")
    else:
        print("  Warning: fonts directory not found")
    
    # Copy all image files to main directory
    images_dir = export_dir / "images"
    if images_dir.exists():
        for file_path in images_dir.iterdir():
            if file_path.is_file():
                shutil.copy2(file_path, script_dir / file_path.name)
                print(f"  Copied image file: {file_path.name}")
    else:
        print("  Warning: images directory not found")
    
    # Copy all component files to main directory
    components_dir = export_dir / "components"
    if components_dir.exists():
        for file_path in components_dir.iterdir():
            if file_path.is_file():
                shutil.copy2(file_path, script_dir / file_path.name)
                print(f"  Copied component file: {file_path.name}")
    else:
        print("  Warning: components directory not found")
    
    print("Updating include paths in vitmitatronics.ino...")
    
    # Update the include path in the main Arduino file
    ino_file = script_dir / "vitmitatronics.ino"
    if ino_file.exists():
        update_file_content(ino_file, r'#include "ui\.h"', '#include "ui.h"')
    
    print("Fixing lvgl include path in ui.h...")
    
    # Fix the lvgl include path (remove the lvgl/ prefix)
    ui_h_file = script_dir / "ui.h"
    if ui_h_file.exists():
        update_file_content(ui_h_file, r'#include "lvgl/lvgl\.h"', '#include "lvgl.h"')
    
    print("Fixing include paths in ui.h to remove screens/ prefix...")
    
    # Fix the screen include paths in ui.h (remove screens/ prefix)
    if ui_h_file.exists():
        update_file_content(ui_h_file, r'#include "screens/([^"]*)"', r'#include "\1"')
    
    print("Fixing include paths in screen files...")
    
    # Fix the relative include paths in screen files (change ../ui.h to ui.h)
    ui_files = list(script_dir.glob("ui_*.c")) + list(script_dir.glob("ui_*.h"))
    for file_path in ui_files:
        update_file_content(file_path, r'#include "\.\./ui\.h"', '#include "ui.h"')
    
    print("UI update completed successfully!")
    print()
    print("All UI files have been copied to the main directory:")
    print("- Main files: ui.h, ui.c, ui_helpers.h, ui_helpers.c, ui_events.h")
    print("- Screen files: ui_*.h, ui_*.c")
    print("- Font files: ui_font_*.c")
    print("- Image files: ui_img_*.c")
    print("- Component files: ui_comp_*.c")
    print()
    print("Note: The include path in vitmitatronics.ino has been verified.")

def update_file_content(file_path, pattern, replacement):
    """Update file content using regex pattern replacement"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        updated_content = re.sub(pattern, replacement, content)
        
        if content != updated_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(updated_content)
            print(f"  Updated: {file_path.name}")
    except Exception as e:
        print(f"  Warning: Could not update {file_path.name}: {e}")

if __name__ == "__main__":
    main()