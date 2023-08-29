#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WifiManager/WifiManager.h"
#include "SinalIRManager/SinalIRManager.h"
#include "PinConstants/PinConstants.cpp"

WifiManager wifiManager;
SinalIRManager sinalIRManager;

const char *SERVER_MQTT = "broker.hivemq.com";
const char *SUBSCRIBED_TOPIC_ACTION = "air-conditioner/1/state";
const char *PUBLISH_TOPIC_TEMP = "air-conditioner/1/temp";

char msgTemperatura[1];
float temperatura;
boolean retained = true;
float tempExterna = 30;

WiFiClient espClient;
PubSubClient client(espClient);

void displayMessageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");

  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

void displayMqttNotConnected()
{
  Serial.print("Não foi possível se conectar. Estado do esp: ");
  Serial.print(client.state());
  Serial.println("Tentando novamente em 5 segundos");
}

void reconnect()
{
  byte willQoS = 0;
  const char *willTopic = "esp/status";
  const char *willMessage = "OFF_LINE";
  boolean willRetain = true;
  const char *message = "ON_LINE";
  boolean retained = true;
  String clientId = "ESP8266Client01p";

  while (!client.connected())
  {
    Serial.print("Tentando estabelecer conexão MQTT...");

    if (client.connect(clientId.c_str(), willTopic, willQoS, willRetain,
                       willMessage))
    {
      // client.publish("esp/status", (byte*)message, strlen(message),
      // retained);
      client.subscribe(SUBSCRIBED_TOPIC_ACTION);
      Serial.print("Conectado e conectado ao tópico: ");
      Serial.println(SUBSCRIBED_TOPIC_ACTION);
    }
    else
    {
      displayMqttNotConnected();
      delay(5000);
    }
  }
}

void measureTemperature()
{
  int valorObtido = analogRead(A0);
  float milivolts = (valorObtido/1024.0) * 3300; 
  float temperatura = milivolts/10;
  Serial.println("");
  Serial.print("temperatura medida: ");
  Serial.println(temperatura);
}

// Mqtt protocol
void callback(char *topic, byte *payload, unsigned int length)
{
  displayMessageReceived(topic, payload, length);
  char command = (char)payload[0];

  sinalIRManager.getIrrecvInstance().disableIRIn();

  if (command == TURN_ON_COMMAND)
  {
    sinalIRManager.turnOnAirConditionerSignal();
    Serial.println("Ligando o ar-condicionado...");
    delay(60000);
    measureTemperature();
    if (temperatura < tempExterna)
    {
      Serial.println("Ar ligado");
      msgTemperatura[0] = {1};
      client.publish("air-conditioner/1/temp", (byte*)msgTemperatura, strlen(msgTemperatura), retained);
    } 
    else
    {
      Serial.print("Ar com defeito");
      msgTemperatura[0] = {0};
      client.publish("air-conditioner/1/temp", (byte*)msgTemperatura, strlen(msgTemperatura), retained);
    }
  }
  else if (command == TURN_OFF_COMMAND)
  {
    sinalIRManager.turnOffAirConditionerSignal();
    Serial.println("Desligando o ar-condicionado...");
  }
}

void setupMqtt()
{
  client.setServer(SERVER_MQTT, 1883);
  client.setCallback(callback);
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  decode_results currentDecodedSignal;
  if (sinalIRManager.getIrrecvInstance().decode(&currentDecodedSignal) &&
      decodedSignalsTotal < MAX_DECODING_SIGNAL_ATTEMPTS)
  {
    sinalIRManager.receiveDecodeSignals(currentDecodedSignal);
    decodedSignalsTotal++;
  }
}

// Set up methods
void setup()
{
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  wifiManager.connectToWiFi();
  setupMqtt();
  Serial.begin(PinConstants::kBaudRate);
  sinalIRManager.setupSignalsDecoding();
}