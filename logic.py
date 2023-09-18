from threading import Thread
import paho.mqtt.client as mqtt
import time

# MQTT variables
lights_topic = "esp32Chorizo/Lights"
btns_topic = "esp32Chorizo/Buttons"
broker_ip = "192.168.1.9"

# Global variables for logic convenience
button_array = [0, 0, 0, 0]
lights = [1, 0, 0, 0]
light_time = 2
do_sequence = True

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server. """
    print('Connected with result code ' + str(rc))
    client.subscribe(f'{btns_topic}/#')

def on_message(client, userdata, msg):
    """ The callback for when a PUBLISH message is received from the server. """
    global button_array
    button_array[int(msg.topic.split('/')[-1][-1])-1] = int(str(msg.payload).split("'")[1][0]) #Topic indicates which button was pressed and is equal to the value of the button

def publish_lights(lights):
    """ Publishes the current state of the lights to the broker. """
    for i in range(4):
        client.publish(f'{lights_topic}/Light{i}', lights[i]) # Publishes the state of each light into a Light{0, 1, 2, 3} topic

def update_lights(lights):
    """ Updates the state of the lights. """
    for idx, element in enumerate(lights):
        if element:
            if idx == len(lights)-1:
                lights[0], lights[idx] = 1, 0
            else:
                lights[idx+1], lights[idx] = 1, 0
            return lights

# MQTT client setup
client = mqtt.Client()
client.on_connect = on_connect

client.on_message = on_message

client.connect(broker_ip, 1883, 60)

client.loop_start()

def produce_sequence():
    """ Produces the sequence of lights in a thread that can be interruptec with a global variable updated in the main loop. """
    global do_sequence, lights
    while True:
        if do_sequence:
            print(lights)
            publish_lights(lights)
            lights = update_lights(lights)
            start = time.time()
            while time.time()-start < light_time and do_sequence == True:
                pass

# Thread that produces the sequence of lights
thread1 = Thread(target=produce_sequence)
thread1.start()

# Main loop
while True:
    if button_array[0] or button_array[1] or button_array[2] or button_array[3]: # If any button is pressed, the sequence is interrupted and the lights are updated
        do_sequence = False
        if button_array[0]:
            lights = [1, 0, 0, 0]
        elif button_array[1]:
            lights = [0, 1, 0, 0]
        elif button_array[2]:
            lights = [0, 0, 1, 0]
        elif button_array[3]:
            lights = [0, 0, 0, 1]
        publish_lights(lights)
        idxs = []
        for idx, element in enumerate(lights): # Gets the indexes of the lights that are off
            if element == 0:
                idxs.append(idx)
        # Waits until the buttons of the lights that are off are turned on and then off again
        while not(button_array[idxs[0]] or button_array[idxs[1]] or button_array[idxs[2]]):
            print("Crossing")
        while button_array[idxs[0]] or button_array[idxs[1]] or button_array[idxs[2]]:
            print("Crossed")
        lights = update_lights(lights)
    else:
        do_sequence = True