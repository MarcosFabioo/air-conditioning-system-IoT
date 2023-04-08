#include <ESP8266WiFi.h>     // includes the ESP8266WiFi library for configuring the Wi-Fi connection
#include <PubSubClient.h>    // includes the PubSubClient library for connecting to an MQTT server

const char* nameWifi = "Marcos_NET";                // Wi-Fi network name
const char* password = "05102202";                 // Wi-Fi network password

const char* server_MQTT = "broker.hivemq.com"; // MQTT server address

WiFiClient espClient;                           // Creates a WiFiClient object
PubSubClient client(espClient);                 // Creates a PubSubClient object and passes the WiFiClient object as parameter

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);                 // Configures the LED_BUILTIN pin as output
  Serial.begin(115200);                         // Initializes serial communication at the speed of 115200 bps
  connect_WiFi();                              // Calls the connect_WiFi() function to connect to the Wi-Fi network
  client.setServer(server_MQTT, 1883);         // Configures the MQTT server with the address and port
  client.setCallback(callback);                 // Set the callback function to receive MQTT messages
}

void loop() { 
  if(!client.connected()){                      // Checks if the MQTT client is connected
    reconnect();                                 // If not connected, calls the reconnect() function to connect again
  }
  client.loop();                                 // Loops the MQTT client
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
  if((char)payload[0] == '1'){
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
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
      client.publish("esp/status", (byte*)message, length, retained);

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
