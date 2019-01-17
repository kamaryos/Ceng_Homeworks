#### Baris Sugur 2099315
#### Abdullah Mert Tuncay 2099422
#### Group 2

### Being sure the servers are running, first run scp.py 
### This script (scp.py), scp's all files to related nodes
### Important note: Don't forget to edit scp.py as specified in scp.py file.
### Below are the files scp.py will send to the related nodes.
###
source.py  		(to source node)
input.txt 		(to source node)
broker.py 		(to broker node)
destination.py 	(to destination node)

### We have already made the necessary configurations for router 1 and router 2
### r1.rh and r2.rh for those configurations still exists in the router nodes,
### and can be seen. They are extremely simple commands that alters the IP table. 
### After making the ssh connections, execute scripts as follows (in the order):
###
1) python3 destination.py                (in destination node)
2) ./broker.py  (./ instead of python)   (in broker node)
3) python source.py input.txt            (in source node)

### After the execution of source.py with input.txt, source will first connect broker and
### send the file with TCP. It can be seen from broker that source is connected (will be printed).
### After that Reliable Data Transfer with UDP will starts from broker to destination (server)
### We printed the time of data transfer from Broker to Destination for tracking purposes
### So one can track the transfer and can see how long it takes. (without any packet loss or
### other commands, it take approximately 90 sec to transfer the data (without any data hazard)
### so please wait 90 sec.)2 threads are used, one operated on router 1 (r1) and one operated on 
### router 2 (r2). The individual time for the threads will also be printed, which is expected 
### to be close to each other. The time will be printed when a thread is finished for observation.

### We have already made the necessarcy configurations in the routers. But just in case there is
### a problem, below are the commands we run in each router to pass the message forward. Or simply 
### put them in a .sh file and scp/run them in the routers. After that related IP tables are shown.
### We have those .sh files but did not provide them as it has been stated in the homework text.

### In router1;
###
sudo route add -host 10.10.2.1 gw 10.10.2.2
sudo route add -host 10.10.3.2 gw 10.10.3.1
route -n

### In router2;
###
sudo route add -host 10.10.4.1 gw 10.10.4.2
sudo route add -host 10.10.5.2 gw 10.10.5.1
route -n

### For tc loss, corrupt, reorder commands, run following commands in related nodes. Note that
### these simulated data hazards are applied on a link BOTH ways. Also to change the loss or 
### add corrupt or duplicate, just simply change the percentages below. But make sure that 
#### [INTERFACE]  (eth1,eth2,eth3) parts stays same. 

### For broker;
###
sudo tc qdisc change dev eth1 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%
sudo tc qdisc change dev eth3 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%

### For r1 and r2;
###
sudo tc qdisc change dev eth1 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%
sudo tc qdisc change dev eth2 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%

### For destination;
###
sudo tc qdisc change dev eth1 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%
sudo tc qdisc change dev eth2 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%

#### Note: First time adding the tc commands, "change" needs to be "add", after that, above commands are OK.
#### Note: For larger loss, corrupt and destination, file transfer may take long time due to our go_back_N method.

#### Note: To rerun the scripts, terminate the previous processes before the execution. (lsof -i ; kill -9 pid)
