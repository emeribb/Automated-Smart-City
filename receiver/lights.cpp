#include "lights.h"

void Lights::update_lights() {
  for (int i = 0; i < LEDS_AMOUNT; i+=2) {
    digitalWrite(this->leds_pins[i], this->lights[i/2]);
    digitalWrite(this->leds_pins[i+1], !this->lights[i/2]);
  }
}

Lights::Lights(uint8_t* leds) {
  for (uint8_t i = 0; i < LIGHTS_AMOUNT; i++) {
    this->message_received[i] = 0;
    this->lights[i] = 0;
  }
  for (uint8_t i = 0; i < LEDS_AMOUNT; i++) {
    this->leds_pins[i] = leds[i];
    pinMode(leds[i], OUTPUT);
  }
}

void Lights::update_lights_on_message(char* topic, uint8_t value) {
  uint8_t light_idx = topic[sizeof(topic)/sizeof(topic[0])-1];
  this->message_received[light_idx] = 1;
  this->lights[light_idx] = value;
  uint8_t messages_amount = 0;
  for (uint8_t i = 0; i < LIGHTS_AMOUNT; i++)
    messages_amount += this->message_received[i];
  if (messages_amount == LIGHTS_AMOUNT) {
    for (uint8_t i = 0; i < LIGHTS_AMOUNT; i++) {
      this->message_received[i] = 0;
      this->update_lights();
    }
  }
}
