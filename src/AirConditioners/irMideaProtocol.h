#ifndef IR_MIDEA_PROTOCOL_H
#define IR_MIDEA_PROTOCOL_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Importa o protocolo do ar-condicionado
#include <ir_Midea.h>
#include <AirConditioners/irBaseProtocol.h>

class IrMideaProtocol : public IrBaseProtocol
{
public:
  void initialize() override;
  void setOn() override;
  void setOff() override;
  void setTemperature(const uint8_t temp) override;
  void setMode(const char *mode);
};

#endif
