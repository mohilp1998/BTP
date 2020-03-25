# import serial
# import time
#
# ser = serial.Serial()
# ser.baudrate = 230400
# ser.port = 'COM3'
# # ser.set_buffer_size(rx_size=12800, tx_size=12800) this does not work for our driver
# ser.open()
# while True:
#     # start = time.time_ns()
#     # # print(start)
#     # a = ser.read()
#     # b = ser.read()
#     # # print(a)
#     # # print(b)
#     # y = ser.in_waiting
#     # print(y)
#     # ser.write(a)
#     # ser.write(b)
#     # end = time.time_ns()
#     # print(end-start)
#     start = time.time_ns()
#     ser.write("hello".encode('utf-8'))
#     ser.read(ser.in_waiting)
#     # print(time.time_ns() - start)
#     print(ser.in_waiting)

# # Few things are working here so keep it in not.
# import serial
# import time
# import queue
#
# ser = serial.Serial()
# ser.baudrate = 230400
# ser.port = 'COM10'
# queuesize = 100000
# q = queue.Queue(maxsize=queuesize)
# ser.open()
# ser.reset_input_buffer()
# ser.reset_output_buffer()
# while True:
#     # Not working this part
#     # if  ser.in_waiting > 0 :
#     #     start = time.time_ns()
#     #     print(ser.in_waiting)
#     #     a = ser.read(ser.in_waiting)
#     #     q.put(a)
#     #     print("Reading Time")
#     #     print(time.time_ns() - start)
#     #
#     # if ser.out_waiting < 4070 :
#     #     start_out = time.time_ns()
#     #     ser.write(q.get())
#     #     print("Writing Time")
#     #     print(time.time_ns() - start_out)
#
#
#     # This part of code is working but there is slight loss of data in here atleast.
#     # print(ser.in_waiting)
#     # print(ser.out_waiting)
#     # ser.write(ser.read(ser.in_waiting))
#
#     # # This idea won't work at all as writing one by one to the serial induces too much delay and thus no concrete signal is see in real time at all
#     # if  ser.in_waiting > 0:
#     #     print("Here")
#     #     a = ser.read(ser.in_waiting)
#     #     if queuesize - q.qsize() > len(a) :
#     #         for index in range(len(a)):
#     #             q.put(bytes([a[index]]))
#     # if q.qsize() > 1:
#     #     ser.write(q.get())
#     #     ser.write(q.get())
#     #     print(q.qsize())
#
#
#     # Somewhat working case
#     if ser.in_waiting > 0:
#         a = ser.read(ser.in_waiting)
#         # print(a)
#         # if  q.qsize() < queuesize:
#         #     q.put(a)
#         q.put(a) # One intereseting observation is that if I directly write a in ser.write the system is getting too much noise but if I use q.put() sound is coming out better
#         ser.write(q.get())
#
#
#     # Timepass
#     # ser.write('a'.encode('utf-8'))
#     # start = time.time_ns()
#     # ser.read(1)
#     # ser.read(ser.in_waiting)
#     # print(time.time_ns() - start)
#
#     # # Timpass 2.0
#     # if ser.in_waiting > 0:
#     #     a = ser.read(ser.in_waiting)
#     #     if queuesize - q.qsize() > len(a):
#     #         for index in range(len(a)):
#     #             q.put(bytes([a[index]]))



# # MQTT server code below here- in this part I will be the publisher as well as the subscriber
# import paho.mqtt.client as mqtt
# import time
# import serial
# import queue
#
# ####### On Message client ########
# def on_message(client, userdata, message):
#     temp = message.payload
#     # print("Received Message", temp)
#     q2.put(temp)
# ###################################
#
# ### Basic queue and serial setup #####
# ser = serial.Serial()
# ser.baudrate = 230400
# ser.port = 'COM8'
# # q1size = 100
# q2size = 10000
# # q1 = queue.Queue(maxsize=q1size)
# q2 = queue.Queue(maxsize=q2size)
# ser.open()
# ser.reset_input_buffer()
# ser.reset_output_buffer()
#
# ##### Basic Server Setting #####
# broker_add = "localhost"
# topic = "audio_service"
# # print("Creating a new client for MQTT server which can either publish or subscribe")
# client = mqtt.Client("P1")
# client.on_message = on_message # attaching the function for when we receive the message
# # print("Now connecting to the broker")
# client.connect(broker_add, 1883, 6000) #Third one is the timeout
#
# ### The main part of code to receive the data and send it to the server and receive it back
# client.loop_start() # this is necessary else the on_message will not work
# # print("subscribing to the",topic)
# client.subscribe(topic)
# while True:
#     if ser.in_waiting > 0:
#         a = ser.read(ser.in_waiting)
#         print("Sending Message", a)
#         q2.put(a)
#         # client.publish(topic, a)
#     if q2.qsize() > 0:
#         ser.write(q2.get())
# client.loop_stop()
# client.disconnect()


# # Here I am trying to write a multi-threaded program one for reader and one for the writer thread.
# import serial
# import time
# import queue
# import sched
# import threading
#
# ser = serial.Serial()
# ser.baudrate = 230400
# ser.port = 'COM8'
# queuesize = 100000
# q = queue.Queue(maxsize=queuesize)
# ser.open()
# ser.reset_input_buffer()
# ser.reset_output_buffer()
#
# def reader_thread():
#     while True:
#         if ser.in_waiting > 0:
#                 a = ser.read(ser.in_waiting)
#                 if queuesize - q.qsize() > len(a):
#                     for index in range(len(a)):
#                         q.put(bytes([a[index]]))
#
# def writer_thread():
#     while True:
#         if q.qsize() > 1:
#             b_1 = q.get()
#             b_2 = q.get()
#             ser.write(b_1)
#             ser.write(b_2)
#
#
#
# t1 = threading.Thread(target=reader_thread)
# t2 = threading.Thread(target=writer_thread)
#
# t1.start()
# t2.start()
#
# t1.join()
# t2.join()




# Threading another try
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
    broker_add = "10.9.162.79"
    topic = "MOHIL/topic_message_a"
    client = mqtt.Client("Client_1_sender")
    client.connect(broker_add, 1883, 6000)  # Third one is the timeout
    while True:
        if q.qsize() > 0:
            a = q.get()
            # print(a)
            client.publish(topic, a)
            # print("Published",client.publish(topic, a),a)

def client_receiver():
    broker_add = "10.9.162.79"
    topic = "MOHIL/topic_message_b"
    client_2 = mqtt.Client("Client_1_receiver")
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