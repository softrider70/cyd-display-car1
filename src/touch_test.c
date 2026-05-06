// CYD ESP32-2432S028R - Touch Test (Vollbild Farbwechsel)
// XPT2046 Touch und ILI9341 Display teilen sich den HSPI Bus
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_chip_info.h"

static const char *TAG = "cyd_touch";

// === Pin-Belegung CYD ESP32-2432S028R ===
// Display SPI Bus (HSPI)
#define PIN_MOSI    13   // Display MOSI
#define PIN_MISO    12   // Display MISO
#define PIN_CLK     14   // Display SCLK
#define PIN_CS      15   // Display CS
#define PIN_DC      2    // Display DC
#define PIN_BCKL    21   // Backlight

// Touch SPI Bus (SEPARAT!)
#define TOUCH_MOSI  32   // Touch MOSI
#define TOUCH_MISO  39   // Touch MISO
#define TOUCH_CLK   25   // Touch SCLK
#define TOUCH_CS    33   // Touch CS
#define TOUCH_IRQ   36   // Touch IRQ (input-only)

// SPI Hosts - ZWEI separate Busse
#define LCD_SPI_HOST   SPI2_HOST  // Display Bus
#define TOUCH_SPI_HOST SPI3_HOST  // Touch Bus

// Handles
static spi_device_handle_t lcd_spi = NULL;
static spi_device_handle_t touch_spi = NULL;

// === Display Funktionen ===

static void lcd_cmd(uint8_t cmd) {
    gpio_set_level(PIN_DC, 0);
    spi_transaction_t t = {
        .length = 8,
        .tx_data = {cmd, 0, 0, 0},
        .flags = SPI_TRANS_USE_TXDATA,
    };
    spi_device_transmit(lcd_spi, &t);
}

static void lcd_data(const uint8_t *data, size_t len) {
    if (len == 0) return;
    gpio_set_level(PIN_DC, 1);
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_transmit(lcd_spi, &t);
}

static void lcd_data_byte(uint8_t val) {
    gpio_set_level(PIN_DC, 1);
    spi_transaction_t t = {
        .length = 8,
        .tx_data = {val, 0, 0, 0},
        .flags = SPI_TRANS_USE_TXDATA,
    };
    spi_device_transmit(lcd_spi, &t);
}

static void lcd_init(void) {
    ESP_LOGI(TAG, "ILI9341 Init...");

    lcd_cmd(0x01); // Software Reset
    vTaskDelay(pdMS_TO_TICKS(150));

    lcd_cmd(0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(150));

    lcd_cmd(0x36); // MADCTL
    lcd_data_byte(0x48);

    lcd_cmd(0x3A); // Pixel Format 16bit
    lcd_data_byte(0x55);

    lcd_cmd(0xB1); // Frame Rate
    uint8_t frm[] = {0x00, 0x18};
    lcd_data(frm, 2);

    lcd_cmd(0x29); // Display ON

    ESP_LOGI(TAG, "ILI9341 OK!");
}

// Vollbild mit einer Farbe füllen
static void lcd_fill(uint16_t color) {
    // Set Window 0,0 -> 239,319
    lcd_cmd(0x2A);
    uint8_t ca[] = {0, 0, 0, 239};
    lcd_data(ca, 4);

    lcd_cmd(0x2B);
    uint8_t ra[] = {0, 0, 1, 63}; // 319 = 0x013F
    lcd_data(ra, 4);

    lcd_cmd(0x2C);

    // Farb-Buffer senden (zeilenweise)
    uint16_t line[240];
    for (int i = 0; i < 240; i++) {
        line[i] = color;
    }

    gpio_set_level(PIN_DC, 1);
    for (int row = 0; row < 320; row++) {
        spi_transaction_t t = {
            .length = 240 * 16,
            .tx_buffer = line,
        };
        spi_device_transmit(lcd_spi, &t);
    }
}

// === XPT2046 Touch Funktionen ===

