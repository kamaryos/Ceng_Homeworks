import os
# Replace [path_to..] with the directory path of the related file.
# Replace [identity] with yours (example: e2099315@pc3.instageni.maxgigapop.net:~)
#Replace input.txt file with your input file
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 25906 [path_to_broker.py] [identity]pc1.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 25907 [path_to_destination.py] [identity]@pc1.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 25910 [path_to_source.py] [identity]@pc1.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 25910 [path_to_input.txt] [identity]@pc1.instageni.maxgigapop.net:~")

