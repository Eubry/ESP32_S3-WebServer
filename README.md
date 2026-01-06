| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# ESP32-S3 WiFi & Web Server

A comprehensive ESP32-S3 project that connects to a WiFi network, starts a web server, and displays status information on an SSD1306 OLED display.

## Features

- **WiFi Connectivity**: Connects to a configured WiFi network with automatic retry mechanism and mDNS hostname support
- **OLED Display**: Real-time status display using SSD1306 (128x64) with LVGL graphics library
  - Displays WiFi connection status
  - Shows IP address when connected
  - Displays device hostname
- **Task Management**: FreeRTOS task manager with watchdog integration
  - Display status task (Core 0, Priority 1)
  - WiFi connection task (Core 1, Priority 6)
- **Modular Architecture**: Clean component-based design with external libraries via git submodules
- **Hardware Abstraction**: Pin manager for GPIO and PWM control
- **Timing Utilities**: Custom counter and timer classes for non-blocking operations

## Hardware Requirements

- **ESP32-S3 Supermini** development board
- **SSD1306 OLED Display** (I2C interface)
  - SCL: GPIO 13
  - SDA: GPIO 12
  - 3.3V and GND connections

## Software Requirements

- ESP-IDF v6.0 or later
- Python 3.12+
- CMake 3.16+
- Git (for submodule management)

## Getting Started

### Clone the Repository

Clone the repository with all submodules:

```bash
git clone --recurse-submodules https://github.com/Eubry/ESP32_S3-WebServer.git
cd ESP32_S3-WebServer/main
```

If you already cloned without submodules, initialize them:

```bash
git submodule update --init --recursive
```

## Project Structure

```
├── .gitmodules                 # Git submodule configuration
├── CMakeLists.txt              # Root project configuration
├── sdkconfig                   # ESP-IDF configuration
├── components/
│   └── DevLib/                 # Git submodule: Development Board Libraries
│       ├── Common/libraries/   # Platform-agnostic libraries
│       │   ├── Counter/        # Counter implementation
│       │   └── Utils/          # Common utilities
│       └── ESP-IDF/libraries/  # ESP-IDF specific libraries
│           └── Utils/          # ESP32 task manager & utilities
├── main/
│   ├── CMakeLists.txt          # Main component configuration
│   ├── main.cpp                # Application entry point
│   ├── include/
│   │   └── main.h              # Main application header
│   ├── lib/
│   │   └── SSD1306/            # OLED display driver
│   │       ├── SSD1306.h
│   │       └── SSD1306.cpp
│   └── managed_components/
│       ├── espressif__esp_lvgl_port/
│       └── lvgl__lvgl/
└── build/                      # Build output directory
```

## External Libraries

