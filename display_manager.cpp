#include "display_manager.h"

DisplayManager::DisplayManager() 
  : display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1), 
    initialized(false),
    lastUpdateTime(0) {
}

bool DisplayManager::begin() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    DEBUG_PRINTLN("OLED display not found!");
    return false;
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Alarm Clock");
  display.println("Initializing...");
  display.display();
  
  initialized = true;
  DEBUG_PRINTLN("Display initialized successfully");
  return true;
}

void DisplayManager::clear() {
  display.clearDisplay();
}

void DisplayManager::showTime(uint8_t hour, uint8_t minute) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 15);
  
  if (hour < 10) display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) display.print("0");
  display.print(minute);
  
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println("Press [MENU] for options");
  display.display();
}

void DisplayManager::showAlarmInfo(uint8_t hour, uint8_t minute, bool enabled) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);
  display.println("Next Alarm");
  
  display.setTextSize(2);
  display.setCursor(30, 30);
  if (hour < 10) display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) display.print("0");
  display.print(minute);
  
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.print(enabled ? "ENABLED" : "DISABLED");
  display.display();
}

void DisplayManager::showMenu(const char* items[], uint8_t count, uint8_t selected) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("=== MENU ===");
  
  for (uint8_t i = 0; i < count && i < 5; i++) {
    display.setCursor(5, 12 + (i * 10));
    if (i == selected) {
      display.print("> ");
      display.println(items[i]);
    } else {
      display.print("  ");
      display.println(items[i]);
    }
  }
  
  display.display();
}

void DisplayManager::showAlarmTriggered(uint8_t hour, uint8_t minute) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  display.println("ALARM!");
  
  display.setTextSize(2);
  display.setCursor(30, 30);
  if (hour < 10) display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) display.print("0");
  display.print(minute);
  
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println("Press SNOOZE or wait...");
  display.display();
}

void DisplayManager::showMessage(const char* line1, const char* line2, 
                                 const char* line3, const char* line4) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println(line1);
  display.setCursor(0, 15);
  display.println(line2);
  if (line3[0] != '\0') {
    display.setCursor(0, 30);
    display.println(line3);
  }
  if (line4[0] != '\0') {
    display.setCursor(0, 45);
    display.println(line4);
  }
  
  display.display();
}

void DisplayManager::showAlarmList(uint8_t alarms[][2], bool enabled[], uint8_t count, uint8_t selected) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("=== ALARMS ===");
  
  for (uint8_t i = 0; i < count; i++) {
    display.setCursor(5, 12 + (i * 10));
    if (i == selected) display.print("> ");
    else display.print("  ");
    
    if (alarms[i][0] < 10) display.print("0");
    display.print(alarms[i][0]);
    display.print(":");
    if (alarms[i][1] < 10) display.print("0");
    display.print(alarms[i][1]);
    display.print(" ");
    display.println(enabled[i] ? "ON" : "OFF");
  }
  
  display.display();
}

void DisplayManager::showSetAlarmUI(uint8_t hour, uint8_t minute, uint8_t editing) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("Set Alarm Time");
  
  display.setTextSize(2);
  display.setCursor(20, 20);
  
  if (editing == 0) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  if (hour < 10) display.print("0");
  display.print(hour);
  display.setTextColor(SSD1306_WHITE);
  display.print(":");
  
  if (editing == 1) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  if (minute < 10) display.print("0");
  display.print(minute);
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println("[+/-] adjust [OK] confirm");
  display.display();
}

void DisplayManager::showWiFiSetup(bool connecting, const char* ssid) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("WiFi Setup");
  
  if (connecting) {
    display.setCursor(0, 20);
    display.println("Connecting...");
    display.setCursor(0, 35);
    display.print("SSID: ");
    display.println(ssid);
  } else {
    display.setCursor(0, 20);
    display.println("Connect to WiFi:");
    display.setCursor(0, 35);
    display.println("SmartAlarmClock");
    display.setCursor(0, 50);
    display.println("Config: 192.168.4.1");
  }
  
  display.display();
}

void DisplayManager::showWiFiConnected(const char* ip) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.setCursor(0, 15);
  display.print("IP: ");
  display.println(ip);
  display.setCursor(0, 35);
  display.println("Access web interface");
  display.setCursor(0, 45);
  display.println("http://");
  display.println(ip);
  
  display.display();
}

void DisplayManager::update() {
  // Called periodically to refresh display if needed
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= DISPLAY_REFRESH_MS) {
    display.display();
    lastUpdateTime = currentTime;
  }
}

bool DisplayManager::isInitialized() {
  return initialized;
}
