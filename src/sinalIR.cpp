#include <IRrecv.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Toshiba.h>
#include "sinalIR.h"

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



void turnOnAirConditionerSignal() {
  irsend.sendRaw(turnOnSignal.rawData, turnOnSignal.rawDataLength, 38);
}

void turnOffAirConditionerSignal() {
  irsend.sendRaw(turnOffSignal.rawData, turnOffSignal.rawDataLength, 38);
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