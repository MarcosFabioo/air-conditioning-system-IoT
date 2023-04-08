#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* nome = "Marcos_NET";
const char* senha = "05102202";

const char* servidor_MQTT = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  conectar_wifi();
  client.setServer(servidor_MQTT, 1883);
  client.setCallback(callback);
}


void loop() { 
  if(!client.connected()){
    reconnect();
  }
  client.loop();
}


void conectar_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Conectando na rede: ");
  Serial.println(nome);
  WiFi.mode(WIFI_STA);
  WiFi.begin(nome, senha);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");

  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if((char)payload[0] == '1'){
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}


void reconnect(){
  byte willQoS = 0;
  const char* willTopic = "esp/status";
  const char* willMessage = "OFF_LINE";
  boolean willRetain = true;

  while(!client.connected()){
    Serial.print("Tentando estabelecer conexão MQTT...");

    String clientId = "ESP8266Client01p";

    if (client.connect(clientId.c_str(), willTopic, willQoS, willRetain, willMessage)) {
      Serial.println("Conectado!");
      
      char* message = "ON_LINE";
      int length = strlen(message);
      boolean retained = true;
      
      client.publish("esp/status", (byte*)message, length, retained);
      client.subscribe("room/12/air-conditioner/1/state");
      
    } else {
      Serial.print("Não foi possível se conectar. Estado do esp: ");
      Serial.print(client.state());
      Serial.println("Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}
