#include "button_handler.h"

ButtonHandler::ButtonHandler() : callback(nullptr), lastCheckTime(0) {
}

void ButtonHandler::begin() {
  uint8_t pins[] = {BUTTON_MODE, BUTTON_INC, BUTTON_DEC, BUTTON_CONFIRM, BUTTON_SNOOZE_STOP};
  
  for (uint8_t i = 0; i < 5; i++) {
    buttons[i].pin = pins[i];
    buttons[i].lastState = HIGH;
    buttons[i].currentState = HIGH;
    buttons[i].lastDebounceTime = 0;
    buttons[i].pressStartTime = 0;
    buttons[i].longPressTriggered = false;
    
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  
  DEBUG_PRINTLN("Button handler initialized");
}

void ButtonHandler::setCallback(ButtonCallback cb) {
  callback = cb;
}

void ButtonHandler::update() {
  unsigned long currentTime = millis();
  
  for (uint8_t i = 0; i < 5; i++) {
    bool reading = digitalRead(buttons[i].pin);
    
    // Debouncing logic
    if (reading != buttons[i].lastState) {
      buttons[i].lastDebounceTime = currentTime;
    }
    
    if ((currentTime - buttons[i].lastDebounceTime) > BUTTON_DEBOUNCE_MS) {
      if (reading != buttons[i].currentState) {
        buttons[i].currentState = reading;
        
        // Button pressed (LOW)
        if (buttons[i].currentState == LOW) {
          buttons[i].pressStartTime = currentTime;
          buttons[i].longPressTriggered = false;
          DEBUG_PRINTF("Button %d pressed\n", i);
        }
        // Button released (HIGH)
        else if (buttons[i].currentState == HIGH) {
          unsigned long pressDuration = currentTime - buttons[i].pressStartTime;
          
          if (callback && !buttons[i].longPressTriggered) {
            callback(i, false); // Short press
          }
          DEBUG_PRINTF("Button %d released (duration: %lu ms)\n", i, pressDuration);
        }
      }
    }
    
    // Long press detection
    if (buttons[i].currentState == LOW && !buttons[i].longPressTriggered) {
      if ((currentTime - buttons[i].pressStartTime) > BUTTON_LONG_PRESS_MS) {
        buttons[i].longPressTriggered = true;
        if (callback) {
          callback(i, true); // Long press
        }
        DEBUG_PRINTF("Button %d long pressed\n", i);
      }
    }
    
    buttons[i].lastState = reading;
  }
  
  lastCheckTime = currentTime;
}

bool ButtonHandler::isPressed(uint8_t buttonID) {
  if (buttonID >= 5) return false;
  return buttons[buttonID].currentState == LOW;
}

bool ButtonHandler::wasPressed(uint8_t buttonID) {
  if (buttonID >= 5) return false;
  return buttons[buttonID].currentState == LOW && buttons[buttonID].lastState == HIGH;
}
