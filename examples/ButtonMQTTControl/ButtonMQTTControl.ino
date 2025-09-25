/*
 * ButtonMQTTControl
 * 
 * Description:
 * This example demonstrates how to monitor a button using press detection callbacks,
 * and send control commands to an MQTT server based on different press types. The program 
 * monitors pin 5 for single press and long press events from a button using dedicated 
 * onSinglePress and onLongPress callbacks, and publishes MQTT messages to a remote server. 
 * The onSinglePress callback is triggered when a button is pressed and released within a 
 * short time (single press), publishing "switch:on" to the MQTT control topic. The 
 * onLongPress callback is triggered when a button is held down for a longer duration 
 * (long press), publishing "switch:off" to the MQTT control topic. This example is 
 * specifically designed for button control with MQTT integration and uses separate press 
 * detection callbacks for more precise button interaction. The input pin is configured as 
 * INPUT_PULLUP to use the internal pull-up resistor, which is the standard configuration 
 * for buttons that connect to ground when pressed.
 * 
 * Author: AvantMaker <admin@avantmaker.com>
 * Author Website: `https://www.AvantMaker.com` 
 * Date: 2025-09-21
 * Version: 0.0.1
 * 
 * Hardware Requirements:
 * - ESP32-based microcontroller with WiFi support (e.g., ESP32 DevKitC, DOIT ESP32 DevKit, etc.)
 * - A momentary push button connected to pin 5
 * - WiFi network access
 * 
 * Dependencies:
 * - AvantDigitalRead library
 * - WiFi library (built-in for ESP32)
 * - PubSubClient library for MQTT communication
 * 
 * 
 * Usage Notes:
 * 1. BUTTON CONNECTION (Pin 5):
 *    - Connect a momentary push button to pin 5
 *    - Standard button wiring:
 *        * One terminal of the button to pin 5 of the ESP32
 *        * The other terminal to GROUND
 *    - The input pin will be configured as INPUT_PULLUP
 *    - This configuration uses the internal pull-up resistor of the ESP32
 *    - When the button is not pressed, the pin reads HIGH (due to pull-up)
 *    - When the button is pressed, the pin reads LOW (connected to ground)
 *    - This program uses separate callbacks for different press types:
 *        * onSinglePress callback: Triggered when button is pressed and released quickly
 *        * onLongPress callback: Triggered when button is held down for longer duration
 *    - The debounce time and press duration thresholds can be adjusted in the code
 * 
 * 2. WIFI CONFIGURATION:
 *    - Before uploading, modify the WiFi credentials in the code:
 *        const char* ssid = "your_wifi_ssid";
 *        const char* password = "your_wifi_password";
 *    - The ESP32 will automatically connect to the specified WiFi network during setup
 *    - Connection status will be displayed in the Serial Monitor
 *    - If WiFi connection fails, the device will retry automatically
 * 
 * 3. MQTT CONFIGURATION:
 *    - This example uses a public MQTT broker for demonstration:
 *        * Server: test.mosquitto.org
 *        * Port: 1883
 *        * No username/password required
 *    - The device will publish messages to the topic: "avantmaker/avantdigitalread/control"
 *    - Messages published:
 *        * "switch:on" when a single press is detected
 *        * "switch:off" when a long press is detected
 *    - You can use any MQTT client to subscribe to the topic and monitor the messages
 *    - If MQTT connection is lost, the device will attempt to reconnect automatically
 * 
 * 4. BUTTON PRESS DETECTION:
 *    - This program is ideal for momentary push buttons using press detection callbacks
 *    - The program uses dedicated callbacks for each press type:
 *        * onSinglePress callback: Triggered on quick press and release (MQTT "switch:on")
 *        * onLongPress callback: Triggered on sustained press (MQTT "switch:off")
 *    - Using separate callbacks provides cleaner code structure and more precise button interaction
 *    - The debounce time prevents false triggers from button bouncing (set to 50ms)
 *    - The long press time threshold determines how long the button must be held to trigger a long press (set to 1000ms)
 *    - Serial monitor provides real-time feedback of button events and MQTT publishing status
 *    - MQTT messages allow remote control and integration with home automation systems
 * 
 * 5. UPLOAD AND USAGE:
 *    - Before uploading, modify the WiFi credentials in the code
 *    - Upload this sketch to your ESP32 board
 *    - Open the Serial Monitor (baud rate: 115200) to see status information
 *    - MQTT messages will be published on button press events
 *    - You can modify the INPUT_PIN value to use a different pin
 *    - Adjust debounce time and long press threshold as needed for your specific button
 *    - Use an MQTT client to subscribe to "avantmaker/avantdigitalread/control" to monitor messages
 *    - Test the button functionality:
 *        * Quick press and release should publish "switch:on"
 *        * Hold the button down for about 1 second should publish "switch:off"
 *
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantDigitalRead.h"
#include <WiFi.h>
#include <PubSubClient.h>

// Define the pins
#define INPUT_PIN 5     // Pin to monitor (button)

// WiFi credentials - MODIFY THESE VALUES
const char* ssid = "your_wifi_ssid";      // Replace with your WiFi network name
const char* password = "your_wifi_password";  // Replace with your WiFi password

// MQTT Configuration
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

// MQTT Topics
const char* mqtt_control_topic = "avantmaker/avantdigitalread/control";

// Create an instance of AvantDigitalRead
AvantDigitalRead pinManager;

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variables for connection management
unsigned long lastWifiConnectionAttempt = 0;
unsigned long lastMqttConnectionAttempt = 0;
const unsigned long wifiReconnectInterval = 10000;  // 10 seconds
const unsigned long mqttReconnectInterval = 5000;   // 5 seconds

// Button timing parameters
const unsigned long debounceTime = 50;      // Debounce time in milliseconds
const unsigned long longPressTime = 1000;  // Long press threshold in milliseconds

// Callback function for single press
void onSinglePressCallback(int pin, PinState newState, PinState oldState, 
                          EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" SINGLE PRESS at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Publish MQTT message for single press
  if (mqttClient.connected()) {
    if (mqttClient.publish(mqtt_control_topic, "switch:on")) {
      Serial.println("  Action: Published 'switch:on' to control topic");
    } else {
      Serial.println("  Action: Failed to publish 'switch:on' message");
    }
  } else {
    Serial.println("  Action: MQTT not connected, message not published");
  }
}

// Callback function for long press
void onLongPressCallback(int pin, PinState newState, PinState oldState, 
                        EventType event, unsigned long timestamp) {
  // Print pin information
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" LONG PRESS at ");
  Serial.print(timestamp);
  Serial.println(" ms");
  
  // Publish MQTT message for long press
  if (mqttClient.connected()) {
    if (mqttClient.publish(mqtt_control_topic, "switch:off")) {
      Serial.println("  Action: Published 'switch:off' to control topic");
    } else {
      Serial.println("  Action: Failed to publish 'switch:off' message");
    }
  } else {
    Serial.println("  Action: MQTT not connected, message not published");
  }
}

// Function to connect to WiFi
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

// Function to connect to MQTT broker
void connectToMQTT() {
  Serial.println("Connecting to MQTT broker...");
  
  String clientId = "AvantMaker_" + String(random(0xffff), HEX);
  if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
    Serial.println("MQTT connected");
    
    // Once connected, publish an announcement...
    mqttClient.publish(mqtt_control_topic, "ButtonMQTTControl device connected");
    Serial.println("MQTT: Published connection announcement");
  } else {
    Serial.print("MQTT connection failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" will try again later");
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Print welcome message
  Serial.println("ButtonMQTTControl Example Starting...");
  Serial.print("Monitoring button on pin: ");
  Serial.println(INPUT_PIN);
  Serial.println("Button configured with INPUT_PULLUP (HIGH when not pressed, LOW when pressed)");
  Serial.println("MQTT messages will be published on button press events.");
  Serial.println("onSinglePress callback: Quick press and release (MQTT 'switch:on')");
  Serial.println("onLongPress callback: Hold button for 1 second (MQTT 'switch:off')");
  Serial.println("----------------------------------------");
  
  // Initialize the input pin to monitor with INPUT_PULLUP
  if (pinManager.addPin(INPUT_PIN, INPUT_PULLUP)) {
    Serial.println("Input pin initialized successfully with INPUT_PULLUP");
  } else {
    Serial.println("Failed to initialize input pin");
    while (1) {
      delay(100); // Halt execution if pin initialization fails
    }
  }
  
  // Set debounce time to prevent false triggers from button bouncing
  pinManager.setDebounceTime(INPUT_PIN, debounceTime);
  Serial.print("Debounce time set to: ");
  Serial.print(debounceTime);
  Serial.println(" ms");
  
  // Register callback functions for single press and long press
  pinManager.onSinglePress(INPUT_PIN, onSinglePressCallback);
  Serial.println("Single press callback registered");
  
  pinManager.onLongPress(INPUT_PIN, onLongPressCallback, 0, longPressTime);
  Serial.print("Long press callback registered with threshold: ");
  Serial.print(longPressTime);
  Serial.println(" ms");
  
  // Initialize WiFi connection
  connectToWiFi();
  
  // Configure MQTT server
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  // Initialize MQTT connection
  if (WiFi.status() == WL_CONNECTED) {
    connectToMQTT();
  }
  
  Serial.println("----------------------------------------");
  Serial.println("Ready for button input.");
  Serial.println("Quick press and release: Publish 'switch:on'");
  Serial.println("Hold button for 1 second: Publish 'switch:off'");
  Serial.println("----------------------------------------");
}

void loop() {
  // Must call update() regularly to process events
  pinManager.update();
  
  // Handle WiFi connection (reconnect if needed)
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastWifiConnectionAttempt > wifiReconnectInterval) {
      lastWifiConnectionAttempt = currentMillis;
      connectToWiFi();
    }
  }
  
  // Handle MQTT connection (reconnect if needed)
  if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastMqttConnectionAttempt > mqttReconnectInterval) {
      lastMqttConnectionAttempt = currentMillis;
      connectToMQTT();
    }
  }
  
  // Maintain MQTT connection
  if (mqttClient.connected()) {
    mqttClient.loop();
  }
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}