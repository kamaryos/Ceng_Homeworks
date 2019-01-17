import os
# Replace [path_to..] with the directory path of the related file.
# Replace [identity] with yours (example: e2099315@pc3.instageni.maxgigapop.net:~)

os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 26710 [path_to_source.py] [identity]@pc3.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 26706 [path_to_broker.py] [identity]@pc3.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 26708 [path_to_r1.py] [identity]@pc3.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 26709 [path_to_r2.py] [identity]@pc3.instageni.maxgigapop.net:~")
os.system("scp -i ~/.ssh/id_geni_ssh_rsa -P 26707 [path_to_destination.py] [identity]@pc3.instageni.maxgigapop.net:~")
