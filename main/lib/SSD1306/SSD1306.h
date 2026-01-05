#pragma once
#include "freertos/FreeRTOS.h"// Include FreeRTOS base
#include "freertos/task.h"// Include FreeRTOS task support
#include "driver/gpio.h"// Include GPIO driver
#include "esp_err.h"// Include ESP error codes
#include "esp_log.h"// Add ESP logging support
#include "esp_timer.h"// Include ESP timer support
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c_master.h"// Include I2C master driver
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include <string>
#include <map>
#define PIN_SDA 21
#define PIN_SCL 22
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8
#define LCD_PIXEL_CLOCK_HZ 400 * 1000
#define PIN_RST -1

struct oledParam_t{
    std::string tag="OLED";
    uint32_t address=0x3C;
    int busPort=0;
    int8_t pinSDA=PIN_SDA;
    int8_t pinSCL=PIN_SCL;
    uint8_t lcdWidth=128;
    uint8_t lcdHeight=64;
    lvgl_port_rotation_cfg_t rotation={
        .swap_xy = false,
        .mirror_x = true,
        .mirror_y = true
    };
};
class OLEDDisplay{
    private:
        i2c_master_bus_handle_t _i2c_bus=NULL;
        esp_lcd_panel_io_handle_t _io_handle=NULL;
        esp_lcd_panel_handle_t _panel_handle=NULL;
        oledParam_t *_par=nullptr;
        std::map<std::string, lv_obj_t*> _lvgl_objs;
    public:
        lv_disp_t *dev=nullptr;
        OLEDDisplay(){};
        void begin(oledParam_t &param);
        void addLabel(std::string index,int32_t posX,int32_t posY);
        void addLabel(std::string index,int32_t posX,int32_t posY, lv_align_t align);
        void setLabel(std::string index,const char* text);
        void setLabel(std::string index,int value);
        void clear();
        void drawPixel(int16_t x, int16_t y, uint16_t color);
        void display();
        ~OLEDDisplay();
};


