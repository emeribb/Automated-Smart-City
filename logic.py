from threading import Thread
from resources.circuit import Lights, Buttons
import paho.mqtt.client as mqtt
import time

# MQTT variables
lights_topic = "esp32Chorizo/Lights"
lights_subtopic = "/Light"
buttons_topic = "esp32Chorizo/Buttons"
broker_ip = "192.168.1.9"

# Global variables for logic convenience
lights = Lights()
buttons = Buttons()
light_time = 2
do_sequence = True

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server. """
    print('Connected with result code ' + str(rc))
    client.subscribe(f'{buttons_topic}/#')

def on_message(client, userdata, msg):
    """ The callback for when a PUBLISH message is received from the server. """
    global buttons
    buttons.update_buttons(int(msg.topic.split('/')[-1][-1])-1, int(str(msg.payload).split("'")[1][0])) #Topic indicates which button was pressed and is equal to the value of the button

# MQTT client setup
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_ip, 1883, 60)
client.loop_start()

def produce_sequence() -> None:
    """ Produces the sequence of lights in a thread that can be interruptec with a global variable updated in the main loop. """
    global do_sequence, lights, lights_topic, lights_subtopic, light_time
    while True:
        if do_sequence:
            print(lights)
            lights.publish_lights(lights_topic, lights_subtopic)
            lights.update_lights()
            lights.delay(light_time, do_sequence)

# Thread that produces the sequence of lights
thread1 = Thread(target=produce_sequence)
thread1.start()

# Main loop
while True:
    if buttons.button_pressed(): # If any button is pressed, the sequence is interrupted and the lights are updated
        do_sequence = False
        lights.update_lights(buttons.get_buttons_state())
        lights.publish_lights(lights_topic, lights_subtopic)
        idxs = lights.get_lights_off() # Gets the indexes of the lights that are off
        # Waits until the buttons of the lights that are off are turned on and then off again
        while buttons.wait_for_crossing(idxs):
            print("Crossing")
        while not(buttons.wait_for_crossing(idxs)):
            print("Crossed")
        lights.update_lights()
    else:
        do_sequence = True