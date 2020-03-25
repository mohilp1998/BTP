import paho.mqtt.client as mqtt
import serial
import time
import queue
import threading

ser = serial.Serial()
ser.baudrate = 230400
ser.port = 'COM6'
queuesize = 1000
q = queue.Queue(maxsize=queuesize)
q2 = queue.Queue(maxsize=queuesize)
ser.open()
ser.reset_input_buffer()
ser.reset_output_buffer()

####### On Message client ########
def on_message(client, userdata, message):
    temp = message.payload
    # print("Received Message", temp)
    q2.put(temp)
###################################

##### Basic Server Setting #####

def reader_thread():
    while True:
        if ser.in_waiting > 0:
            a = ser.read(ser.in_waiting)
            # print(a)
            q.put(a)

def client_sender():
    broker_add = "localhost"
    topic = "MOHIL/topic_message_b"
    client = mqtt.Client("Client_2_sender")
    client.connect(broker_add, 1883, 6000)  # Third one is the timeout
    while True:
        if q.qsize() > 0:
            a = q.get()
            # print(a)
            client.publish(topic, a)
            # print("Published",client.publish(topic, a),a)

def client_receiver():
    broker_add = "localhost"
    topic = "MOHIL/topic_message_a"
    client_2 = mqtt.Client("Client_2_receiver")
    client_2.connect(broker_add, 1883, 6000)  # Third one is the timeout
    client_2.on_message = on_message
    client_2.loop_start()
    client_2.subscribe(topic)
    a = 0
    while True:
        a = a + 1


def writer_thread():
    while True:
        ser.write(q2.get())

t1 = threading.Thread(target=reader_thread)
t2 = threading.Thread(target=writer_thread)
t3 = threading.Thread(target=client_sender)
t4 = threading.Thread(target=client_receiver)

t1.start()
t2.start()
t3.start()
t4.start()

print("Here")
t1.join()
t2.join()
t3.join()
t4.join()