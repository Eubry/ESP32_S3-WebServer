/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "main.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define WIFI_SSID "mywifissid"
*/
#define LOG_TAG "MAIN"
#define WIFI_SSID CONFIG_ESP_WIFI_SSID
#define WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
//------Global Objects------
OLEDDisplay disp;
oledParam_t oledParams;
taskManager tskMgr;
//------Function Prototypes------
void DspStat(void *pvParameters);
//------Main Application------
extern "C" void app_main(void) {
   oledParams.pinSCL = 13;
   oledParams.pinSDA = 12;

   disp.begin(oledParams);
   disp.addLabel("stat", 0, 0);
   disp.setLabel("stat", "Program started!");
   vTaskDelay(pdMS_TO_TICKS(500));
   disp.setLabel("stat", "Init tasks...");

   tskMgr.add("Display status", DspStat, NULL, 1, 0, 2000);
   tskMgr.add("Wifi Connection", DspStat, NULL, 1, 0, 2000);
   
   
}

//------Function Definitions------
void DspStat(void *pvParameters) {
   while(1) {
      disp.setLabel("stat", "OLED Started...");
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}
//Testing GitHub commit
//Testing GitHub commit2
//Changes ready to push
//Changes from virtual machine
