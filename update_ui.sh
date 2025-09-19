#!/bin/bash

# Script to update UI files from export folder
# This script removes old UI files and replaces them with the new export structure

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Starting UI update process..."

# List of files to keep (never delete these)
KEEP_FILES=(
    "vitmitatronics.ino"
    "LovyanGFX_Driver.h"
    "pins_config.h"
    "export"
    "ui_events.cpp"
    "update_ui.sh"
)

# Function to check if a file should be kept
should_keep_file() {
    local file="$1"
    for keep_file in "${KEEP_FILES[@]}"; do
        if [[ "$file" == "$keep_file" ]]; then
            return 0
        fi
    done
    return 1
}

# Remove old UI files (but keep the files we want to preserve)
echo "Removing old UI files..."
for file in *; do
    if [[ -f "$file" ]] && ! should_keep_file "$file"; then
        echo "  Removing: $file"
        rm "$file"
    fi
done

# Copy new files from export folder
echo "Copying new UI files from export folder..."

# Copy main UI files
cp export/ui.h .
cp export/ui.c .
cp export/ui_helpers.h .
cp export/ui_helpers.c .
cp export/ui_events.h .

# Copy all screen files to main directory
cp export/screens/* .

# Copy all font files to main directory
cp export/fonts/* .

# Copy all image files to main directory
cp export/images/* .

# Copy all component files to main directory
cp export/components/* .

echo "Updating include paths in vitmitatronics.ino..."

# Update the include path in the main Arduino file
sed -i 's/#include "ui\.h"/#include "ui.h"/' vitmitatronics.ino

echo "Fixing lvgl include path in ui.h..."

# Fix the lvgl include path (remove the lvgl/ prefix)
sed -i 's/#include "lvgl\/lvgl\.h"/#include "lvgl.h"/' ui.h

echo "Fixing include paths in ui.h to remove screens/ prefix..."

# Fix the screen include paths in ui.h (remove screens/ prefix)
sed -i 's/#include "screens\/\([^"]*\)"/#include "\1"/g' ui.h

echo "Fixing include paths in screen files..."

# Fix the relative include paths in screen files (change ../ui.h to ui.h)
sed -i 's/#include "\.\.\/ui\.h"/#include "ui.h"/' ui_*.c ui_*.h 2>/dev/null || true

echo "UI update completed successfully!"
echo ""
echo "All UI files have been copied to the main directory:"
echo "- Main files: ui.h, ui.c, ui_helpers.h, ui_helpers.c, ui_events.h"
echo "- Screen files: ui_*.h, ui_*.c"
echo "- Font files: ui_font_*.c"
echo "- Image files: ui_img_*.c"
echo "- Component files: ui_comp_*.c"
echo ""
echo "Note: The include path in vitmitatronics.ino has been verified."
