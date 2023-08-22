#include <Arduino.h>

void turnOnAirConditionerSignal();

void turnOffAirConditionerSignal();

void setupSignalsDecoding();

void setTurnOnSignal(decode_results currentDecodedSignal);

void setTurnOffSignal(decode_results currentDecodedSignal);

void decodeDaikin(decode_results* decodedSignal);

void decodeFujitsu(decode_results* decodedSignal);

void decodeToshiba(decode_results* decodedSignal);

void receiveDecodeSignals(decode_results currentDecodedSignal);

