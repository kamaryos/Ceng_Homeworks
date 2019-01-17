#!/usr/bin/env python3
 
import socket # Needed for socket creation
import ntplib # Needed for NCP time (syncronize the time between source and destination)
from datetime import datetime # Needed for NCP time

HOST = '10.10.2.2' # local host
PORT = 31337       # PORT to read the message

sendsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # Create a UDP socket to send the message to destination.

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # Create a UDP socket to get the message from broker.
s.bind((HOST, PORT)) # bind the broker with specified port
while True:
    data = s.recv(1024) # receive the message and simply send to the destination as designated port (31337)
    # Important Note: r1 and r2 uses different ports for receiving and sending message. In our design, we
    # decided this is safer way since they cannot intersect with one another.

    c = ntplib.NTPClient() 
    response = c.request('time.google.com', version=3)
    time = datetime.fromtimestamp(response.orig_time)
    print(time) # prints time for r1, can be used to compare the delays 

    if not data:
        break
    sendsock.sendto(data, ('10.10.3.2', 31337)) # sends the message to destination through port 31337
