#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WifiManager/WifiManager.h"
#include "PinConstants/PinConstants.cpp"
#include "AirConditioners/ir.Midea.h"
#include <ArduinoJson.h>
#include <iostream>

WifiManager wifiManager;
IrMidea irMidea;

char TURN_ON_COMMAND = '1';
char TURN_OFF_COMMAND = '0';

const char *SERVER_MQTT = "broker.hivemq.com";

// É necessário usar "std::string" para fazer a concatenação de string com o id
// passado durante a criação do ar-condicionado
std::string SUBSCRIBED_TOPIC = "air-conditioner/1/state";

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
      //

      // É necessário converter para const char* para utilizar o método
      // A conversão é feita com o método .c_str()
      const char *subscribed_topic_ptr = SUBSCRIBED_TOPIC.c_str();
      client.subscribe(subscribed_topic_ptr);
      Serial.print("Conectado e conectado ao tópico: ");
      Serial.println(subscribed_topic_ptr);
    }
    else
    {
      displayMqttNotConnected();
      delay(5000);
    }
  }
}

void handleChangeState(DynamicJsonDocument &doc)
{
  String state = doc["data"]["state"];

  // TODO: Lidar com diferentes protocolos
  // O ar-condicionado aqui está "hardcoded"
  if (state == "on")
  {
    irMidea.setOn();
  }
  else if (state == "off")
  {
    irMidea.setOff();
  }
}

void handleCreateAirConditioner(DynamicJsonDocument &doc)
{
  int air_conditioner_id = doc["data"]["id"];
  String protocol = doc["data"]["protocol"];

  // TODO: Lidar com subtópicos: create, state, temperature...
  // Dá pra utilizar wildcards
  // https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/
  SUBSCRIBED_TOPIC = "air-conditioners/" + std::to_string(air_conditioner_id);
}

// Mqtt protocol
void callback(char *topic, byte *payload, unsigned int length)
{
  displayMessageReceived(topic, payload, length);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  String action = doc["action"];

  if (action == "create")
  {
    handleCreateAirConditioner(doc);
  }
  else if (action == "change-temperature")
  {
    int temperature_in_celsius = doc["data"]["temperature"];
    Serial.println(temperature_in_celsius);
  }
  else if (action == "change-state")
  {
    handleChangeState(doc);
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
}

// Set up methods
void setup()
{
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  wifiManager.connectToWiFi();
  setupMqtt();
  Serial.begin(PinConstants::kBaudRate);
  irMidea.initialize();
}