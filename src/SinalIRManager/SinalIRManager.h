#ifndef SinalIRManager_h
#define SinalIRManager_h

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <PinConstants/PinConstants.h>

const uint16_t kIrLed = 4;
IRsend irsend(kIrLed);

bool isTurnedOn = false;
int decodedSignalsTotal = 0;
const int MAX_DECODING_SIGNAL_ATTEMPTS = 2;

struct IRData
{
  uint16_t *rawData;
  uint16_t rawDataLength;
};

IRData turnOffSignal;
IRData turnOnSignal;

char TURN_ON_COMMAND = '1';
char TURN_OFF_COMMAND = '0';

IRrecv irrecv(PinConstants::kRecvPin, PinConstants::kCaptureBufferSize, PinConstants::kTimeout, true);
String description = "";

class SinalIRManager
{
public:
  void turnOnAirConditionerSignal();
  void turnOffAirConditionerSignal();
  void setupSignalsDecoding();
  void receiveDecodeSignals(decode_results currentDecodedSignal);

private:
  void setTurnOnSignal(decode_results currentDecodedSignal);
  void setTurnOffSignal(decode_results currentDecodedSignal);
  void decodeDaikin(decode_results *decodedSignal);
  void decodeFujitsu(decode_results *decodedSignal);
};

#endif