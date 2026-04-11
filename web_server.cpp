#include "web_server.h"

WebServer::WebServer(uint16_t port) 
  : server(port),
    alarmManager(nullptr),
    rtcManager(nullptr),
    initialized(false) {
}

void WebServer::begin(AlarmManager* alm, RTCManager* rtc) {
  alarmManager = alm;
  rtcManager = rtc;
  
  setupRoutes();
  
  server.begin();
  initialized = true;
  DEBUG_PRINTLN("Web server started on port 80");
}

void WebServer::setupRoutes() {
  // GET /api/time - Get current time
  server.on("/api/time", HTTP_GET, [this](AsyncWebServerRequest* request) {
    DateTime now = rtcManager->getCurrentTime();
    
    char response[100];
    snprintf(response, sizeof(response), 
             "{\"hour\":%d,\"minute\":%d,\"second\":%d}",
             now.hour(), now.minute(), now.second());
    
    request->send(200, "application/json", response);
  });
  
  // GET /api/alarms - Get all alarms
  server.on("/api/alarms", HTTP_GET, [this](AsyncWebServerRequest* request) {
    String response = "[";
    
    for (uint8_t i = 0; i < MAX_ALARMS; i++) {
      Alarm alm = alarmManager->getAlarm(i);
      
      if (i > 0) response += ",";
      
      char alarmStr[150];
      snprintf(alarmStr, sizeof(alarmStr),
               "{\"id\":%d,\"hour\":%d,\"minute\":%d,\"enabled\":%s,\"days\":\"0x%02x\",\"pattern\":%d}",
               i, alm.hour, alm.minute,
               alm.enabled ? "true" : "false",
               alm.daysMask, alm.pattern);
      
      response += alarmStr;
    }
    
    response += "]";
    request->send(200, "application/json", response);
  });
  
  // POST /api/alarms - Create/Update alarm
  server.on("/api/alarms", HTTP_POST, [this](AsyncWebServerRequest* request) {}, nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
    // Parse JSON and update alarm
    if (request->hasParam("id") && request->hasParam("hour") && request->hasParam("minute")) {
      uint8_t id = request->getParam("id")->value().toInt();
      uint8_t hour = request->getParam("hour")->value().toInt();
      uint8_t minute = request->getParam("minute")->value().toInt();
      uint8_t days = request->hasParam("days") ? request->getParam("days")->value().toInt() : 0x7F;
      uint8_t pattern = request->hasParam("pattern") ? request->getParam("pattern")->value().toInt() : 1;
      
      if (id < MAX_ALARMS) {
        alarmManager->updateAlarm(id, hour, minute, days, pattern);
        request->send(200, "application/json", "{\"status\":\"ok\"}");
      } else {
        request->send(400, "application/json", "{\"status\":\"error\"}");
      }
    } else {
      request->send(400, "application/json", "{\"status\":\"missing params\"}");
    }
  });
  
  // DELETE /api/alarms/{id} - Delete alarm
  server.on("/api/alarms", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
    if (request->hasParam("id")) {
      uint8_t id = request->getParam("id")->value().toInt();
      
      if (id < MAX_ALARMS) {
        alarmManager->disableAlarm(id);
        request->send(200, "application/json", "{\"status\":\"deleted\"}");
      } else {
        request->send(400, "application/json", "{\"status\":\"error\"}");
      }
    } else {
      request->send(400, "application/json", "{\"status\":\"missing id\"}");
    }
  });
  
  // PUT /api/alarms/{id}/toggle - Toggle alarm
  server.on("/api/alarms", HTTP_PUT, [this](AsyncWebServerRequest* request) {
    if (request->hasParam("id") && request->hasParam("enabled")) {
      uint8_t id = request->getParam("id")->value().toInt();
      bool enabled = request->getParam("enabled")->value() == "true";
      
      if (id < MAX_ALARMS) {
        if (enabled) {
          alarmManager->enableAlarm(id);
        } else {
          alarmManager->disableAlarm(id);
        }
        request->send(200, "application/json", "{\"status\":\"updated\"}");
      } else {
        request->send(400, "application/json", "{\"status\":\"error\"}");
      }
    } else {
      request->send(400, "application/json", "{\"status\":\"missing params\"}");
    }
  });
  
  // GET /api/status - Device status
  server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest* request) {
    DateTime now = rtcManager->getCurrentTime();
    
    char response[200];
    snprintf(response, sizeof(response),
             "{\"time\":\"%04d-%02d-%02d %02d:%02d:%02d\",\"uptime\":%lu}",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second(),
             millis() / 1000);
    
    request->send(200, "application/json", response);
  });
  
  // Serve index.html for root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  // Serve static files (CSS, JS)
  server.serveStatic("/", SPIFFS, "/");
  
  // 404 handler
  server.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not Found");
  });
}

void WebServer::stop() {
  server.end();
  initialized = false;
}

bool WebServer::isInitialized() {
  return initialized;
}
