#!/usr/bin/env python3

from socket import timeout  # Needed for timeout
from sys import argv        # Needed library
import socket               # Needed for socket creation
import sys                  # Needed library
import marshal
import threading            # Needed for thread creation and implementation
import hashlib              # Needed for checksum computation
import _pickle as cPickle   # Needed library
import time                 # Needed for the time
import os                   # Needed for os commands
# GENERAL DESCRIPTION OF THE SCRIPT
# In our design, there are 2 threads that is working accross broker
# and server. First thread is using r1 and second thread is using r2 
# while sending data and acknowledgements. We decided for the window
# size to be 5 considered to be a feasible choice. For each window 
# broker sends data to the server and waits for the acknowledgement.
# For each acknowledment, broker waits for maximum 0.1 sec and consider
# the packet lost passed that time. The whole file take about 90 seconds
# to send from broker to destination with acknowledgement for each 
# packet sent hence making the connection extremely reliable and faster 
# than composite acknowledgements (instead of 1 ACK for whole window). 
# And if there is a missing packet in a window, whole window is sent again.
# Notice that our implementation also checks for the out of order 
# packet sends and fixes them. More on this in the destination.py file.
# Also note that we calculate the size of the message before sending it so 
# message file can be any size. 

# GLOBAL variables
thread_array = []               # Will be used to store the threads. Is a list for easier handling (joins).
number_of_thread = 2            # In our implementation, 2 threads are used, this indicated the number of threads.
HOST = '10.10.1.2'              # Local host (IP address of the broker that is used to connect to source).
PORT = 31336                    # PORT used to read the message from source with TCP.

def send_message(message_len, message_offset):
    # For thread 1, broker address[0] (router 1) is used, for thread-2, broker address[1] (router 2) is used.
    # For thread 1, dest.  address[0] (router 1) is used. for thread-2, dest.  address[1] (router 2) is used.
    broker_address = [("10.10.2.1", 52000), ("10.10.4.1", 52002)]
    dest_address   = [("10.10.3.2", 52000), ("10.10.5.2", 52002)]
    max_message_segment_size = 900  # Max size of the segment (set to 900 bytes since max message segment is 1000 bytes).
    window_size = 5                 # Window size is set to 5 (max # of segment sent before ACK receive).
    unique_checksum_number = 127271 # The unique prime number for checksum calculations.
    #input_message = []              # Initialization of the input message.

    message_len    = int(message_len)       # Make message_len int
    message_offset = int(message_offset)    # Make offsett int

    close = False       # Set close true when the message is finished, is sent to destination in each packet.
    closer_counter = 0  # close_counter is used to close the connection between broker and destination if there is a problem.
    go_back_N = 0       # For sent segments, go back N is used. If # of ACK waits = go_back_N, stop sending and wait ACKs.

    # Below three lines are to set the client and destination addressess according the thread number.
    current_thread_no   = threading.currentThread().getName().split('-')[1]
    curr_broker_address = broker_address[int(current_thread_no) - 1]
    curr_dest_address   = dest_address[int(current_thread_no) - 1]

    # Below 2 lines are used to create a socket for connecting to destination node
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
    client_socket.bind(('', curr_broker_address[1]))

    # Timeout is set to 0.5. If ACK's does not come in specified time, broker thinks there
    # was a problem with the sent segments and will send the whole window (5 segment) again.
    client_socket.settimeout(1) 

    packet_no = 0   # First packet number set to zero, will be increased at every iteration

    while message_offset < message_len:
        #input_message = inp.read(900)
        # Transfer can be maximum "max_message_segment_size bytes" at each iteration.
        if message_offset + max_message_segment_size > message_len:     ## This means we reach the end of message
            message_part = file_message[message_offset:message_len] ## Take the remaning last piece
            close = True  ## Let server know that it is the last piece
        else:  ## That means we have still more message then max_message_segment_size
            message_part = file_message[message_offset:message_offset + max_message_segment_size]  ## Take the only max_message_segment_size
        message_offset += max_message_segment_size;  ## Increment the offSet

        # Below 4 lines are used to compute the checksum by hashlib.md5() function according to the message.
        # This is one of the implementation that makes the data transfer reliable.
        check_sum = hashlib.md5()
        checking = str(message_part) + str(unique_checksum_number)
        check_sum.update(checking.encode("UTF-8"))
        check_sum = check_sum.digest()

        packet_no += 1 # Increment packet_number by 1. Destination will check this whether it is equal to the expected packet number.

        # Create the message header. It contains 5 parts
        # 1) Current broker address, differs between the threads.
        # 2) Current destination address, differs between the threads.
        # 3) Packet number to keep track of the segments. Destination will check whether it is the expected packet or not.
        # 4) Check_sum is used to determine whether the message is curropted or not.
        # 5) close is the flag that set to 1 when it is end of file. 
        message_header = (curr_broker_address, curr_dest_address, packet_no, check_sum, close) 

        # Below 2 lines prepare the message itself. First 100 bytes as header and remaining 900 bytes as message itself
        message_segment = (message_header, message_part) 
        message_segment = marshal.dumps(message_segment)

        client_socket.sendto(message_segment, curr_dest_address) # send the message to destination according the current thread dest_address

        go_back_N += 1 # Since a segment is send, increase the go_back_N by 1.

        if (go_back_N == window_size): # If go_back_N is 5 stop sending segments and wait for the appropriote ACKnowledgments.
            try:
                for i in range(go_back_N):
                    server_response, address = client_socket.recvfrom(100) # Receive ACKonwledgment for the related segment.
            except timeout: # If ACK receives waits are too long, resend the whole window (5 segment).
                message_offset = message_offset - max_message_segment_size * go_back_N # Reduce the message_offset correctly.
                packet_no = packet_no - go_back_N       # Reduce the packet_no correctly.
                go_back_N = 0                           # Set go back N to 0.
            else:
                server_response = marshal.loads(server_response)
                # Check the order, if it is wrong send the whole window again.
                if (server_response != packet_no):
                    message_offset = message_offset - max_message_segment_size * go_back_N
                    packet_no = packet_no - go_back_N # Reduce the packet_no correctly.
                go_back_N = 0 # Set go_back_N to 0.
        elif (close): # If end of file is reached, prepare to close the connection.
            try:
                for i in range(go_back_N): # Receive last ACK's.
                    server_response, curr_dest_address = client_socket.recvfrom(100) 
            except timeout: # If ACK receives waits are too long, resend the whole window (5 segment) 
                closer_counter += 1
                if closer_counter == 3: # Used check the connection, if timeout occurs 3 times, break and close the connection.
                    break;
                message_offset = message_offset - max_message_segment_size * go_back_N # Reduce the message_offset correctly.
                packet_no = packet_no - go_back_N   # Reduce the packet_no correctly.
                go_back_N = 0 # Set go back N to 0.
            else:
                server_response = marshal.loads(server_response)
                # Check the order, if it is wrong send the whole window again.
                if (server_response != packet_no):
                    message_offset = message_offset - max_message_segment_size * go_back_N # Reduce the message_offset correctly.
                    packet_no = packet_no - go_back_N   # Reduce the packet_no correctly.
                go_back_N = 0 # Set go back N to 0.
    

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    print("Thread",current_thread_no,"finished at ",current_time)

    client_socket.close() # Close the connection between broker and destination
    os.system('rm -rf output.txt')

