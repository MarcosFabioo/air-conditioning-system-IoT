#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <PubSubClient.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Toshiba.h>
#include "WifiManager/WifiManager.h"

WifiManager wifiManager;

const char* SERVER_MQTT = "broker.hivemq.com";
const char* SUBSCRIBED_TOPIC = "air-conditioner/1/state";

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t kRecvPin = 14;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 15;
const uint16_t kMinUnknownSize = 12;

const uint16_t kIrLed = 4;
IRsend irsend(kIrLed);

bool isTurnedOn = false;
int decodedSignalsTotal = 0;
const int MAX_DECODING_SIGNAL_ATTEMPTS = 2;

struct IRData {
  uint16_t* rawData;
  uint16_t rawDataLength;
};

IRData turnOffSignal;
IRData turnOnSignal;

char TURN_ON_COMMAND = '1';
char TURN_OFF_COMMAND = '0';

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
String description = "";

// Logging methods


void displayMessageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");

  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

void displayMqttNotConnected() {
  Serial.print("Não foi possível se conectar. Estado do esp: ");
  Serial.print(client.state());
  Serial.println("Tentando novamente em 5 segundos");
}


void reconnect() {
  byte willQoS = 0;
  const char* willTopic = "esp/status";
  const char* willMessage = "OFF_LINE";
  boolean willRetain = true;
  const char * message = "ON_LINE";
  boolean retained = true;
  String clientId = "ESP8266Client01p";

  while (!client.connected()) {
    Serial.print("Tentando estabelecer conexão MQTT...");

    if (client.connect(clientId.c_str(), willTopic, willQoS, willRetain,
                       willMessage)) {
      // client.publish("esp/status", (byte*)message, strlen(message),
      // retained);
      client.subscribe(SUBSCRIBED_TOPIC);
      Serial.print("Conectado e conectado ao tópico: ");
      Serial.println(SUBSCRIBED_TOPIC);
    } else {
      displayMqttNotConnected();
      delay(5000);
    }
  }
}

// Signal management methods
void turnOnAirConditionerSignal() {
  irsend.sendRaw(turnOnSignal.rawData, turnOnSignal.rawDataLength, 38);
}

void turnOffAirConditionerSignal() {
  irsend.sendRaw(turnOffSignal.rawData, turnOffSignal.rawDataLength, 38);
}

// Decoding methods
void decodeDaikin(decode_results* decodedSignal) {
  IRDaikinESP ac(0);
  ac.setRaw(decodedSignal->state);
  description = ac.toString();
}

void decodeFujitsu(decode_results* decodedSignal) {
  IRFujitsuAC ac(0);
  ac.setRaw(decodedSignal->state, decodedSignal->bits / 8);
  description = ac.toString();
}

void decodeToshiba(decode_results* decodedSignal) {
  IRToshibaAC ac(0);
  ac.setRaw(decodedSignal->state);
  description = ac.toString();
}

// Mqtt protocol
void callback(char* topic, byte* payload, unsigned int length) {
  displayMessageReceived(topic, payload, length);
  char command = (char)payload[0];
  irrecv.disableIRIn();

  if (command == TURN_ON_COMMAND) {
    turnOnAirConditionerSignal();
    Serial.println("Ligando o ar-condicionado...");
  } else if (command == TURN_OFF_COMMAND) {
    turnOffAirConditionerSignal();
    Serial.println("Desligando o ar-condicionado...");
  }
}

void setupMqtt() {
  client.setServer(SERVER_MQTT, 1883);
  client.setCallback(callback);
}

void setupSignalsDecoding() {
  irrecv.enableIRIn();
  irsend.begin();
}

void setTurnOnSignal(decode_results currentDecodedSignal) {
  Serial.println("Sinal de ligar decodificado!");
  turnOnSignal.rawData = resultToRawArray(&currentDecodedSignal);
  turnOnSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
}

void setTurnOffSignal(decode_results currentDecodedSignal) {
  Serial.println("Sinal de desligar decodificado!");
  turnOffSignal.rawData = resultToRawArray(&currentDecodedSignal);
  turnOffSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
}

// Signal decoding methods
void receiveDecodeSignals(decode_results currentDecodedSignal) {
  switch (currentDecodedSignal.decode_type) {
    case DAIKIN:
    case DAIKIN2:
      decodeDaikin(&currentDecodedSignal);
      yield();
      break;
    case FUJITSU_AC:
      decodeFujitsu(&currentDecodedSignal);
      yield();
      break;
  }

  if (description != "") {
    Serial.println("Mesg Desc.: " + description);
  }

  Serial.println(resultToSourceCode(&currentDecodedSignal));

  if (isTurnedOn) {
    setTurnOffSignal(currentDecodedSignal);
  } else {
    setTurnOnSignal(currentDecodedSignal);
  }

  isTurnedOn = !isTurnedOn;
  irrecv.resume();
}

void loop() {
  
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  decode_results currentDecodedSignal;
  if (irrecv.decode(&currentDecodedSignal) &&
      decodedSignalsTotal < MAX_DECODING_SIGNAL_ATTEMPTS) {
    receiveDecodeSignals(currentDecodedSignal);
    decodedSignalsTotal++;
  }
}

// Set up methods
void setup() {
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  wifiManager.connectToWiFi();
  setupMqtt();
  Serial.begin(kBaudRate);
  setupSignalsDecoding();
}