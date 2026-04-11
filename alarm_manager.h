#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <EEPROM.h>
#include "config.h"

struct Alarm {
  uint8_t hour;
  uint8_t minute;
  bool enabled;
  uint8_t daysMask; // Bit 0=Sun, 1=Mon, ..., 6=Sat
  uint8_t pattern;  // Vibration pattern: 0=gentle, 1=medium, 2=strong
};

class AlarmManager {
private:
  Alarm alarms[MAX_ALARMS];
  bool initialized;
  
  void saveAlarmToEEPROM(uint8_t index);
  void loadAlarmFromEEPROM(uint8_t index);

public:
  AlarmManager();
  bool begin();
  void createAlarm(uint8_t index, uint8_t hour, uint8_t minute, uint8_t daysMask, uint8_t pattern);
  void updateAlarm(uint8_t index, uint8_t hour, uint8_t minute, uint8_t daysMask, uint8_t pattern);
  void deleteAlarm(uint8_t index);
  void enableAlarm(uint8_t index);
  void disableAlarm(uint8_t index);
  
  Alarm getAlarm(uint8_t index);
  uint8_t getAlarmCount();
  bool shouldAlarmTrigger(uint8_t index, uint8_t currentHour, uint8_t currentMinute, uint8_t dayOfWeek);
  
  void printAllAlarms();
  bool isInitialized();
};

#endif // ALARM_MANAGER_H
