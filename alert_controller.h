#ifndef ALERT_CONTROLLER_H
#define ALERT_CONTROLLER_H

#include "config.h"
#include <Adafruit_NeoPixel.h>

enum AlertState {
  ALERT_IDLE,
  ALERT_ACTIVE,
  ALERT_SNOOZED,
  ALERT_STOPPED
};

class AlertController {
private:
  Adafruit_NeoPixel strip;
  AlertState currentState;
  VibrationType currentPattern;
  unsigned long alertStartTime;
  unsigned long lastMotorToggle;
  unsigned long lastLEDToggle;
  bool motorState;
  bool escalating;
  uint8_t escalationLevel;
  
  void applyPattern(VibrationType pattern);
  void updateMotor();
  void updateLED();
  uint32_t getAlertColor();

public:
  AlertController();
  void begin();
  void startAlert(VibrationType pattern);
  void stopAlert();
  void update();
  AlertState getState();
  unsigned long getAlertDuration();
  bool shouldAutoStop();
};

#endif // ALERT_CONTROLLER_H
