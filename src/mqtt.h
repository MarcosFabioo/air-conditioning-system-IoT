#include <Arduino.h>
#include <PubSubClient.h>

void displayMessageReceived(char* topic, byte* payload, unsigned int length);

void displayMqttNotConnected();

void reconnect();

void callback(char* topic, byte* payload, unsigned int length);

void setupMqtt();