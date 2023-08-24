#ifndef SINAL_IR_MANAGER_H
#define SINAL_IR_MANAGER_H

#include <PinConstants/PinConstants.h>
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>

// extern = palavra para declarar variáveis globais
extern const uint16_t kIrLed;
extern IRsend irsend;
extern bool isTurnedOn;
extern int decodedSignalsTotal;
extern const int MAX_DECODING_SIGNAL_ATTEMPTS;

struct IRData
{
  uint16_t *rawData;
  uint16_t rawDataLength;
};

extern IRData turnOffSignal;
extern IRData turnOnSignal;

extern char TURN_ON_COMMAND;
extern char TURN_OFF_COMMAND;

extern IRrecv irrecv;
extern String description;

class SinalIRManager
{
public:
  void turnOnAirConditionerSignal();
  void turnOffAirConditionerSignal();
  void setupSignalsDecoding();
  void receiveDecodeSignals(decode_results currentDecodedSignal);
  IRrecv &getIrrecvInstance();

private:
  void setTurnOnSignal(decode_results currentDecodedSignal);
  void setTurnOffSignal(decode_results currentDecodedSignal);
  void decodeDaikin(decode_results *decodedSignal);
  void decodeFujitsu(decode_results *decodedSignal);
};

#endif