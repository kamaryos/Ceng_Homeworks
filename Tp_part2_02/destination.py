#!/usr/bin/env python3

from subprocess import call 	# Needed library
from datetime import datetime 	# Needed for NCP time
from sys import argv			# Needed library
import threading				# Needed for threading
import hashlib					# Needed for checksum calculations
import _pickle as cPickle		# Needed library
import sys 						# Needed library
import marshal                  # Needed library
import socket 					# Needed for socket creation
import fcntl  					# Needed library
import os    					# Needed library
#import ntplib 					# Needed for NCP time (syncronize the time between source and destination)

# GENERAL DESCRIPTION OF THE SCRIPT
# Server waits for a packet to come, when a packet comes following things happen:
# 1) Calculate tbe expected checksum, compare with the checksum sent by the broker.
# 2) Compare expected packet number and the packet number sent by the broker.
# 3) Send ACK according to the comparisons, if packet is wrong, server sends the 
# last packet ACK to the broker, stating that last packet is not received correctly. 
# Our implementation for the broker handles these cases and sends the window again.
# We have used hashlib.md5() function for checksum calculations that python have.
# Since we have 2 threads, each thread has their own file called output1.txt and 
# putput2.txt. When both threads are finished, our script will concenate these two
# output files into a single file called output.txt, while deleting others. Please 
# that the order will be correct because first thread sends the first half of the 
# file while second sends the second half. And we managed the order check for each 
# window in both destination.py and broker.py, hence guaranteeing the order to be
# correct in both input.txt and output.txt files. (The sizes of them are also equal, 
# one might check it with the following command: ls -lh;)

# GLOBAL variables
number_of_thread = 2			# In our implementation, 2 threads are used, this indicated the number of threads.
thread_array = []				# Will be used to store the threads. Is a list for easier handling (joins). 
unique_checksum_number = 127271	# The unique prime number for checksum calculations.
# For thread 1, server address[0] (router 1) is used, for thread-2, server address[1] (router 2) is used.
client_address = [("10.10.2.1", 52000), ("10.10.4.1", 52002)]
server_address = [("10.10.3.2", 52000), ("10.10.5.2", 52002)]

def receive_message():
    expected_packet_number = 0 	# Will be used to keep track of the packets. (Whether they are in order or not)

    # Below three lines are to set the broker-server address according the thread number.
    current_thread_no   = threading.currentThread().getName().split('-')[1]

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind(('', server_address[int(current_thread_no) - 1][1]))

    # Below two lines open a file called output.txt.
    output_file = "output" + current_thread_no + ".txt"
    target = open(output_file, 'wb')

    while True: # Destination (server) is listening
        expected_packet_number += 1 # expected packet number increased at every iteration

        # Below two lines gets the data and deserializes it.
        broker_data, curr_broker_address = server_socket.recvfrom(1000)
        broker_data = marshal.loads(broker_data)

        # Broker data has 2 main parts which are;
        # 1) Message header
        # 2) Message itself
        broker_data_header = broker_data[0]
        main_message = broker_data[1]

		# Message header has 3 main parts which are;
		# 1) Broker address
		# 2) Packet number
		# 3) Checksum value 
        broker_address = broker_data_header[0]
        message_packet_number = broker_data_header[2]
        message_checksum = broker_data_header[3]


        # Calculate the expected_checksum to later compare with received checksum value.
        expected_checksum = hashlib.md5();
        checking = str(main_message) + str(unique_checksum_number)
        expected_checksum.update(checking.encode("UTF-8"))
        expected_checksum = expected_checksum.digest()

        # Check checksum value to determine whether data is corrupted or not
        if message_checksum != expected_checksum: 
            message_packet_number = expected_packet_number - 1			# Reduce message packet number by 1.
            expected_packet_number = expected_packet_number - 1 		# Reduce expected packet number by 1.
            message_packet_number = marshal.dumps(message_packet_number)
            server_socket.sendto(message_packet_number, broker_address)	# Send old ACK to broker and cause retransmission of this packet.
        # Check whether there is a lost packet or the order is wrong.
        elif message_packet_number != expected_packet_number:
            message_packet_number = expected_packet_number - 1			# Reduce message packet number by 1.
            expected_packet_number = expected_packet_number - 1			# Reduce expected packet number by 1
            message_packet_number = marshal.dumps(message_packet_number)
            server_socket.sendto(message_packet_number, broker_address)	# Send old ACK to broker and cause retransmission of this packet. 
        # If received message is correct and not curropted:
        else:
            message_packet_number = marshal.dumps(message_packet_number)
            server_socket.sendto(message_packet_number, broker_address) # Send ACK to the broker.
            target.write(main_message)	# Write the message to file
            sys.stdout.flush()
            if (broker_data_header[4] == True): # If close flag is up, break and close the connection.
                break

    server_socket.close() # Close the connection between broker and destination.

if __name__ == "__main__":
    output_file_names = "cat " # Will be used to create output files for each thread.
    for i in range(number_of_thread):
    	# Assign the threads to the receive function.
        t = threading.Thread(target=receive_message,)
        # Add thread to the thread array so they can be used for join() later.
        # t.daemon = True
        thread_array.append(t)
        # Create a unique output file for the thread
        output_file_names += "output" + str((i + 1)) + ".txt "
        # Start the thread
        t.start()
        os.system(output_file_names) # Create output files for each thread.

    for i in range(number_of_thread):
        thread_array[i].join() # join() the threads.

    # Below 3 lines combine the output files into one called output.txt and deletes the rest.
    output_file_names += "> output.txt" 
    os.system(output_file_names)
    os.system('rm -rf output1.txt;rm -rf output2.txt;')
