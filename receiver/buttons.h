#include <Arduino.h>
#include <PubSubClient.h>
#include <string.h>
#include "constants.h"

#define INPUT_PULLDOWN 0x09

class Buttons {
  private:
    uint8_t buttons_values[BUTTONS_AMOUNT];
    uint8_t buttons_pins[LIGHTS_AMOUNT];
    uint8_t btn_sum, previous_btn_sum = 0;
    
  public:
    Buttons(uint8_t* buttons);
    void mqtt_read_and_send(PubSubClient* client, char* topic);
};
