#!/usr/bin/env python3
 
import socket # Needed for socket creation
import ntplib # Needed for NCP time (syncronize the time between source and destination)
from datetime import datetime # Needed for NCP time
 
HOST = '10.10.1.2'  # Broker's IP adress (used to send the message)
PORT = 31337        # PORT used for message transfer

c = ntplib.NTPClient() #Used to get time from time.google.com
response = c.request('time.google.com', version=3) # Get time from time.google.com
time = datetime.fromtimestamp(response.orig_time)  # Get time from time.google.com
print(time) #Print current time (Used to compare with destination time)

# In our design, time itself is sent as the message
data_time = str(time).encode() # Encode the time as message

s =socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Create socket which will be used to connect to the broker
s.connect((HOST, PORT)) # Connect to broker
s.sendall(data_time) 	# Send the message