
Abdullah Mert Tuncay
Baris Sugur
Group 2

## Being sure the servers are running, first run scp.py 
## This script (scp.py), scp's all files to related nodes, edit scp.py as specified.
## After making ssh connections, execute scripts as follows (in the order):

### 
1) python destination.py                 (in destination node)
2) python r2.py                          (in r2 node)
3) python r1.py                          (in r1 node)
4) ./broker.py  (./ instead of python)   (in broker node)
5) python source.py                      (in source node)

### After the execution of source.py in the server node, end-to-end time(ms) will be printed.
### Server will keep listening until connection is closed. (Or Ctrl+Z)
### Hence source.py can be executed as many times as one wants.

## For tc/netem delays, execute the following command in broker,r1,r2 and destination nodes:
"sudo tc qdisc change dev eth0 root netem delay Xms 5ms distribution normal" (X=wanted delay)
## Note that this command will change the current delay with X.

### After delay commands, rerun the script executions and observe the change in end-to-end delay.

### Note: To rerun the scripts, terminate the previous processes. (lsof -i ; kill -9 pid)

### Note: The tex file is inside the TP_Part1_02 directory named TP_Part1_02.tex
