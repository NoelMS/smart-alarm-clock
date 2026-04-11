#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "config.h"

class DisplayManager {
private:
  Adafruit_SSD1306 display;
  bool initialized;
  unsigned long lastUpdateTime;

public:
  DisplayManager();
  bool begin();
  void clear();
  void showTime(uint8_t hour, uint8_t minute);
  void showAlarmInfo(uint8_t hour, uint8_t minute, bool enabled);
  void showMenu(const char* items[], uint8_t count, uint8_t selected);
  void showAlarmTriggered(uint8_t hour, uint8_t minute);
  void showMessage(const char* line1, const char* line2, const char* line3 = "", const char* line4 = "");
  void showAlarmList(uint8_t alarms[][2], bool enabled[], uint8_t count, uint8_t selected);
  void showSetAlarmUI(uint8_t hour, uint8_t minute, uint8_t editing);
  void showWiFiSetup(bool connecting, const char* ssid = "");
  void showWiFiConnected(const char* ip);
  void update();
  bool isInitialized();
};

#endif // DISPLAY_MANAGER_H
