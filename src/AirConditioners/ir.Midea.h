#ifndef IR_MIDEA_H
#define IR_MIDEA_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Midea.h>

class IrMidea
{
public:
  void initialize();  
  void setOn();
  void setOff();
  void setTemperature(const uint8_t temp, const bool useCelsius);

private:
};

#endif