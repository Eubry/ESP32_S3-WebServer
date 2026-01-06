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
esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t status_get_handler(httpd_req_t *req);
httpd_handle_t start_https_server(void);
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
         // Check if WiFi is connected and server is not started
         if(wConn.isConnected() && server == NULL) {
            // Start HTTPS server when WiFi is connected
            server = start_https_server();
            if(server != NULL) {
               ESP_LOGI(LOG_TAG, "HTTPS Server started successfully");
               disp.setLabel("swifi", "HTTPS Running");
            } else {
               ESP_LOGE(LOG_TAG, "Failed to start HTTPS server");
               disp.setLabel("swifi", "HTTPS Failed");
            }
         } else if(!wConn.isConnected() && server != NULL) {
            // Stop server if WiFi disconnected
            httpd_ssl_stop(server);
            server = NULL;
            disp.setLabel("swifi", "HTTPS Stopped");
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

//------HTTPS Server Handlers------
esp_err_t root_get_handler(httpd_req_t *req) {
   const char* resp_str = 
      "<!DOCTYPE html><html><head><title>ESP32-S3 WebServer</title>"
      "<style>body{font-family:Arial;margin:40px;background:#f0f0f0;}"
      "h1{color:#333;}p{font-size:18px;}</style></head><body>"
      "<h1>ESP32-S3 HTTPS WebServer</h1>"
      "<p>Server is running successfully!</p>"
      "<p><a href='/status'>View Status</a></p>"
      "</body></html>";
   httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
   return ESP_OK;
}

esp_err_t status_get_handler(httpd_req_t *req) {
   char resp_str[1024];
   snprintf(resp_str, sizeof(resp_str),
      "<!DOCTYPE html><html><head><title>Status</title>"
      "<style>body{font-family:Arial;margin:40px;background:#f0f0f0;}"
      "h1{color:#333;}table{border-collapse:collapse;width:100%%;}"
      "td,th{border:1px solid #ddd;padding:8px;text-align:left;}"
      "th{background-color:#4CAF50;color:white;}</style></head><body>"
      "<h1>ESP32-S3 Status</h1>"
      "<table><tr><th>Parameter</th><th>Value</th></tr>"
      "<tr><td>Hostname</td><td>%s</td></tr>"
      "<tr><td>IP Address</td><td>%s</td></tr>"
      "<tr><td>WiFi Status</td><td>%s</td></tr>"
      "</table><br><p><a href='/'>Back to Home</a></p>"
      "</body></html>",
      wConn.getHostname().c_str(),
      wConn.getIp().c_str(),
      wConn.isConnected() ? "Connected" : "Disconnected");
   httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
   return ESP_OK;
}

httpd_handle_t start_https_server(void) {
   httpd_handle_t server_handle = NULL;
   httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
   
   conf.httpd.max_open_sockets = 7;
   conf.httpd.stack_size = 10240;
   
   // Configure SSL certificate and private key
   conf.servercert = (const uint8_t *)cacert_pem_start;
   conf.servercert_len = strlen(cacert_pem_start) + 1;
   conf.prvtkey_pem = (const uint8_t *)prvtkey_pem_start;
   conf.prvtkey_len = strlen(prvtkey_pem_start) + 1;
   
   ESP_LOGI(LOG_TAG, "Server certificate ptr: %p, len: %u", conf.servercert, conf.servercert_len);
   ESP_LOGI(LOG_TAG, "Private key ptr: %p, len: %u", conf.prvtkey_pem, conf.prvtkey_len);
   
   ESP_LOGI(LOG_TAG, "Starting HTTPS server...");
   
   esp_err_t ret = httpd_ssl_start(&server_handle, &conf);
   if (ret != ESP_OK) {
      ESP_LOGE(LOG_TAG, "Error starting HTTPS server: %s", esp_err_to_name(ret));
      return NULL;
   }
   
   // Register URI handlers
   httpd_uri_t root_uri = {
      .uri       = "/",
      .method    = HTTP_GET,
      .handler   = root_get_handler,
      .user_ctx  = NULL
   };
   httpd_register_uri_handler(server_handle, &root_uri);
   
   httpd_uri_t status_uri = {
      .uri       = "/status",
      .method    = HTTP_GET,
      .handler   = status_get_handler,
      .user_ctx  = NULL
   };
   httpd_register_uri_handler(server_handle, &status_uri);
   
   ESP_LOGI(LOG_TAG, "HTTPS server started on port 443");
   return server_handle;
}
