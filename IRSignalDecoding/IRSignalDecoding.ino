// Libraries used for signal decoding
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
const uint16_t kRecvPin = 14; // D5; pin to which the infrared receiver is connected.
const uint32_t kBaudRate = 115200; // data transmission rate in bits per second (baud rate) for serial communication.
const uint16_t kCaptureBufferSize = 1024; // The size of the capture buffer, which is the maximum number of marks and spaces that will be stored during the reception of an IR signal.
const uint8_t kTimeout = 15; // time to receive data; can be redefined to avoid reading repetitions
const uint16_t kMinUnknownSize = 12; // minimum received data size

// Hardware settings for sending the IR signal
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// Configuration for IR signal handling
String rawDataString;
int rawDataInt;
int rawDataIntOn;
int rawDataIntOff;
int cont = 0;
uint16_t rawDataOn[] = {atoi(rawDataString.c_str())};
uint16_t rawDataOff[] = {atoi(rawDataString.c_str())};

// Create IRrecv object and decode_results
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results decodedSignal;
String description = "";

// Function to decode signals from Daikin air conditioners
void decodeDaikin(decode_results *decodedSignal) {
  #if DECODE_DAIKIN
  if (decodedSignal->decode_type == DAIKIN) { // colocar isso no loop e chamar a função caso seja vdd
    IRDaikinESP ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
  #endif  // DECODE_DAIKIN

  #if DECODE_DAIKIN2
  if (decodedSignal->decode_type == DAIKIN2) {
    IRDaikin2 ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
  #endif  // DECODE_DAIKIN2
}

// Function to decode signals from Fujitsu air conditioners
void decodeFujitsu(decode_results *decodedSignal){
  #if DECODE_FUJITSU_AC
  if (decodedSignal->decode_type == FUJITSU_AC){
    IRFujitsuAC ac(0);
    ac.setRaw(decodedSignal->state, decodedSignal->bits /8);
    description = ac.toString();
  }
  #endif // DECODE_FUJITSU_AC
}

// Function to decode signals from TOSHIBA air conditioners
void decodeToshiba(decode_results *decodedSignal){
  #if DECODE_TOSHIBA_AC
  if (decodedSignal->decode_type == TOSHIBA_AC){
    IRToshibaAC ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
  #endif // DECODE_TOSHIBA_AC
}

//Function to handle the IR signal
String treatSignal(String originalRawData){
  int startIndex = originalRawData.indexOf("{");
  int endIndex = originalRawData.indexOf("}");
  String rawDataString = originalRawData.substring(startIndex+1, endIndex);
  return rawDataString;
}

//Function to handle the number of blocks of the IR signal
int treatSignalNumber(String originalRawData){
  int startIndexNumber = originalRawData.indexOf("[");
  int endIndexNumber = originalRawData.indexOf("]");
  String rawDataNumber = originalRawData.substring(startIndexNumber+1, endIndexNumber+1);
  int rawDataInt = atoi(rawDataNumber.c_str());
  return rawDataInt;
}

// Main function
void setup() {
  Serial.begin(kBaudRate);
  irrecv.enableIRIn(); // Starts IR signal reception
  #if DECODE_HASH
    irrecv.setUnknownThreshold(kMinUnknownSize); // Ignore messages with less than the minimum on or off pulses.
  #endif // DECODE_HASH
    irrecv.enableIRIn(); // Starts IR signal reception 
  irsend.begin(); // Start IR signal emitter
}

void loop() {
  if (irrecv.decode(&decodedSignal)) { // If an IR signal has been received
    cont += 1;
    switch (decodedSignal.decode_type) { // Checks the type of received signal
      case DAIKIN:
      case DAIKIN2:
        decodeDaikin(&decodedSignal); // Call function to decode Daikin signal
        yield(); // prevents the program from crashing or restarting
      break;
      case FUJITSU_AC:
        decodeFujitsu(&decodedSignal); // Call function to decode Fujitsu signal
        yield(); // prevents the program from crashing or restarting
      break;
    }
    if (description != "") Serial.println("Mesg Desc.: " + description);
    Serial.println(resultToSourceCode(&decodedSignal));
    yield();
    
    //tract the IR signal block
    rawDataString = treatSignal(resultToSourceCode(&decodedSignal));
    Serial.println(rawDataString);

    //tract the number of blocks of the IR signal
    rawDataInt = treatSignalNumber(resultToSourceCode(&decodedSignal));
    Serial.println(rawDataInt);

    // store processed IR signal (signal to power on device)
    if(cont == 1){
      rawDataIntOn = rawDataInt;
      rawDataOn[rawDataIntOn] = {atoi(rawDataString.c_str())};
    }

    // store treated IR signal (signal to turn off device)
    if(cont == 2){
      rawDataIntOff = rawDataInt;
      rawDataOff[rawDataIntOff] = {atoi(rawDataString.c_str())};
    }
    
    irrecv.resume(); // Prepares the receiver to receive the next signal
  }
  
  if(Serial.available() > 0){
    int command = Serial.read();
    if (command == '1'){
      // sending the decoded and stored signal
      irsend.sendRaw(rawDataOn, rawDataIntOn, 38);  // Send a raw data capture at 38kHz.
      delay(2000);
    }
    
    if (command == '0'){
      irsend.sendRaw(rawDataOff, rawDataIntOff, 38);
      delay(2000);
    }
  }
}
