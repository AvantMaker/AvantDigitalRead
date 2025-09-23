#ifndef AVANTDIGITALREAD_H
#define AVANTDIGITALREAD_H

#include <Arduino.h>
#include <vector>

// Default values for button parameters
const unsigned long DEFAULT_MIN_PRESS_MS = 50;      // Default minimum valid press duration
const unsigned long DEFAULT_MAX_PRESS_MS = 300;     // Default maximum valid press duration
const unsigned long DEFAULT_MAX_INTERVAL_MS = 300;  // Default maximum interval for double press
const unsigned long DEFAULT_PRESS_DURATION_MS = 1000; // Default long press detection duration
const bool DEFAULT_REPEAT_LONG_PRESS = false;      // Default whether long press repeats
const unsigned long DEFAULT_DEBOUNCE_TIME = 50;     // Default debounce time

// Pin state enumeration
enum PinState {
  PIN_LOW = 0,
  PIN_HIGH = 1,
  PIN_UNINITIALIZED = -1,
  PIN_ERROR = -2
};

// Event type enumeration
enum EventType {
  EVENT_CHANGE,       // State change (HIGH↔LOW)
  EVENT_RISING,       // Rising edge (LOW→HIGH)
  EVENT_FALLING,      // Falling edge (HIGH→LOW)
  EVENT_SINGLE_PRESS, // Single press
  EVENT_DOUBLE_PRESS, // Double press
  EVENT_LONG_PRESS    // Long press
};

// Callback function prototype (all callbacks use this format)
typedef void (*PinCallback)(int pin, PinState newState, PinState oldState,
                           EventType event, unsigned long timestamp);

// Structure to store delayed callback information
struct DelayedCallback {
  PinCallback callback;
  int pin;
  PinState newState;
  PinState oldState;
  EventType event;
  unsigned long timestamp;
  unsigned long delayMs;
  bool executed;
};

// Pin configuration and status structure
struct PinInfo {
  int pin;                      // Pin number
  int mode;                     // Pin mode
  PinState currentState;        // Current state
  PinState lastState;           // Previous state
  unsigned long lastDebounceTime; // Last debounce time
  unsigned long debounceTime;   // Debounce time
  bool eventsEnabled;           // Whether event detection is enabled
  
  // Event callback functions
  PinCallback onChangeCallback;
  unsigned long onChangeDelay;
  PinCallback onRisingCallback;
  unsigned long onRisingDelay;
  PinCallback onFallingCallback;
  unsigned long onFallingDelay;
  PinCallback onSinglePressCallback;
  unsigned long onSinglePressDelay;
  PinCallback onDoublePressCallback;
  unsigned long onDoublePressDelay;
  PinCallback onLongPressCallback;
  unsigned long onLongPressDelay;
  
  // Button detection parameters
  unsigned long minPressMs;     // Minimum valid press duration
  unsigned long maxPressMs;     // Maximum valid press duration
  unsigned long maxIntervalMs;  // Maximum interval for double press
  unsigned long pressDurationMs; // Long press detection duration
  bool repeatLongPress;         // Whether long press repeats
  
  // Button state tracking
  unsigned long pressStartTime; // Press start time
  unsigned long releaseTime;    // Release time
  unsigned long lastClickTime;  // Last click time
  int clickCount;               // Click count
  bool longPressTriggered;      // Whether long press has been triggered
};

class AvantDigitalRead {
private:
  std::vector<PinInfo> pinList;  // Vector storing all pin information
  std::vector<DelayedCallback> delayedCallbacks;  // Vector storing delayed callback information
  
  // Find pin information
  PinInfo* findPin(int pin);
  
  // Trigger callback function
  void triggerCallback(PinCallback callback, int pin, PinState newState, 
                      PinState oldState, EventType event, 
                      unsigned long timestamp, unsigned long delayMs);
  
  // Process delayed callbacks
  void processDelayedCallbacks(unsigned long currentTime);
  
  // Detect button gestures
  void detectButtonGestures(PinInfo* pinInfo, unsigned long currentTime);
  
public:
  AvantDigitalRead();
  ~AvantDigitalRead();
  
  // Pin management functions
  bool addPin(int pin, int mode);
  bool removePin(int pin);
  bool isInitialized(int pin);
  int getPinMode(int pin);
  PinState readPin(int pin);
  
  // Debounce setting functions
  bool setDebounceTime(int pin, unsigned long debounceMs);
  unsigned long getDebounceTime(int pin);
  
  // Event callback management functions
  bool onChange(int pin, PinCallback callback, unsigned long delayMs = 0);
  bool onRising(int pin, PinCallback callback, unsigned long delayMs = 0);
  bool onFalling(int pin, PinCallback callback, unsigned long delayMs = 0);
  
  // Button gesture detection functions
  bool onSinglePress(int pin, PinCallback callback, unsigned long delayMs = 0);
  bool setClickParameters(int pin, unsigned long minPressMs = DEFAULT_MIN_PRESS_MS, unsigned long maxPressMs = DEFAULT_MAX_PRESS_MS);
  bool onDoublePress(int pin, PinCallback callback, unsigned long delayMs = 0, unsigned long maxIntervalMs = DEFAULT_MAX_INTERVAL_MS);
  bool onLongPress(int pin, PinCallback callback, unsigned long delayMs = 0, unsigned long pressDurationMs = DEFAULT_PRESS_DURATION_MS, bool repeat = DEFAULT_REPEAT_LONG_PRESS);
  
  // Event management functions
  bool enablePinEvents(int pin);
  bool disablePinEvents(int pin);
  void enableAllEvents();
  void disableAllEvents();
  
  // Core processing function
  void update();
};

#endif // AVANTDIGITALREAD_H