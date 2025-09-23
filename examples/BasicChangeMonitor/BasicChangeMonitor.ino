/*
 * BasicChangeMonitor
 * 
 * Description:
 * This example demonstrates how to monitor a pin for state changes using the AvantDigitalRead library.
 * The program monitors the TEST_PIN (default pin 5) for any state changes and calls a callback
 * function to output information through the serial monitor when changes occur.
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
 * 3. UPLOAD AND USAGE:
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200)
 *    - Press the button to see pin change notifications
 *    - You can change the TEST_PIN value to use a different pin
 *
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantDigitalRead.h"

// Define the pin to monitor
#define TEST_PIN 5

// Create an instance of AvantDigitalRead
AvantDigitalRead pinManager;

// Callback function for pin state changes
void pinChangeCallback(int pin, PinState newState, PinState oldState, 
                      EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" state changed from ");
  Serial.print(oldState);
  Serial.print(" to ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information based on the event type
  switch (event) {
    case EVENT_RISING:
      Serial.println("  Event: Rising edge (LOW to HIGH)");
      break;
    case EVENT_FALLING:
      Serial.println("  Event: Falling edge (HIGH to LOW)");
      break;
    case EVENT_CHANGE:
      Serial.println("  Event: General state change");
      break;
    default:
      Serial.println("  Event: Other event type");
      break;
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("PinChangeMonitor Example Starting...");
  Serial.print("Monitoring pin: ");
  Serial.println(TEST_PIN);
  Serial.println("Press the button to see pin change notifications.");
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
  
  // Register the callback function for pin state changes
  pinManager.onChange(TEST_PIN, pinChangeCallback);
  
  Serial.println("Ready for input.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}