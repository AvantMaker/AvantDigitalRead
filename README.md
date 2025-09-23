# AvantDigitalRead

[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Language](https://img.shields.io/badge/Language-Arduino-teal.svg)](https://www.arduino.cc/)
[![AvantMaker](https://img.shields.io/badge/By-AvantMaker-red.svg)](https://www.avantmaker.com)

This project is proudly brought to you by the team at **AvantMaker.com**.

Visit us at [AvantMaker.com](https://www.avantmaker.com) where we've crafted a comprehensive collection of Reference and Tutorial materials for the ESP32, a mighty microcontroller that powers countless IoT creations.

---

![AvantMaker AvantDigitalRead - Advanced digital input management library for ESP32!](https://avantmaker.com/wp-content/uploads/2025/09/ESP32-Avant-Digital-Read-Theme-Image.jpg)

## Overview

AvantDigitalRead is an advanced digital input management library for ESP32, enhancing the standard Arduino `digitalRead` functionality. It provides multi-pin management, event detection, and gesture recognition, making it ideal for complex interactive scenarios such as user interfaces and sensor data acquisition.

## Features

- **Multi-pin Management**: Dynamically add and remove pins for unified control.
- **Built-in Debouncing**: Customizable debounce time for stable readings.
- **Rich Event Detection**: Detects state changes, rising edges, and falling edges.
- **Advanced Button Gestures**: Recognizes single-press, double-press, and long-press events with configurable timings and repeat options.
- **Delayed Callbacks**: Supports delayed execution of callback functions.
- **Non-Blocking Design**: Ensures the main program flow is not interrupted.
- **Unified Callback Format**: Simplifies code and reduces the learning curve.

## Installation

### Method 1: Install via Arduino IDE Library Manager
1. Open the Arduino IDE.
2. Go to `Tools` > `Manage Libraries...`.
3. Search for `AvantDigitalRead`.
4. Click the `Install` button.

### Method 2: Manual Installation
1. Download the ZIP file of this repository.
2. Open the Arduino IDE.
3. Go to `Sketch` > `Include Library` > `Add .ZIP Library...`.
4. Select the downloaded ZIP file.

### Method 3: Direct File Copy
Copy the `AvantDigitalRead.h` and `AvantDigitalRead.cpp` files from the `src` folder into your project folder.

## Quick Start

Here is a simple example demonstrating how to use the AvantDigitalRead library to detect button events (the button is attached to GPIO Pin 5 of ESP32):

```cpp

#include "AvantDigitalRead.h"

// Define the pin to monitor
#define BUTTON_PIN 5 

// Create an instance of AvantDigitalRead
AvantDigitalRead pinManager;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("BasicButtonMonitor Example Starting...");
  Serial.print("Monitoring pin for button presses: ");
  Serial.println(BUTTON_PIN);
  Serial.println("Press the button to see single/double/long press notifications.");
  Serial.println("----------------------------------------");
  
  // Initialize the pin to monitor
  if (pinManager.addPin(BUTTON_PIN, INPUT_PULLUP)) {
    Serial.println("Pin initialized successfully");
  } else {
    Serial.println("Failed to initialize pin");
    while (1) {
      delay(100); // Halt execution if pin initialization fails
    }
  }
  
  // Set debounce time to prevent false triggers (30 milliseconds)
  pinManager.setDebounceTime(BUTTON_PIN, 30);
  
  // Register the callback functions for single, double, and long press events
  pinManager.onSinglePress(BUTTON_PIN, singlePressCallback);
  pinManager.onDoublePress(BUTTON_PIN, doublePressCallback);
  pinManager.onLongPress(BUTTON_PIN, longPressCallback);
  
  Serial.println("Ready for input.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}


// Callback function for single press events
void singlePressCallback(int pin, PinState newState, PinState oldState, 
                        EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" SINGLE PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button pressed and released once");
}

// Callback function for double press events
void doublePressCallback(int pin, PinState newState, PinState oldState, 
                        EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" DOUBLE PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button pressed and released twice in quick succession");
}

// Callback function for long press events
void longPressCallback(int pin, PinState newState, PinState oldState, 
                      EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" LONG PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button held down for extended duration");
}
```

## API Reference

### Pin Management
- `addPin(int pin, int mode)`: Initializes a specified pin and adds it to the management list.
- `removePin(int pin)`: Removes a pin from the management list and releases its resources.
- `isInitialized(int pin)`: Checks if a pin has been initialized.
- `getPinMode(int pin)`: Gets the input mode of a specified pin.
- `readPin(int pin)`: Reads the current state of a specified pin.

### Debounce Settings
- `setDebounceTime(int pin, unsigned long debounceMs)`: Sets the debounce time for a specified pin.
- `getDebounceTime(int pin)`: Gets the debounce time setting for a specified pin.

### Event Callback Management
- `onChange(int pin, PinCallback callback, unsigned long delayMs = 0)`: Sets the callback function for pin state changes.
- `onRising(int pin, PinCallback callback, unsigned long delayMs = 0)`: Sets the callback function for rising edges.
- `onFalling(int pin, PinCallback callback, unsigned long delayMs = 0)`: Sets the callback function for falling edges.

### Button Gesture Detection
- `onSinglePress(int pin, PinCallback callback, unsigned long delayMs = 0)`: Sets the callback function for single-press detection.
- `setClickParameters(int pin, unsigned long minPressMs = 50, unsigned long maxPressMs = 300)`: Sets the parameters for single/double-press detection.
- `onDoublePress(int pin, PinCallback callback, unsigned long delayMs = 0, unsigned long maxIntervalMs = 500)`: Sets the callback function for double-press detection.
- `onLongPress(int pin, PinCallback callback, unsigned long delayMs = 0, unsigned long pressDurationMs = 1000, bool repeat = false)`: Sets the callback function for long-press detection.

### Event Management
- `enablePinEvents(int pin)`: Enables all event detection for a specified pin.
- `disablePinEvents(int pin)`: Disables all event detection for a specified pin.
- `enableAllEvents()`: Enables event detection for all initialized pins.
- `disableAllEvents()`: Disables event detection for all pins.

### Core Processing
- `update()`: Processes the state detection and event triggering for all pins. Must be called regularly in `loop()`.

## Important Notes

1. All pins must be initialized with `addPin()` before use.
2. The `update()` function must be called regularly in `loop()`, preferably with an interval of no more than 50ms.
3. Callback functions should be kept as short as possible to avoid delaying other operations.
4. For high-precision detection, it is recommended to reduce the `update()` call interval.
5. In memory-constrained environments, `removePin()` can be used to release resources for pins that are no longer needed.

## License

This library is released under the MIT License. See the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

---

💡 **Check out our other ESP32 libraries at [AvantMaker GitHub](https://github.com/avantmaker)!**
