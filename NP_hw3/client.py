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
login_bucket = s3.Bucket('')
post_acc = 1
login_name = ''

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
        command = msg.split(' ')
        welcome_msg = ''
        if len(command) > 1 : welcome_msg = "Welcome, " + command[1] + '.\n'
        #print(recv == 'Register successfully.\n')
        if recv == "Register successfully.\n":
            #print("to herer")
            #recv = client.recv(1024)
            #recv = recv.decode()
            tmp = prefix + command[1].lower()
            s3.create_bucket(Bucket=tmp)
        elif recv == welcome_msg:         #login success
            #print("login_success")
            login_bucket = s3.Bucket(prefix + command[1].lower())
            login_name = command[1]
            #login_bucket.upload_file('./hello.txt', 'hello.txt')
        elif recv == "Create post successfully.\n":
            #print("here now")
            tmp = msg.find('--content')
            the_content = msg[tmp+10:]
            #print(the_content)
            post_name = login_name + '_post'+str(post_acc)+'.txt'
            fp = open(post_name, 'w')
            fp.write(the_content)
            fp.close()
            #print(post_name, the_content)
            post_acc = post_acc + 1
            login_bucket.upload_file(post_name, post_name)

client.close()
print('Bye!!!!!!')
