#include <ESP8266WiFi.h>     // includes the ESP8266WiFi library for configuring the Wi-Fi connection
#include <PubSubClient.h>    // includes the PubSubClient library for connecting to an MQTT server
#include <IRrecv.h>
#include <IRsend.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Toshiba.h>

const char* nameWifi = "Maconaria";                // Wi-Fi network name
const char* password = "loja1979";                 // Wi-Fi network password

const char* server_MQTT = "broker.hivemq.com"; // MQTT server address

WiFiClient espClient;                           // Creates a WiFiClient object
PubSubClient client(espClient);                 // Creates a PubSubClient object and passes the WiFiClient object as parameter

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
int MAX_DECODING_COUNTER = 2;

// Classe para armazenar o rawData e o rawDataLength do sinal recebido
struct IRData
{
  uint16_t *rawData;
  uint16_t rawDataLength;
};

IRData turnOffSignal;
IRData turnOnSignal;

// Comandos recebidos para ligar e desligar o ar condicionado
char TURN_ON_COMMAND = '1';
char TURN_OFF_COMMAND = '0';

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

void receiveDecodeSignals(decode_results currentDecodedSignal){
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
  if (description != ""){
    Serial.println("Mesg Desc.: " + description); 
  }
  Serial.println(resultToSourceCode(&currentDecodedSignal));
  yield();
  
  if (isTurnedOn)
  {
    Serial.println("Sinal de desligar decodificado!");
    turnOffSignal.rawData = resultToRawArray(&currentDecodedSignal);
    turnOffSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
  }
  else
  {
    Serial.println("Sinal de ligar decodificado!");
    turnOnSignal.rawData = resultToRawArray(&currentDecodedSignal);
    turnOnSignal.rawDataLength = getCorrectedRawLength(&currentDecodedSignal);
  }

  isTurnedOn = !isTurnedOn; // inverte o valor da variável
  irrecv.resume(); 
}

void setup() {
  Serial.begin(115200);                         // Initializes serial communication at the speed of 115200 bps
  connect_WiFi();                              // Calls the connect_WiFi() function to connect to the Wi-Fi network
  client.setServer(server_MQTT, 1883);         // Configures the MQTT server with the address and port
  client.setCallback(callback);                 // Set the callback function to receive MQTT messages

  Serial.begin(kBaudRate);
  #if DECODE_HASH
    irrecv.setUnknownThreshold(kMinUnknownSize); // Ignore messages with less than the minimum on or off pulses.
  #endif                                         // DECODE_HASH
  irrecv.enableIRIn();                         // Starts IR signal reception
  irsend.begin();                              // Start IR signal emitter
}

void loop() { 
  if(!client.connected()){                      // Checks if the MQTT client is connected
    reconnect();                                 // If not connected, calls the reconnect() function to connect again
  }
  client.loop();                                 // Loops the MQTT client
  decode_results currentDecodedSignal;
  if (irrecv.decode(&currentDecodedSignal) && decodingCounter < MAX_DECODING_COUNTER){
    receiveDecodeSignals(currentDecodedSignal); 
    decodingCounter++; 
  }
}

void connect_WiFi(){
  delay(10);                                     // Wait a short period of time
  Serial.println();
  Serial.print("Conectando na rede: ");
  Serial.println(nameWifi);
  WiFi.mode(WIFI_STA);                           // Configures the ESP8266 module as a Wi-Fi client
  WiFi.begin(nameWifi, password);                       // Connects to the specified Wi-Fi network with the provided name and password

  while(WiFi.status() != WL_CONNECTED){          // Wait until the Wi-Fi connection is established
    delay(500);                                  // 500ms Wait
    Serial.print(".");                            // Prints a dot on the console to indicate that the connection is still being established
  }
  Serial.println("");
  Serial.println("WiFi conectado!");             // Prints a message indicating that the Wi-Fi connection was successfully established
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());                 // Prints the IP address assigned to the ESP8266 module by the Wi-Fi network
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Mensagem recebida no tópico [");  // Prints a message indicating that a new MQTT message has been received
  Serial.print(topic);                            // Prints the topic of the received message
  Serial.print("]: ");

  // Loop through the payload array and print its contents to the serial monitor
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Check if the payload's first character is '1'. If it is, turn the built-in LED on, otherwise turn it off.
  if((char)payload[0] == TURN_ON_COMMAND)
  {
    irrecv.disableIRIn();
    irsend.sendRaw(turnOnSignal.rawData, turnOnSignal.rawDataLength, 38);
    Serial.println("Ligando o ar-condicionado...");
    //Serial.println(resultToSourceCode(&turnOnSignal));
  } 
  else if ((char)payload[0] == TURN_OFF_COMMAND)
  {
    irrecv.disableIRIn();
    irsend.sendRaw(turnOffSignal.rawData, turnOffSignal.rawDataLength, 38);
    Serial.println("Desligando o ar-condicionado...");
    //Serial.println(resultToSourceCode(&turnOffSignal));
  }
}

void reconnect(){                                // Function attempts to establish a connection with the MQTT broker.
  byte willQoS = 0;
  const char* willTopic = "esp/status";
  const char* willMessage = "OFF_LINE";
  boolean willRetain = true;

  // Attempt to connect to the MQTT broker until a connection is established
  while(!client.connected()){
    Serial.print("Tentando estabelecer conexão MQTT...");

    // Define the MQTT client ID
    String clientId = "ESP8266Client01p";

    // Attempt to connect to the MQTT broker with the defined parameters
    if (client.connect(clientId.c_str(), willTopic, willQoS, willRetain, willMessage)) {
      Serial.println("Conectado!");

      // Publish a message to the "esp/status" topic to indicate that the device is online
      char* message = "ON_LINE";
      int length = strlen(message);
      boolean retained = true;
      client.publish("esp/status", (byte*)message, length, retained); // publica status de conexão com WI-FI

      // Subscribe to the "room/12/air-conditioner/1/state" topic
      client.subscribe("room/12/air-conditioner/1/state");
      
    } else {
      // If a connection cannot be established, print the error message and delay for 5 seconds before trying again
      Serial.print("Não foi possível se conectar. Estado do esp: ");
      Serial.print(client.state());
      Serial.println("Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}
