#include "AvantDigitalRead.h"

AvantDigitalRead::AvantDigitalRead() {
  // Constructor, initialize vector
}

AvantDigitalRead::~AvantDigitalRead() {
  // Destructor, clean up resources
  pinList.clear();
  delayedCallbacks.clear();
}

// Find pin information
PinInfo* AvantDigitalRead::findPin(int pin) {
  for (auto& pinInfo : pinList) {
    if (pinInfo.pin == pin) {
      return &pinInfo;
    }
  }
  return nullptr;
}

// Trigger callback function
void AvantDigitalRead::triggerCallback(PinCallback callback, int pin, PinState newState, 
                                     PinState oldState, EventType event, 
                                     unsigned long timestamp, unsigned long delayMs) {
  if (callback == nullptr) {
    return;
  }
  
  if (delayMs == 0) {
    callback(pin, newState, oldState, event, timestamp);
  } else {
    // Create a new delayed callback entry
    DelayedCallback delayedCb;
    delayedCb.callback = callback;
    delayedCb.pin = pin;
    delayedCb.newState = newState;
    delayedCb.oldState = oldState;
    delayedCb.event = event;
    delayedCb.timestamp = timestamp;
    delayedCb.delayMs = delayMs;
    delayedCb.executed = false;
    
    // Add to the list of delayed callbacks
    delayedCallbacks.push_back(delayedCb);
    
    // Debug output - comment out or remove in production
    #ifdef DEBUG_DELAYED_CALLBACKS
    Serial.print("DEBUG: Added delayed callback for pin ");
    Serial.print(pin);
    Serial.print(", event: ");
    Serial.print(event);
    Serial.print(", delay: ");
    Serial.print(delayMs);
    Serial.print("ms, scheduled at ");
    Serial.println(timestamp);
    #endif
  }
}

// Process delayed callbacks
void AvantDigitalRead::processDelayedCallbacks(unsigned long currentTime) {
  // Create a temporary list to store callbacks that are ready to execute
  std::vector<DelayedCallback> readyCallbacks;
  
  // Debug output - comment out or remove in production
  #ifdef DEBUG_DELAYED_CALLBACKS
  if (!delayedCallbacks.empty()) {
    Serial.print("DEBUG: Processing ");
    Serial.print(delayedCallbacks.size());
    Serial.print(" delayed callbacks at time ");
    Serial.println(currentTime);
  }
  #endif
  
  // Check each delayed callback
  for (auto it = delayedCallbacks.begin(); it != delayedCallbacks.end(); ) {
    if (!it->executed && currentTime - it->timestamp >= it->delayMs) {
      // Debug output - comment out or remove in production
      #ifdef DEBUG_DELAYED_CALLBACKS
      Serial.print("DEBUG: Executing delayed callback for pin ");
      Serial.print(it->pin);
      Serial.print(", event: ");
      Serial.print(it->event);
      Serial.print(", scheduled at ");
      Serial.print(it->timestamp);
      Serial.print(", delay: ");
      Serial.print(it->delayMs);
      Serial.print("ms, elapsed: ");
      Serial.print(currentTime - it->timestamp);
      Serial.println("ms");
      #endif
      
      // Add to ready list
      readyCallbacks.push_back(*it);
      // Remove from delayed list
      it = delayedCallbacks.erase(it);
    } else {
      ++it;
    }
  }
  
  // Execute all ready callbacks
  for (auto& cb : readyCallbacks) {
    cb.callback(cb.pin, cb.newState, cb.oldState, cb.event, currentTime);
  }
}

