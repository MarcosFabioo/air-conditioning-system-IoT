#ifndef IR_ELECTRA_PROTOCOL_H
#define IR_ELECTRA_PROTOCOL_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Importa o protocolo do ar-condicionado
#include <ir_Electra.h>
#include <AirConditioners/irBaseProtocol.h>

class IrElectraProtocol : public IrBaseProtocol
{
public:
    void initialize() override;
    void setOn() override;
    void setOff() override;
    void setTemperature(const uint8_t temp) override;
    void setMode(const char *mode);
};

#endif