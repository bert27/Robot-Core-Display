
from PIL import Image
import os
import sys

# Define paths
BASE_DIR = "display/src/ui/assets"
DRINKS_DIR = os.path.join(BASE_DIR, "drinks")
FILES_TO_CONVERT = [
    # (Source Path, Base Array Name, Struct Variable Name)
    (os.path.join(DRINKS_DIR, "cocacola.png"), "img_cocacola", "img_cocacola"),
    (os.path.join(DRINKS_DIR, "gintonic.png"), "img_gintonic", "img_gintonic"),
    (os.path.join(DRINKS_DIR, "pornstar martini.png"), "img_pornstar_martini", "img_pornstar_martini"),
    (os.path.join(DRINKS_DIR, "ron.png"), "img_ron", "img_ron"),
    (os.path.join(DRINKS_DIR, "sex on the beach.png"), "img_sex_on_the_beach", "img_sex_on_the_beach"),
    (os.path.join(DRINKS_DIR, "vodka.png"), "img_vodka", "img_vodka"),
    (os.path.join(BASE_DIR, "config.png"), "img_config", "img_config"),
]

def generate_c_array(image_path, array_name, struct_name):
    try:
        # Open image and convert to RGBA (32-bit: Red, Green, Blue, Alpha)
        img = Image.open(image_path).convert("RGBA")
        width, height = img.size
        print(f"Processing {image_path} ({width}x{height})...")

        # Get raw data
        # LVGL ARGB8888 usually expects: Blue, Green, Red, Alpha (Little Endian of uint32_t 0xAARRGGBB)
        # Note: LVGL v9 LV_COLOR_FORMAT_ARGB8888 has a specific layout.
        # On Little Endian systems (ESP32), a color 0xAARRGGBB is stored in memory as: BB, GG, RR, AA.
        # PIL 'RGBA' exports bytes: R, G, B, A.
        # We need to reorder to: B, G, R, A to match what DMA/LVGL expects if we use direct 32-bit format.
        
        # We assume standard memory layout: B, G, R, A (BGRA).
        
        r, g, b, a = img.split()
        # Re-merge as BGRA
        img_bgra = Image.merge("RGBA", (b, g, r, a))
        data = img_bgra.tobytes()
        
        # Generate C file content
        c_lines = []
        c_lines.append(f'#include "lvgl.h"')
        c_lines.append(f'')
        
        # Data Array
        c_lines.append(f'const uint8_t {array_name}_map[] = {{')
        
        # Convert bytes to hex string
        hex_data = [f"0x{b:02x}" for b in data]
        
        # Format in lines of 16 bytes
        for i in range(0, len(hex_data), 16):
            chunk = hex_data[i:i+16]
            c_lines.append("  " + ", ".join(chunk) + ",")
            
        c_lines.append(f'}};')
        c_lines.append(f'')
        
        # Definition of struct lv_img_dsc_t V9 (or V8 compatible via macros)
        # We use LV_COLOR_FORMAT_ARGB8888
        c_lines.append(f'const lv_img_dsc_t {struct_name} = {{')
        c_lines.append(f'  .header.cf = LV_COLOR_FORMAT_ARGB8888,')
        # c_lines.append(f'  .header.always_zero = 0,') # Removed for LVGL v9 compatibility
        c_lines.append(f'  .header.w = {width},')
        c_lines.append(f'  .header.h = {height},')
        c_lines.append(f'  .data_size = {len(data)},')
        c_lines.append(f'  .data = {array_name}_map,')
        c_lines.append(f'}};')
        
        # Save file
        output_file = os.path.join(BASE_DIR, f"{array_name}.c")
        with open(output_file, "w") as f:
            f.write("\n".join(c_lines))
            
        print(f"Saved {output_file} ({len(data)} bytes)")

    except Exception as e:
        print(f"Error processing {image_path}: {e}")
        sys.exit(1)

# Execute conversion
for src, name, var in FILES_TO_CONVERT:
    generate_c_array(src, name, var)

print("All conversions completed successfully.")
