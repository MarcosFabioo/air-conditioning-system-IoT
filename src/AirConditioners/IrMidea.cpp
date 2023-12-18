#include "ir.Midea.h"
#include <PinConstants/PinConstants.h>

const uint16_t kIrLed = 4;
IRMideaAC ac(kIrLed);

void IrMidea::initialize(){
    ac.begin();
    ac.setUseCelsius(true);
}

void IrMidea::setOn(){
    ac.on();
    #if SEND_MIDEA
        ac.send();
    #endif
}

void IrMidea::setOff(){
    ac.off();
    #if SEND_MIDEA
        ac.send();
    #endif
}

void IrMidea::setTemperature(const uint8_t temp){
    bool useCelsius = true;
    ac.setTemp(temp, useCelsius);
    Serial.println(ac.toString());
    #if SEND_MIDEA
        ac.send();
    #endif
}