/*
 * BasicRiseFallMonitorDelay
 * 
 * Description:
 * This example demonstrates how to use the delayed callback feature of the AvantDigitalRead library
 * with the onRise and onFall functions. The program monitors the TEST_PIN (default pin 5) for rising
 * and falling edge transitions and calls separate callback functions with a 2000ms delay when these
 * events occur. This allows for delayed execution of actions, which can be useful in scenarios where
 * you want to wait before triggering a response, such as debounce confirmation or multi-step interactions.
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
 *        * FALLING EDGE occurs when the button is pressed (HIGH to LOW, callback delayed by 2000ms)
 *        * RISING EDGE occurs when the button is released (LOW to HIGH, callback delayed by 2000ms)
 * 
 * 3. DELAYED CALLBACK FEATURE:
 *    - The onRise and onFall callbacks are set with a 2000ms delay
 *    - This means the callback functions will be executed 2 seconds after the actual edge event
 *    - The delay parameter is the third argument in the onRise and onFall methods
 *    - This feature is useful for:
 *        * Implementing confirmation delays
 *        * Creating multi-step interactions
 *        * Debouncing complex button sequences
 *        * Adding a "grace period" for user actions
 * 
 * 4. UPLOAD AND USAGE:
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200)
 *    - Press the button to see rising and falling edge notifications after 2 seconds
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

// Callback function for rising edge events (with 2000ms delay)
void pinRisingCallback(int pin, PinState newState, PinState oldState, 
                      EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" RISING EDGE detected (DELAYED): ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button released (LOW to HIGH transition, callback delayed by 2000ms)");
  Serial.println("  Note: This callback was executed 2 seconds after the actual rising edge");
}

// Callback function for falling edge events (with 2000ms delay)
void pinFallingCallback(int pin, PinState newState, PinState oldState, 
                       EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" FALLING EDGE detected (DELAYED): ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button pressed (HIGH to LOW transition, callback delayed by 2000ms)");
  Serial.println("  Note: This callback was executed 2 seconds after the actual falling edge");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("BasicRiseFallMonitorDelay Example Starting...");
  Serial.print("Monitoring pin for rising/falling edges with delayed callbacks: ");
  Serial.println(TEST_PIN);
  Serial.println("All rising and falling edge callbacks are delayed by 2000ms.");
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
  
  // Register the callback functions for rising and falling edge events with 2000ms delay
  pinManager.onRising(TEST_PIN, pinRisingCallback, 2000);   // 2000ms delay
  pinManager.onFalling(TEST_PIN, pinFallingCallback, 2000); // 2000ms delay
  
  Serial.println("Ready for input.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}