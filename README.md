| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# ESP32-S3 WiFi & Web Server

A comprehensive ESP32-S3 project that connects to a WiFi network, starts a web server, and displays status information on an SSD1306 OLED display.

## Features

- **WiFi Connectivity**: Connects to a configured WiFi network
- **Web Server**: HTTP server for remote control and monitoring
- **OLED Display**: Real-time status display using SSD1306 (128x64)
- **Task Management**: Custom task manager with watchdog integration
- **Modular Architecture**: Clean component-based design with external libraries

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

## Project Structure

```
├── CMakeLists.txt              # Root project configuration
├── sdkconfig                   # ESP-IDF configuration
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

This project uses external component libraries located at:
- `C:/Users/eubry/OneDrive/Documents/Programs/Protoboard/ESP32/ESPIDF/Libraries/Common/libraries`
- `C:/Users/eubry/OneDrive/Documents/Programs/Protoboard/ESP32/ESPIDF/Libraries/ESP-IDF/libraries`

### Utils Library
Custom utility library providing:
- **taskManager**: FreeRTOS task management with watchdog integration
  - `add()`: Create and register tasks with watchdog
  - `del()`: Delete tasks and cleanup
  - `resetWatchdog()`: Reset watchdog timer for tasks
- **Helper Functions**: Template-based map utilities

### Counter Library
Custom counter implementation for event tracking and statistics.

## Configuration

### WiFi Settings

Configure WiFi credentials using the ESP-IDF menuconfig:

```bash
idf.py menuconfig
```

Navigate to `Example Configuration` and set:
- **WiFi SSID**: Your network name
- **WiFi Password**: Your network password

Or edit `sdkconfig` directly:
```
CONFIG_ESP_WIFI_SSID="your_ssid"
CONFIG_ESP_WIFI_PASSWORD="your_password"
```

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

```cpp
extern "C" void app_main(void) {
    // Initialize OLED display
    oledParams.pinSCL = 13;
    oledParams.pinSDA = 12;
    disp.begin(oledParams);
    
    // Create display labels
    disp.addLabel("stat", 0, 0);
    disp.setLabel("stat", "Program started!");
    
    // Initialize tasks
    tskMgr.add("Display status", DspStat, NULL, 1, 0, 2000);
}
```

### Task Management

The `taskManager` class provides:
- Automatic watchdog registration
- Task lifecycle management
- Error logging and status reporting

Example usage:
```cpp
tskMgr.add("TaskName", TaskFunction, NULL, priority, core, stackSize);
tskMgr.resetWatchdog("TaskName");
tskMgr.del("TaskName");
```

## Development Notes

### CMake Configuration

The project uses `MINIMAL_BUILD` mode to reduce binary size:
```cmake
idf_build_set_property(MINIMAL_BUILD ON)
```

External component directories are configured in the root `CMakeLists.txt`:
```cmake
set(EXTRA_COMPONENT_DIRS 
    "C:/Users/eubry/OneDrive/Documents/Programs/Protoboard/ESP32/ESPIDF/Libraries/Common/libraries"
    "C:/Users/eubry/OneDrive/Documents/Programs/Protoboard/ESP32/ESPIDF/Libraries/ESP-IDF/libraries"
)
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

### Build Issues

**Problem**: Component not found
```
Failed to resolve component 'Utils'
```
**Solution**: Ensure external library paths are correct and CMakeLists.txt files exist in component directories.

**Problem**: Missing dependencies
```
fatal error: esp_timer.h: No such file or directory
```
**Solution**: Add missing component to `REQUIRES` in component's CMakeLists.txt

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
- Verify SSID and password in menuconfig
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP32-S3 doesn't support 5GHz)

## Example Output

```
I (328) MAIN: Program started!
I (328) TASK_MANAGER: Display status Task created successfully
I (1328) MAIN: OLED Started...
I (2328) MAIN: OLED Started...
```

## Future Development

- [ ] Implement web server endpoints
- [ ] Add WiFi event handlers
- [ ] Implement OTA (Over-The-Air) updates
- [ ] Add sensor data collection
- [ ] Implement RESTful API
- [ ] Add web-based configuration interface

## Resources

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/index.html)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [FreeRTOS Documentation](https://www.freertos.org/index.html)
- [LVGL Documentation](https://docs.lvgl.io/)

## License

This project is in the Public Domain (or CC0 licensed, at your option.)

## Contributing

Issues and pull requests are welcome at the [GitHub repository](https://github.com/Eubry/ESP32_S3-WebServer).

## Author

Eubry - [GitHub Profile](https://github.com/Eubry)
