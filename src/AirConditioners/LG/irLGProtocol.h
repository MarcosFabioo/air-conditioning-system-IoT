#ifndef LGIR_LG_H_
#define LGIR_LG_H_

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Importa o protocolo do ar-condicionado
#include <ir_LG.h>
#include <AirConditioners/irBaseProtocol.h>

class IrLGProtocol : public IrBaseProtocol
{
public:
    void initialize() override;
    void setOn() override;
    void setOff() override;
    void setTemperature(const uint8_t temp) override;
    void setMode(const char *mode);
};

#endif