if __name__ == "__main__":

    #First part collects the TCP packets of the input file
    s =  socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Create a socket to connect to the source
    s.bind((HOST, PORT))        # Bind with the source
    f = open('output.txt','wb') # Input file creation
    s.listen()                  # listen the source
    line = 0                    # Initialize line = 0, for tracking purpose.
    # Below 10 lines gets the file from source node. It uses TCP 
    # connection with 1024 bytes segments. Since it uses TCP, the
    # data transfer is reliable. 
    conn, addr = s.accept()             # When source tries to connect, accept.
    print('Connected by', addr)         # Print connected by, enables us to see the connections clearly, for tracking purpose.

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)

    data = conn.recv(1024)              # Read the message sent by source, also initialize the data.
    while (data):   
        f.write(data)            # Write the data to file.
        data = conn.recv(1024)   # Read next 1024 bytes.
    f.close()                    # Close the file.

    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)

    inp = open("output.txt",'rb')   # Open the file.
    file_message = inp.read()       # Read the file
    sys.stdout.flush()              

    message_size = len(file_message)     # Will be used for the calculations of the arguments to our send_message function

    print("Message size =", message_size, "bytes") # Print the size, must be 5.000.000 (since the file is 5MB)

    # Below 3 lines print the current time to calculate the total time to send the file.
    t = time.localtime()
    current_time = time.strftime("%H:%M:%S", t)
    print("Threads are started at ", current_time)

    
    for i  in range(number_of_thread):
        # Assign the threads to the sender function.
        t = threading.Thread(target=send_message, args=((message_size * (i + 1)) / number_of_thread, (message_size * i) / number_of_thread),)
        thread_array.append(t) # Add thread to the thread array so they can be used for join() later.
        t.start() # Start the thread.
    for i in range(number_of_thread):
        thread_array[i].join() # join() the threads.
