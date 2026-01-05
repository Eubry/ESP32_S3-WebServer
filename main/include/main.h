#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <string.h>
#include "SSD1306/SSD1306.h"

template<typename T>
bool inMap(const std::string& key, const std::map<std::string, T>& myMap){
    return myMap.find(key) != myMap.end();
}

void statusTask(BaseType_t gpTaskResult, const char* TAG, const char* taskName) {
   if(gpTaskResult != pdPASS) {
      ESP_LOGE(TAG, "Failed to create %s task!", taskName);
   } else {
      ESP_LOGI(TAG, "%s Task created successfully", taskName);
   }
}
class taskManager{
    private:
        struct taskStruct{
            TaskHandle_t handle;
            std::string name;
            void* param = NULL;
            UBaseType_t priority = 1;
            BaseType_t core = 0;
            uint32_t stackSize = 1024;
        }; 
        std::map <std::string, taskStruct> _taskMap;
    public:
        taskManager(){};
        void add(const std::string& name, TaskFunction_t taskFunc, void* param = NULL, UBaseType_t priority = 1, BaseType_t core = 0, uint32_t stackSize = 1024){
            taskStruct newTask;
            newTask.name = name;
            newTask.param = param;
            newTask.priority = priority;
            newTask.core = core;
            newTask.stackSize = stackSize;
            BaseType_t result = xTaskCreatePinnedToCore(taskFunc, name.c_str(), stackSize, param, priority, &newTask.handle, core);
            statusTask(result, "TASK_MANAGER", name.c_str());
            if(result == pdPASS){
                _taskMap[name] = newTask;
                esp_err_t wdt_err = esp_task_wdt_add(newTask.handle);
                if(wdt_err != ESP_OK){
                    ESP_LOGW("TASK_MANAGER", "Failed to add task %s to watchdog: %s", name.c_str(), esp_err_to_name(wdt_err));
                }
            }
        };
        //Delete task by name
        void del(const std::string& name){
            if(inMap(name, _taskMap)){
                esp_err_t wdt_err = esp_task_wdt_delete(_taskMap[name].handle);
                if(wdt_err != ESP_OK && wdt_err != ESP_ERR_NOT_FOUND){
                    ESP_LOGW("TASK_MANAGER", "Failed to unsubscribe task %s from watchdog: %s", name.c_str(), esp_err_to_name(wdt_err));
                }
                vTaskDelete(_taskMap[name].handle);
                _taskMap.erase(name);
                ESP_LOGI("TASK_MANAGER", "Deleted task: %s", name.c_str());
            } else {
                ESP_LOGE("TASK_MANAGER", "Task not found: %s", name.c_str());
            }
        };
        //Reset watchdog for a task by name
        void resetWatchdog(const std::string& name){
            if(inMap(name, _taskMap)){
                esp_err_t wdt_err = esp_task_wdt_reset();
                if(wdt_err != ESP_OK){
                    ESP_LOGW("TASK_MANAGER", "Failed to reset watchdog for task %s: %s", name.c_str(), esp_err_to_name(wdt_err));
                }
            } else {
                ESP_LOGE("TASK_MANAGER", "Task not found: %s", name.c_str());
            }
        };
        ~taskManager(){};
};