static void touch_init(void) {
    ESP_LOGI(TAG, "XPT2046 Init...");

    // IRQ Pin als Input (GPIO36 ist input-only)
    gpio_config_t irq_conf = {
        .pin_bit_mask = (1ULL << TOUCH_IRQ),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&irq_conf);

    ESP_LOGI(TAG, "XPT2046 OK! (CS=%d, IRQ=%d, Bus=HSPI shared)", TOUCH_CS, TOUCH_IRQ);
}

// XPT2046 Kanal lesen (12-bit Ergebnis)
static uint16_t touch_read_channel(uint8_t cmd) {
    // XPT2046 Kommando: 1 Byte senden, 2 Bytes Antwort lesen
    uint8_t tx[3] = {cmd, 0x00, 0x00};
    uint8_t rx[3] = {0};

    spi_transaction_t t = {
        .length = 24,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };

    esp_err_t ret = spi_device_transmit(touch_spi, &t);
    if (ret != ESP_OK) {
        return 0;
    }

    // 12-bit Wert aus Byte 1+2 extrahieren
    uint16_t val = ((rx[1] << 8) | rx[2]) >> 3;
    return val & 0x0FFF;
}

// Touch lesen - ohne IRQ Check für Hardware-Debugging
static bool touch_read(uint16_t *x, uint16_t *y, uint16_t *z) {
    // Z1 Pressure lesen: S=1, A2A1A0=011, MODE=0, SER=0, PD=00
    // = 1_011_0_0_00 = 0xB0
    uint16_t z1 = touch_read_channel(0xB0);
    // Z2 Pressure: S=1, A2A1A0=100, MODE=0, SER=0, PD=00
    // = 1_100_0_0_00 = 0xC0
    uint16_t z2 = touch_read_channel(0xC0);

    // Pressure berechnen
    int pressure = (z1 > 0) ? ((int)z1 - (int)z2 + 4095) : 0;
    if (pressure < 0) pressure = 0;

    if (pressure < 50) {
        // Power-Down senden: S=1, PD=00
        touch_read_channel(0x80);
        return false;
    }

    // X lesen: S=1, A2A1A0=001, MODE=0, SER=0, PD=00
    // = 1_001_0_0_00 = 0x90
    uint16_t x_raw = touch_read_channel(0x90);
    // Y lesen: S=1, A2A1A0=101, MODE=0, SER=0, PD=00
    // = 1_101_0_0_00 = 0xD0
    uint16_t y_raw = touch_read_channel(0xD0);

    // Power-Down
    touch_read_channel(0x80);

    *x = x_raw;
    *y = y_raw;
    *z = (uint16_t)pressure;

    return true;
}

// === Hauptprogramm ===

// Farben (RGB565)
static const uint16_t colors[] = {
    0xF800, // Rot
    0x07E0, // Grün
    0x001F, // Blau
    0xFFE0, // Gelb
    0xF81F, // Magenta
    0x07FF, // Cyan
    0xFFFF, // Weiß
};
static const char *color_names[] = {
    "Rot", "Gruen", "Blau", "Gelb", "Magenta", "Cyan", "Weiss"
};
#define NUM_COLORS (sizeof(colors) / sizeof(colors[0]))

