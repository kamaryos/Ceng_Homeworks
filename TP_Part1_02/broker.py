#!/usr/bin/env python3
 
import socket # Needed for socket creation
import sys

HOST = '10.10.1.2' # local host
PORT = 31337       # PORT to read the message

sendsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # create a socket which will be used to connect r1
sendsock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # create a socket which will be used to connect r2

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT)) # bind the source
    s.listen() # listen the source
    while True:
        conn, addr = s.accept() # When source tries to connect, accept 
        print('Connected by', addr) # Print the connected by, enables us to see the connections clearly
        data = conn.recv(1024) # Read the message sent by source
        if not data:
            break
        length = len(data)
        if length%2 == 0: # If length%2 of the message is 0, use r1 router to send the message.
            sendsock.sendto(data, ('10.10.2.2', 31337))
        else: # else user r2 router.
            sendsock2.sendto(data, ('10.10.4.2', 31338))
