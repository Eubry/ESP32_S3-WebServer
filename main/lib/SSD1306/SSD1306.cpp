#include "SSD1306.h"

void OLEDDisplay::begin(oledParam_t &param){
    _par=&param;
    ESP_LOGI(_par->tag.c_str(), "Initialize I2C bus");
    // i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .i2c_port = _par->busPort,
        .sda_io_num = (gpio_num_t)_par->pinSDA,
        .scl_io_num = (gpio_num_t)_par->pinSCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .flags = {
            .enable_internal_pullup = true,
        },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &_i2c_bus));
    ESP_LOGI(_par->tag.c_str(), "Install panel IO");
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = _par->address,
        .control_phase_bytes = 1,               // According to SSD1306 datasheet
        .dc_bit_offset = 6,                     // According to SSD1306 datasheet
        .lcd_cmd_bits = LCD_CMD_BITS,           // According to SSD1306 datasheet
        .lcd_param_bits = LCD_PARAM_BITS,       // According to SSD1306 datasheet
        .scl_speed_hz = LCD_PIXEL_CLOCK_HZ,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(_i2c_bus, &io_config, &_io_handle));
    ESP_LOGI(_par->tag.c_str(), "Install SSD1306 panel driver");
    // esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = GPIO_NUM_NC,
        .bits_per_pixel = 1,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = _par->lcdHeight,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(_io_handle, &panel_config, &_panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(_panel_handle, true));
    
    // Apply rotation settings to the panel
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(_panel_handle, _par->rotation.mirror_x, _par->rotation.mirror_y));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(_panel_handle, _par->rotation.swap_xy));
    
    // _i2c_bus = i2c_bus;
    // _io_handle = _io_handle;
    // _panel_handle = panel_handle;
    ESP_LOGI(_par->tag.c_str(), "OLED Display initialized successfully");
    // Initialize LVGL
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = _io_handle,
        .panel_handle = _panel_handle,
        .buffer_size = static_cast<uint32_t>(_par->lcdWidth * _par->lcdHeight),
        .double_buffer = true,
        .hres = _par->lcdWidth,
        .vres = _par->lcdHeight,
        .monochrome = true,
        .rotation = _par->rotation,
        .flags = {}
    };
    dev = lvgl_port_add_disp(&disp_cfg);

}
void OLEDDisplay::addLabel(std::string index,int32_t posX,int32_t posY){
    lvgl_port_lock(0);
    lv_obj_t *scr = lv_disp_get_scr_act(dev);
    _lvgl_objs[index] = lv_label_create(scr);
    lv_obj_align(_lvgl_objs[index], LV_ALIGN_TOP_LEFT, posX, posY);
    lvgl_port_unlock();
    // This function can be used to add the display to LVGL if needed
}

void OLEDDisplay::addLabel(std::string index,int32_t posX,int32_t posY, lv_align_t align){
    lvgl_port_lock(0);
    lv_obj_t *scr = lv_disp_get_scr_act(dev);
    _lvgl_objs[index] = lv_label_create(scr);
    lv_obj_align(_lvgl_objs[index], align, posX, posY);
    lvgl_port_unlock();
}

void OLEDDisplay::setLabel(std::string index, const char* text){
    if(_lvgl_objs.find(index) != _lvgl_objs.end()) {
        lvgl_port_lock(0);
        lv_label_set_text(_lvgl_objs[index], text);
        lvgl_port_unlock();
    }
}

void OLEDDisplay::setLabel(std::string index, int value){
    if(_lvgl_objs.find(index) != _lvgl_objs.end()) {
        lvgl_port_lock(0);
        lv_label_set_text_fmt(_lvgl_objs[index], "%d", value);
        lvgl_port_unlock();
    }
}

void OLEDDisplay::clear(){
    // Clear the display by filling it with black pixels
    
}
void OLEDDisplay::drawPixel(int16_t x, int16_t y, uint16_t color){
    // For SSD1306, color is either 0 (black) or 1 (white)
    //uint8_t data = (color != 0) ? 0xFF : 0x00; // Set all bits in the byte for white, none for black
    //ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, &data));
}
void OLEDDisplay::display(){
    // For SSD1306, the display is updated automatically with each draw command.
    // This function can be left empty or used for future enhancements.
}
OLEDDisplay::~OLEDDisplay(){
    _par=nullptr;
    _panel_handle=NULL;
    _io_handle=NULL;
    _i2c_bus=NULL;
    dev=nullptr;
    _lvgl_objs.clear();
}