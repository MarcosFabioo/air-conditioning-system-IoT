#include "SinalIRManager.h"

void SinalIRManager::turnOnAirConditionerSignal()
{
    irsend.sendRaw(turnOnSignal.rawData, turnOnSignal.rawDataLength, 38);
}

void SinalIRManager::turnOffAirConditionerSignal()
{
    irsend.sendRaw(turnOffSignal.rawData, turnOffSignal.rawDataLength, 38);
}

void SinalIRManager::setupSignalsDecoding()
{
    irrecv.enableIRIn();
    irsend.begin();
}

void SinalIRManager::receiveDecodeSignals(decode_results currentDecodedSignal)
{
    switch (currentDecodedSignal.decode_type) {
        case DAIKIN:
        case DAIKIN2:
            decodeDaikin(&currentDecodedSignal);
            yield();
        break;
        case FUJITSU_AC:
            decodeFujitsu(&currentDecodedSignal);
            yield();
        break;
    }

    if (description != "") {
        Serial.println("Mesg Desc.: " + description);
    }

    Serial.println(resultToSourceCode(&currentDecodedSignal));

    if (isTurnedOn) {
        setTurnOffSignal(currentDecodedSignal);
    } else {
        setTurnOnSignal(currentDecodedSignal);
    }

    isTurnedOn = !isTurnedOn;
    irrecv.resume();
}

void SinalIRManager::setTurnOnSignal(decode_results currentDecodedSignal)
{
    Serial.println("Sinal de ligar decodificado!");
    turnOnSignal.rawData = resultToRawArray(&currentDecodedSignal);
    turnOnSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
}

void SinalIRManager::setTurnOffSignal(decode_results currentDecodedSignal)
{
    Serial.println("Sinal de desligar decodificado!");
    turnOffSignal.rawData = resultToRawArray(&currentDecodedSignal);
    turnOffSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
}

void SinalIRManager::decodeDaikin(decode_results* decodedSignal)
{
    IRDaikinESP ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
}

void SinalIRManager::decodeFujitsu(decode_results* decodedSignal)
{
    IRFujitsuAC ac(0);
    ac.setRaw(decodedSignal->state, decodedSignal->bits / 8);
    description = ac.toString();
}