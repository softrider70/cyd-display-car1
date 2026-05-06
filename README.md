# cyd-display-car1 — ESP32 Project

A custom ESP32 project built with native ESP-IDF.

## Overview

ESP32 Car Dashboard mit CYD (Cheap Yellow Display) - Ein interaktives Auto-Dashboard mit Touch-Display und LVGL GUI.

- **Board:** ESP32-2432S028R CYD (Cheap Yellow Display) - Target: esp32 ✅
- **Chip:** ESP32-D0WD-V3 (Revision 301) ✅
- **Display:** 2.8" ILI9341 TFT (320x240 Pixel, 16-bit)
- **Touch:** XPT2046 resistiver Touch Controller ✅
- **GUI:** LVGL v8.3.0 mit Touch-Steuerung
- **Version:** 0.1.0
- **Status:** ✅ Hardware verifiziert, RGB LEDs funktionieren, Build konfiguriert

## Quick Start

### Build Requirements

**ESP-IDF v6.0 Setup (Windows):**
```powershell
# ESP-IDF v6.0 Installation unter C:\esp\v6.0\esp-idf
# Build mit PowerShell Script:
powershell -ExecutionPolicy Bypass -File .\build_cyd.ps1

# Oder manuell:
. C:\esp\v6.0\esp-idf\export.ps1
idf.py build
```

### Build Commands
```bash
/build-project      # Optimized build (ESP-IDF v6.0)
/compile           # Advanced options
idf.py build        # Standard ESP-IDF
./build_cyd.ps1     # PowerShell Build Script
```

### Flash (First Time)
```bash
/initial-upload     # Complete installation
idf.py flash monitor # Manual method
```

### Flash (Iteration)
```bash
/upload-firmware    # Fast app-only (~3 seconds)
/upload            # Smart router (auto-detect)
idf.py app-flash     # Manual app-only
```

### Monitor
```bash
idf.py monitor       # Serial output
idf.py monitor --no-reset # Keep running
```

## Project Structure

```
cyd-display-car1/
├── src/
│   ├── main.c              Main application code
│   ├── lv_port_disp.c      LVGL Display Driver (ILI9341)
│   ├── lv_port_indev.c      LVGL Input Driver (FT6236 Touch)
│   └── CMakeLists.txt      Component build config
├── include/
│   └── config.h            Hardware configuration (pins, settings)
├── CMakeLists.txt          Project build config (ESP-IDF v6.0)
├── build_cyd.ps1           PowerShell Build Script
├── demo_simulation.html    Browser Demo (für Tests)
├── sdkconfig               Build configuration (auto-generated)
├── PROJECT.md              Detailed project specs
└── README.md               This file
```

## Configuration

### Hardware Setup

### ✅ **Verifizierte CYD Pin-Belegung**

**RGB LEDs (Active Low - LOW=AN, HIGH=AUS):**
```
🔴 ROT LED    - GPIO4
🟢 GRÜN LED   - GPIO16
🔵 BLAU LED   - GPIO17
⚪ WEISS      - GPIO4+GPIO16+GPIO17
```

**Display SPI (HSPI):**
```
📺 TFT_DC    - GPIO2
📺 TFT_CS    - GPIO15
📺 TFT_MOSI  - GPIO13
📺 TFT_MISO  - GPIO12
📺 TFT_SCLK  - GPIO14
💡 TFT_BL    - GPIO21 (Backlight)
```

**System:**
```
UART      - GPIO1(TX), GPIO3(RX)
USB       - COM11 (Windows)
Flash     - 2MB (erkannt als 4MB)
```

### **Konfiguration**

Edit `include/config.h` to configure:
- GPIO pin assignments
- UART baudrates
- WiFi/BLE settings
- Display configurations
- Sensor parameters

### Board Selection

The target board is configured in `sdkconfig` and `sdkconfig.defaults.*`

To change boards:
```bash
idf.py set-target esp32s3
idf.py menuconfig
```

## Development Workflow

1. **Edit code** in `src/main.c` or `include/config.h`
2. **Build:** `/build-project` (optimized compilation)
3. **Flash:** `/upload-firmware` (fast iteration ~3 seconds)
4. **Test & Debug**
5. **Commit:** `/commit` (auto-generates commit message)

## Available Skills

```bash
/build-project      Optimized ESP-IDF build with parallel compilation
/upload-firmware    Fast app-only flash (~3 seconds)
/initial-upload     Complete first-time installation
/upload            Smart upload router (auto-detects best method)
/compile           Advanced compilation options and optimization
/add-ota           Add OTA firmware updates
/add-webui         Add web dashboard
/add-library       Manage components
/add-security      Enable Secure Boot/encryption
/setup-ci          GitHub Actions CI/CD
/add-profiling     Performance monitoring
/commit            Git operations
```

## Useful Skills

