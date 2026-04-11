#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <EEPROM.h>
#include "config.h"

enum WiFiState {
  WIFI_OFF,
  WIFI_AP_MODE,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
  WIFI_ERROR
};

class WiFiManager {
private:
  WiFiState currentState;
  bool initialized;
  unsigned long lastConnectionAttempt;
  uint8_t connectionAttempts;
  
  void loadCredentials();
  void saveCredentials(const char* ssid, const char* password);

public:
  WiFiManager();
  void begin();
  void startAPMode();
  void connectToWiFi();
  void handleWiFiConfig();
  void update();
  
  WiFiState getState();
  String getIP();
  String getSSID();
  int getRSSI();
  
  bool isConnected();
  bool isInitialized();
};

#endif // WIFI_MANAGER_H
