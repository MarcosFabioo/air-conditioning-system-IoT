#include <AirConditioners/irMideaProtocol.h>
#include <Arduino.h>

#define SEND_MIDEA true

const uint16_t kIrLed = 4;
IRMideaAC irMideaAc(kIrLed);

void IrMideaProtocol::initialize()
{
  irMideaAc.begin();
  irMideaAc.setUseCelsius(true);
}

void IrMideaProtocol::setOn()
{
  Serial.println("Ligando o ar condicionado");
  irMideaAc.on();
#if SEND_MIDEA
  irMideaAc.send();
#endif
}

void IrMideaProtocol::setOff()
{
  Serial.println("Desligando o ar condicionado");
  irMideaAc.off();
#if SEND_MIDEA
  irMideaAc.send();
#endif
}

void IrMideaProtocol::setTemperature(const uint8_t temp)
{
  Serial.print("Definindo a temperatura do ar condicionado para ");
  Serial.print(temp);

  bool useCelsius = true;
  irMideaAc.setTemp(temp, useCelsius);
  Serial.println(irMideaAc.toString());
}
