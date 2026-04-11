#include "wifi_manager.h"

WiFiManager::WiFiManager() 
  : currentState(WIFI_OFF),
    initialized(false),
    lastConnectionAttempt(0),
    connectionAttempts(0) {
}

void WiFiManager::begin() {
  WiFi.mode(WIFI_STA);
  initialized = true;
  DEBUG_PRINTLN("WiFi manager initialized");
}

void WiFiManager::loadCredentials() {
  EEPROM.begin(EEPROM_SIZE);
  
  char ssid[EEPROM_WIFI_SSID_SIZE] = {0};
  char password[EEPROM_WIFI_PASS_SIZE] = {0};
  
  for (int i = 0; i < EEPROM_WIFI_SSID_SIZE; i++) {
    ssid[i] = EEPROM.read(EEPROM_WIFI_SSID_ADDR + i);
  }
  
  for (int i = 0; i < EEPROM_WIFI_PASS_SIZE; i++) {
    password[i] = EEPROM.read(EEPROM_WIFI_PASS_ADDR + i);
  }
  
  if (strlen(ssid) > 0) {
    DEBUG_PRINTF("Loaded WiFi credentials: SSID=%s\n", ssid);
  }
}

void WiFiManager::saveCredentials(const char* ssid, const char* password) {
  EEPROM.begin(EEPROM_SIZE);
  
  // Write SSID
  for (int i = 0; i < EEPROM_WIFI_SSID_SIZE; i++) {
    if (i < strlen(ssid)) {
      EEPROM.write(EEPROM_WIFI_SSID_ADDR + i, ssid[i]);
    } else {
      EEPROM.write(EEPROM_WIFI_SSID_ADDR + i, 0);
    }
  }
  
  // Write password
  for (int i = 0; i < EEPROM_WIFI_PASS_SIZE; i++) {
    if (i < strlen(password)) {
      EEPROM.write(EEPROM_WIFI_PASS_ADDR + i, password[i]);
    } else {
      EEPROM.write(EEPROM_WIFI_PASS_ADDR + i, 0);
    }
  }
  
  EEPROM.commit();
  DEBUG_PRINTLN("WiFi credentials saved");
}

void WiFiManager::startAPMode() {
  currentState = WIFI_AP_MODE;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SmartAlarmClock", "12345678");
  DEBUG_PRINTLN("AP Mode started");
  DEBUG_PRINT("IP Address: ");
  DEBUG_PRINTLN(WiFi.softAPIP());
}

void WiFiManager::connectToWiFi() {
  loadCredentials();
  currentState = WIFI_CONNECTING;
  
  char ssid[EEPROM_WIFI_SSID_SIZE] = {0};
  char password[EEPROM_WIFI_PASS_SIZE] = {0};
  
  for (int i = 0; i < EEPROM_WIFI_SSID_SIZE; i++) {
    ssid[i] = EEPROM.read(EEPROM_WIFI_SSID_ADDR + i);
  }
  
  for (int i = 0; i < EEPROM_WIFI_PASS_SIZE; i++) {
    password[i] = EEPROM.read(EEPROM_WIFI_PASS_ADDR + i);
  }
  
  if (strlen(ssid) == 0) {
    DEBUG_PRINTLN("No WiFi credentials found, starting AP mode");
    startAPMode();
    return;
  }
  
  DEBUG_PRINTF("Connecting to WiFi: %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  lastConnectionAttempt = millis();
  connectionAttempts = 0;
}

void WiFiManager::update() {
  if (currentState == WIFI_CONNECTING) {
    if (WiFi.status() == WL_CONNECTED) {
      currentState = WIFI_CONNECTED;
      DEBUG_PRINT("WiFi connected! IP: ");
      DEBUG_PRINTLN(WiFi.localIP());
    } else if (millis() - lastConnectionAttempt > WIFI_TIMEOUT_MS) {
      connectionAttempts++;
      if (connectionAttempts >= 3) {
        DEBUG_PRINTLN("WiFi connection failed, starting AP mode");
        startAPMode();
      } else {
        DEBUG_PRINTLN("Retrying WiFi connection...");
        lastConnectionAttempt = millis();
      }
    }
  } else if (currentState == WIFI_CONNECTED) {
    if (WiFi.status() != WL_CONNECTED) {
      currentState = WIFI_DISCONNECTED;
      DEBUG_PRINTLN("WiFi disconnected");
    }
  }
}

WiFiState WiFiManager::getState() {
  return currentState;
}

String WiFiManager::getIP() {
  if (currentState == WIFI_CONNECTED) {
    return WiFi.localIP().toString();
  } else if (currentState == WIFI_AP_MODE) {
    return WiFi.softAPIP().toString();
  }
  return "0.0.0.0";
}

String WiFiManager::getSSID() {
  if (currentState == WIFI_CONNECTED) {
    return WiFi.SSID();
  }
  return "N/A";
}

int WiFiManager::getRSSI() {
  if (currentState == WIFI_CONNECTED) {
    return WiFi.RSSI();
  }
  return -100;
}

bool WiFiManager::isConnected() {
  return currentState == WIFI_CONNECTED;
}

bool WiFiManager::isInitialized() {
  return initialized;
}
