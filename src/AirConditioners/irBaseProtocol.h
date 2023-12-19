#ifndef IR_BASE_PROTOCOL_H
#define IR_BASE_PROTOCOL_H

#include <Arduino.h>

class IrBaseProtocol
{
public:
  virtual void initialize() = 0;
  virtual void setOn() = 0;
  virtual void setOff() = 0;
  virtual void setTemperature(const uint8_t temp) = 0;
};

#endif