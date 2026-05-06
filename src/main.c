#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "config.h"

static const char *TAG = "cyd-display-car1";

// NVS handle for persistent storage
nvs_handle_t nvs_handle;

// LVGL objects
lv_obj_t *main_screen;
lv_obj_t *car_label;
lv_obj_t *speed_label;
lv_obj_t *battery_label;
lv_obj_t *status_label;

// Car simulation data
typedef struct {
    int speed;          // km/h
    int battery;        // percentage
    char status[32];    // status text
} car_data_t;

static car_data_t car_data = {
    .speed = 0,
    .battery = 100,
    .status = "Ready"
};

/**
 * @brief Initialize NVS (Non-Volatile Storage)
 */
static esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition invalid, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ret = nvs_open("cyd-display-car1", NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS handle: %s", esp_err_to_name(ret));
        return ret;
    }
    
    return ESP_OK;
}

/**
 * @brief Create enhanced demo dashboard with animations
 */
static void create_car_dashboard(void)
{
    // Create main screen with gradient background
    main_screen = lv_scr_act();
    lv_obj_set_style_bg_color(main_screen, lv_color_hex(0x001133), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(main_screen, lv_color_hex(0x003366), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(main_screen, LV_GRAD_DIR_VER, LV_PART_MAIN);
    
    // Create header container
    lv_obj_t *header = lv_obj_create(main_screen);
    lv_obj_set_size(header, LCD_WIDTH, 40);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x002244), LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 5, LV_PART_MAIN);
    
    // Title with shadow effect
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "🏁 CYD DEMO Dashboard 🏁");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);
    
    // Create main display area
    lv_obj_t *display_area = lv_obj_create(main_screen);
    lv_obj_set_size(display_area, LCD_WIDTH - 20, 120);
    lv_obj_align(display_area, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_color(display_area, lv_color_hex(0x001122), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(display_area, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_border_width(display_area, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(display_area, lv_color_hex(0x00FFFF), LV_PART_MAIN);
    lv_obj_set_style_radius(display_area, 10, LV_PART_MAIN);
    
    // Speed meter with style
    speed_label = lv_label_create(display_area);
    lv_label_set_text_fmt(speed_label, "⚡ %d km/h", car_data.speed);
    lv_obj_set_style_text_color(speed_label, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_align(speed_label, LV_ALIGN_TOP_LEFT, 10, 10);
    
    // Battery meter with style
    battery_label = lv_label_create(display_area);
    lv_label_set_text_fmt(battery_label, "🔋 %d%%", car_data.battery);
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFF00), LV_PART_MAIN);
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -10, 10);
    
    // Car animation area
    lv_obj_t *car_area = lv_obj_create(display_area);
    lv_obj_set_size(car_area, 100, 60);
    lv_obj_align(car_area, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_opa(car_area, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(car_area, 0, LV_PART_MAIN);
    
    // Animated car icon
    car_label = lv_label_create(car_area);
    lv_label_set_text(car_label, "🏎️");
    lv_obj_set_style_text_font(car_label, &lv_font_montserrat_36, LV_PART_MAIN);
    lv_obj_align(car_label, LV_ALIGN_CENTER, 0, 0);
    
    // Status bar with style
    lv_obj_t *status_bar = lv_obj_create(main_screen);
    lv_obj_set_size(status_bar, LCD_WIDTH - 40, 30);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_MID, 0, -70);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x003344), LV_PART_MAIN);
    lv_obj_set_style_border_width(status_bar, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(status_bar, lv_color_hex(0x00FFFF), LV_PART_MAIN);
    lv_obj_set_style_radius(status_bar, 15, LV_PART_MAIN);
    
    status_label = lv_label_create(status_bar);
    lv_label_set_text_fmt(status_label, "🎯 %s", car_data.status);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x00FFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 0);
    
    // Enhanced control buttons with colors
    lv_obj_t *btn_up = lv_btn_create(main_screen);
    lv_obj_set_size(btn_up, 70, 35);
    lv_obj_align(btn_up, LV_ALIGN_BOTTOM_LEFT, 20, -25);
    lv_obj_set_style_bg_color(btn_up, lv_color_hex(0x00AA00), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_up, 5, LV_PART_MAIN);
    lv_obj_t *label_up = lv_label_create(btn_up);
    lv_label_set_text(label_up, "⬆️ Speed");
    lv_obj_center(label_up);
    
    lv_obj_t *btn_down = lv_btn_create(main_screen);
    lv_obj_set_size(btn_down, 70, 35);
    lv_obj_align(btn_down, LV_ALIGN_BOTTOM_LEFT, 100, -25);
    lv_obj_set_style_bg_color(btn_down, lv_color_hex(0xAA0000), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_down, 5, LV_PART_MAIN);
    lv_obj_t *label_down = lv_label_create(btn_down);
    lv_label_set_text(label_down, "⬇️ Brake");
    lv_obj_center(label_down);
    
    lv_obj_t *btn_charge = lv_btn_create(main_screen);
    lv_obj_set_size(btn_charge, 70, 35);
    lv_obj_align(btn_charge, LV_ALIGN_BOTTOM_RIGHT, -100, -25);
    lv_obj_set_style_bg_color(btn_charge, lv_color_hex(0x0000AA), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_charge, 5, LV_PART_MAIN);
    lv_obj_t *label_charge = lv_label_create(btn_charge);
    lv_label_set_text(label_charge, "⚡ Charge");
    lv_obj_center(label_charge);
    
    lv_obj_t *btn_reset = lv_btn_create(main_screen);
    lv_obj_set_size(btn_reset, 70, 35);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_RIGHT, -20, -25);
    lv_obj_set_style_bg_color(btn_reset, lv_color_hex(0xAA6600), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_reset, 5, LV_PART_MAIN);
    lv_obj_t *label_reset = lv_label_create(btn_reset);
    lv_label_set_text(label_reset, "🔄 Reset");
    lv_obj_center(label_reset);
    
    // Enhanced button event handlers with feedback
    lv_obj_add_event_cb(btn_up, [](lv_event_t *e) {
        if (car_data.speed < 200) {
            car_data.speed += 15;
            strcpy(car_data.status, "🚀 Accelerating!");
            // Visual feedback
            lv_obj_set_style_bg_color(e->target, lv_color_hex(0x00FF00), LV_PART_MAIN);
        }
    }, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_down, [](lv_event_t *e) {
        if (car_data.speed > 0) {
            car_data.speed -= 15;
            if (car_data.speed == 0) {
                strcpy(car_data.status, "🛑 Stopped");
            } else {
                strcpy(car_data.status, "🔽 Braking!");
            }
            lv_obj_set_style_bg_color(e->target, lv_color_hex(0xFF0000), LV_PART_MAIN);
        }
    }, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_charge, [](lv_event_t *e) {
        if (car_data.battery < 100) {
            car_data.battery = 100;
            strcpy(car_data.status, "⚡ Fully Charged!");
            lv_obj_set_style_bg_color(e->target, lv_color_hex(0x0088FF), LV_PART_MAIN);
        }
    }, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_reset, [](lv_event_t *e) {
        car_data.speed = 0;
        car_data.battery = 100;
        strcpy(car_data.status, "🎯 Ready to Race!");
        lv_obj_set_style_bg_color(e->target, lv_color_hex(0xFFAA00), LV_PART_MAIN);
    }, LV_EVENT_CLICKED, NULL);
    
    // Add demo animation timer
    static lv_timer_t *demo_timer;
    demo_timer = lv_timer_create([](lv_timer_t *t) {
        // Demo mode: automatic speed changes
        static int demo_phase = 0;
        static uint32_t last_change = 0;
        uint32_t now = lv_tick_get();
        
        if (now - last_change > 2000) { // Change every 2 seconds
            last_change = now;
            
            switch(demo_phase % 4) {
                case 0:
                    car_data.speed = 60;
                    strcpy(car_data.status, "🏁 Demo Mode - City Driving");
                    break;
                case 1:
                    car_data.speed = 120;
                    strcpy(car_data.status, "🛣️ Demo Mode - Highway Speed");
                    break;
                case 2:
                    car_data.speed = 180;
                    strcpy(car_data.status, "🏎️ Demo Mode - Racing!");
                    break;
                case 3:
                    car_data.speed = 0;
                    strcpy(car_data.status, "🛑 Demo Mode - Pit Stop");
                    car_data.battery = 100;
                    break;
            }
            demo_phase++;
        }
    }, 1000, NULL); // Check every second
}

