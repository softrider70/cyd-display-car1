#ifndef CONFIG_H
#define CONFIG_H

#include "sdkconfig.h"

// ============================================================================
// GPIO Pin Configuration
// ============================================================================
// CYD (Cheap Yellow Display) specific pins
#define GPIO_LED        2   // Built-in LED
#define GPIO_BUTTON     0   // BOOT button

// CYD Display SPI pins (ESP32 - Verifizierte Pin-Belegung)
#define LCD_MOSI        13  // SPI MOSI
#define LCD_MISO        12  // SPI MISO
#define LCD_CLK         14  // SPI CLK
#define LCD_CS          15  // SPI CS
#define LCD_DC          2   // Data/Command (GPIO2)
#define LCD_RST         -1  // Reset (Connected to ESP32 RST)
#define LCD_BLK         21  // Backlight

// CYD Touch I2C pins
#define TOUCH_SDA       6   // I2C SDA
#define TOUCH_SCL       5   // I2C SCL

// ============================================================================
// FreeRTOS Configuration
// ============================================================================
// Get these values from sdkconfig.defaults
// CONFIG_APP_STACK_SIZE = Task stack size (bytes)
// CONFIG_APP_PRIORITY   = Task priority (0-24, higher = more important)
// CONFIG_APP_CORE       = Core affinity (0, 1, or tskNO_AFFINITY)

// ============================================================================
// NVS Configuration
// ============================================================================
#define NVS_NAMESPACE "cyd-display-car1"
#define NVS_STORE_NAME "config"

// ============================================================================
// Application Defaults
// ============================================================================
#define APP_VERSION "1.0.0"
#define APP_LOGLEVEL CONFIG_APP_LOGLEVEL  // From sdkconfig

// ============================================================================
// Display Configuration
// ============================================================================
#define LCD_WIDTH       240  // ILI9341 Portrait
#define LCD_HEIGHT      320
#define LCD_PIXEL_FORMAT LV_COLOR_FORMAT_16
#define LVGL_BUFFER_HEIGHT  40

// ILI9341 MADCTL (Memory Access Control) für korrekte Farben
#define ILI9341_MADCTL  0x40  // Portrait + RGB Mode

// ============================================================================
// LVGL Configuration
// ============================================================================
#define LVGL_TICK_PERIOD_MS    5
#define LVGL_TASK_STACK_SIZE   4096
#define LVGL_TASK_PRIORITY     2

// ============================================================================
// Security Configuration (optional)
// ============================================================================
// TLS: Configure your certificates here
// #define USE_TLS_CERTIFICATE 1
// #define TLS_CERT_FILE "certificates/ca-cert.pem"

// ============================================================================
// Logging Configuration
// ============================================================================
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#endif // CONFIG_H
