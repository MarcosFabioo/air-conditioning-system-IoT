#include "WifiManager.h"

const char *WIFI_SSID = "Marcos_NET";
const char *WIFI_PASSWORD = "05102202";

void WifiManager::connectToWiFi()
{
  unsigned long startTime = millis();
  bool connecting = true;

  displayWifiConnecting();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (connecting)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      connecting = false;
    }

    if (millis() - startTime >= 10 * 1000) // 10 seconds
    {
      connecting = false;
      Serial.println("Falha ao conectar na rede WiFi");
    }

    delay(100); // Avoid spamming WiFi network
  }

  displayWifiSuccess();
}

void WifiManager::displayWifiConnecting()
{
  Serial.println();
  Serial.print("Conectando na rede: ");
  Serial.println(WIFI_SSID);
}

void WifiManager::displayWifiSuccess()
{
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}