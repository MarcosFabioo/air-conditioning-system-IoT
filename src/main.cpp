#include <Arduino.h>
#include "wifi.h"
#include "mqtt.h"
#include "sinalIR.h"

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

void setup() {
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  connectToWiFi();
  setupMqtt();
  Serial.begin(kBaudRate);
  setupSignalsDecoding();
}