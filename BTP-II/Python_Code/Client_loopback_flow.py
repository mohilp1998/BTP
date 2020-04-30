# ----------------------------------------------------------------------------------------------------------
# Function for password initialization
def password_logic():
    # msg_len = len("Please set new 8 character password:")
    while(ser.in_waiting < 1):
        pass
    time.sleep(1)
    msg = ser.read(ser.in_waiting).decode('utf-8')
    if msg == "Please set new 8 character password:":
        print(msg)
        new_password = input()

        if len(new_password) == 8:
            ser.write(new_password.encode('utf-8'))
        else:
            print("Password must be of 8 digits only exiting system now")
            quit()

        msg_len = len("Password Set")
        while (ser.in_waiting < msg_len):
            pass
        msg = ser.read(msg_len).decode('utf-8')
        if msg == "Password Set":
            print(msg)

        msg_len = len("Please input the 8 character password:")
        while (ser.in_waiting < msg_len):
            pass
        msg = ser.read(msg_len).decode('utf-8')
        if msg == "Please input the 8 character password:":
            print(msg)
            password_input = input()
            if len(password_input) == 8:
                ser.write(password_input.encode('utf-8'))
            else:
                print("Password must be of 8 character exiting the system now")
                quit()

    else:
        temp_password = "12345678" # This is used as to ensure that FPGA flowing data in between devices will have fix number of things to flow
        ser.write(temp_password.encode('utf-8'))

        msg_len = len("Password Set")
        while(ser.in_waiting < msg_len):
            pass
        msg = ser.read(msg_len).decode('utf-8')

        msg_len = len("Please input the 8 character password:")
        while (ser.in_waiting < msg_len):
            pass
        msg = ser.read(msg_len).decode('utf-8')
        if msg == "Please input the 8 character password:":
            print(msg)
        else:
            print("Error needs some Debugging should not have come here")
            quit()
        
        password_input = input()
        if len(password_input) == 8:
            ser.write(password_input.encode('utf-8'))
        else:
            print("Password must be of 8 character exiting the system now")
            quit()

    msg_len = len("Password Incorrect")
    while (ser.in_waiting < msg_len):
        pass
    msg = ser.read(msg_len).decode('utf-8')
    print(msg)

    if msg == "Password Incorrect":
        print("Exiting the system")
        quit()

    # Here onwards we will ask for encryption key input this will be handled by a different function in main
    return

# Function to support encryption key
# ----------------------------------------------------------------------------------------------------------
def convert_string_to_int(character):
    switcher = {
        '0' : 0,
        '1' : 1,
        '2' : 2,
        '3' : 3,
        '4' : 4,
        '5' : 5,
        '6' : 6,
        '7' : 7,
        '8' : 8,
        '9' : 9,
        'a' : 10,
        'b' : 11,
        'c' : 12,
        'd' : 13,
        'e' : 14,
        'f' : 15
    }
    return switcher.get(character, -1)

# Function to send encryption key
# ----------------------------------------------------------------------------------------------------------
def send_encryption_key():
    print("Please input the 128 bit encryption key to be used for this session in Hex format")
    encry_key = input()
    if len(encry_key) != 32:
        print("The key must have 32 hex characters. Exiting the system")
        quit()
    key_in_list_form = []
    for x in range(16):
        upper = convert_string_to_int(encry_key[2*x])
        lower = convert_string_to_int(encry_key[(2*x) + 1])

        if upper == -1 or lower == -1:
            print("The input must be in hex format with no capitilization of alphabets, there is some error please check")
            print("Exiting the system")
            quit()

        complete_character = ((upper << 4) | lower)
        key_in_list_form.append(complete_character)

    key_in_bytes_form = bytes(key_in_list_form)
    # print("sending the encryption key to the system")
    ser.write(key_in_bytes_form)

# Below are the functions for loop operation
# ----------------------------------------------------------------------------------------------------------
####### On Message client ########
def on_message(client, userdata, message):
    temp = message.payload
    # print("Received Message", temp)
    q_sending.put(temp)
###################################

# These are the threads which will run continuous
# ----------------------------------------------------------------------------------------------------------
##### Basic Server Setting #####

def reader_thread():
    while True:
        if ser.in_waiting > 0:
            a = ser.read(ser.in_waiting)
            # print(a)
            q_receiving.put(a)

# ----------------------------------------------------------------------------------------------------------
def client_sender():
    broker_add = "127.0.0.1"
    topic = "MOHIL/topic_message"
    client = mqtt.Client("Client_1_sender")
    time.sleep(2)
    client.connect(broker_add, 1883, 6000)  # Third one is the timeout
    while True:
        if q_receiving.qsize() > 0:
            a = q_receiving.get()
            # print(a)
            client.publish(topic, a)
            # print("Published",client.publish(topic, a),a)

# ----------------------------------------------------------------------------------------------------------
def client_receiver():
    broker_add = "127.0.0.1"
    topic = "MOHIL/topic_message"
    client_2 = mqtt.Client("Client_1_receiver")
    client_2.connect(broker_add, 1883, 6000)  # Third one is the timeout
    client_2.on_message = on_message
    client_2.loop_start()
    client_2.subscribe(topic)
    while True:
        pass

# ----------------------------------------------------------------------------------------------------------
def writer_thread():
    while True:
        ser.write(q_sending.get())

import paho.mqtt.client as mqtt
import serial
import time
import queue
import threading

# Initializing the system for further steps
ser = serial.Serial()
ser.baudrate = 230400
ser.port = 'COM6'

# Initializing the queues for receiving and storing data while transmission is going on
queuesize = 1000
q_receiving = queue.Queue(maxsize=queuesize)
q_sending = queue.Queue(maxsize=queuesize)

ser.open()
ser.reset_input_buffer()
ser.reset_output_buffer()

# print("Starting Password and encryption key reception work")
password_logic()
send_encryption_key()

# print("Starting the threads for main loop function")
t1 = threading.Thread(target=reader_thread)
t2 = threading.Thread(target=writer_thread)
t3 = threading.Thread(target=client_sender)
t4 = threading.Thread(target=client_receiver)



t1.start()
t2.start()
t3.start()
t4.start()

# print("All Threads started")
t1.join()
t2.join()
t3.join()
t4.join()