// Detect button gestures
void AvantDigitalRead::detectButtonGestures(PinInfo* pinInfo, unsigned long currentTime) {
  if (!pinInfo->eventsEnabled) return;
  
  // Check for long press (when button is pressed)
  if (pinInfo->currentState == PIN_LOW && pinInfo->onLongPressCallback != nullptr) {
    if (currentTime - pinInfo->pressStartTime >= pinInfo->pressDurationMs) {
      // Long press triggered
      if (pinInfo->repeatLongPress || !pinInfo->longPressTriggered) {
        triggerCallback(pinInfo->onLongPressCallback, pinInfo->pin, pinInfo->currentState, 
                       pinInfo->currentState, EVENT_LONG_PRESS, currentTime, pinInfo->onLongPressDelay);
        pinInfo->longPressTriggered = true;
      }
    }
  } else {
    pinInfo->longPressTriggered = false;
  }
  
  // Check for click (when button is released)
  if (pinInfo->currentState == PIN_HIGH) {
    // If there was a previous press record
    if (pinInfo->pressStartTime > 0) {
      unsigned long pressDuration = currentTime - pinInfo->pressStartTime;
      
      // Check if press duration is within valid range
      if (pressDuration >= pinInfo->minPressMs && pressDuration <= pinInfo->maxPressMs) {
        // Check if it's a double press
        if (pinInfo->clickCount == 2 && pinInfo->onDoublePressCallback != nullptr) {
          // Check if interval between two clicks is within valid range
          if (currentTime - pinInfo->lastClickTime <= pinInfo->maxIntervalMs) {
            // Trigger double press event
            triggerCallback(pinInfo->onDoublePressCallback, pinInfo->pin, pinInfo->currentState, 
                           pinInfo->currentState, EVENT_DOUBLE_PRESS, currentTime, pinInfo->onDoublePressDelay);
            pinInfo->clickCount = 0; // Reset click count
          } else {
            // Interval too long, treat as two single presses
            if (pinInfo->onSinglePressCallback != nullptr) {
              triggerCallback(pinInfo->onSinglePressCallback, pinInfo->pin, pinInfo->currentState, 
                             pinInfo->currentState, EVENT_SINGLE_PRESS, currentTime, pinInfo->onSinglePressDelay);
            }
            pinInfo->clickCount = 1; // Keep current click as first click
          }
        } else if (pinInfo->clickCount == 1) {
          // If no double press callback is set, or no second click after timeout, trigger single press event
          if (pinInfo->onDoublePressCallback == nullptr) {
            // No double press callback, directly trigger single press event
            if (pinInfo->onSinglePressCallback != nullptr) {
              triggerCallback(pinInfo->onSinglePressCallback, pinInfo->pin, pinInfo->currentState, 
                             pinInfo->currentState, EVENT_SINGLE_PRESS, currentTime, pinInfo->onSinglePressDelay);
            }
            pinInfo->clickCount = 0; // Reset click count
          }
          // If double press callback is set, don't immediately trigger single press event, wait for possible second click
          // Single press event will only be triggered after timeout, this logic is handled in the timeout check below
        }
        
        // Update last click time
        pinInfo->lastClickTime = currentTime;
      } else if (pressDuration > pinInfo->maxPressMs) {
        // Press duration too long, not considered a valid click
        pinInfo->clickCount = 0;
      }
      
      // Reset press start time
      pinInfo->pressStartTime = 0;
    }
  }
  
  // Check for single press timeout (when button is released)
  if (pinInfo->currentState == PIN_HIGH && pinInfo->clickCount == 1 && 
      pinInfo->onDoublePressCallback != nullptr && pinInfo->pressStartTime == 0) {
    // If waited longer than maximum interval time, trigger single press event
    if (currentTime - pinInfo->lastClickTime > pinInfo->maxIntervalMs) {
      if (pinInfo->onSinglePressCallback != nullptr) {
        triggerCallback(pinInfo->onSinglePressCallback, pinInfo->pin, pinInfo->currentState, 
                       pinInfo->currentState, EVENT_SINGLE_PRESS, currentTime, pinInfo->onSinglePressDelay);
      }
      pinInfo->clickCount = 0; // Reset click count
    }
  }
}

