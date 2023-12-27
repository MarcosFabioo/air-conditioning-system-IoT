#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WifiManager/WifiManager.h"
#include "PinConstants/PinConstants.cpp"
#include "AirConditioners/irMideaProtocol.h"
#include "AirConditioners/irTecoProtocol.h"
#include <ArduinoJson.h>

WifiManager wifiManager;
// IrMideaProtocol irMidea;

char TURN_ON_COMMAND = '1';
char TURN_OFF_COMMAND = '0';

const char *SERVER_MQTT = "broker.hivemq.com";

String SUBSCRIBED_TOPIC = "";
String AIR_CONDITIONER_TOPIC = "";

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

void reconnect()
{
  wifiManager.connectToWiFi();

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
      client.subscribe("air-conditioners/create");

      if (AIR_CONDITIONER_TOPIC != "")
      {
        const char *air_conditioner_topic_ptr = AIR_CONDITIONER_TOPIC.c_str();
        client.subscribe(air_conditioner_topic_ptr);
        Serial.print("Conectado e conectado ao tópico: ");
        Serial.println(air_conditioner_topic_ptr);
      }
    }
    else
    {
      Serial.println("Protocolo MQTT não foi conectado");
    }
  }
}

// Perceba que retorna um IrBaseProtocol
// Isso é possível porque IrMideaProtocol herda de IrBaseProtocol
IrBaseProtocol *getProtocol(String protocol)
{
  if (protocol == "midea")
  {
    IrMideaProtocol *irMideaProtocol = new IrMideaProtocol();
    irMideaProtocol->initialize();
    return irMideaProtocol;
  }
  else if (protocol == "teco")
  {
    IrTecoProtocol *irTecoProtocol = new IrTecoProtocol();
    irTecoProtocol->initialize();
    return irTecoProtocol;
  }
  else
  {
    return NULL;
  }
}

void handleChangeState(DynamicJsonDocument &doc)
{
  String state = doc["data"]["state"];
  String protocol = doc["data"]["protocol"];
  IrBaseProtocol *irProtocol = getProtocol(protocol);

  if (state == "on")
  {
    irProtocol->setOn();
  }
  else if (state == "off")
  {
    irProtocol->setOff();
  }
}

void handleCreateAirConditioner(DynamicJsonDocument &doc)
{
  int air_conditioner_id = doc["data"]["id"];
  // String protocol = doc["data"]["protocol"];

  // TODO: Lidar com subtópicos: create, state, temperature...
  // Dá pra utilizar wildcards
  // https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/
  AIR_CONDITIONER_TOPIC = "air-conditioners/" + String(air_conditioner_id);
  Serial.println(AIR_CONDITIONER_TOPIC);
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
  wifiManager.connectToWiFi();
  // setupMqtt();
  Serial.begin(PinConstants::kBaudRate);
}