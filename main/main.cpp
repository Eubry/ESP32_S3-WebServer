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
#define BUZZER_PIN 14
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT
//------Global Objects------
OLEDDisplay disp;
oledParam_t oledParams;
taskManager tskMgr;
wifiConnection wConn(WIFI_SSID, WIFI_PASS,5,"ESP32S3WS");
httpd_handle_t server = NULL;
pinManager pMgr;
serverManager webSrv;
//------Function Prototypes------
void DspStat(void *pvParameters);
void wifi(void *pvParameters);
//------Main Application------
extern "C" void app_main(void) {
   // Initialize OLED Display------
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
   //------------------------------
   // Small delay to ensure display is ready
   vTaskDelay(pdMS_TO_TICKS(200));
   //------------------------------
   // Pin Manager Setup------------
   pMgr.pwmPin("buzzer", 10, 5000, LEDC_TIMER, LEDC_TIMER_13_BIT);
   // Initialize Tasks-------------
   disp.setLabel("stat", "Init tasks...");
   //------------------------------
   wConn.begin(WIFI_MODE_STA);
   tskMgr.add("DisplayStat", DspStat, NULL, 1, 0, 4096);
   tskMgr.add("WifiConn", wifi, NULL, 6, 1, 4096);
}

//------Function Definitions------
void DspStat(void *pvParameters) {
   static cTime rwdt;
   static cTime upt;
   // Small delay to ensure task is registered before calling resetWatchdog
   vTaskDelay(pdMS_TO_TICKS(100));
   while(1) {
      disp.setLabel("stat", "OLED Started");
      upt.wait(500000); // 500000 microseconds = 0.5 seconds
      if(upt.finish()) {
         if(wConn.isConnected()) {
            disp.setLabel("wstat", wConn.getIp().c_str());
            disp.setLabel("outloop", wConn.getHostname().c_str());
         } else {
            disp.setLabel("wstat", "WiFi Disconnected");
         }
      }
      rwdt.wait(4000000); // 4000000 microseconds = 4 seconds
      if(rwdt.finish()) {
         // Reset watchdog every loop iteration to prevent timeout
         tskMgr.resetWatchdog("DisplayStat");
      }
      vTaskDelay(pdMS_TO_TICKS(10));
   }
}
void buzzActive(void *pvPar) {
   pMgr.tone("buzzer", 1000); // 1000Hz tone
}
void buzzdeActive(void *pvPar) {
   pMgr.noTone("buzzer");
}

void wifi(void *pvParameters) {
   static cTime rwdt;
   static cTime upt;
   webSrv.addHTMLPath("root",HTTP_GET,serverManager::webData{"/","<html><body><h1>ESP32-S3 WebServer</h1><p>Welcome to ESP32-S3 HTTP Server!</p></body></html>","200 OK",""});
   webSrv.addHTMLPath("favicon",HTTP_GET,serverManager::webData{"/favicon.ico",nullptr,"204 No Content","favicon not available"});
   webSrv.addAPIPath("buzzer", serverManager::apiData{
      HTTP_POST,
      "Content too long",
      "ledGreen",
      {
         {"active", serverManager::apiOption{"\"value\":\"on\"","{\"ledGreen\":{\"status\":\"tone\",\"type\":\"digital\",\"value\":\"1\"}}","application/json",buzzActive}},
         {"stop", serverManager::apiOption{"\"value\":\"off\"","{\"ledGreen\":{\"status\":\"stopped\",\"type\":\"digital\",\"value\":\"0\"}}","application/json",buzzdeActive}}
      }
   });
   disp.setLabel("swifi", "Wifi Started");
   while(1) {
      // Update WiFi status display
      upt.wait(500000); // 500000 microseconds = 0.5 seconds
      if(upt.finish()) {
         // Start HTTP server when WiFi is connected
         if(wConn.isConnected() && server == NULL) {
            server = webSrv.begin();
            if(server != NULL) {
               ESP_LOGI(LOG_TAG, "HTTP Server started");
            }
         }
         // Stop HTTP server when WiFi is disconnected
         if(!wConn.isConnected() && server != NULL) {
            httpd_stop(server);
            server = NULL;
            ESP_LOGI(LOG_TAG, "HTTP Server stopped");
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