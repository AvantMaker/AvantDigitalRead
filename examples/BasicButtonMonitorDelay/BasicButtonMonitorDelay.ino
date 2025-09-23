/*
 * BasicButtonMonitorDelay
 * 
 * Description:
 * This example demonstrates how to use the delayed callback feature of the AvantDigitalRead library.
 * The program monitors the TEST_PIN (default pin 5) for button presses and calls separate callback 
 * functions with a 3000ms delay when single press or double press events occur. This allows for 
 * delayed execution of actions, which can be useful in scenarios where you want to wait before 
 * triggering a response, such as debounce confirmation or multi-step interactions.
 * 
 * Author: AvantMaker <admin@avantmaker.com>
 * Author Website: `https://www.AvantMaker.com` 
 * Date: 2025-09-21
 * Version: 0.0.1
 * 
 * Hardware Requirements:
 * - ESP32-based microcontroller (e.g., ESP32 DevKitC, DOIT ESP32 DevKit, etc.)
 * - A button or switch connected to TEST_PIN
 * 
 * Dependencies:
 * - AvantDigitalRead library
 * 
 * 
 * Usage Notes:
 * 1. BUTTON CONNECTION:
 *    - Connect one terminal of the button to the TEST_PIN (default pin 5)
 *    - Connect the other terminal of the button to GROUND (GND)
 *    - No external pull-up resistor is needed because we use INPUT_PULLUP
 * 
 * 2. HOW INPUT_PULLUP WORKS:
 *    - INPUT_PULLUP enables the internal pull-up resistor in the ESP32
 *    - This keeps the pin at HIGH state when the button is not pressed
 *    - When the button is pressed, it connects the pin to GROUND, changing the state to LOW
 *    - This means: 
 *        * Button NOT pressed = Pin state HIGH
 *        * Button PRESSED = Pin state LOW
 *    - This configuration prevents the pin from "floating" when the button is not pressed
 *    - In this example:
 *        * SINGLE PRESS occurs when the button is pressed and released once, with callback delayed by 3000ms
 *        * DOUBLE PRESS occurs when the button is pressed and released twice in quick succession, with callback delayed by 3000ms
 *        * LONG PRESS occurs when the button is held down for a longer duration (default 1000ms), with immediate callback
 * 
 * 3. DELAYED CALLBACK FEATURE:
 *    - The onSinglePress and onDoublePress callbacks are set with a 3000ms delay
 *    - This means the callback functions will be executed 3 seconds after the actual button event
 *    - The delay parameter is the third argument in the onSinglePress and onDoublePress methods
 *    - This feature is useful for:
 *        * Implementing confirmation delays
 *        * Creating multi-step interactions
 *        * Debouncing complex button sequences
 *        * Adding a "grace period" for user actions
 * 
 * 4. UPLOAD AND USAGE:
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200)
 *    - Press the button once to see single press notification after 3 seconds
 *    - Press the button twice quickly to see double press notification after 3 seconds
 *    - Press and hold the button for about 1 second to see immediate long press notification
 *    - You can change the TEST_PIN value to use a different pin
 *    - You can modify the delayMs parameter to change the delay duration
 *
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

// Enable debug output for delayed callbacks
#define DEBUG_DELAYED_CALLBACKS

#include "AvantDigitalRead.h"

// Define the pin to monitor
#define TEST_PIN 5

// Create an instance of AvantDigitalRead
AvantDigitalRead pinManager;

// Callback function for single press events (with 3000ms delay)
void singlePressCallback(int pin, PinState newState, PinState oldState, 
                        EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" SINGLE PRESS detected (DELAYED): ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button pressed and released once (callback delayed by 3000ms)");
  Serial.println("  Note: This callback was executed 3 seconds after the actual button press");
}

// Callback function for double press events (with 3000ms delay)
void doublePressCallback(int pin, PinState newState, PinState oldState, 
                        EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" DOUBLE PRESS detected (DELAYED): ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button pressed and released twice in quick succession (callback delayed by 3000ms)");
  Serial.println("  Note: This callback was executed 3 seconds after the actual button press sequence");
}

// Callback function for long press events (immediate, no delay)
void longPressCallback(int pin, PinState newState, PinState oldState, 
                      EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" LONG PRESS detected (IMMEDIATE): ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button held down for extended duration (callback executed immediately)");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("BasicButtonDelay Example Starting...");
  Serial.print("Monitoring pin for button presses with delayed callbacks: ");
  Serial.println(TEST_PIN);
  Serial.println("Single and double press callbacks are delayed by 3000ms.");
  Serial.println("Long press callback is executed immediately.");
  Serial.println("Debug output for delayed callbacks is enabled.");
  Serial.println("----------------------------------------");
  
  // Initialize the pin to monitor
  if (pinManager.addPin(TEST_PIN, INPUT_PULLUP)) {
    Serial.println("Pin initialized successfully");
  } else {
    Serial.println("Failed to initialize pin");
    while (1) {
      delay(100); // Halt execution if pin initialization fails
    }
  }
  
  // Set debounce time to prevent false triggers (30 milliseconds)
  pinManager.setDebounceTime(TEST_PIN, 30);
  
  // Set click parameters (optional, using default values here)
  // pinManager.setClickParameters(TEST_PIN, 50, 300, 500);
  // Parameters: minPressMs, maxPressMs, maxIntervalMs
  
  // Set long press duration (optional, default is 1000ms)
  // pinManager.setLongPressDuration(TEST_PIN, 1000);
  
  // Register the callback functions for single, double, and long press events
  // Note the 3000ms delay parameter for single and double press callbacks
  pinManager.onSinglePress(TEST_PIN, singlePressCallback, 3000);  // 3000ms delay
  pinManager.onDoublePress(TEST_PIN, doublePressCallback, 3000);  // 3000ms delay
  pinManager.onLongPress(TEST_PIN, longPressCallback);            // No delay (immediate)
  
  Serial.println("Ready for input.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}