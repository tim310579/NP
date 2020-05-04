import boto3
import socket, sys

from sys import argv

def fix_content(content):
    content = '     ' + content
    content = content.replace('<br>', '\n     ')
    content = '    --\n' + content + '\n    --\n'
    return content
def fix_comment(comment):
    comment = comment.replace('<br>', '\n         ')
    return comment
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
            #the_content = '    --\n' + the_content + '\n    --\n'
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
            
            remain_data = read_data[1]
            data = remain_data.split(' ')

            bucket_name = prefix + data[1].lower()
            target_bucket = s3.Bucket(bucket_name)
            filename = data[1] + '_post'+ data[2] +'.txt'
            
            target_object = target_bucket.Object(filename)
            object_content = target_object.get()['Body'].read().decode()
            #print('    --')
            print(object_content, end = '')
            #print('    --')
        elif command[0] == 'delete-post' and recv[0:21] == 'Delete successfully.\n':
            #delete success
            remain_data = recv.split('\n')
            print(remain_data[0])
            author_data = remain_data[1].split(' ')
            filename = author_data[0] + '_post' + author_data[1] + '.txt'
            target_object = login_bucket.Object(filename)
            target_object.delete()
        
        elif command[0] == 'update-post' and msg.find('--content') > 0 and recv == 'Update successfully.\n':
            #update success
            print(recv, end = '')
            tmp = msg.find('--content')
            the_content = msg[tmp+10:]
            the_content = fix_content(the_content)
            filename = login_name + '_post' + command[1] + '.txt'
            fp = open(filename, 'w')
            fp.write(the_content)
            fp.close()
            login_bucket.upload_file(filename, filename)
        
        elif command[0] == 'comment' and recv[0:22] == 'Comment successfully.\n':
            print('Comment successfully.\n', end = '')
            the_comment = msg[8:]
            tmp = the_comment.find(' ')
            the_comment = the_comment[tmp+1:]
            the_comment = fix_comment(the_comment)

            remain_data = recv.split('\n')
            author_data = remain_data[1].split(' ') #au[0]->name, au[1]->id
            bucket_name = prefix + author_data[0].lower()
            the_comment = '     ' + login_name + ': ' + the_comment + '\n'

            target_bucket = s3.Bucket(bucket_name)
            filename = author_data[0] + '_post' + author_data[1] + '.txt'
            fp = open(filename, 'a')
            fp.write(the_comment)
            fp.close()
            target_bucket.upload_file(filename, filename)
        elif command[0] == 'mail-to' and recv[0:19] == 'Sent successfully.\n':
            print('Sent successfully.\n', end = '')
            the_id = recv.split('\n')
            tmp = msg.find('--content')
            the_content = msg[tmp+10:]
            the_content = fix_content(the_content) 
            r_name = command[1]     #receiver name
            filename = r_name + '_mail' + the_id[1] + '.txt'
            bucket_name = prefix + r_name.lower()
            target_bucket = s3.Bucket(bucket_name)
            fp = open(filename, 'w')
            fp.write(the_content)
            fp.close()
            target_bucket.upload_file(filename, filename)
        elif command[0] == 'retr-mail' and recv[0:7] == 'Subject':
            part = recv.split('UNI_ID')
            print(part[0], end = '')
            filename = login_name + '_mail'+ part[1] +'.txt'
            target_object = login_bucket.Object(filename)
            object_content = target_object.get()['Body'].read().decode()
            print(object_content, end = '')
        elif command[0] == 'delete-mail' and recv[0:14] == 'Mail deleted.\n':
            part = recv.split('\n')
            print(part[0])
            filename = login_name + '_mail' + part[1] + '.txt'
            target_object = login_bucket.Object(filename)
            target_object.delete()
        else:
            print(recv, end = '')
        
client.close()
print('Bye!!!!!!')
