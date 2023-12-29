#include <AirConditioners/Electra/irElectraProtocol.h>
#include <Arduino.h>

#define SEND_ELECTRA_AC true

const uint16_t kIrLed = 4;
IRElectraAc irElectraAc(kIrLed);

void IrElectraProtocol::initialize()
{
    irElectraAc.begin();
    Serial.println("Inicializando o protocolo do Electra...");
}

void IrElectraProtocol::setOn()
{
    Serial.println("Ligando o ar condicionado");
    irElectraAc.on();
#if SEND_ELECTRA_AC
    irElectraAc.send();
#endif
}

void IrElectraProtocol::setOff()
{
    Serial.println("Desligando o ar condicionado");
    irElectraAc.off();
#if SEND_ELECTRA_AC
    irElectraAc.send();
#endif
}

void IrElectraProtocol::setTemperature(const uint8_t temp)
{
    Serial.print("Definindo a temperatura do ar condicionado para ");
    Serial.print(temp);
    irElectraAc.setTemp(temp);
    Serial.println(irElectraAc.toString());
}