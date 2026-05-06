---
project_name: cyd-display-car1
author: Your Name
version: 0.1.0
description: |
  ESP32 Car Dashboard mit CYD (Cheap Yellow Display) - Ein interaktives Auto-Dashboard mit Touch-Display.

## Project Overview
ESP32 Car Dashboard mit CYD (Cheap Yellow Display) - Ein interaktives Auto-Dashboard mit Touch-Display.

## Features
- **LVGL GUI**: Moderne grafische Oberfläche mit Touch-Steuerung
- **Car Dashboard**: Echtzeit-Anzeige von Geschwindigkeit, Batterie und Status
- **Touch Steuerung**: 4 Steuer-Buttons für Geschwindigkeit und Batterie
- **Farbige Anzeigen**: Dynamische Farbgebung basierend auf Batteriestand
- **Simulation**: Echtzeit-Simulation von Auto-Funktionen

## Hardware Configuration
- Board: ESP32 (CYD Modul)
- Display: 2.8" ILI9341 TFT (320x240 Pixel, 16-bit)
- Touch: FT6236 kapazitiver Touch Controller
- SPI: Display-Ansteuerung (40MHz)
- I2C: Touch-Controller (400kHz)

### Pin-Konfiguration (CYD-spezifisch):
```
Display SPI:
  MOSI: GPIO13
  MISO: GPIO12
  CLK:  GPIO14
  CS:   GPIO15
  DC:   GPIO16 (Data/Command)
  RST:  GPIO17 (Reset)
  BLK:  GPIO21 (Backlight)

Touch I2C:
  SDA: GPIO6
  SCL: GPIO5
```

## Dashboard Funktionen
- **Geschwindigkeit**: 0-200 km/h in 10 km/h Schritten
- **Batterie**: 0-100% mit farblicher Warnung
  - >50%: Grün
  - 20-50%: Gelb
  - <20%: Rot
- **Status**: Echtzeit-Statusmeldungen
- **Steuerung**: Touch-Buttons für Bedienung

## Software-Architektur
- **Framework**: ESP-IDF Native (nicht Arduino)
- **GUI**: LVGL v8.3.0
- **Display Driver**: ILI9341 SPI
- **Touch Driver**: FT6236 I2C
- **RTOS**: FreeRTOS mit dedizierten Tasks

## Development Notes
- Created: 2026-05-05
- Template Version: 1.0
- ESP-IDF Version: Latest stable
- LVGL Version: 8.3.0

## Usage
1. Projekt kompilieren und flashen
2. Display zeigt "CYD Car Dashboard"
3. Touch-Buttons bedienen:
   - Speed +: Geschwindigkeit erhöhen
   - Speed -: Geschwindigkeit verringern
   - Charge: Batterie aufladen
   - Reset: Alles zurücksetzen

## Future Enhancements
- [ ] WiFi Integration für Fernsteuerung
- [ ] Sensor-Anbindung (GPS, Temperatur)
- [ ] Sound-Effekte
- [ ] Animationen und Grafiken
- [ ] Multi-Sprach-Unterstützung
- [ ] Daten-Logging

target_board: esp32  # Options: esp32, esp32s2, esp32s3, esp32c3, esp32c6
target_version: ${ESP_IDF_VERSION}

components:
  # List any external components you depend on
  # Example:
  # - name: json_parser
  #   version: "^1.0.0"

security:
  nvs_encryption: false      # Enable NVS encryption for sensitive data
  secure_boot: false         # Enable Secure Boot (requires keys)
  flash_encryption: false    # Enable flash encryption

hardware:
  description: "Describe your hardware here"
  external_peripherals:
    - "Example: 16x2 LCD display on I2C"
    - "Example: DHT22 temperature sensor on GPIO 4"

notes: |
  Add any additional notes, setup instructions, or troubleshooting tips here.
