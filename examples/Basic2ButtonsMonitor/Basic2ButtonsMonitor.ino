/*
 * Basic2ButtonsMonitor
 * 
 * Description:
 * This example demonstrates how to monitor two pins for single press, double press, and long press events
 * using the AvantDigitalRead library. The program monitors BUTTON1_PIN (default pin 5) and BUTTON2_PIN (default pin 12)
 * for button presses and calls separate callback functions to output information through
 * the serial monitor when single press, double press, or long press events occur on either button.
 * 
 * Author: AvantMaker <admin@avantmaker.com>
 * Author Website: `https://www.AvantMaker.com` 
 * Date: 2025-09-21
 * Version: 0.0.1
 * 
 * Hardware Requirements:
 * - ESP32-based microcontroller (e.g., ESP32 DevKitC, DOIT ESP32 DevKit, etc.)
 * - Two buttons or switches connected to BUTTON1_PIN and BUTTON2_PIN
 * 
 * Dependencies:
 * - AvantDigitalRead library
 * 
 * 
 * Usage Notes:
 * 1. BUTTON CONNECTION:
 *    - Connect one terminal of the first button to the BUTTON1_PIN (default pin 5)
 *    - Connect the other terminal of the first button to GROUND (GND)
 *    - Connect one terminal of the second button to the BUTTON2_PIN (default pin 12)
 *    - Connect the other terminal of the second button to GROUND (GND)
 *    - No external pull-up resistors are needed because we use INPUT_PULLUP
 * 
 * 2. HOW INPUT_PULLUP WORKS:
 *    - INPUT_PULLUP enables the internal pull-up resistor in the ESP32
 *    - This keeps the pins at HIGH state when the buttons are not pressed
 *    - When a button is pressed, it connects the pin to GROUND, changing the state to LOW
 *    - This means: 
 *        * Button NOT pressed = Pin state HIGH
 *        * Button PRESSED = Pin state LOW
 *    - This configuration prevents the pins from "floating" when the buttons are not pressed
 *    - In this example:
 *        * SINGLE PRESS occurs when a button is pressed and released once
 *        * DOUBLE PRESS occurs when a button is pressed and released twice in quick succession
 *        * LONG PRESS occurs when a button is held down for a longer duration (default 1000ms)
 * 
 * 3. UPLOAD AND USAGE:
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200)
 *    - Press either button once to see single press notifications
 *    - Press either button twice quickly to see double press notifications
 *    - Press and hold either button for about 1 second to see long press notifications
 *    - You can change the BUTTON1_PIN and BUTTON2_PIN values to use different pins
 *
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantDigitalRead.h"

// Define the pins to monitor
#define BUTTON1_PIN 5
#define BUTTON2_PIN 12

// Create an instance of AvantDigitalRead
AvantDigitalRead pinManager;

// Callback function for single press events on button 1
void button1SinglePressCallback(int pin, PinState newState, PinState oldState, 
                               EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Button 1 (Pin ");
  Serial.print(pin);
  Serial.print(") SINGLE PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button 1 pressed and released once");
}

// Callback function for double press events on button 1
void button1DoublePressCallback(int pin, PinState newState, PinState oldState, 
                               EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Button 1 (Pin ");
  Serial.print(pin);
  Serial.print(") DOUBLE PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button 1 pressed and released twice in quick succession");
}

// Callback function for long press events on button 1
void button1LongPressCallback(int pin, PinState newState, PinState oldState, 
                              EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Button 1 (Pin ");
  Serial.print(pin);
  Serial.print(") LONG PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button 1 held down for extended duration");
}

// Callback function for single press events on button 2
void button2SinglePressCallback(int pin, PinState newState, PinState oldState, 
                               EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Button 2 (Pin ");
  Serial.print(pin);
  Serial.print(") SINGLE PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button 2 pressed and released once");
}

// Callback function for double press events on button 2
void button2DoublePressCallback(int pin, PinState newState, PinState oldState, 
                               EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Button 2 (Pin ");
  Serial.print(pin);
  Serial.print(") DOUBLE PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button 2 pressed and released twice in quick succession");
}

// Callback function for long press events on button 2
void button2LongPressCallback(int pin, PinState newState, PinState oldState, 
                              EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Button 2 (Pin ");
  Serial.print(pin);
  Serial.print(") LONG PRESS detected: ");
  Serial.print(oldState);
  Serial.print(" -> ");
  Serial.print(newState);
  
  // Print timestamp
  Serial.print(" at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Additional information
  Serial.println("  Event: Button 2 held down for extended duration");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("Basic2ButtonsMonitor Example Starting...");
  Serial.print("Monitoring pins for button presses: ");
  Serial.print(BUTTON1_PIN);
  Serial.print(" and ");
  Serial.println(BUTTON2_PIN);
  Serial.println("Press either button to see single/double/long press notifications.");
  Serial.println("----------------------------------------");
  
  // Initialize the first pin to monitor
  if (pinManager.addPin(BUTTON1_PIN, INPUT_PULLUP)) {
    Serial.print("Button 1 (Pin ");
    Serial.print(BUTTON1_PIN);
    Serial.println(") initialized successfully");
  } else {
    Serial.print("Failed to initialize Button 1 (Pin ");
    Serial.print(BUTTON1_PIN);
    Serial.println(")");
    while (1) {
      delay(100); // Halt execution if pin initialization fails
    }
  }
  
  // Initialize the second pin to monitor
  if (pinManager.addPin(BUTTON2_PIN, INPUT_PULLUP)) {
    Serial.print("Button 2 (Pin ");
    Serial.print(BUTTON2_PIN);
    Serial.println(") initialized successfully");
  } else {
    Serial.print("Failed to initialize Button 2 (Pin ");
    Serial.print(BUTTON2_PIN);
    Serial.println(")");
    while (1) {
      delay(100); // Halt execution if pin initialization fails
    }
  }
  
  // Set debounce time to prevent false triggers (30 milliseconds)
  pinManager.setDebounceTime(BUTTON1_PIN, 30);
  pinManager.setDebounceTime(BUTTON2_PIN, 30);
  
  // Set click parameters (optional, using default values here)
  // pinManager.setClickParameters(BUTTON1_PIN, 50, 300, 500);
  // pinManager.setClickParameters(BUTTON2_PIN, 50, 300, 500);
  // Parameters: minPressMs, maxPressMs, maxIntervalMs
  
  // Set long press duration (optional, default is 1000ms)
  // pinManager.setLongPressDuration(BUTTON1_PIN, 1000);
  // pinManager.setLongPressDuration(BUTTON2_PIN, 1000);
  
  // Register the callback functions for button 1
  pinManager.onSinglePress(BUTTON1_PIN, button1SinglePressCallback);
  pinManager.onDoublePress(BUTTON1_PIN, button1DoublePressCallback);
  pinManager.onLongPress(BUTTON1_PIN, button1LongPressCallback);
  
  // Register the callback functions for button 2
  pinManager.onSinglePress(BUTTON2_PIN, button2SinglePressCallback);
  pinManager.onDoublePress(BUTTON2_PIN, button2DoublePressCallback);
  pinManager.onLongPress(BUTTON2_PIN, button2LongPressCallback);
  
  Serial.println("Ready for input.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}