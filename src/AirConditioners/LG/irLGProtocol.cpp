#include <AirConditioners/LG/irLGProtocol.h>
#include <Arduino.h>

#define SEND_LG true

const uint16_t kIrLed = 4;
IRLgAc irLGAc(kIrLed);

void IrLGProtocol::initialize()
{
    irLGAc.begin();
    Serial.println("Inicializando o protocolo do LG...");
}

void IrLGProtocol::setOn()
{
    Serial.println("Ligando o ar condicionado");
    irLGAc.on();
#if SEND_LG
    irLGAc.send();
#endif
}

void IrLGProtocol::setOff()
{
    Serial.println("Desligando o ar condicionado");
    irLGAc.off();
#if SEND_LG
    irLGAc.send();
#endif
}

void IrLGProtocol::setTemperature(const uint8_t temp)
{
    Serial.print("Definindo a temperatura do ar condicionado para ");
    Serial.print(temp);
    irLGAc.setTemp(temp);
    Serial.println(irLGAc.toString());
}