/**
 * @brief Update display task
 */
static void update_display_task(void *pvParameters)
{
    while (1) {
        // Update speed display
        lv_label_set_text_fmt(speed_label, "Speed: %d km/h", car_data.speed);
        
        // Update battery display with color coding
        if (car_data.battery > 50) {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(0x00FF00), LV_PART_MAIN);
        } else if (car_data.battery > 20) {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFFFF00), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(0xFF0000), LV_PART_MAIN);
        }
        lv_label_set_text_fmt(battery_label, "Battery: %d%%", car_data.battery);
        
        // Update status
        lv_label_set_text_fmt(status_label, "Status: %s", car_data.status);
        
        // Simulate battery drain when moving
        if (car_data.speed > 0 && car_data.battery > 0) {
            car_data.battery--;
            if (car_data.battery == 0) {
                car_data.speed = 0;
                strcpy(car_data.status, "Battery Empty");
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));  // Update every 100ms
    }
}

/**
 * @brief Initialize LVGL
 */
static esp_err_t init_lvgl(void)
{
    ESP_LOGI(TAG, "Initializing LVGL");
    
    // Initialize LVGL
    lv_init();
    
    // Initialize display driver
    lv_port_disp_init();
    
    // Initialize input driver (touch)
    lv_port_indev_init();
    
    // Create UI
    create_car_dashboard();
    
    // Create display update task
    xTaskCreate(update_display_task, "update_display", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "LVGL initialized successfully");
    return ESP_OK;
}

