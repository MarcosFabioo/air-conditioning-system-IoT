#include "WifiManager.h"

const char *WIFI_SSID = "wIFRN-IoT";
const char *WIFI_PASSWORD = "deviceiotifrn";

void WifiManager::connectToWiFi()
{
  bool connecting = true;
  const int DELAY_TO_RECONNECT_IN_MS = 1000; // 1 second

  displayWifiConnecting();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (connecting)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      connecting = false;
    }

    delay(DELAY_TO_RECONNECT_IN_MS);

    if (WiFi.status() != WL_CONNECTED)
    {
      connecting = false;
      Serial.println("Falha ao conectar na rede WiFi");
    }
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
