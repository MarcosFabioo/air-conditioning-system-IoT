// Bibliotecas utilizadas para decodificação de sinais
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

// Configurações de hardware e tempo
const uint16_t kRecvPin = 14; // D5; pino no qual o receptor infravermelho está conectado.
const uint32_t kBaudRate = 115200; // taxa de transmissão de dados em bits por segundo (baud rate) para a comunicação serial.
const uint16_t kCaptureBufferSize = 1024; // O tamanho do buffer de captura, que é o número máximo de marcas e espaços que serão armazenados durante a recepção de um sinal IR.
const uint8_t kTimeout = 15; // pode ser redefinido para evitar repetições de leitura
const uint16_t kMinUnknownSize = 12; // tamanho mínimo dos dados recebidos

// Configurações de hardware para envio de sinal IR
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

String rawDataString;
int rawDataInt;
int rawDataIntOn;
int rawDataIntOff;
int cont = 0;
uint16_t rawDataOn[] = {atoi(rawDataString.c_str())};
uint16_t rawDataOff[] = {atoi(rawDataString.c_str())};

// Cria objeto IRrecv e decode_results
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results decodedSignal;
String description = "";

// Função para decodificar sinais do ar condicionado da marca Daikin
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

// Função para decodificar sinais do ar condicionado da marca Fujitsu
void decodeFujitsu(decode_results *decodedSignal){
  #if DECODE_FUJITSU_AC
  if (decodedSignal->decode_type == FUJITSU_AC){
    IRFujitsuAC ac(0);
    ac.setRaw(decodedSignal->state, decodedSignal->bits /8);
    description = ac.toString();
  }
  #endif // DECODE_FUJITSU_AC
}

// Função para decodificar sinais do ar condicionado da marca TOSHIBA
void decodeToshiba(decode_results *decodedSignal){
  #if DECODE_TOSHIBA_AC
  if (decodedSignal->decode_type == TOSHIBA_AC){
    IRToshibaAC ac(0);
    ac.setRaw(decodedSignal->state);
    description = ac.toString();
  }
  #endif // DECODE_TOSHIBA_AC
}

String treatSignal(String originalRawData){
  int startIndex = originalRawData.indexOf("{");
  int endIndex = originalRawData.indexOf("}");
  String rawDataString = originalRawData.substring(startIndex+1, endIndex);
  return rawDataString;
}

int treatSignalNumber(String originalRawData){
  int startIndexNumber = originalRawData.indexOf("[");
  int endIndexNumber = originalRawData.indexOf("]");
  String rawDataNumber = originalRawData.substring(startIndexNumber+1, endIndexNumber+1);
  int rawDataInt = atoi(rawDataNumber.c_str());
  return rawDataInt;
}

// Função principal
void setup() {
  Serial.begin(kBaudRate);
  irrecv.enableIRIn(); // Inicia recepção de sinais IR
  #if DECODE_HASH
    irrecv.setUnknownThreshold(kMinUnknownSize); // Ignorar mensagens com menos do que o mínimo de pulsos ligados ou desligados.
  #endif // DECODE_HASH
    irrecv.enableIRIn(); // Inicia recepção de sinais IR 
  irsend.begin();
}

void loop() {
  if (irrecv.decode(&decodedSignal)) { // Se um sinal IR foi recebido
    cont += 1;
    switch (decodedSignal.decode_type) { // Verifica o tipo de sinal recebido
      case DAIKIN:
      case DAIKIN2:
        decodeDaikin(&decodedSignal); // Chama função para decodificar sinal da Daikin
        yield(); // evita que o programa trave ou reinicie
      break;
      case FUJITSU_AC:
        decodeFujitsu(&decodedSignal); // Chama função para decodificar sinal da Fujitsu
        yield(); // evita que o programa trave ou reinicie
      break;
    }
    if (description != "") Serial.println("Mesg Desc.: " + description);
    Serial.println(resultToSourceCode(&decodedSignal));
    yield(); // evita que o programa trave ou reinicie
    
    //tratar o bloco do sinal
    rawDataString = treatSignal(resultToSourceCode(&decodedSignal));
    Serial.println(rawDataString);

    //tratar a quantidade de blocos
    rawDataInt = treatSignalNumber(resultToSourceCode(&decodedSignal));
    Serial.println(rawDataInt);

    if(cont == 1){
      rawDataIntOn = rawDataInt;
      rawDataOn[rawDataIntOn] = {atoi(rawDataString.c_str())};
    }

    if(cont == 2){
      rawDataIntOff = rawDataInt;
      rawDataOff[rawDataIntOff] = {atoi(rawDataString.c_str())};
    }
    
    irrecv.resume(); // Prepara o receptor para receber o próximo sinal
  }
  
  if(Serial.available() > 0){
    int comando = Serial.read();
    if (comando == '1'){
      //sinal decodificado e armazenado
      irsend.sendRaw(rawDataOn, rawDataIntOn, 38);  // Send a raw data capture at 38kHz.
      delay(2000);
    }
    
    if (comando == '0'){
      irsend.sendRaw(rawDataOff, rawDataIntOff, 38);
      delay(2000);
    }
  }
}
