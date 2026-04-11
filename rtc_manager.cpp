#include "rtc_manager.h"

RTCManager::RTCManager() : initialized(false) {
}

bool RTCManager::begin() {
  if (!rtc.begin()) {
    DEBUG_PRINTLN("RTC not found!");
    return false;
  }
  
  if (rtc.lostPower()) {
    DEBUG_PRINTLN("RTC lost power, setting to compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  initialized = true;
  DEBUG_PRINTLN("RTC initialized successfully");
  return true;
}

DateTime RTCManager::getCurrentTime() {
  return rtc.now();
}

void RTCManager::setTime(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
  DEBUG_PRINTF("Time set to: %04d-%02d-%02d %02d:%02d:%02d\n", 
              year, month, day, hour, minute, second);
}

uint8_t RTCManager::getHour() {
  return rtc.now().hour();
}

uint8_t RTCManager::getMinute() {
  return rtc.now().minute();
}

uint8_t RTCManager::getSecond() {
  return rtc.now().second();
}

uint8_t RTCManager::getDayOfWeek() {
  // DS3231 returns 0=Sunday, 1=Monday, etc
  return rtc.now().dayoftheweek();
}

bool RTCManager::isInitialized() {
  return initialized;
}

void RTCManager::printTime() {
  DateTime now = rtc.now();
  DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
              now.year(), now.month(), now.day(),
              now.hour(), now.minute(), now.second());
}
