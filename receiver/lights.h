#include <Arduino.h>
#include "constants.h"

class Lights {
  private:
    uint8_t message_received[LIGHTS_AMOUNT];
    uint8_t lights[LIGHTS_AMOUNT];
    uint8_t leds_pins[LEDS_AMOUNT];
    void update_lights();
    
  public:
    Lights(uint8_t* leds);
    void update_lights_on_message(char* topic, uint8_t value);
};
