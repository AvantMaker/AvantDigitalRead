/*
 * ButtonConfig
 * 
 * Description:
 * This example demonstrates how to configure button parameters using the setClickParameters function
 * and callback registration functions in the AvantDigitalRead library. The program shows how to 
 * customize the behavior of single press, double press, and long press events by adjusting timing 
 * parameters. It monitors the TEST_PIN (default pin 5) for button presses and calls separate callback 
 * functions to output information through the serial monitor when different button events occur.
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
 * 3. CLICK PARAMETERS EXPLANATION:
 *    - setClickParameters(pin, minPressMs, maxPressMs) configures basic button press timing
 *    - minPressMs: Minimum press time in milliseconds to register as a valid press
 *                 * Too short: May miss quick presses
 *                 * Too long: Requires deliberate button presses
 *                 * Default: 50ms (good for most buttons)
 *    
 *    - maxPressMs: Maximum press time in milliseconds to be considered a normal press (not a long press)
 *                 * Presses longer than this will not be considered for single/double press detection
 *                 * Too short: Normal presses might be treated as long presses
 *                 * Too long: Long presses require very long button holds
 *                 * Default: 300ms (good balance between responsiveness and long press detection)
 *    
 *    - Note: maxIntervalMs for double press is set in the onDoublePress function
 *    - Note: pressDurationMs for long press is set in the onLongPress function
 * 
 * 4. CALLBACK FUNCTION PARAMETERS:
 *    - onSinglePress(pin, callback, delayMs): Registers single press callback
 *                 * delayMs: Optional delay before triggering the callback
 *    
 *    - onDoublePress(pin, callback, delayMs, maxIntervalMs): Registers double press callback
 *                 * delayMs: Optional delay before triggering the callback
 *                 * maxIntervalMs: Maximum time between presses to be considered a double press
 *                               * Default: 300ms
 *                               * Too short: Difficult to perform double presses quickly enough
 *                               * Too long: Single presses might be interpreted as first part of double press
 *    
 *    - onLongPress(pin, callback, delayMs, pressDurationMs, repeat): Registers long press callback
 *                 * delayMs: Optional delay before triggering the callback
 *                 * pressDurationMs: Time in milliseconds to trigger long press
 *                                  * Default: 1000ms (1 second)
 *                                  * Adjust based on your application needs
 *                 * repeat: Whether to repeatedly trigger while button is held down
 *                         * Default: false
 * 
 * 5. UPLOAD AND USAGE:
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200)
 *    - Try different button press patterns to see how the parameters affect event detection
 *    - Experiment with different parameter values to understand their impact
 *    - You can change the TEST_PIN value to use a different pin
 *
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantDigitalRead.h"

// Define the pin to monitor
#define TEST_PIN 5

// Create an instance of AvantDigitalRead
AvantDigitalRead pinManager;

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

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("ButtonConfig Example Starting...");
  Serial.print("Monitoring pin for button presses: ");
  Serial.println(TEST_PIN);
  Serial.println("This example demonstrates button parameter configuration.");
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
  
  // Configure click parameters with detailed explanations
  Serial.println("Configuring button parameters:");
  
  // Set click parameters with custom values
  // Parameters: pin, minPressMs, maxPressMs
  pinManager.setClickParameters(TEST_PIN, 50, 300);
  
  // Explanation of parameters:
  Serial.println("  setClickParameters(TEST_PIN, 50, 300);");
  Serial.println("    - minPressMs: 50ms - Minimum time to register a valid press");
  Serial.println("      * Prevents very short accidental presses from being registered");
  Serial.println("      * 50ms is a good balance between responsiveness and noise filtering");
  Serial.println("    - maxPressMs: 300ms - Maximum time for a normal press");
  Serial.println("      * Presses longer than this won't be considered for single/double press");
  Serial.println("      * Helps distinguish between normal presses and long presses");
  
  // Note: maxIntervalMs for double press is set in onDoublePress function
  // Note: pressDurationMs for long press is set in onLongPress function
  
  Serial.println("----------------------------------------");
  
  // Register the callback functions for single, double, and long press events
  // Each registration can include additional parameters if needed
  
  // Register single press callback
  pinManager.onSinglePress(TEST_PIN, singlePressCallback);
  Serial.println("Registered single press callback");
  Serial.println("  - Will trigger when button is pressed and released once");
  Serial.println("  - Press time must be between minPressMs (50ms) and maxPressMs (300ms)");
  Serial.println("  - Parameters: pin, callback, delayMs (optional)");
  
  // Register double press callback with maxIntervalMs parameter
  pinManager.onDoublePress(TEST_PIN, doublePressCallback, 0, 500);
  Serial.println("Registered double press callback");
  Serial.println("  - Will trigger when button is pressed twice in quick succession");
  Serial.println("  - Each press must be between minPressMs (50ms) and maxPressMs (300ms)");
  Serial.println("  - Time between presses must be less than maxIntervalMs (500ms)");
  Serial.println("  - Parameters: pin, callback, delayMs (optional), maxIntervalMs");
  
  // Register long press callback with pressDurationMs parameter
  pinManager.onLongPress(TEST_PIN, longPressCallback, 0, 1000);
  Serial.println("Registered long press callback");
  Serial.println("  - Will trigger when button is held down for pressDurationMs (1000ms)");
  Serial.println("  - Long press detection is independent of single/double press detection");
  Serial.println("  - Parameters: pin, callback, delayMs (optional), pressDurationMs, repeat (optional)");
  
  Serial.println("----------------------------------------");
  Serial.println("Ready for input. Try different button press patterns!");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}