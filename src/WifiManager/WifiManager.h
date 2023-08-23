#ifndef WifiManager_h
#define WifiManager_h

#include <Arduino.h>
#include <ESP8266WiFi.h>

class WifiManager {
public:
  void connectToWiFi();

private:
  void displayWifiConnecting();
  void displayWifiSuccess();
};

#endif