- **`/build-project`** — Compile firmware
- **`/upload-firmware`** — Fast app update (~3 seconds)
- **`/upload`** — Smart session router
- **`/commit`** — Git commit with auto-message

## Adding Features

Use Copilot skills to extend functionality:

```
/add-ota          Enable OTA firmware updates
/add-webui        Add responsive web dashboard
/add-library      Manage external components
/add-security     Enable Secure Boot, encryption
/setup-ci         GitHub Actions CI/CD
/add-profiling    Performance monitoring
```

## Documentation

- **`PROJECT.md`** — Detailed project specifications
- **`sdkconfig`** — Build configuration (auto-generated)
- **`include/config.h`** — Hardware pin mappings

## Build Configuration (ESP-IDF v6.0)

### Wichtige Anpassungen für Build-Kompatibilität

**1. CMakeLists.txt (Hauptverzeichnis):**
```cmake
cmake_minimum_required(VERSION 3.16)  # WICHTIG: 3.16 statt 3.5
include($ENV{IDF_PATH}/tools/cmake/project.cmake)  # Muss VOR project() stehen
set(EXTRA_COMPONENT_DIRS "src")  # Component-Verzeichnis
project(cyd-display-car1)
```

**2. src/CMakeLists.txt:**
```cmake
idf_component_register(
    SRCS "main.c" "lv_port_disp.c" "lv_port_indev.c"
    INCLUDE_DIRS "../include"
    REQUIRES esp_system nvs_flash freertos esp_wifi esp_netif esp_http_server esp_event lwip esp_driver_gpio mbedtls esp-tls spi_flash
)
```

**3. ESP-IDF v6.0 Kompatibilität:**
- **CMake Version**: 3.16 (erforderlich für ESP-IDF v6.0)
- **Component-Architektur**: Modulares Build-System
- **API-Kompatibilität**: Nur dokumentierte ESP-IDF APIs verwenden
- **Include-Pfade**: Korrekte Header-Verzeichnisse

### Build-Umgebung Setup

**Windows PowerShell:**
```powershell
# ESP-IDF v6.0 Environment laden
. C:\esp\v6.0\esp-idf\export.ps1

# Build ausführen
idf.py build

# Oder mit automatisiertem Script
powershell -ExecutionPolicy Bypass -File .\build_cyd.ps1
```

**Toolchain-Überprüfung:**
```bash
# ESP-IDF Version prüfen
idf.py --version

# Target prüfen
idf.py set-target esp32s3
```

## Troubleshooting

**Build fails:**
```bash
# 1. ESP-IDF Environment prüfen
idf.py --version

# 2. Clean Build
idf.py fullclean
idf.py build

# 3. CMake Konfiguration prüfen
# - cmake_minimum_required(VERSION 3.16)
# - include($ENV{IDF_PATH}/tools/cmake/project.cmake) VOR project()

# 4. Component Dependencies prüfen
# - REQUIRES mit allen benötigten ESP-IDF Komponenten
```

**Compiler nicht gefunden:**
```bash
# ESP-IDF Tools neu installieren
python.exe C:\esp\v6.0\esp-idf\tools\idf_tools.py install

# Environment neu laden
. C:\esp\v6.0\esp-idf\export.ps1
```

**Flash doesn't work:**
- Check USB connection: `idf.py monitor --no-reset`
- Select port manually: `idf.py -p /dev/ttyUSB0 flash`
- ESP32-S3 Target prüfen: `idf.py set-target esp32s3`

**Memory issues:**
- Check heap with `/add-profiling`
- Review `sdkconfig` memory settings
- Use PSRAM if available (ESP32-S3 hat 8MB)

## Touch Implementation Details

### 🔧 **XPT2046 Touch Controller - Vollständig Implementiert**

**Wichtigste Erkenntnis:** Das CYD Board verwendet **ZWEI separate SPI Busse** - nicht einen geteilten!

**Pin-Belegung (verifiziert):**
```
📺 Display SPI (HSPI):   MOSI=13, MISO=12, CLK=14, CS=15
👆 Touch SPI (VSPI):   MOSI=32, MISO=39, CLK=25, CS=33, IRQ=36
```

**SPI Konfiguration:**
```
Display: 26MHz, DMA aktiviert, SPI2_HOST
Touch:   2.5MHz, DMA deaktiviert, SPI3_HOST
```

**XPT2046 Kommandos (12-bit DFR Mode):**
```
X-Position:  0x90 (S=1, A2A1A0=001, MODE=0, SER=0, PD=00)
Y-Position:  0xD0 (S=1, A2A1A0=101, MODE=0, SER=0, PD=00)
Z1 Pressure:  0xB0 (S=1, A2A1A0=011, MODE=0, SER=0, PD=00)
Z2 Pressure:  0xC0 (S=1, A2A1A0=100, MODE=0, SER=0, PD=00)
Power-Down:   0x80 (S=1, PD=00)
```

