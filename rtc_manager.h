#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <Wire.h>
#include "RTClib.h"
#include "config.h"

class RTCManager {
private:
  RTC_DS3231 rtc;
  bool initialized;

public:
  RTCManager();
  bool begin();
  DateTime getCurrentTime();
  void setTime(uint16_t year, uint8_t month, uint8_t day, 
               uint8_t hour, uint8_t minute, uint8_t second);
  uint8_t getHour();
  uint8_t getMinute();
  uint8_t getSecond();
  uint8_t getDayOfWeek(); // 0=Sunday, 1=Monday, ..., 6=Saturday
  bool isInitialized();
  void printTime();
};

#endif // RTC_MANAGER_H
