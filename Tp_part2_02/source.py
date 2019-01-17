#!/usr/bin/env python3

import socket # Needed for socket creation
import ntplib # Needed for NCP time (syncronize the time between source and destination)
import sys	  # Needed library
from datetime import datetime # Needed for NCP time

# GENERAL DESCRIPTION OF THE SCRIPT
# According to our design, since there is a TCP connection between source and broker,
# reliable data transfer exists. So from source to broker, TCP is used.
# This script first creates and connects to broker with a socket, then starts to read
# the related 5 MB file (input.txt) 1024 bytes per segment and sends to the broker.
# At the end of the file, file is closed and "Closed" is printed for tracking purposes.
# When this script is run and finished, there suppose to be an inout.txt file at the 
# broker, sent by source. As soon as broker receives the file, it will start to transfer 
# the data with reliable UDP implemented by us. (more detailed explanation in broker.py) 

# GLOBAL variables
HOST = '10.10.1.2'  # Broker's IP adress (used to send the file)
PORT = 31336        # PORT used for file transfer from source to broker (TCP)
input_file = ''		# Initialize the input file name (to read input.txt)
input_message = []	# Initialize the message, (segment by segment, this will be updated)

if __name__ == "__main__":
    input_file = sys.argv[1] # Get the input file as argument.

    # Below three lines are used to get time from time.google.com, for tracking purposes
    c = ntplib.NTPClient()
    response = c.request('time.google.com', version=3)
    time = datetime.fromtimestamp(response.orig_time)

    print(time) #Print current time (Used to compare with destination time)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Create socket (which will be used to connect to the broker)
    s.connect((HOST, PORT)) # Connect to the broker

    # sys.stdout.flush() # calculate input file size

    inp = open('input.txt','rb') 	# Open the input.txt file to start reading and sending.
    input_message = inp.read(1024)	# Send the message as 1024 bytes segments.
    print("Sending...")				# Print for tracking purposes

    # This while loop sends the file as 1024 bytes segment untill reaching the end of file.
    while (input_message):				# Will be over when input message is NULL (eof)
        s.sendall(input_message) 		# Send the segment
        input_message = inp.read(1024)	# Read next 1024 from input.txt

    inp.close() 	# Close the file
