#pragma once
#include <map>
#include <string>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "../counter/counter.h"

class pinManager{
    private:
        enum class PinType : uint8_t {
            DIGITAL = 0,
            PWM = 1
        };
        
        struct PinInfo {
            gpio_num_t pin;
            gpio_mode_t mode;
            PinType type;
            uint8_t _padding[3];  // Explicit padding for alignment
        };
        struct PwmInfo {
            gpio_num_t pin;
            ledc_channel_t channel;
            ledc_timer_t timer;
            uint32_t frequency;
            cTime toneTimer;
            int64_t duration_us = 0;
            bool toneActive = false;
        };
        std::map <std::string , PinInfo> pinMap;
        std::map <std::string , PwmInfo> pwmMap;
        uint8_t nextChannel = 0;
        
    public:
        pinManager(){};
        void digitalPin(std::string name, int8_t pin, gpio_mode_t mode=GPIO_MODE_INPUT);
        int digitalRead(std::string name);
        void digitalWrite(std::string name, uint8_t value);
        void pwmPin(std::string name, int8_t pin, uint32_t frequency=5000, ledc_timer_t timer=LEDC_TIMER_0, ledc_timer_bit_t duty_resolution=LEDC_TIMER_13_BIT);
        void setPwmDuty(std::string name, uint32_t duty);
        void setPwmDutyPercent(std::string name, float percent);
        void setPwmDutyPercent(std::string name, int8_t percent);
        void setPwmDutyMicros(std::string name, uint32_t micros);
        void setPwmFrequency(std::string name, uint32_t frequency);
        void tone(std::string name, uint32_t frequency, uint8_t volume=50, uint32_t duration_ms=0);
        void noTone(std::string name);
        void update(); // Call this regularly to handle timed tones
        //gpio_num_t getPin(std::string name);
        //void configureInputPin(gpio_num_t pin, gpio_pull_mode_t pullMode);
};