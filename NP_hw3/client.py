import boto3
import socket, sys

from sys import argv

def fix_content(content):
    content = '     ' + content
    content = content.replace('<br>', '\n     ')
    return content

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
        #print(recv, end = '')
        command = msg.split(' ')
        welcome_msg = ''
        if len(command) > 1 : welcome_msg = "Welcome, " + command[1] + '.\n'
            
        if recv == "Register successfully.\n":
            print(recv, end = '')
            #print("to herer")
            #recv = client.recv(1024)
            #recv = recv.decode()
            tmp = prefix + command[1].lower()
            s3.create_bucket(Bucket=tmp)
        elif recv == welcome_msg:         #login success
            print(recv, end = '')
            login_bucket = s3.Bucket(prefix + command[1].lower())
            login_name = command[1]
            #login_bucket.upload_file('./hello.txt', 'hello.txt')
        elif recv[0:26] == "Create post successfully.\n":
            print(recv[0:26], end = '')
            tmp = msg.find('--content')
            the_content = msg[tmp+10:]
            the_content = fix_content(the_content)
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
            read_data = recv.split('--------------------\n')
            print(read_data[0], end = '')
            
            tmp = recv.find('read_is_over')
            remain_data = recv[tmp:]
            data = remain_data.split(' ')

            bucket_name = prefix + data[1].lower()
            target_bucket = s3.Bucket(bucket_name)
            filename = data[1] + '_post'+ data[2] +'.txt'
            
            target_object = target_bucket.Object(filename)
            object_content = target_object.get()['Body'].read().decode()
            print('    --')
            print(object_content)
            print('    --')
            loc = read_data[1].find('read_is_over')
            comment = read_data[1]
            if loc > 0 : print(comment[:loc-1]) #with comment

        else:
            print(recv, end = '')
client.close()
print('Bye!!!!!!')
