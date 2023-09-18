#include <PubSubClient.h>

class Buttons {
  private:
    uint8_t buttons_values[BUTTONS_AMOUNT];
    uint8_t buttons_pins[LIGHTS_AMOUNT];
    uint8_t btn_sum, previous_btn_sum = 0;
    
  public:
    Buttons(uint8_t* buttons) {
      for (uint8_t i = 0; i < BUTTONS_AMOUNT; i++) {
        this.buttons_vaules[i] = 0;
        this.buttons_pins[i] = buttons[i];
        pinMode(buttons[i], INPUT_PULLDOWN);
      }
    }

    void mqtt_read_and_send(PubSubClient* client, char* topic) {
      this.btn_sum = 0;
      for (uint8_t i = 0; i < BUTTONS_AMOUNT; i++) {
        this.buttons_values[i] = digitalRead(this.buttons_pins[i])
        this.btn_sum += this.buttons_values[i];
      }
      if (this.btn_sum != this.previous_btn_sum) {
        for (uint8_t i = 0; i < BUTTONS_AMOUNT; i++) {
          String button_topic = topic+String(i);
          client->publish(button_topic, this.buttons_values[i]);
        }
      }
      this.previous_btn_sum = this.btn_sum;
    }
}
