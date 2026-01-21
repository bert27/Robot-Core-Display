# 🤖 Robot-Core Mixology S3 🍹

![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)
## For Cheap ESP32-S3 Displays (AliExpress)

🇺🇸 **[Read in English](README.md)** | 🇪🇸 **[Leer en Español](README_ES.md)**

This project provides a base implementation for **generic 4.3-inch smart displays** (ESP32-S3 IPS 800x480) commonly found on **AliExpress** (Sunton 8048S043 models or clones).
The project is optimized to be uploaded from both the Arduino IDE and VS Code with PlatformIO.

## ✨ Features
- **Display**: 4.3-inch IPS, 800x480 resolution.
- **Controller**: ESP32-S3.
- **Framework**: Arduino.
- **Graphic Interface**: [LVGL v9.1.0](https://lvgl.io/).
- **Graphics Library**: [Arduino_GFX](https://github.com/moononournation/Arduino_GFX).
- **Anti-Flicker Stabilization**: **"Bus Isolation"** strategy (SRAM + Bounce Buffer) to eliminate flickering caused by PSRAM concurrency.
- **Modular UI**: Component-based interface (Pages/Components) using LVGL 9.1.
- **Persistent Configuration**: Settings (pump times) are automatically saved to flash memory (MemoryManager/NVS).
- **ESP-NOW Communication**: Direct wireless communication with the Drinks Machine/Irrigation system. Sends drink selections via broadcast.
- **Smart Channel Sync**: Automatically scans for a target WiFi network to synchronize its radio channel with the receiver.

<p align="center">
  <img src="captures/page1-drinks.webp" width="45%" />
  <img src="captures/page2-configure.webp" width="45%" />
</p>

## 🛠️ Getting Started with Arduino IDE

This project is fully compatible with the Arduino IDE. Follow these steps to set it up:

### 1. 📚 Library Preparation
Install the following **3 libraries** from the Arduino **Library Manager** (**Tools > Manage Libraries**):

1. **lvgl** (v9.1.0) - Graphics engine.
2. **Arduino_GFX_Library** - Display driver.
3. **GT911** - Touch panel driver.

> [!IMPORTANT]
> If you receive `No such file or directory` errors with any of these names, it is because the corresponding library is missing from the Arduino manager.

### 2. ⚙️ Critical LVGL Configuration
For the interface to work and for LVGL to know how to handle this display, you must copy the `lv_conf.h` file located in the `display/` folder of this project to the Arduino libraries folder, **outside** of the `lvgl` folder.

- **On macOS**: `/Users/your_user/Documents/Arduino/libraries/lv_conf.h`
- **On Windows**: `C:\Users\your_user\Documents\Arduino\libraries\lv_conf.h`

> [!CAUTION]
> **Do NOT** place the file inside the `lvgl` folder. It must remain in the `libraries` folder at the same level as the `lvgl` folder. This fixes the `fatal error: ../../lv_conf.h: No such file or directory`.

### 3. 🔌 Board Installation and Selection
If you don't have the ESP32 boards installed in your Arduino IDE:
1. Go to **File > Preferences**.
2. In **Additional Boards Manager URLs**, paste this URL:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Go to **Tools > Board > Boards Manager**, search for **esp32** and install the latest version.

### 4. 🎛️ Board Settings
In the **Tools** menu, select the **ESP32S3 Dev Module** board and apply these confirmed settings:
- **USB CDC On Boot**: Enabled
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: 16M Flash (3MB APP/9.9MB FATFS)
- **PSRAM**: "OPI PSRAM"
- **Flash Mode**: **"DIO"** (more stable for uploading than QIO on some systems)
- **Upload Speed**: **921600** (recommended) or 115200 (if there are errors). You can use any available speed.

### 5. 📤 Uploading the Project
1. Open the `display/display.ino` file.
2. Click **Upload**. (With **DIO** mode configured, the upload should be automatic without needing to touch buttons).
3. **Help Note**: Only if the IDE gets stuck on `Connecting...`, hold down the **BOOT** button for a second to force programming mode.

## 💻 Alternative Usage with PlatformIO

If you prefer using PlatformIO, the project is ready to go:
1. Open the root folder in VS Code.
2. PlatformIO will automatically download dependencies based on the `platformio.ini` file.
3. Build and upload directly from the bottom bar.

## 🎨 Interface Design

### 3. UI Customization
Now the user interface is managed manually in C++ (native LVGL). **We use our own component library** to ensure cleanliness and compatibility.

The UI structure is located in `src/ui/`:
*   `ui.cpp / .h`: Entry point and screen definition.
*   `assets/`: Icons and resources (currently `icons.h` with vector symbols).
*   `components/`: Reusable elements:
    *   `MyButton`: Buttons with icon support and customization.
    *   `MySlider`: Sliders.
    *   `MyCard`: Interactive cards (Image on top + Text below).
    *   `MyIcon`: Icon wrapper.

To add new elements, edit `create_screen1()` or `create_screen2()` directly in `ui.cpp` using these components.


## 🛡️ Display Stabilization (Flicker Solution)

This project implements a **Bus Isolation** architecture designed specifically for the ESP32-S3 with high-resolution RGB panels (800x480). This configuration eliminates flickering that occurs when WiFi or other tasks compete for PSRAM access.

### Advantages of Super Buffer / Bus Isolation:
- **LVGL Buffers in SRAM**: LVGL drawing buffers are hosted in **Internal RAM (SRAM)**. This allows redrawing the interface at maximum speed without relying on the PSRAM bus.
- **Bounce Buffer**: An intermediate buffer of 40 lines in SRAM is used as a "shield" for the LCD hardware, ensuring a constant signal even during intense network processes (WiFi/OTA).
- **Golden Timings (8/4/43)**: Optimized timings for Sunton hardware revisions, ensuring the panel does not lose signal lock.


## 📡 Remote Communication (ESP-NOW)

The display communicates with the **Drinks Machine** (or any compatible ESP-NOW receiver) to send selection commands.

### 1. 🔑 Configuration
Since ESP-NOW requires both devices to be on the same WiFi channel, the display scans for an existing network to synchronize itself.
1. Create a `src/core/secrets.h` file (or rename `secrets_example.h`).
2. Define the SSID of the network your receiver is connected to:
   ```cpp
   #define TARGET_WIFI_SSID "Your_WiFi_Network"
   ```

### 2. 📊 Protocol
The communication uses a shared structure defined in `src/core/remote_protocol.h`. If you modify the protocol, ensure it is binary compatible on both the sender (Display) and the receiver.


## 🛠️ Scripts and Tools

In the `scripts/` folder you will find Python tools to facilitate development and graphic asset management:

### 1. `update_project_assets.py` (Project Specific)
Automatically updates all project assets (`src/ui/assets/*.c`) based on the original PNGs located in `src/ui/assets/drinks/` and `config.png`.
- **Usage**: Run from the project root: `python3 scripts/update_project_assets.py`
- **Function**: Converts PNGs to **ARGB8888** (32-bit bitmaps) arrays optimized for instant rendering, eliminating real-time decoding lag.

### 2. `png2lvgl.py` (Generic)
General-purpose tool to convert *any* PNG image to an LVGL v9 compatible C source file.
- **Usage**: `python3 scripts/png2lvgl.py <image.png> [-o output.c] [-n variable_name]`
- **Example**: `python3 scripts/png2lvgl.py logo.png` will generate `logo.c` ready to include in your project with the `lv_img_dsc_t` structure.

---
*This project aims to simplify the use of these Sunton displays in the Arduino environment, centralizing the necessary configuration for the ESP32-S3 hardware.*
