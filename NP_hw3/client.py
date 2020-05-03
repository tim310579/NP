import boto3
import socket, sys

from sys import argv
prefix = '0616027hw3-16567629137-10923492449-'
cli, ip, port = argv

port = int(port)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect((ip, port))

recv = client.recv(1024)
print(recv.decode(), end = '')
#recv = client.recv(1024)
#print(recv.decode(), end = '')

s3 = boto3.resource('s3')
while True:
    print('% ', end = '')
    msg = input()
    if msg == '':
        client.send("nothing to do".encode())
        
    else:
        client.send(msg.encode())
        recv = client.recv(4096)
        recv = recv.decode()
        if recv == 'close':
            break
        print(recv, end = '')
        
        #print(recv == 'Register successfully.\n')
        if recv == "Register successfully.\n":
            #print("to herer")
            #recv = client.recv(1024)
            #recv = recv.decode()
            name = msg.split(' ')
            tmp = prefix + name[1].lower()
            s3.create_bucket(Bucket=tmp)
        
client.close()
print('Bye!!!!!!')
