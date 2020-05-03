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
target_bucket = s3.Bucket('')
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
        elif recv[0:26] == "Create post successfully.\n":
            #print("here now")
            tmp = msg.find('--content')
            the_content = msg[tmp+10:]
            post_id_loc = recv.find('\n')
            post_id = recv[post_id_loc+1:]
            post_name = login_name + '_post' + post_id + '.txt'
            fp = open(post_name, 'w')
            fp.write(the_content)
            fp.close()
            #print(post_name, the_content)
            post_acc = post_acc + 1
            login_bucket.upload_file(post_name, post_name)
        elif command[0] == 'read' and recv != "Post does not exist.\n":
            tmp = recv.find('read_is_over')
            remain_data = recv[tmp:]
            data = remain_data.split(' ')

            bucket_name = prefix + data[1].lower()
            target_bucket = s3.Bucket(bucket_name)
            filename = data[1] + '_post'+ data[2] +'.txt'
            print(filename)
            target_object = target_bucket.Object(filename)
            object_content = target_object.get()['Body'].read().decode()
            print(object_content)

            print("read success")
client.close()
print('Bye!!!!!!')
