#include "pinManagement.h"
#include "esp_log.h"

#define PIN_TAG "PinManager"

void pinManager::digitalPin(std::string name, int8_t pin, gpio_mode_t mode){
    // Validate pin number for ESP32 (GPIO 0-39, with some reserved pins)
    if(pin < 0 || pin > 39) {
        ESP_LOGE(PIN_TAG, "Invalid GPIO pin number: %d. ESP32 supports GPIO 0-39 only.", pin);
        return;
    }
    
    pinMap[name]={static_cast<gpio_num_t>(pin), mode, PinType::DIGITAL, {0, 0, 0}};
    // Configure GPIO 32 as input
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = mode;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}
int pinManager::digitalRead(std::string name){
    if(pinMap.find(name) != pinMap.end()){
        if(pinMap[name].type == PinType::DIGITAL && (pinMap[name].mode == GPIO_MODE_INPUT || pinMap[name].mode == GPIO_MODE_INPUT_OUTPUT)){
            gpio_num_t pin = pinMap[name].pin;
            return gpio_get_level(pin);
        }else{return -1;}
    }else{return -1;}
}
void pinManager::digitalWrite(std::string name, uint8_t value){
    if(pinMap.find(name) != pinMap.end()){
        if(pinMap[name].type == PinType::DIGITAL && (pinMap[name].mode == GPIO_MODE_OUTPUT || pinMap[name].mode == GPIO_MODE_INPUT_OUTPUT)){
            gpio_num_t pin = pinMap[name].pin;
            gpio_set_level(pin, value);
        }
    }
}

void pinManager::pwmPin(std::string name, int8_t pin, uint32_t frequency, ledc_timer_t timer, ledc_timer_bit_t duty_resolution){
    // Validate pin number for ESP32 (GPIO 0-39, with some reserved pins)
    if(pin < 0 || pin > 39) {
        ESP_LOGE(PIN_TAG, "Invalid GPIO pin number: %d. ESP32 supports GPIO 0-39 only.", pin);
        return;
    }
    
    // Assign next available channel
    ledc_channel_t channel = static_cast<ledc_channel_t>(nextChannel);
    nextChannel++;
    
    // Configure timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = duty_resolution,
        .timer_num = timer,
        .freq_hz = frequency,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    ledc_timer_config(&ledc_timer);
    
    // Configure channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {}
    };
    ledc_channel_config(&ledc_channel);
    
    // Store PWM info
    pwmMap[name] = {static_cast<gpio_num_t>(pin), channel, timer, frequency, cTime(), 0, false};
}
// Set absolute duty (0-8191)
void pinManager::setPwmDuty(std::string name, uint32_t duty){
    if(pwmMap.find(name) != pwmMap.end()){
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel);
    }
}
// Set duty cycle by percentage (0-100%)
void pinManager::setPwmDutyPercent(std::string name, int8_t percent){setPwmDutyPercent(name, (float) percent);}
void pinManager::setPwmDutyPercent(std::string name, float percent){
    if(pwmMap.find(name) != pwmMap.end()){
        // 13-bit resolution = 8192 levels (0-8191)
        uint32_t duty = (uint32_t)((percent / 100.0f) * 8191.0f);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel);
    }
}
// Set duty cycle by microseconds (for servo control)
void pinManager::setPwmDutyMicros(std::string name, uint32_t micros){
    if(pwmMap.find(name) != pwmMap.end()){
        // Calculate duty cycle from microseconds
        // duty = (micros * frequency * max_duty) / 1000000
        uint32_t frequency = pwmMap[name].frequency;
        uint32_t duty = (micros * frequency * 8192) / 1000000;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel);
    }
}
// Set PWM frequency
void pinManager::setPwmFrequency(std::string name, uint32_t frequency){
    if(pwmMap.find(name) != pwmMap.end()){
        ledc_set_freq(LEDC_LOW_SPEED_MODE, pwmMap[name].timer, frequency);
        pwmMap[name].frequency = frequency;
    }
}
// Generate tone at specific frequency with adjustable volume (0-100%) and optional duration
void pinManager::tone(std::string name, uint32_t frequency, uint8_t volume, uint32_t duration_ms){
    if(pwmMap.find(name) != pwmMap.end()){
        ledc_set_freq(LEDC_LOW_SPEED_MODE, pwmMap[name].timer, frequency);
        pwmMap[name].frequency = frequency;
        // Volume controls duty cycle: 0% = silent, 50% = default, 100% = loudest
        // Clamp volume to 0-100 range
        if(volume > 100) volume = 100;
        uint32_t duty = (volume * 8192) / 100; // Convert percentage to 13-bit duty value
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel);
        
        // If duration specified, start non-blocking timer
        if(duration_ms > 0){
            pwmMap[name].duration_us = duration_ms * 1000; // Convert ms to microseconds
            pwmMap[name].toneTimer.reset();
            pwmMap[name].toneActive = true;
        } else {
            pwmMap[name].toneActive = false;
        }
    }
}
// Stop tone
void pinManager::noTone(std::string name){
    if(pwmMap.find(name) != pwmMap.end()){
        ledc_set_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmMap[name].channel);
        pwmMap[name].toneActive = false;
    }
}
// Update method - call this regularly in your main loop to handle timed tones
void pinManager::update(){
    for(auto& pwm : pwmMap){
        if(pwm.second.toneActive){
            pwm.second.toneTimer.wait(pwm.second.duration_us);
            if(pwm.second.toneTimer.finish()){
                noTone(pwm.first);
            }
        }
    }
}