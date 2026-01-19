#pragma once
#include "Utils.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <errno.h>
#include "SSD1306/SSD1306.h"
#include "wifiManager.h"
#include "pinManager.h"
#include "webManager.h"
#include "esp_http_server.h"
#include "driver/ledc.h"
#include <string.h>

using namespace Utils;