void app_main(void)
{
    ESP_LOGI(TAG, "=== CYD Touch Test ===");
    ESP_LOGI(TAG, "Board: ESP32-2432S028R");
    ESP_LOGI(TAG, "Touch: XPT2046 auf HSPI (shared mit Display)");
    ESP_LOGI(TAG, "Pins: MOSI=%d, MISO=%d, CLK=%d, LCD_CS=%d, TOUCH_CS=%d, IRQ=%d",
             PIN_MOSI, PIN_MISO, PIN_CLK, PIN_CS, TOUCH_CS, TOUCH_IRQ);

    // GPIO für DC und Backlight
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_DC) | (1ULL << PIN_BCKL),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    gpio_set_level(PIN_BCKL, 1); // Backlight an

    // Display SPI Bus initialisieren
    spi_bus_config_t lcd_bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 240 * 2 + 8,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_SPI_HOST, &lcd_bus_cfg, SPI_DMA_CH_AUTO));

    // Touch SPI Bus initialisieren (SEPARAT)
    spi_bus_config_t touch_bus_cfg = {
        .mosi_io_num = TOUCH_MOSI,
        .miso_io_num = TOUCH_MISO,
        .sclk_io_num = TOUCH_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 8,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(TOUCH_SPI_HOST, &touch_bus_cfg, SPI_DMA_DISABLED));

    // Display SPI Device
    spi_device_interface_config_t lcd_cfg = {
        .clock_speed_hz = 26 * 1000 * 1000, // 26MHz für Display
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(LCD_SPI_HOST, &lcd_cfg, &lcd_spi));

    // Touch SPI Device (SEPARATER BUS!)
    spi_device_interface_config_t touch_cfg = {
        .clock_speed_hz = 2500000, // 2.5MHz für Touch
        .mode = 0,
        .spics_io_num = TOUCH_CS,
        .queue_size = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(TOUCH_SPI_HOST, &touch_cfg, &touch_spi));

    ESP_LOGI(TAG, "SPI Bus OK - Display und Touch registriert");

    // Display initialisieren
    lcd_init();

    // Touch initialisieren
    touch_init();

    // Startfarbe
    int ci = 0;
    lcd_fill(colors[ci]);
    ESP_LOGI(TAG, "Startfarbe: %s", color_names[ci]);
    ESP_LOGI(TAG, ">>> Bildschirm beruehren fuer Farbwechsel! <<<");

    // Diagnose: IRQ Pin Status prüfen
    ESP_LOGI(TAG, "IRQ Pin (GPIO%d) Status: %d (0=Touch, 1=kein Touch)",
             TOUCH_IRQ, gpio_get_level(TOUCH_IRQ));

    // Main Loop
    int touch_count = 0;
    bool was_touched = false;
    int diag_counter = 0;

    while (1) {
        uint16_t tx, ty, tz;

        if (touch_read(&tx, &ty, &tz)) {
            if (!was_touched) {
                // Neuer Touch - Farbe wechseln
                ci = (ci + 1) % NUM_COLORS;
                lcd_fill(colors[ci]);
                touch_count++;

                ESP_LOGI(TAG, "TOUCH #%d: raw X=%d, Y=%d, Z=%d -> %s",
                         touch_count, tx, ty, tz, color_names[ci]);
                was_touched = true;
            }
        } else {
            if (was_touched) {
                ESP_LOGI(TAG, "Touch losgelassen");
                was_touched = false;
            }
        }

        // Diagnose alle 5 Sekunden
        diag_counter++;
        if (diag_counter >= 250) {
            diag_counter = 0;
            int irq = gpio_get_level(TOUCH_IRQ);
            ESP_LOGI(TAG, "[DIAG] IRQ=%d, Touches=%d, Heap=%u",
                     irq, touch_count, (unsigned)esp_get_free_heap_size());

            // Rohwerte lesen (verschiedene Kommando-Varianten testen)
            uint16_t raw_x = touch_read_channel(0x90);
            uint16_t raw_y = touch_read_channel(0xD0);
            uint16_t raw_z1 = touch_read_channel(0xB0);
            uint16_t raw_z2 = touch_read_channel(0xC0);
            touch_read_channel(0x80);
            ESP_LOGI(TAG, "[DIAG] X=%d Y=%d Z1=%d Z2=%d (ohne Touch)",
                     raw_x, raw_y, raw_z1, raw_z2);

            // SPI Kommunikations-Check: Rohe Bytes anzeigen
            uint8_t tx_test[3] = {0x90, 0x00, 0x00};
            uint8_t rx_test[3] = {0xFF, 0xFF, 0xFF};
            spi_transaction_t t_test = {
                .length = 24,
                .tx_buffer = tx_test,
                .rx_buffer = rx_test,
            };
            spi_device_transmit(touch_spi, &t_test);
            ESP_LOGI(TAG, "[DIAG] SPI raw response: 0x%02X 0x%02X 0x%02X",
                     rx_test[0], rx_test[1], rx_test[2]);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
