idf_component_register(SRCS "main.cpp" "lib/SSD1306/SSD1306.cpp"
                    PRIV_REQUIRES 
                        freertos
                        esp_common 
                        log 
                        driver 
                        esp_timer 
                        esp_lcd
                        lvgl
                        esp_lvgl_port
                        esp_wifi 
                        esp_netif_stack
                        nvs_flash
                        esp_http_server
                        Utils
                        wifiManager
                        pinManager
                        webServer
                    INCLUDE_DIRS "." "include" "lib")

# Force C++17 standard for this component (required for some ESP-IDF toolchains)
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)

# Ensure C++17 standard is used
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)


