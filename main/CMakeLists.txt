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
                        Utils
                        wifiManager
                        pinManager
                    INCLUDE_DIRS "." "include" "lib")


