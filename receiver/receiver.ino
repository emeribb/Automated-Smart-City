#include <WiFi.h>
#include <PubSubClient.h>

//LEDs Definition
#define LED_1_GREEN 5
#define LED_1_RED 32
#define LED_2_GREEN 25
#define LED_2_RED 26
#define LED_3_GREEN 14
#define LED_3_RED 12
#define LED_4_GREEN 15
#define LED_4_RED 2

//Buttons Definition
#define BUTTON_1 33
#define BUTTON_2 27
#define BUTTON_3 13
#define BUTTON_4 4

//Global Constants
#define LEDS_AMOUNT 8

// Set the network SSID/Password
const char* ssid = "steren_2_4G";
const char* password = "password";

// MQTT global variables
const char* mqtt_server = "192.168.1.9";
const String lights_topic = "esp32Chorizo/Lights";
const String lights_subtopic = "/Light";
const String buttons_topic = "esp32Chorizo/Buttons";
const String buttons_subtopic = "/Button";
const uint8_t leds[LEDS_AMOUNT] = {LED_1_GREEN, LED_1_RED, LED_2_GREEN, LED_2_RED, LED_3_GREEN, LED_3_RED, LED_4_GREEN, LED_4_RED};
const uint8_t buttons[LEDS_AMOUNT/2] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4}

uint8_t message_received[LEDS_AMOUNT/2];
uint8_t lights[LEDS_AMOUNT/2];
uint8_t buttons_values[LEDS_AMOUNT/2];
uint8_t messages_amount = 0;
uint8_t btn_sum, previous_btn_sum = 0;

WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi();
void callback(char* topic, byte* message, unsigned int len);

void setup() {
  Serial.begin(115200);
  //Arrays initialization
  for (uint8_t i = 0; i < LEDS_AMOUNT/2; i++) {
    messaged_recieved[i] = 0;
    lights[i] = 0;
    buttons_vaules[i] = 0;
  }
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // LED configuration
  for (uint8_t i = 0; i < LEDS_AMOUNT; i++) {
    pinMode(leds[i], OUTPUT);
  }
  // Button configuration
  for (uint8_t i = 0; i < LEDS_AMOUNT/2; i++) {
    pinMode(buttons[i], INPUT_PULLDOWN);
  }
}

void loop(){
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  //Waits for all messages for be recieved
  if (messages_amount == LEDS_AMOUNT/2) {
    //Resets messages
    for (uint8_t i = 0; i < LEDS_AMOUNT/2; i++) {
      message_received[i] = 0;
    }
    //Outputs lights status
    for (int i = 0; i < LEDS_AMOUNT; i+=2) {
      Serial.println(lights[i/2]);
      digitalWrite(leds[i], lights[i/2]);
      digitalWrite(leds[i+1], !lights[i/2]);
    }
  }

  btn_sum = 0;
  for (uint8_t i = 0; i < LEDS_AMOUNT; i++) {
    buttons_values[i] = digitalRead(buttons[i])
    btn_sum += buttons_values[i];
  }
  
  if (btn_sum != previous_btn_sum) {
    for (uint8_t i = 0; i < LEDS_AMOUNT/2; i++) {
      String topic_comparation_string = buttons_topic+buttons_subtopic+i;
      if (buttons_values[i])
        client.publish(topic_comparation_string, buttons_values[i]);
    }
  }
  previous_btn_sum = btn_sum;
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

//Callback for MQTT received messages
void callback(char* topic, byte* message, unsigned int len) {
  messages_amount = 0;
  for (uint8_t i = 0; i < LEDS_AMOUNT/2; i++) {
    String topic_comparation_string = light_topic+light_subtopic+i;
    if (strcmp(topic, topic_comparation_string) == 0) {
      message_received[i] = 1;
      lights[i] = (char)message[0] == '1';
    }
    messages_amount += message_received[i];
  }
}