// Add pin
bool AvantDigitalRead::addPin(int pin, int mode) {
  // Check if pin is already initialized
  if (findPin(pin) != nullptr) {
    return false;
  }
  
  // Initialize pin
  pinMode(pin, mode);
  
  // Create new pin information
  PinInfo newPin;
  newPin.pin = pin;
  newPin.mode = mode;
  newPin.currentState = (PinState)digitalRead(pin);
  newPin.lastState = newPin.currentState;
  newPin.lastDebounceTime = 0;
  newPin.debounceTime = DEFAULT_DEBOUNCE_TIME; // Default debounce time
  newPin.eventsEnabled = true;
  
  // Initialize callback functions
  newPin.onChangeCallback = nullptr;
  newPin.onChangeDelay = 0;
  newPin.onRisingCallback = nullptr;
  newPin.onRisingDelay = 0;
  newPin.onFallingCallback = nullptr;
  newPin.onFallingDelay = 0;
  newPin.onSinglePressCallback = nullptr;
  newPin.onSinglePressDelay = 0;
  newPin.onDoublePressCallback = nullptr;
  newPin.onDoublePressDelay = 0;
  newPin.onLongPressCallback = nullptr;
  newPin.onLongPressDelay = 0;
  
  // Initialize button parameters
  newPin.minPressMs = DEFAULT_MIN_PRESS_MS;
  newPin.maxPressMs = DEFAULT_MAX_PRESS_MS;
  newPin.maxIntervalMs = DEFAULT_MAX_INTERVAL_MS;
  newPin.pressDurationMs = DEFAULT_PRESS_DURATION_MS;
  newPin.repeatLongPress = DEFAULT_REPEAT_LONG_PRESS;
  
  // Initialize button state tracking
  newPin.pressStartTime = 0;
  newPin.releaseTime = 0;
  newPin.lastClickTime = 0;  // Add initialization for last click time
  newPin.clickCount = 0;
  newPin.longPressTriggered = false;
  
  // Add to list
  pinList.push_back(newPin);
  return true;
}

// Remove pin
bool AvantDigitalRead::removePin(int pin) {
  for (auto it = pinList.begin(); it != pinList.end(); ++it) {
    if (it->pin == pin) {
      pinList.erase(it);
      return true;
    }
  }
  return false;
}

// Check if pin is initialized
bool AvantDigitalRead::isInitialized(int pin) {
  return findPin(pin) != nullptr;
}

// Get pin mode
int AvantDigitalRead::getPinMode(int pin) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return PIN_UNINITIALIZED;
  }
  return pinInfo->mode;
}

// Read pin state
PinState AvantDigitalRead::readPin(int pin) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return PIN_UNINITIALIZED;
  }
  return pinInfo->currentState;
}

// Set debounce time
bool AvantDigitalRead::setDebounceTime(int pin, unsigned long debounceMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->debounceTime = debounceMs;
  return true;
}

// Get debounce time
unsigned long AvantDigitalRead::getDebounceTime(int pin) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return DEFAULT_DEBOUNCE_TIME; // Return default value
  }
  return pinInfo->debounceTime;
}

// Set state change callback
bool AvantDigitalRead::onChange(int pin, PinCallback callback, unsigned long delayMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->onChangeCallback = callback;
  pinInfo->onChangeDelay = delayMs;
  return true;
}

// Set rising edge callback
bool AvantDigitalRead::onRising(int pin, PinCallback callback, unsigned long delayMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->onRisingCallback = callback;
  pinInfo->onRisingDelay = delayMs;
  return true;
}

// Set falling edge callback
bool AvantDigitalRead::onFalling(int pin, PinCallback callback, unsigned long delayMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->onFallingCallback = callback;
  pinInfo->onFallingDelay = delayMs;
  return true;
}

// Set single press callback
bool AvantDigitalRead::onSinglePress(int pin, PinCallback callback, unsigned long delayMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->onSinglePressCallback = callback;
  pinInfo->onSinglePressDelay = delayMs;
  return true;
}

// Set click parameters
bool AvantDigitalRead::setClickParameters(int pin, unsigned long minPressMs, unsigned long maxPressMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->minPressMs = minPressMs;
  pinInfo->maxPressMs = maxPressMs;
  return true;
}

