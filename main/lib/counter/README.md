# Counter Library

A lightweight counting and timing library for ESP32 and Arduino platforms.

## Description

This library provides two main classes for managing counts and timing operations:
- **cCount**: Event counter with goal-based triggers and flag management
- **cTime**: High-precision timer using ESP32's hardware timer

## Features

- 🎯 Goal-based counting with automatic reset
- ⏱️ Microsecond precision timing using `esp_timer`
- 🚩 Boolean flag management for state transitions
- 📞 Callback support with `after()` method
- 🔄 Automatic goal change detection and counter reset
- 💾 Minimal memory footprint

## Installation

### Arduino IDE
1. Download this library as a ZIP file
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

### PlatformIO
Add to `platformio.ini`:
```ini
lib_deps = 
    file://path/to/counter
```

### ESP-IDF
Copy the `counter` folder to your project's `components` directory.

## Usage

### cCount - Event Counter

```cpp
#include <counter.h>

cCount myCounter;
bool currentState = false;
bool nextState = false;

void loop() {
    // Count to 10 and toggle flags
    myCounter.count(10, currentState, nextState);
    
    if (myCounter.finish()) {
        Serial.println("Goal reached!");
    }
    
    // Execute callback when goal is reached
    myCounter.after([]() {
        Serial.println("Counter finished!");
    });
}
```

### cTime - Precision Timer

```cpp
#include <counter.h>

cTime myTimer;

void loop() {
    // Wait for 1 second (1,000,000 microseconds)
    myTimer.wait(1000000);
    
    if (myTimer.finish()) {
        Serial.println("1 second elapsed!");
    }
    
    // Get elapsed time in microseconds
    int64_t elapsed = myTimer.get();
}
```

## API Reference

### cCount Class

#### Methods

- `void count(int goal)` - Increment counter towards goal
- `void count(int goal, bool &currentFlag, bool &nextFlag)` - Count with flag management
- `int get()` - Get current count value
- `bool start()` - Returns true when counter resets to 0
- `bool finish()` - Returns true when goal is reached
- `void after(Func callback, Args&&... args)` - Execute callback when ready
- `bool currentFlag()` - Get current flag state
- `bool nextFlag()` - Get next flag state
- `void reset()` - Reset counter to 0

### cTime Class

#### Methods

- `void wait(int64_t goalMicrosec)` - Wait for specified microseconds
- `bool start()` - Returns true when timer resets
- `bool finish()` - Returns true when goal time is reached
- `int64_t get()` - Get elapsed time in microseconds
- `void reset()` - Reset timer

## Examples

See the `examples` folder for complete working examples:
- `BasicCount` - Simple counter example
- `TimerExample` - Timing operations example
- `FlagManagement` - State machine with flags

## Compatibility

- ESP32
- ESP32-S2
- ESP32-S3
- ESP32-C3

## Author

Eubry Gomez Ramirez

## License

This library is released under the MIT License.

## Version History

- **1.0.0** (2025-01-05) - Initial release
  - cCount class for event counting
  - cTime class for precision timing
  - Flag management support
  - Callback functionality
