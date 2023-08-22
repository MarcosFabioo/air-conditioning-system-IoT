#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "wifi.h"

const char* WIFI_SSID = "Consultorio";
const char* WIFI_PASSWORD = "10203040";

void displayWifiConnecting() {
  Serial.println();
  Serial.print("Conectando na rede: ");
  Serial.println(WIFI_SSID);
}

void displayWifiSuccess() {
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectToWiFi() {
  delay(10);
  displayWifiConnecting();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.macAddress());
  }

  displayWifiSuccess();
}