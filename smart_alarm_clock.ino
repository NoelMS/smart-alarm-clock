#include <Wire.h>
#include <SPIFFS.h>
#include "config.h"
#include "rtc_manager.h"
#include "display_manager.h"
#include "button_handler.h"
#include "alarm_manager.h"
#include "alert_controller.h"
#include "wifi_manager.h"
#include "web_server.h"

// Global Objects
RTCManager rtcManager;
DisplayManager displayManager;
ButtonHandler buttonHandler;
AlarmManager alarmManager;
AlertController alertController;
WiFiManager wifiManager;
AsyncWebServer asyncServer(80);

// State Management
enum SystemState {
  STATE_IDLE,
  STATE_MENU,
  STATE_SET_ALARM,
  STATE_ALARM_ACTIVE,
  STATE_ALARM_SNOOZED,
  STATE_WIFI_CONFIG
};

SystemState currentState = STATE_IDLE;
uint8_t menuSelection = 0;
uint8_t selectedAlarmIndex = 0;
uint8_t editingField = 0; // 0=hour, 1=minute
uint8_t tempHour = 7;
uint8_t tempMinute = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long snoozeEndTime = 0;
bool activeAlarmTriggered = false;

// Forward declarations
void handleButtonPress(uint8_t buttonID, bool longPress);
void updateDisplay();
void checkAlarmTrigger();
void handleAlarmActive();

void setup() {
  Serial.begin(DEBUG_SERIAL);
  delay(1000);
  
  DEBUG_PRINTLN("\n\n=== Smart Alarm Clock Starting ===\n");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    DEBUG_PRINTLN("SPIFFS Mount Failed");
  } else {
    DEBUG_PRINTLN("SPIFFS Mounted Successfully");
  }
  
  // Initialize all managers
  DEBUG_PRINTLN("Initializing components...");
  
  rtcManager.begin();
  delay(500);
  
  displayManager.begin();
  delay(500);
  
  buttonHandler.begin();
  buttonHandler.setCallback(handleButtonPress);
  delay(500);
  
  alarmManager.begin();
  delay(500);
  
  alertController.begin();
  delay(500);
  
  wifiManager.begin();
  delay(500);
  
  // Show initialization complete
  displayManager.showMessage("Smart Alarm", "Clock", "Initialized", "Press MENU");
  delay(2000);
  
  // Start WiFi (on-demand mode - can be triggered from menu)
  DEBUG_PRINTLN("System initialized - WiFi disabled by default (on-demand mode)");
  DEBUG_PRINTLN("Press button 1 (Mode/Menu) to access menu and enable WiFi");
  
  DEBUG_PRINTLN("\n=== Setup Complete ===\n");
}

void loop() {
  // Update all components
  buttonHandler.update();
  alertController.update();
  wifiManager.update();
  
  // Update display every 500ms
  if (millis() - lastDisplayUpdate >= 500) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  // Check for alarm trigger (only if not already triggered in this minute)
  if (currentState != STATE_ALARM_ACTIVE && currentState != STATE_ALARM_SNOOZED) {
    checkAlarmTrigger();
  }
  
  // Handle active alarm
  if (currentState == STATE_ALARM_ACTIVE) {
    handleAlarmActive();
  }
  
  // Handle snoozed alarm
  if (currentState == STATE_ALARM_SNOOZED) {
    if (millis() >= snoozeEndTime) {
      DEBUG_PRINTLN("Snooze ended, restarting alarm");
      alertController.startAlert(alarmManager.getAlarm(selectedAlarmIndex).pattern);
      currentState = STATE_ALARM_ACTIVE;
      activeAlarmTriggered = false;
    }
  }
}

