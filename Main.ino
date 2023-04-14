#include <IRrecv.h>
#include <IRsend.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <ir_Coolix.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Gree.h>
#include <ir_Haier.h>
#include <ir_Hitachi.h>
#include <ir_Kelvinator.h>
#include <ir_Midea.h>
#include <ir_Mitsubishi.h>
#include <ir_Panasonic.h>
#include <ir_Samsung.h>
#include <ir_Tcl.h>
#include <ir_Teco.h>
#include <ir_Toshiba.h>
#include <ir_Vestel.h>
#include <ir_Whirlpool.h>

// Hardware and time settings
const uint16_t kRecvPin = 14;             // D5; pin to which the infrared receiver is connected.
const uint32_t kBaudRate = 115200;        // data transmission rate in bits per second (baud rate) for serial communication.
const uint16_t kCaptureBufferSize = 1024; // The size of the capture buffer, which is the maximum number of marks and spaces that will be stored during the reception of an IR signal.
const uint8_t kTimeout = 15;              // time to receive data; can be redefined to avoid reading repetitions
const uint16_t kMinUnknownSize = 12;      // minimum received data size

// Hardware settings for sending the IR signal
const uint16_t kIrLed = 4; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);     // Set the GPIO to be used to sending the message.

// Configuration for IR signal handling
bool isTurnedOn = false;
int decodingCounter = 0;
const MAX_DECODING_COUNTER = 2;

// Classe para armazenar o rawData e o rawDataLength do sinal recebido
struct IRData
{
  uint16_t *rawData;
  uint16_t rawDataLength;
};

// Comandos recebidos para ligar e desligar o ar condicionado
const string TURN_ON_COMMAND = "1";
const string TURN_OFF_COMMAND = "0";

// Create IRrecv object and decode_results
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
String description = "";

// Function to decode signals from Daikin air conditioners
void decodeDaikin(decode_results *decodedSignal)
{
#if DECODE_DAIKIN
  if (decodedSignal->decode_type == DAIKIN)
  { // colocar isso no loop e chamar a função caso seja vdd
    IRDaikinESP ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
#endif // DECODE_DAIKIN

#if DECODE_DAIKIN2
  if (decodedSignal->decode_type == DAIKIN2)
  {
    IRDaikin2 ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
#endif // DECODE_DAIKIN2
}

// Function to decode signals from Fujitsu air conditioners
void decodeFujitsu(decode_results *decodedSignal)
{
#if DECODE_FUJITSU_AC
  if (decodedSignal->decode_type == FUJITSU_AC)
  {
    IRFujitsuAC ac(0);
    ac.setRaw(decodedSignal->state, decodedSignal->bits / 8);
    description = ac.toString();
  }
#endif // DECODE_FUJITSU_AC
}

// Function to decode signals from TOSHIBA air conditioners
void decodeToshiba(decode_results *decodedSignal)
{
#if DECODE_TOSHIBA_AC
  if (decodedSignal->decode_type == TOSHIBA_AC)
  {
    IRToshibaAC ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
#endif // DECODE_TOSHIBA_AC
}

void setup()
{
  Serial.begin(kBaudRate);
#if DECODE_HASH
  irrecv.setUnknownThreshold(kMinUnknownSize); // Ignore messages with less than the minimum on or off pulses.
#endif                                         // DECODE_HASH
  irrecv.enableIRIn();                         // Starts IR signal reception
  irsend.begin();                              // Start IR signal emitter
}

void loop()
{
  decode_results currentDecodedSignal;

  if (irrecv.decode(&currentDecodedSignal))
  { // If an IR signal has been received
    switch (currentDecodedSignal.decode_type)
    { // Checks the type of received signal
    case DAIKIN:
    case DAIKIN2:
      decodeDaikin(&currentDecodedSignal); // Call function to decode Daikin signal
      yield();                             // prevents the program from crashing or restarting
      break;
    case FUJITSU_AC:
      decodeFujitsu(&currentDecodedSignal); // Call function to decode Fujitsu signal
      yield();                              // prevents the program from crashing or restarting
      break;
    }
    if (description != "")
      Serial.println("Mesg Desc.: " + description);
    Serial.println(resultToSourceCode(&currentDecodedSignal));
    yield();

    if (decodingCounter < MAX_DECODING_COUNTER)
    {
      if (isTurnedOn)
      {
        Serial.println("Sinal de desligar decodificado!");
        IRData turnOffSignal;
        turnOffSignal.rawData = resultToRawArray(&currentDecodedSignal);
        turnOffSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
      }
      else
      {
        Serial.println("Sinal de ligar decodificado!");
        IRData turnOnSignal;
        turnOnSignal.rawData = resultToRawArray(&currentDecodedSignal);
        turnOnSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
      }

      isTurnedOn = !isTurnedOn; // inverte o valor da variável
      decodingCounter++;
    }
    else
    {
      Serial.println("Recebi um sinal, mas não vou decodificar porque excedi o número máximo de decodificações permitidas.")
    }
    irrecv.resume();
  }

  if (Serial.available() > 0)
  {
    irrecv.disableIRIn();
    int readCommand = Serial.read();

    if (readCommand == TURN_ON_COMMAND)
    {
      irsend.sendRaw(rawDataOn, rawDataLengthOn, 38);
      Serial.println("Ligando o ar-condicionado...");
      Serial.println(resultToSourceCode(&decodedTurnOnSignal));
    }
    else if (readCommand == TURN_OFF_COMMAND)
    {
      irsend.sendRaw(rawDataOff, rawDataLengthOff, 38);
      Serial.println("Desligando o ar-condicionado...");
      Serial.println(resultToSourceCode(&decodedTurnOffSignal));
    }
    delay(2000);
  }
}