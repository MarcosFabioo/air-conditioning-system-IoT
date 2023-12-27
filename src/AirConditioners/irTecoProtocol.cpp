#include <AirConditioners/irTecoProtocol.h>
#include <Arduino.h>

#define SEND_TECO true

const uint16_t kIrLed = 4;
IRTecoAc irTecoAc(kIrLed);

void IrTecoProtocol::initialize()
{
    irTecoAc.begin();
    Serial.println("Inicializando o protocolo do Teco...");
}

void IrTecoProtocol::setOn()
{
    Serial.println("Ligando o ar condicionado");
    irTecoAc.on();
#if SEND_TECO
    irTecoAc.send();
#endif
}

void IrTecoProtocol::setOff()
{
    Serial.println("Desligando o ar condicionado");
    irTecoAc.off();
#if SEND_TECO
    irTecoAc.send();
#endif
}

void IrTecoProtocol::setTemperature(const uint8_t temp)
{
    Serial.print("Definindo a temperatura do ar condicionado para ");
    Serial.print(temp);
    irTecoAc.setTemp(temp);
    Serial.println(irTecoAc.toString());
}