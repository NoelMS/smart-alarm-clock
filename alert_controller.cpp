#include "alert_controller.h"

AlertController::AlertController() 
  : strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800),
    currentState(ALERT_IDLE),
    currentPattern(MEDIUM),
    alertStartTime(0),
    lastMotorToggle(0),
    lastLEDToggle(0),
    motorState(false),
    escalating(false),
    escalationLevel(0) {
}

void AlertController::begin() {
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  
  strip.begin();
  strip.show();
  
  DEBUG_PRINTLN("Alert controller initialized");
}

void AlertController::startAlert(VibrationType pattern) {
  currentPattern = pattern;
  currentState = ALERT_ACTIVE;
  alertStartTime = millis();
  motorState = false;
  escalating = false;
  escalationLevel = 0;
  lastMotorToggle = alertStartTime;
  lastLEDToggle = alertStartTime;
  
  DEBUG_PRINTF("Alert started with pattern: %d\n", pattern);
}

void AlertController::stopAlert() {
  currentState = ALERT_STOPPED;
  digitalWrite(MOTOR_PIN, LOW);
  strip.clear();
  strip.show();
  
  DEBUG_PRINTLN("Alert stopped");
}

void AlertController::updateMotor() {
  if (currentState != ALERT_ACTIVE) return;
  
  unsigned long currentTime = millis();
  unsigned long onTime, offTime;
  uint8_t pwmValue;
  
  switch (currentPattern) {
    case GENTLE:
      onTime = 100;
      offTime = 100;
      pwmValue = 150; // 60% of 255
      break;
    case MEDIUM:
      onTime = 200;
      offTime = 150;
      pwmValue = 205; // 80% of 255
      break;
    case STRONG:
      onTime = 300;
      offTime = 100;
      pwmValue = 255; // 100% of 255
      break;
    default:
      return;
  }
  
  // Escalation logic for STRONG pattern
  if (currentPattern == STRONG) {
    unsigned long alertDuration = currentTime - alertStartTime;
    if (alertDuration > 30000 && escalationLevel == 0) {
      escalating = true;
      escalationLevel = 1;
      DEBUG_PRINTLN("Escalation Level 1");
    }
  }
  
  unsigned long timeSinceToggle = currentTime - lastMotorToggle;
  
  if (motorState && timeSinceToggle >= onTime) {
    motorState = false;
    lastMotorToggle = currentTime;
    digitalWrite(MOTOR_PIN, LOW);
  } else if (!motorState && timeSinceToggle >= offTime) {
    motorState = true;
    lastMotorToggle = currentTime;
    analogWrite(MOTOR_PIN, pwmValue);
  }
}

void AlertController::updateLED() {
  if (currentState != ALERT_ACTIVE) return;
  
  unsigned long currentTime = millis();
  uint32_t color = getAlertColor();
  
  unsigned long flashInterval;
  
  switch (currentPattern) {
    case GENTLE:
      flashInterval = 500; // 0.5Hz
      break;
    case MEDIUM:
      flashInterval = 500; // 1Hz
      break;
    case STRONG:
      flashInterval = 333; // 3Hz
      break;
    default:
      return;
  }
  
  unsigned long timeSinceToggle = currentTime - lastLEDToggle;
  
  if (timeSinceToggle >= flashInterval) {
    // Toggle LED on/off
    if (strip.getPixelColor(0) == 0) {
      strip.setPixelColor(0, color);
    } else {
      strip.clear();
    }
    strip.show();
    lastLEDToggle = currentTime;
  }
}

uint32_t AlertController::getAlertColor() {
  switch (currentPattern) {
    case GENTLE:
      return strip.Color(255, 200, 0); // Yellow
    case MEDIUM:
      return strip.Color(255, 100, 0); // Orange
    case STRONG:
      return strip.Color(255, 0, 0);   // Red
    default:
      return 0;
  }
}

void AlertController::update() {
  updateMotor();
  updateLED();
}

AlertState AlertController::getState() {
  return currentState;
}

unsigned long AlertController::getAlertDuration() {
  if (currentState == ALERT_ACTIVE) {
    return millis() - alertStartTime;
  }
  return 0;
}

bool AlertController::shouldAutoStop() {
  if (currentState != ALERT_ACTIVE) return false;
  return (millis() - alertStartTime) >= ALARM_DURATION_MS;
}
