#include "buttons.h"

Buttons::Buttons(uint8_t* buttons) {
  for (uint8_t i = 0; i < BUTTONS_AMOUNT; i++) {
    this->buttons_values[i] = 0;
    this->buttons_pins[i] = buttons[i];
    pinMode(this->buttons_pins[i], INPUT_PULLDOWN);
  }
}

void Buttons::mqtt_read_and_send(PubSubClient* client, char* topic) {
  this->btn_sum = 0;
  for (uint8_t i = 0; i < BUTTONS_AMOUNT; i++) {
    this->buttons_values[i] = digitalRead(this->buttons_pins[i]);
    this->btn_sum += this->buttons_values[i];
  }
  if (this->btn_sum != this->previous_btn_sum) {
    for (uint8_t i = 0; i < BUTTONS_AMOUNT; i++) {
      char* button_topic;
      const char* idx = '0'+i;
      strcat(button_topic, topic);
      strcat(button_topic, idx);
      client->publish(button_topic, this->buttons_values[i]);
    }
  }
  this->previous_btn_sum = this->btn_sum;
}
