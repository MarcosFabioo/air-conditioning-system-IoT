#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WifiManager/WifiManager.h"
#include "PinConstants/PinConstants.cpp"
#include "AirConditioners/Midea/irMideaProtocol.h"
#include "AirConditioners/Teco/irTecoProtocol.h"
#include <ArduinoJson.h>

WifiManager wifiManager;

char TURN_ON_COMMAND = '1';
char TURN_OFF_COMMAND = '0';

const char *SERVER_MQTT = "broker.hivemq.com";

String SUBSCRIBED_TOPIC = "";

WiFiClient espClient;
PubSubClient client(espClient);
IrBaseProtocol *irProtocol = NULL;

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
    if (client.connect(clientId.c_str(), willTopic, willQoS, willRetain,
                       willMessage))
    {

      client.subscribe("air-conditioners/create");
      Serial.println("Tópico ainda não foi definido, crie um ar-condicionado via tópico");
    }
    else
    {
      displayMqttNotConnected();
      delay(1500);
    }
  }
}

// Perceba que retorna um IrBaseProtocol
// Isso é possível porque IrMideaProtocol herda de IrBaseProtocol
void setProtocol(String protocol)
{
  if (protocol == "midea")
  {
    irProtocol = new IrMideaProtocol();
  }
  else if (protocol == "teco")
  {
    irProtocol = new IrTecoProtocol();
  }
  else
  {
    return;
  }

  irProtocol->initialize();
}

void handleChangeState(DynamicJsonDocument &doc)
{
  String state = doc["data"]["state"];
  String protocol = doc["data"]["protocol"];

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
  String protocol = doc["data"]["protocol"];

  // TODO: Lidar com subtópicos: create, state, temperature...
  // Dá pra utilizar wildcards
  // https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/
  SUBSCRIBED_TOPIC = "air-conditioners/" + String(air_conditioner_id);
  const char *subscribed_topic_ptr = SUBSCRIBED_TOPIC.c_str();
  client.subscribe(subscribed_topic_ptr);
  Serial.print("Conectado ao tópico: ");
  Serial.println(subscribed_topic_ptr);

  setProtocol(protocol);
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
}