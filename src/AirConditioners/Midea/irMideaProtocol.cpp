#include <AirConditioners/Midea/irMideaProtocol.h>
#include <Arduino.h>

#define SEND_MIDEA true

const uint16_t kIrLed = 4;
IRMideaAC ac(kIrLed);

void IrMideaProtocol::initialize()
{
  ac.begin();
  ac.setUseCelsius(true);
}

void IrMideaProtocol::setOn()
{
  Serial.println("Ligando o ar condicionado");
  ac.on();
#if SEND_MIDEA
  ac.send();
#endif
}

void IrMideaProtocol::setOff()
{
  Serial.println("Desligando o ar condicionado");
  ac.off();
#if SEND_MIDEA
  ac.send();
#endif
}

void IrMideaProtocol::setTemperature(const uint8_t temp)
{
  Serial.print("Definindo a temperatura do ar condicionado para ");
  Serial.print(temp);

  bool useCelsius = true;
  ac.setTemp(temp, useCelsius);
  Serial.println(ac.toString());
}
