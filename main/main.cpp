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
#define WIFI_SSID "R3X4S"//CONFIG_ESP_WIFI_SSID
#define WIFI_PASS "N1C0L4T3SL4"//CONFIG_ESP_WIFI_PASSWORD
//------Global Objects------
OLEDDisplay disp;
oledParam_t oledParams;
taskManager tskMgr;
wifiConnection wConn(WIFI_SSID, WIFI_PASS);
//------Function Prototypes------
void DspStat(void *pvParameters);
void wifi(void *pvParameters);
//------Main Application------
extern "C" void app_main(void) {
   oledParams.pinSCL = 13;
   oledParams.pinSDA = 12;

   disp.begin(oledParams);
   disp.addLabel("stat", 0, 0);
   disp.addLabel("swifi", 0, 12);
   disp.addLabel("wstat", 0, 24);
   disp.addLabel("outloop", 0, 36);
   disp.setLabel("stat", "Program started!");
   disp.setLabel("swifi", "-");
   disp.setLabel("wstat", "-");
   disp.setLabel("outloop", "-");

   vTaskDelay(pdMS_TO_TICKS(500));
   disp.setLabel("stat", "Init tasks...");
   wConn.begin(WIFI_MODE_STA);
   tskMgr.add("DisplayStat", DspStat, NULL, 1, 0, 4096);
   tskMgr.add("WifiConn", wifi, NULL, 6, 1, 4096);
   
   
}

//------Function Definitions------
void DspStat(void *pvParameters) {
   // Small delay to ensure task is registered before calling resetWatchdog
   while(1) {
      disp.setLabel("stat", "OLED Started");
      tskMgr.resetWatchdog("DisplayStat");
      vTaskDelay(pdMS_TO_TICKS(1000));
   }
}
void wifi(void *pvParameters) {
   // Small delay to ensure task is registered before calling resetWatchdog
   static cTime rwdt;
   static cTime upt;
   while(1) {
      // Update WiFi status display
      upt.wait(500000); // 500000 microseconds = 0.5 seconds
      if(upt.finish()) {
         disp.setLabel("swifi", "Wifi Started");
         if(wConn.isConnected()) {
            disp.setLabel("wstat", wConn.getIp().c_str());
         } else {
            disp.setLabel("wstat", "WiFi Disconnected");
         }
      }
      rwdt.wait(4000000); // 4000000 microseconds = 4 seconds
      if(rwdt.finish()) {
         // Reset watchdog every loop iteration to prevent timeout
         tskMgr.resetWatchdog("WifiConn");
      }
      
      vTaskDelay(pdMS_TO_TICKS(10));

   }
}
//Testing GitHub commit
//Testing GitHub commit2
//Changes ready to push
//Changes from virtual machine
