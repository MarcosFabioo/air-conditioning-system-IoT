#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

class WifiManager
{
public:
  void connectToWiFi();

private:
  void displayWifiConnecting();
  void displayWifiSuccess();
};

#endif