void handleButtonPress(uint8_t buttonID, bool longPress) {
  DEBUG_PRINTF("Button %d pressed (Long: %s)\n", buttonID, longPress ? "true" : "false");
  
  switch (currentState) {
    case STATE_IDLE:
      if (buttonID == 0) { // Mode/Menu button
        currentState = STATE_MENU;
        menuSelection = 0;
        DEBUG_PRINTLN("Entering menu");
      }
      break;
    
    case STATE_MENU: {
      const char* menuItems[] = {"Set Alarm", "View Alarms", "WiFi Setup", "Back"};
      
      if (buttonID == 1) { // Increment
        menuSelection = (menuSelection + 1) % 4;
      } else if (buttonID == 2) { // Decrement
        menuSelection = (menuSelection == 0) ? 3 : menuSelection - 1;
      } else if (buttonID == 3) { // Confirm
        if (menuSelection == 0) { // Set Alarm
          currentState = STATE_SET_ALARM;
          selectedAlarmIndex = 0;
          Alarm alm = alarmManager.getAlarm(0);
          tempHour = alm.hour;
          tempMinute = alm.minute;
          editingField = 0;
        } else if (menuSelection == 1) { // View Alarms
          // Navigate to view alarms state
          menuSelection = 0;
        } else if (menuSelection == 2) { // WiFi Setup
          currentState = STATE_WIFI_CONFIG;
          displayManager.showWiFiSetup(true, "Connecting...");
          wifiManager.startAPMode();
        }
      } else if (buttonID == 4) { // Snooze/Stop - exit menu
        currentState = STATE_IDLE;
      }
      break;
    }
    
    case STATE_SET_ALARM:
      if (buttonID == 1) { // Increment
        if (editingField == 0) {
          tempHour = (tempHour + 1) % 24;
        } else {
          tempMinute = (tempMinute + 5) % 60; // Increment by 5 minutes
        }
      } else if (buttonID == 2) { // Decrement
        if (editingField == 0) {
          tempHour = (tempHour == 0) ? 23 : tempHour - 1;
        } else {
          tempMinute = (tempMinute < 5) ? 55 : tempMinute - 5;
        }
      } else if (buttonID == 3) { // Confirm
        if (editingField == 0) {
          editingField = 1; // Move to minute editing
        } else {
          // Save alarm
          alarmManager.createAlarm(selectedAlarmIndex, tempHour, tempMinute, 0x7F, DEFAULT_VIBRATION_PATTERN);
          alarmManager.enableAlarm(selectedAlarmIndex);
          currentState = STATE_MENU;
          menuSelection = 0;
          DEBUG_PRINTF("Alarm saved: %02d:%02d\n", tempHour, tempMinute);
        }
      } else if (buttonID == 4) { // Snooze/Stop - cancel
        currentState = STATE_MENU;
        menuSelection = 0;
      }
      break;
    
    case STATE_ALARM_ACTIVE:
      if (buttonID == 4 && !longPress) { // Short press snooze
        DEBUG_PRINTLN("Alarm snoozed");
        alertController.stopAlert();
        currentState = STATE_ALARM_SNOOZED;
        snoozeEndTime = millis() + SNOOZE_DURATION_MS;
      } else if (buttonID == 4 && longPress) { // Long press to stop
        DEBUG_PRINTLN("Alarm stopped");
        alertController.stopAlert();
        currentState = STATE_IDLE;
        activeAlarmTriggered = false;
      }
      break;
    
    case STATE_WIFI_CONFIG:
      if (buttonID == 4) { // Exit WiFi config
        currentState = STATE_MENU;
        menuSelection = 0;
      }
      break;
    
    default:
      break;
  }
}

void updateDisplay() {
  uint8_t hour = rtcManager.getHour();
  uint8_t minute = rtcManager.getMinute();
  
  switch (currentState) {
    case STATE_IDLE: {
      Alarm nextAlarm = alarmManager.getAlarm(0);
      displayManager.showTime(hour, minute);
      break;
    }
    
    case STATE_MENU: {
      const char* menuItems[] = {"Set Alarm", "View Alarms", "WiFi Setup", "Back"};
      displayManager.showMenu(menuItems, 4, menuSelection);
      break;
    }
    
    case STATE_SET_ALARM:
      displayManager.showSetAlarmUI(tempHour, tempMinute, editingField);
      break;
    
    case STATE_ALARM_ACTIVE:
      displayManager.showAlarmTriggered(hour, minute);
      break;
    
    case STATE_ALARM_SNOOZED: {
      char snoozedMsg[50];
      unsigned long remainingSnooze = snoozeEndTime - millis();
      snprintf(snoozedMsg, sizeof(snoozedMsg), "Snoozed: %lu sec", remainingSnooze / 1000);
      displayManager.showMessage("ALARM SNOOZED", snoozedMsg, "Press MENU", "to continue");
      break;
    }
    
    case STATE_WIFI_CONFIG: {
      if (wifiManager.getState() == WIFI_CONNECTED) {
        displayManager.showWiFiConnected(wifiManager.getIP().c_str());
      } else {
        displayManager.showWiFiSetup(true, "AP Mode");
      }
      break;
    }
    
    default:
      break;
  }
}

void checkAlarmTrigger() {
  static uint8_t lastMinuteChecked = 255;
  
  uint8_t currentMinute = rtcManager.getMinute();
  
  // Only check once per minute
  if (currentMinute == lastMinuteChecked) {
    return;
  }
  
  lastMinuteChecked = currentMinute;
  
  uint8_t hour = rtcManager.getHour();
  uint8_t dayOfWeek = rtcManager.getDayOfWeek();
  
  // Check all alarms
  for (uint8_t i = 0; i < MAX_ALARMS; i++) {
    if (alarmManager.shouldAlarmTrigger(i, hour, currentMinute, dayOfWeek)) {
      DEBUG_PRINTF("Alarm %d triggered!\n", i);
      selectedAlarmIndex = i;
      Alarm alm = alarmManager.getAlarm(i);
      alertController.startAlert((VibrationType)alm.pattern);
      currentState = STATE_ALARM_ACTIVE;
      activeAlarmTriggered = true;
      return; // Only trigger one alarm at a time
    }
  }
}

void handleAlarmActive() {
  alertController.update();
  
  // Auto-stop after timeout
  if (alertController.shouldAutoStop() && !activeAlarmTriggered) {
    DEBUG_PRINTLN("Alarm auto-stopped after timeout");
    alertController.stopAlert();
    currentState = STATE_IDLE;
    activeAlarmTriggered = false;
  }
}