/**
 * @brief FreeRTOS task - Main application logic
 */
static void app_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Application task started");
    
    // Initialize LVGL display
    if (init_lvgl() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LVGL");
        vTaskDelete(NULL);
        return;
    }
    
    // Main application loop
    for (int i = 0; ; i++) {
        ESP_LOGI(TAG, "Car dashboard running [%d]", i);
        vTaskDelay(pdMS_TO_TICKS(10000));  // 10 second delay
        
        // TODO: Add additional car logic here
    }
    
    vTaskDelete(NULL);
}

/**
 * @brief Application entry point
 */
void app_main(void)
{
    ESP_LOGI(TAG, "CYD Car Dashboard Application Started");
    ESP_LOGI(TAG, "Project: cyd-display-car1");
    
    // Initialize NVS
    if (init_nvs() != ESP_OK) {
        ESP_LOGE(TAG, "NVS initialization failed, halting");
        return;
    }
    
    // Print system info
    ESP_LOGI(TAG, "Chip revision: %d", esp_chip_revision());
    ESP_LOGI(TAG, "Free heap: %u bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap (ever): %u bytes", esp_get_minimum_free_heap_size());
    
    // Create main application task
    BaseType_t ret = xTaskCreate(
        app_task,              // Task function
        "cyd-display-car1_app", // Task name
        CONFIG_APP_STACK_SIZE, // Stack size (from sdkconfig)
        NULL,                  // Task parameter
        CONFIG_APP_PRIORITY,   // Task priority
        NULL                   // Task handle
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create application task");
    }
}
