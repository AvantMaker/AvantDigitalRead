/*
 * BasicChangeMonitorDelay
 * 
 * Description:
 * This example demonstrates how to use the delayed callback feature of the AvantDigitalRead library
 * with the onChange function. The program monitors the TEST_PIN (default pin 5) for any state changes
 * and calls a callback function with a 2000ms delay when changes occur. This allows for delayed
 * execution of actions, which can be useful in scenarios where you want to wait before triggering
 * a response, such as debounce confirmation or multi-step interactions.
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
 * 
 * 3. DELAYED CALLBACK FEATURE:
 *    - The onChange callback is set with a 2000ms delay
 *    - This means the callback function will be executed 2 seconds after the actual pin state change
 *    - The delay parameter is the third argument in the onChange method
 *    - This feature is useful for:
 *        * Implementing confirmation delays
 *        * Creating multi-step interactions
 *        * Debouncing complex button sequences
 *        * Adding a "grace period" for user actions
 * 
 * 4. UPLOAD AND USAGE:
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200)
 *    - Press the button to see pin change notifications after 2 seconds
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

// Callback function for pin state changes (with 2000ms delay)
void pinChangeCallback(int pin, PinState newState, PinState oldState, 
                      EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" state changed from ");
  Serial.print(oldState);
  Serial.print(" to ");
  Serial.print(newState);
  Serial.print(" (DELAYED): ");
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information based on the event type
  switch (event) {
    case EVENT_RISING:
      Serial.println("  Event: Rising edge (LOW to HIGH, callback delayed by 2000ms)");
      break;
    case EVENT_FALLING:
      Serial.println("  Event: Falling edge (HIGH to LOW, callback delayed by 2000ms)");
      break;
    case EVENT_CHANGE:
      Serial.println("  Event: General state change (callback delayed by 2000ms)");
      break;
    default:
      Serial.println("  Event: Other event type (callback delayed by 2000ms)");
      break;
  }
  
  Serial.println("  Note: This callback was executed 2 seconds after the actual pin state change");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("BasicChangeMonitorDelay Example Starting...");
  Serial.print("Monitoring pin for state changes with delayed callbacks: ");
  Serial.println(TEST_PIN);
  Serial.println("All state change callbacks are delayed by 2000ms.");
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
  
  // Register the callback function for pin state changes with 2000ms delay
  pinManager.onChange(TEST_PIN, pinChangeCallback, 2000);  // 2000ms delay
  
  Serial.println("Ready for input.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}