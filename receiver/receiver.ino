#include <WiFi.h>
#include <PubSubClient.h>
#include "light.cpp"
#include "buttons.cpp"

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
const uint8_t leds_pins[LEDS_AMOUNT] = {LED_1_GREEN, LED_1_RED, LED_2_GREEN, LED_2_RED, LED_3_GREEN, LED_3_RED, LED_4_GREEN, LED_4_RED};
const uint8_t buttons_pins[LIGHTS_AMOUNT] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4}

WiFiClient espClient;
PubSubClient client(espClient);
Lights lights(*leds_pins);
Buttons buttons(*buttons_pins);

void setup_wifi();
void callback(char* topic, byte* message, unsigned int len);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(){
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  buttons.mqtt_read_and_send(*client, buttons_topic+buttons_subtopic);
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
  lights.update_lights_on_message(topic, (char)message[0] == '1');
}
