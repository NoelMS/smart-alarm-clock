#include "alarm_manager.h"

AlarmManager::AlarmManager() : initialized(false) {
  for (int i = 0; i < MAX_ALARMS; i++) {
    alarms[i].hour = 7;
    alarms[i].minute = 0;
    alarms[i].enabled = false;
    alarms[i].daysMask = 0x7F; // All days
    alarms[i].pattern = DEFAULT_VIBRATION_PATTERN;
  }
}

bool AlarmManager::begin() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Load all alarms from EEPROM
  for (int i = 0; i < MAX_ALARMS; i++) {
    loadAlarmFromEEPROM(i);
  }
  
  initialized = true;
  DEBUG_PRINTLN("Alarm manager initialized");
  printAllAlarms();
  return true;
}

void AlarmManager::saveAlarmToEEPROM(uint8_t index) {
  if (index >= MAX_ALARMS) return;
  
  uint16_t addr = EEPROM_ALARM_START + (index * EEPROM_ALARM_SIZE);
  
  EEPROM.write(addr, alarms[index].hour);
  EEPROM.write(addr + 1, alarms[index].minute);
  EEPROM.write(addr + 2, alarms[index].enabled ? 1 : 0);
  EEPROM.write(addr + 3, alarms[index].daysMask);
  EEPROM.write(addr + 4, alarms[index].pattern);
  
  EEPROM.commit();
  DEBUG_PRINTF("Alarm %d saved to EEPROM\n", index);
}

void AlarmManager::loadAlarmFromEEPROM(uint8_t index) {
  if (index >= MAX_ALARMS) return;
  
  uint16_t addr = EEPROM_ALARM_START + (index * EEPROM_ALARM_SIZE);
  
  alarms[index].hour = EEPROM.read(addr);
  alarms[index].minute = EEPROM.read(addr + 1);
  alarms[index].enabled = EEPROM.read(addr + 2) == 1;
  alarms[index].daysMask = EEPROM.read(addr + 3);
  alarms[index].pattern = EEPROM.read(addr + 4);
  
  // Validate data
  if (alarms[index].hour > 23) alarms[index].hour = 7;
  if (alarms[index].minute > 59) alarms[index].minute = 0;
  if (alarms[index].pattern > 2) alarms[index].pattern = DEFAULT_VIBRATION_PATTERN;
}

void AlarmManager::createAlarm(uint8_t index, uint8_t hour, uint8_t minute, uint8_t daysMask, uint8_t pattern) {
  if (index >= MAX_ALARMS) return;
  
  alarms[index].hour = (hour < 24) ? hour : 0;
  alarms[index].minute = (minute < 60) ? minute : 0;
  alarms[index].enabled = true;
  alarms[index].daysMask = daysMask;
  alarms[index].pattern = (pattern <= 2) ? pattern : DEFAULT_VIBRATION_PATTERN;
  
  saveAlarmToEEPROM(index);
  DEBUG_PRINTF("Alarm %d created: %02d:%02d\n", index, hour, minute);
}

void AlarmManager::updateAlarm(uint8_t index, uint8_t hour, uint8_t minute, uint8_t daysMask, uint8_t pattern) {
  createAlarm(index, hour, minute, daysMask, pattern);
}

void AlarmManager::deleteAlarm(uint8_t index) {
  if (index >= MAX_ALARMS) return;
  
  alarms[index].hour = 7;
  alarms[index].minute = 0;
  alarms[index].enabled = false;
  alarms[index].daysMask = 0;
  alarms[index].pattern = DEFAULT_VIBRATION_PATTERN;
  
  saveAlarmToEEPROM(index);
  DEBUG_PRINTF("Alarm %d deleted\n", index);
}

void AlarmManager::enableAlarm(uint8_t index) {
  if (index < MAX_ALARMS) {
    alarms[index].enabled = true;
    saveAlarmToEEPROM(index);
  }
}

void AlarmManager::disableAlarm(uint8_t index) {
  if (index < MAX_ALARMS) {
    alarms[index].enabled = false;
    saveAlarmToEEPROM(index);
  }
}

Alarm AlarmManager::getAlarm(uint8_t index) {
  static Alarm empty = {7, 0, false, 0, 0};
  if (index >= MAX_ALARMS) return empty;
  return alarms[index];
}

uint8_t AlarmManager::getAlarmCount() {
  return MAX_ALARMS;
}

bool AlarmManager::shouldAlarmTrigger(uint8_t index, uint8_t currentHour, uint8_t currentMinute, uint8_t dayOfWeek) {
  if (index >= MAX_ALARMS) return false;
  if (!alarms[index].enabled) return false;
  if (alarms[index].hour != currentHour) return false;
  if (alarms[index].minute != currentMinute) return false;
  
  // Check if today is in the daysMask
  return (alarms[index].daysMask & (1 << dayOfWeek)) != 0;
}

void AlarmManager::printAllAlarms() {
  DEBUG_PRINTLN("\n=== All Alarms ===");
  for (int i = 0; i < MAX_ALARMS; i++) {
    DEBUG_PRINTF("Alarm %d: %02d:%02d %s Days: 0x%02X Pattern: %d\n",
                i, alarms[i].hour, alarms[i].minute,
                alarms[i].enabled ? "ENABLED" : "DISABLED",
                alarms[i].daysMask, alarms[i].pattern);
  }
  DEBUG_PRINTLN("==================\n");
}

bool AlarmManager::isInitialized() {
  return initialized;
}
