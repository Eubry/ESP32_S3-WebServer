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
wifiConnection wConn(WIFI_SSID, WIFI_PASS,5,"ESP32S3WS");
httpd_handle_t server = NULL;
//------Function Prototypes------
void DspStat(void *pvParameters);
void wifi(void *pvParameters);
esp_err_t root_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
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
void wifi(void *pvParameters) {
   static cTime rwdt;
   static cTime upt;
   disp.setLabel("swifi", "Wifi Started");
   while(1) {
      // Update WiFi status display
      upt.wait(500000); // 500000 microseconds = 0.5 seconds
      if(upt.finish()) {
         // Start HTTP server when WiFi is connected
         if(wConn.isConnected() && server == NULL) {
            server = start_webserver();
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

// HTTP GET handler for root path
esp_err_t root_handler(httpd_req_t *req) {
   const char* resp_str = "<html><body><h1>ESP32-S3 WebServer</h1><p>Welcome to ESP32-S3 HTTP Server!</p></body></html>";
   httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
   return ESP_OK;
}

// Start HTTP server
httpd_handle_t start_webserver(void) {
   httpd_handle_t server_instance = NULL;
   httpd_config_t config = HTTPD_DEFAULT_CONFIG();
   config.lru_purge_enable = true;

   // Start the httpd server
   ESP_LOGI(LOG_TAG, "Starting HTTP server on port: '%d'", config.server_port);
   if (httpd_start(&server_instance, &config) == ESP_OK) {
      // Set URI handlers
      httpd_uri_t root = {
         .uri       = "/",
         .method    = HTTP_GET,
         .handler   = root_handler,
         .user_ctx  = NULL
      };
      httpd_register_uri_handler(server_instance, &root);
      return server_instance;
   }

   ESP_LOGI(LOG_TAG, "Error starting HTTP server!");
   return NULL;
}