**Touch-Algorithmus:**
1. IRQ Pin prüfen (GPIO36, LOW = Touch aktiv)
2. Z1 und Z2 lesen für Pressure-Berechnung
3. Wenn Pressure > 50: X und Y lesen
4. Power-Down Kommando senden
5. Rohwerte auf Screen-Koordinaten mappen

**Getestete Rohwerte:**
```
X: 1600-2000 (typisch bei Touch)
Y: 2300-3100 (typisch bei Touch)
Z: 800-2100  (Pressure, höher = stärkerer Druck)
```

**Code-Beispiel (touch_test.c):**
```c
// Zwei separate SPI Busse initialisieren
spi_bus_initialize(LCD_SPI_HOST, &lcd_bus_cfg, SPI_DMA_CH_AUTO);
spi_bus_initialize(TOUCH_SPI_HOST, &touch_bus_cfg, SPI_DMA_DISABLED);

// Touch Device auf separatem Bus registrieren
spi_bus_add_device(TOUCH_SPI_HOST, &touch_cfg, &touch_spi);

// XPT2046 Kanal lesen
static uint16_t touch_read_channel(uint8_t cmd) {
    uint8_t tx[3] = {cmd, 0x00, 0x00};
    uint8_t rx[3] = {0};
    spi_transaction_t t = {.length = 24, .tx_buffer = tx, .rx_buffer = rx};
    spi_device_transmit(touch_spi, &t);
    return ((rx[1] << 8) | rx[2]) >> 3; // 12-bit extrahieren
}
```

## Demo & Testing

### ✅ **Hardware Verifizierung (2026-05-06)**

**Touch System erfolgreich implementiert:**
```
👆 XPT2046 Touch Controller ✅
📡 SEPARATER SPI Bus (nicht shared!) ✅
🔌 Touch Pins: CS=33, IRQ=36, MOSI=32, MISO=39, CLK=25 ✅
🎯 Vollbild Touch-Erkennung ✅
🌈 Farbwechsel bei Touch ✅
📊 Rohwerte: X=1600-2000, Y=2300-3100, Z=800-2100 ✅
🔄 14+ Touch Events getestet ✅
```

**RGB LED Test erfolgreich:**
```
🌈 RGB Farbdemo läuft - Alle Farben funktionieren:
- ROT (GPIO4)    ✅
- GRÜN (GPIO16)  ✅ 
- BLAU (GPIO17)  ✅
- GELB (R+G)    ✅
- CYAN (G+B)    ✅
- MAGENTA (R+B) ✅
- WEISS (R+G+B) ✅
```

**System Status:**
```
🔧 ESP32-D0WD-V3 (Revision 301) ✅
💾 Heap: 301KB stabil ✅
🔄 Watchdog: Stabil ✅
⚡ Flash: 2MB genutzt ✅
📡 UART: 115200 Baud ✅
👆 Touch: XPT2046 voll funktionsfähig ✅
📺 Display: ILI9341 mit Touch-Steuerung ✅
```

### Browser Demo (sofort verfügbar)
```bash
# Demo im Browser öffnen
start demo_simulation.html
```

**Demo Features:**
- 🏎️ **Enhanced Dashboard** mit Gradient Background
- ⚡ **Touch-Steuerung** (Speed, Brake, Charge, Reset)
- 🎭 **Auto-Demo Mode** (automatische Simulation)
- 🔋 **Battery Simulation** mit farblichen Warnungen
- 📱 **Responsive Design** für verschiedene Bildschirme

### Hardware Testing
```bash
# Build und Flash auf ESP32-CYD
powershell -ExecutionPolicy Bypass -File .\build_cyd.ps1
idf.py flash monitor

# Schneller App-Only Flash
idf.py app-flash monitor
```

## Next Steps

1. ✅ **Build konfiguriert** (ESP-IDF v6.0 kompatibel)
2. ✅ **Demo implementiert** (Browser + Hardware)
3. ✅ **Hardware verifiziert** (CYD Board + RGB LEDs)
4. ✅ **Touch System** (XPT2046 + Vollbild Touch)
5. 🔄 **LVGL Dashboard** mit Touch-Interface
6. 📋 **Display Integration** (ILI9341 + XPT2046)
6. 📋 **WiFi Integration** für Fernsteuerung
7. 📋 **Sensor-Anbindung** (GPS, Temperatur)
8. 📋 **OTA Updates** für Firmware

---

**Build Status:** ✅ ESP-IDF v6.0 konfiguriert und kompiliert  
**Hardware Status:** ✅ CYD Board verifiziert, Touch System voll funktionsfähig  
**Demo Status:** ✅ Touch Vollbild-Demo läuft, Dashboard bereit für LVGL

---

Generated from ESP32 Template  
For template docs: https://github.com/softrider70/esp32-template
