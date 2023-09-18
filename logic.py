from threading import Thread
import paho.mqtt.client as mqtt
import time

semsTopic = "esp32Chorizo/Sems"
btnsTopic = "esp32Chorizo/Botones"
broker_ip = "192.168.1.9"

b = [0, 0, 0, 0]
sems = [1, 0, 0, 0]
semTime = 2
doSequence = True

def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))
    client.subscribe(f'{btnsTopic}/#')

def on_message(client, userdata, msg):
    global b
    b[int(msg.topic.split('/')[-1][-1])-1] = int(str(msg.payload).split("'")[1][0])

def publishSem(sems):
    for i in range(4):
        client.publish(f'{semsTopic}/Sem{i}', sems[i])

def updateSems(sems):
    for idx, element in enumerate(sems):
        if element:
            if idx == len(sems)-1:
                sems[0], sems[idx] = 1, 0
            else:
                sems[idx+1], sems[idx] = 1, 0
            return sems

client = mqtt.Client()
client.on_connect = on_connect

client.on_message = on_message

client.connect(broker_ip, 1883, 60)

client.loop_start()

def produceSequence():
    global doSequence, sems
    while True:
        if doSequence:
            print(sems)
            publishSem(sems)
            sems = updateSems(sems)
            start = time.time()
            while time.time()-start < semTime and doSequence == True:
                pass

thread1 = Thread(target=produceSequence)
thread1.start()

while True:
    if b[0] or b[1] or b[2] or b[3]:
        doSequence = False
        if b[0]:
            sems = [1, 0, 0, 0]
        elif b[1]:
            sems = [0, 1, 0, 0]
        elif b[2]:
            sems = [0, 0, 1, 0]
        elif b[3]:
            sems = [0, 0, 0, 1]
        publishSem(sems)
        idxs = []
        for idx, element in enumerate(sems):
            if element == 0:
                idxs.append(idx)
        while not(b[idxs[0]] or b[idxs[1]] or b[idxs[2]]):
            print("Ambulancia cruzando")
        while b[idxs[0]] or b[idxs[1]] or b[idxs[2]]:
            print("Ambulancia cruzo")
        sems = updateSems(sems)
    else:
        doSequence = True