// Set double press callback
bool AvantDigitalRead::onDoublePress(int pin, PinCallback callback, unsigned long delayMs, unsigned long maxIntervalMs) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->onDoublePressCallback = callback;
  pinInfo->onDoublePressDelay = delayMs;
  pinInfo->maxIntervalMs = maxIntervalMs;
  return true;
}

// Set long press callback
bool AvantDigitalRead::onLongPress(int pin, PinCallback callback, unsigned long delayMs, unsigned long pressDurationMs, bool repeat) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->onLongPressCallback = callback;
  pinInfo->onLongPressDelay = delayMs;
  pinInfo->pressDurationMs = pressDurationMs;
  pinInfo->repeatLongPress = repeat;
  return true;
}

// Enable pin events
bool AvantDigitalRead::enablePinEvents(int pin) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->eventsEnabled = true;
  return true;
}

// Disable pin events
bool AvantDigitalRead::disablePinEvents(int pin) {
  PinInfo* pinInfo = findPin(pin);
  if (pinInfo == nullptr) {
    return false;
  }
  pinInfo->eventsEnabled = false;
  return true;
}

// Enable all events
void AvantDigitalRead::enableAllEvents() {
  for (auto& pinInfo : pinList) {
    pinInfo.eventsEnabled = true;
  }
}

// Disable all events
void AvantDigitalRead::disableAllEvents() {
  for (auto& pinInfo : pinList) {
    pinInfo.eventsEnabled = false;
  }
}

// Core update function
void AvantDigitalRead::update() {
  unsigned long currentTime = millis();
  
  for (auto& pinInfo : pinList) {
    // Read current pin state
    int rawReading = digitalRead(pinInfo.pin);
    
    // Debounce processing
    if (rawReading != pinInfo.lastState) {
      pinInfo.lastDebounceTime = currentTime;
    }
    
    if ((currentTime - pinInfo.lastDebounceTime) > pinInfo.debounceTime) {
      // If state is stable, update state
      if (rawReading != pinInfo.currentState) {
        // Save previous state
        PinState previousState = pinInfo.currentState;
        
        // Update current state
        pinInfo.currentState = (PinState)rawReading;
        
        // Check button press and release
        if (pinInfo.currentState == PIN_LOW && previousState == PIN_HIGH) {
          // Button pressed (in INPUT_PULLUP mode, press is LOW)
          pinInfo.pressStartTime = currentTime;
          pinInfo.clickCount++; // Increase click count
        }
        
        // Trigger event callbacks
        if (pinInfo.eventsEnabled) {
          // State change event
          if (pinInfo.onChangeCallback != nullptr) {
            triggerCallback(pinInfo.onChangeCallback, pinInfo.pin, pinInfo.currentState, 
                           previousState, EVENT_CHANGE, currentTime, pinInfo.onChangeDelay);
          }
          
          // Rising edge event (from LOW to HIGH)
          if (pinInfo.currentState == PIN_HIGH && previousState == PIN_LOW && pinInfo.onRisingCallback != nullptr) {
            triggerCallback(pinInfo.onRisingCallback, pinInfo.pin, pinInfo.currentState, 
                           previousState, EVENT_RISING, currentTime, pinInfo.onRisingDelay);
          }
          
          // Falling edge event (from HIGH to LOW)
          if (pinInfo.currentState == PIN_LOW && previousState == PIN_HIGH && pinInfo.onFallingCallback != nullptr) {
            triggerCallback(pinInfo.onFallingCallback, pinInfo.pin, pinInfo.currentState, 
                           previousState, EVENT_FALLING, currentTime, pinInfo.onFallingDelay);
          }
        }
      }
    }
    
    // Update lastState to current raw reading (for next comparison)
    pinInfo.lastState = (PinState)rawReading;
    
    // Detect button gestures
    detectButtonGestures(&pinInfo, currentTime);
  }
  
  // Process delayed callbacks
  processDelayedCallbacks(currentTime);
}