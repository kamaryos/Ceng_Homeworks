#!/usr/bin/env python3
 
import socket # Needed for socket creation
import fcntl  # Needed libraries
import os     # Needed libraries
import ntplib # Needed for NCP time (syncronize the time between source and destination)
from datetime import datetime # Needed for NCP time

HOST = '10.10.3.2'  # local adress (used with r1)
HOST2 = '10.10.5.2' # local adress (used with r2)
PORT = 31337        # port that message is received (if sent by r1)
PORT2 = 31338       # port that message is received (if sent by r2)


socks = [] # list of sockets (sockets will be appended)

sock1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock1.bind((HOST, PORT))
fcntl.fcntl(sock1, fcntl.F_SETFL, os.O_NONBLOCK)
socks.append(sock1) # append the first socket to socks (socket with r1)

sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)      
sock2.bind((HOST2, PORT2))
fcntl.fcntl(sock2, fcntl.F_SETFL, os.O_NONBLOCK)
socks.append(sock2) # append the second socket to socks (socket with r2)

while True:
    for s in socks: # for every socket in socks
        try:            
            msg = s.recv(1024) # receive message
            c = ntplib.NTPClient() # get the current time from time.google.com
            response = c.request('time.google.com', version=3) # get the current time from time.google.com
            time_d = datetime.fromtimestamp(response.orig_time) # get the current time from time.google.com
            time_s = datetime.strptime(str(msg), "%Y-%m-%d %H:%M:%S.%f") # get the source time from the message
            print(time_d-time_s) # print the time it takes for a message to get from source the destination 
                                 # Compare with the time printed by the source
        except: # continue listening
            continue
