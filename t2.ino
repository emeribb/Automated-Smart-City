#include <WiFi.h>
#include <PubSubClient.h>

//LED1
#define lp1 5
#define lp2 32
//LED2
#define lp3 25  //15 y 2
#define lp4 26
//LED3
#define lp5 14
#define lp6 12
//LED4
#define lp7 15
#define lp8 2

// Set the network SSID/Password
const char* ssid = "steren_2_4G";
const char* password = "password";

// Add mosquitto MQTT Broker IP address
const char* mqtt_server = "192.168.1.9";

const int bp1= 33; // 14 y 12
const int bp2= 27; // 26 y 25
const int bp3= 13;  // 35 y 32
const int bp4= 4;  // 15 y 2 

int messageReceived[4] = {0, 0, 0, 0};
int sems[4] = {0, 0, 0, 0};
int leds[8] = {lp1, lp2, lp3, lp4, lp5, lp6, lp7, lp8};
int btn1, btn2, btn3, btn4 = 0;
int btnSum, previousBtnSum = 0;

//Define
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
//Set-up configurations
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // LED & Button configuration
  pinMode(lp1, OUTPUT);
  pinMode(lp2, OUTPUT);
  pinMode(lp3, OUTPUT);
  pinMode(lp4, OUTPUT);
  pinMode(lp5, OUTPUT);
  pinMode(lp6, OUTPUT);
  pinMode(lp7, OUTPUT);
  pinMode(lp8, OUTPUT);

  pinMode(bp1, INPUT_PULLUP);
  pinMode(bp2, INPUT_PULLUP);
  pinMode(bp3, INPUT_PULLUP);
  pinMode(bp4, INPUT_PULLUP);
}

//Connecting to WiFi network
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Callback for MQTT received messages
void callback(char* topic, byte* message, unsigned int length) {
  //Serial.print("Message arrived on topic: ");
  //Serial.print(topic);
  char  messageSem;

  messageSem = (char)message[0];
  Serial.println(messageSem);

  if (strcmp(topic, "esp32Chorizo/Sems/Sem0") == 0) {  // Norte 1
    messageReceived[0] = 1;
    if(messageSem =='1'){
      sems[0] = true;
    }
    else {
      sems[0] = false;
    }
  }
  if (strcmp(topic, "esp32Chorizo/Sems/Sem1") == 0) {  // Oeste 2
    messageReceived[1] = 1;
    if(messageSem == '1'){
      sems[1] = true;
    }
    else {
      sems[1] = false;
    }
  }

  if (strcmp(topic, "esp32Chorizo/Sems/Sem2") == 0) {  // Sur 3 
    messageReceived[2] = 1;
    if(messageSem == '1'){
      sems[2] = true;
    }
    else {
      sems[2] = false;
    }
  }

  if (strcmp(topic, "esp32Chorizo/Sems/Sem3") == 0) {  //  Este 4
    messageReceived[3] = 1;
    if(messageSem == '1'){
      sems[3] = true;
    }
    else {
      sems[3] = false;
    }
  }
}


//Connecting to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32Chorizo")) {
      Serial.println("connected");
      // Subscribe to topics
      client.subscribe("esp32Chorizo/Sems/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop(){
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  int messagesAmount = 0;
  for (int i = 0; i < 4; i++) {
    messagesAmount += messageReceived[i];
  }

  if (messagesAmount == 4) {
    for (int i = 0; i < 4; i++) {
      messageReceived[i] = 0;
    }
    for (int i = 0; i < 8; i+=2) {
      Serial.println(sems[i/2]);
      digitalWrite(leds[i], sems[i/2]);
      digitalWrite(leds[i+1], !sems[i/2]);
    }
  }
  
  btn1 = digitalRead(bp1);
  btn2 = digitalRead(bp2);
  btn3 = digitalRead(bp3);
  btn4 = digitalRead(bp4);

  btnSum = btn1+btn2+btn3+btn4;
  
  if (btnSum != previousBtnSum) {
    if (btn1 == LOW){
      client.publish("esp32Chorizo/Botones/Btn1","1");
    } else {
      client.publish("esp32Chorizo/Botones/Btn1","0");
    }

    if (btn2 == LOW){
      client.publish("esp32Chorizo/Botones/Btn2","1");
    } else {
      client.publish("esp32Chorizo/Botones/Btn2","0");
    }

    if (btn3 == LOW){
      client.publish("esp32Chorizo/Botones/Btn3","1");
    } else {
      client.publish("esp32Chorizo/Botones/Btn3","0");
    }

    if (btn4 == LOW){
      client.publish("esp32Chorizo/Botones/Btn4","1");
    } else {
      client.publish("esp32Chorizo/Botones/Btn4","0");
    }
  }
  previousBtnSum = btnSum;
}
