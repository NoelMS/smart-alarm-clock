#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include "alarm_manager.h"
#include "rtc_manager.h"

class WebServer {
private:
  AsyncWebServer server;
  AlarmManager* alarmManager;
  RTCManager* rtcManager;
  bool initialized;

public:
  WebServer(uint16_t port = 80);
  void begin(AlarmManager* alm, RTCManager* rtc);
  void setupRoutes();
  void stop();
  bool isInitialized();
};

#endif // WEB_SERVER_H
