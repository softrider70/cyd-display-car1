idf_component_register(
    SRCS "touch_test.c"
    INCLUDE_DIRS "../include"
    REQUIRES esp_system esp_driver_spi esp_driver_gpio
)