This project uses external component libraries via git submodule from the [DevLib-DevelopmentBoardLibraries](https://github.com/Eubry/DevLib-DevelopmentBoardLibraries) repository.

### Common Libraries
Platform-agnostic libraries located at `components/DevLib/Common/libraries/`:

#### Counter Library
Custom counter and timer implementation for event tracking and non-blocking delays:
- **cCount class**: Event counting with goal-based triggers
  - `count(goal)`: Increment counter towards goal
  - `count(goal, currentFlag, nextFlag)`: Count with flag management
  - `get()`: Get current count value
  - `start()`: Check if counting has started
  - `finish()`: Check if goal reached
  - `after(callback, args...)`: Execute callback when goal is reached
  - `reset()`: Reset counter to zero
- **cTime class**: Non-blocking microsecond-precision timing
  - `wait(goalMillisec)`: Non-blocking wait in microseconds
  - `start()`: Check if timing has started
  - `finish()`: Check if wait period completed
  - `get()`: Get elapsed time in microseconds
  - `reset()`: Reset timer
- **Dependencies**: `esp_timer`

#### Utils Library
Common utility functions for general development:
- **mapValue(value, fromLow, fromHigh, toLow, toHigh)**: Map values between ranges
- **constrainValue(value, min, max)**: Constrain value within bounds
- **inMap(key, map)**: Template function to check map key existence
- Platform-agnostic design (Arduino, ESP-IDF, etc.)
- **Dependencies**: `<map>`, `<string>`, `<utility>`

### ESP-IDF Libraries
ESP32-specific libraries located at `components/DevLib/ESP-IDF/libraries/`:

#### Utils Library
ESP-IDF specific utility library providing:
- **taskManager**: FreeRTOS task management with watchdog integration
  - `add(name, taskFunc, param, priority, core, stackSize)`: Create and register tasks with automatic watchdog enrollment
  - `del(name)`: Delete tasks and cleanup watchdog subscription
  - `resetWatchdog(name)`: Reset watchdog timer for specific tasks
  - `statusTask()`: Internal task creation status logging
- **Helper Functions**: Template-based map utilities (`inMap()`)
- **Dependencies**: `freertos`, `esp_system`, `esp_timer`

#### wifiManager Library
Comprehensive WiFi management for ESP32:
- **wifiConnection class**: Complete WiFi station mode support
  - `begin(mode)`: Initialize WiFi with specified mode (default: WIFI_MODE_STA)
  - `isConnected()`: Check connection status
  - `getIp()`: Retrieve assigned IP address as string
  - `getHostname()`: Get mDNS hostname
  - `stop()`: Disconnect and cleanup WiFi
  - Automatic retry mechanism with configurable max retries
  - Event-driven connection handling
  - mDNS hostname resolution support
- **Dependencies**: `esp_wifi`, `esp_netif`, `esp_event`, `nvs_flash`, `mdns`, `Counter`

#### pinManager Library
Hardware pin abstraction and management:
- **Digital I/O**: 
  - `digitalPin(name, pin, mode)`: Configure digital GPIO pins
  - `digitalRead(name)`: Read digital pin state
  - `digitalWrite(name, value)`: Set digital pin output
- **PWM Control**:
  - `pwmPin(name, pin, frequency, timer, duty_resolution)`: Configure PWM output
  - `setPwmDuty(name, duty)`: Set PWM duty cycle (raw value)
  - `setPwmDutyPercent(name, percent)`: Set PWM duty as percentage
  - `setPwmDutyMicros(name, micros)`: Set PWM duty in microseconds
  - `setPwmFrequency(name, frequency)`: Change PWM frequency
- **Audio Functions**:
  - `tone(name, frequency, volume, duration_ms)`: Generate tone with optional duration
  - `noTone(name)`: Stop tone generation
  - `update()`: Must be called regularly to handle timed tones
- **Dependencies**: `driver/gpio`, `driver/ledc`, `Counter`

### Updating Libraries

To update the DevLib submodule to the latest version:
```bash
git submodule update --remote components/DevLib
git add components/DevLib
git commit -m "Update DevLib to latest version"
```

To check the current submodule status:
```bash
git submodule status
```

## Configuration

### WiFi Settings

WiFi credentials are currently hardcoded in `main.cpp` for testing purposes:

```cpp
#define WIFI_SSID "yourSSID"          // Your WiFi network name
#define WIFI_PASS "yourPassword"   // Your WiFi password
```

**To configure for your network**, edit these values in [main/main.cpp](main/main.cpp#L17-L18).

The WiFi connection is initialized with:
```cpp
wifiConnection wConn(WIFI_SSID, WIFI_PASS, 5, "ESP32S3WS");
// Parameters: SSID, Password, Max Retries (5), Hostname ("ESP32S3WS")
```

**Alternative**: You can also use menuconfig for configuration:
```bash
idf.py menuconfig
```
Navigate to `Example Configuration` and uncomment the CONFIG macros in main.cpp.

### Display Settings

Current configuration in `main.cpp`:
```cpp
oledParams.pinSCL = 13;  // I2C Clock
oledParams.pinSDA = 12;  // I2C Data
```

Modify these values if using different GPIO pins.

## Building and Flashing

### Clean Build
```bash
idf.py fullclean
idf.py build
```

### Flash to Device
```bash
idf.py -p COM11 flash
```
*Replace COM11 with your device's serial port*

### Monitor Output
```bash
idf.py -p COM11 monitor
```

### Build, Flash, and Monitor (Combined)
```bash
idf.py -p COM11 flash monitor
```

To exit the monitor, press `Ctrl+]`

## Components and Dependencies

### ESP-IDF Components
- `freertos` - Real-time operating system
- `esp_common` - Common ESP32 utilities
- `log` - Logging functionality
- `driver` - Hardware drivers (I2C, GPIO, etc.)
- `esp_timer` - High-resolution timer
- `esp_lcd` - LCD/display interface
- `esp_wifi` - WiFi stack
- `nvs_flash` - Non-volatile storage
- `lvgl` - Light and Versatile Graphics Library
- `esp_lvgl_port` - LVGL port for ESP32

### Custom Components
- `Utils` - Task management and utilities
- `SSD1306` - OLED display driver

## Code Overview

### Main Application (`main.cpp`)

The application initializes hardware, creates display labels, and spawns two FreeRTOS tasks:

```cpp
extern "C" void app_main(void) {
    // Configure OLED I2C pins
    oledParams.pinSCL = 13;
    oledParams.pinSDA = 12;
    
    // Initialize display and create labels
    disp.begin(oledParams);
    disp.addLabel("stat", 0, 0);      // Program status (line 1)
    disp.addLabel("swifi", 0, 12);    // WiFi task status (line 2)
    disp.addLabel("wstat", 0, 24);    // WiFi connection status/IP (line 3)
    disp.addLabel("outloop", 0, 36);  // Hostname display (line 4)
    
    // Set initial labels
    disp.setLabel("stat", "Program started!");
    
    // Initialize WiFi in station mode
    wConn.begin(WIFI_MODE_STA);
    
    // Create tasks on different cores
    tskMgr.add("DisplayStat", DspStat, NULL, 1, 0, 4096);  // Core 0, Priority 1
    tskMgr.add("WifiConn", wifi, NULL, 6, 1, 4096);        // Core 1, Priority 6
}
```

### Task Implementation

#### Display Status Task (`DspStat`)
Runs on Core 0 with priority 1:
- Updates OLED display every 500ms
- Shows WiFi connection status
- Displays IP address when connected
- Shows device hostname
- Resets watchdog timer every 4 seconds
- Uses `cTime` class for non-blocking delays

```cpp
void DspStat(void *pvParameters) {
    static cTime rwdt;   // Watchdog reset timer
    static cTime upt;    // Update display timer
    
    while(1) {
        // Update display every 500ms
        upt.wait(500000);
        if(upt.finish()) {
            if(wConn.isConnected()) {
                disp.setLabel("wstat", wConn.getIp().c_str());
                disp.setLabel("outloop", wConn.getHostname().c_str());
            }
        }
        
        // Reset watchdog every 4 seconds
        rwdt.wait(4000000);
        if(rwdt.finish()) {
            tskMgr.resetWatchdog("DisplayStat");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

#### WiFi Connection Task (`wifi`)
Runs on Core 1 with priority 6:
- Monitors WiFi connection
- Handles reconnection if needed
- Resets watchdog timer every 4 seconds
- Higher priority for reliable network connectivity

### Task Management

The `taskManager` class provides:
- **Automatic watchdog registration**: Tasks enrolled in ESP32 task watchdog on creation
- **Dual-core task assignment**: Pin tasks to specific CPU cores (0 or 1)
- **Priority management**: FreeRTOS priority levels (0-configMAX_PRIORITIES)
- **Task lifecycle management**: Create, delete, and monitor tasks
- **Error logging**: Automatic status reporting and error handling

Example usage:
```cpp
// Create task on Core 0, Priority 1, 4KB stack
tskMgr.add("TaskName", taskFunction, NULL, 1, 0, 4096);

// Reset watchdog from within task
tskMgr.resetWatchdog("TaskName");

// Delete task when done
tskMgr.del("TaskName");
```

### WiFi Connection Manager

The `wifiConnection` class handles WiFi connectivity:
```cpp
// Initialize with SSID, password, max retries, and hostname
wifiConnection wConn(WIFI_SSID, WIFI_PASS, 5, "ESP32S3WS");

// Start WiFi in station mode
wConn.begin(WIFI_MODE_STA);

// Check connection status
if(wConn.isConnected()) {
    std::string ip = wConn.getIp();            // Get IP address
    std::string hostname = wConn.getHostname(); // Get mDNS hostname
}
```

### OLED Display Driver

The `OLEDDisplay` class uses LVGL for graphics:
```cpp
// Configure display parameters
oledParam_t oledParams;
oledParams.pinSCL = 13;  // I2C Clock pin
oledParams.pinSDA = 12;  // I2C Data pin

// Initialize display
OLEDDisplay disp;
disp.begin(oledParams);

// Create and update labels
disp.addLabel("status", 0, 0);           // Create at position (0,0)
disp.setLabel("status", "Hello World");  // Update text
disp.setLabel("value", 123);             // Update with integer
```

## Development Notes

### CMake Configuration

External component directories are configured via git submodule in the root `CMakeLists.txt`:
```cmake
set(EXTRA_COMPONENT_DIRS 
    "${CMAKE_CURRENT_SOURCE_DIR}/components/DevLib/Common/libraries"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/DevLib/ESP-IDF/libraries"
)
```

This uses the [DevLib-DevelopmentBoardLibraries](https://github.com/Eubry/DevLib-DevelopmentBoardLibraries) repository as a git submodule, ensuring portable and version-controlled external dependencies.

The project also uses managed components:
```cmake
# Managed components (auto-downloaded by ESP-IDF)
- espressif__esp_lvgl_port
- espressif__mdns  
- lvgl__lvgl
```

### Memory Usage

Check memory usage after building:
```bash
idf.py size
```

Monitor free heap at runtime:
```cpp
ESP_LOGI(TAG, "Free heap: %lu bytes", esp_get_free_heap_size());
```

## Troubleshooting

### Submodule Issues

**Problem**: Components not found after cloning
```
Failed to resolve component 'Utils'
```
**Solution**: Initialize and update submodules:
```bash
git submodule update --init --recursive
```

**Problem**: Submodule out of sync
**Solution**: Update to latest version:
```bash
git submodule update --remote components/DevLib
```

### Build Issues

**Problem**: Component not found
```
Failed to resolve component 'Utils', 'wifiManager', or 'pinManager'
```
**Solution**: Ensure submodules are initialized and CMakeLists.txt is properly configured.

**Problem**: Missing dependencies
```
fatal error: esp_timer.h: No such file or directory
```
**Solution**: Add missing component to `REQUIRES` in component's CMakeLists.txt

**Problem**: Managed components not found (lvgl, mdns)
```
Failed to resolve component 'espressif__mdns'
```
**Solution**: Run dependency manager to download managed components:
```bash
idf.py reconfigure
```

### Flash Issues

**Problem**: Device not detected
```
Error: Could not open port COM11
```
**Solution**: Check USB connection, verify correct port, install CH340/CP2102 drivers

### Display Issues

**Problem**: OLED not displaying
- Verify I2C connections (SCL=13, SDA=12)
- Check 3.3V power supply
- Verify I2C address (default: 0x3C)

### WiFi Issues

**Problem**: Connection fails
- Verify SSID and password in [main/main.cpp](main/main.cpp#L17-L18)
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP32-S3 doesn't support 5GHz)
- Monitor serial output for connection attempts and error messages

**Problem**: WiFi connects but no IP address
- Check DHCP server on your router
- Verify network subnet settings
- Check for MAC address filtering on router

## Example Output

```
I (328) boot: ESP-IDF v6.0 2nd stage bootloader
I (328) boot: compile time Dec 25 2025 14:32:10
I (345) boot: Loaded app from partition at offset 0x10000
I (367) main: Program started!
I (367) OLED: Initializing SSD1306 display...
I (412) OLED: Display initialized successfully
I (412) TASK_MANAGER: DisplayStat Task created successfully
I (423) TASK_MANAGER: WifiConn Task created successfully
I (434) wifi: WiFi initializing...
I (445) wifi: Connecting to SSID: R3X4S
I (1523) wifi: WiFi connected!
I (1523) wifi: IP Address: 192.168.1.100
I (1534) wifi: Hostname: ESP32S3WS.local
I (2328) MAIN: OLED Status Update
I (2828) MAIN: OLED Status Update
```

**OLED Display Output:**
```
Line 1: OLED Started
Line 2: Wifi Started  
Line 3: 192.168.1.100
Line 4: ESP32S3WS.local
```

## Current Implementation Status

✅ **Completed Features:**
- WiFi station mode connectivity
- OLED display with LVGL graphics
- Dual-core FreeRTOS task management
- Task watchdog integration
- mDNS hostname support
- Non-blocking timers and counters
- Pin manager for GPIO/PWM control
- Modular library architecture

## Future Development

🚧 **Planned Features:**
- [ ] HTTP web server endpoints for device control
- [ ] RESTful API for remote monitoring
- [ ] WiFi AP (Access Point) mode support
- [ ] Web-based configuration interface
- [ ] OTA (Over-The-Air) firmware updates
- [ ] Sensor data collection and logging
- [ ] WebSocket support for real-time updates
- [ ] MQTT client integration
- [ ] NVS (Non-Volatile Storage) for persistent configuration
- [ ] Advanced OLED UI with menus and user input

## Resources

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/index.html)
- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [FreeRTOS Documentation](https://www.freertos.org/index.html)
- [LVGL Documentation](https://docs.lvgl.io/)
- [DevLib Libraries Repository](https://github.com/Eubry/DevLib-DevelopmentBoardLibraries)

## Related Repositories

- [ESP32_S3-WebServer](https://github.com/Eubry/ESP32_S3-WebServer) - This project
- [DevLib-DevelopmentBoardLibraries](https://github.com/Eubry/DevLib-DevelopmentBoardLibraries) - Shared development libraries

## License

This project is in the Public Domain (or CC0 licensed, at your option.)

## Contributing

Issues and pull requests are welcome at the [GitHub repository](https://github.com/Eubry/ESP32_S3-WebServer).

## Author

Eubry - [GitHub Profile](https://github.com/Eubry)
