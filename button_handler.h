#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "config.h"

typedef void (*ButtonCallback)(uint8_t buttonID, bool longPress);

class ButtonHandler {
private:
  struct ButtonState {
    uint8_t pin;
    bool lastState;
    bool currentState;
    unsigned long lastDebounceTime;
    unsigned long pressStartTime;
    bool longPressTriggered;
  };
  
  ButtonState buttons[5];
  ButtonCallback callback;
  unsigned long lastCheckTime;

public:
  ButtonHandler();
  void begin();
  void setCallback(ButtonCallback cb);
  void update();
  bool isPressed(uint8_t buttonID);
  bool wasPressed(uint8_t buttonID);
};

#endif // BUTTON_HANDLER_H
