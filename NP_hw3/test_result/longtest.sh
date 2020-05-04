#!/bin/bash

SERVER_IP=$1
SERVER_PORT=$2
SESSION="np_demo"
SLEEP_TIME=1
if [ -z ${SERVER_IP} ] || [ -z ${SERVER_PORT} ]; then
    echo "Usage: $0 <server ip> <server port>"
    exit 1
fi

if [ -n "`tmux ls | grep ${SESSION}`" ]; then
  tmux kill-session -t $SESSION
fi

tmux new-session -d -s $SESSION
tmux set remain-on-exit on

tmux select-pane -t 0
tmux split-window -v
tmux split-window -h

tmux select-pane -t 0
tmux split-window -h

# cat testcase | 
# while IFS= read data 
# do
#     tmux send-keys -t 0 "$data" Enter
#     sleep 1
# done

sleep 2.5
echo "Connection test."
for i in $(seq 0 3)
do
	tmux send-keys -t ${i} "python3 client.py ${SERVER_IP} ${SERVER_PORT}" Enter
	sleep $SLEEP_TIME
done

echo "Registeration test"
for i in $(seq 0 3)
do
	tmux send-keys -t ${i} "create-board NP_HW${i}" Enter
	#Please login first. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "register user${i} user${i}@qwer.zxcv user${i}" Enter
   	sleep $SLEEP_TIME
done

echo "Login test"
index=0
for tc in qwer asdf zxcv qwer 
do
    # types wrong account and password
    tmux send-keys -t ${index} "login ${tc} ${tc}" Enter 
    sleep $SLEEP_TIME
    # show login first
    tmux send-keys -t ${index} "whoami" Enter 
    sleep $SLEEP_TIME
    # show login first
    tmux send-keys -t ${index} "logout" Enter 
    sleep $SLEEP_TIME

    # types correct account and password
    tmux send-keys -t ${index} "login user${index} user${index}" Enter 
    sleep $SLEEP_TIME
    # show logout first
    tmux send-keys -t ${index} "login user${index} user${index}" Enter 
    sleep $SLEEP_TIME
    # show username
    tmux send-keys -t ${index} "whoami" Enter 
    sleep $SLEEP_TIME
    let "index++"
done

echo "Board test"
for i in $(seq 0 3)
do
	tmux send-keys -t ${i} "create-board NP_HW${i}" Enter
	#Create board successfully. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "create-board NP_HW${i}" Enter
	#Board is already exist. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "create-board OS_HW${i}" Enter
	#Create board successfully. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "create-board FF${i}" Enter
	#Create board successfully. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-board" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-board ##HW" Enter
	sleep $SLEEP_TIME
done

echo "Post test"
for i in $(seq 0 3)
do
	tmux send-keys -t ${i} "create-post NCTU${i} --title About NP HW_2 --content Help!<br>I have some problem!" Enter
	#Board is not exist. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "create-post NP_HW${i} --title About NP HW_2 --content Help!<br>I have some problem!" Enter
	#Create post successfully. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "create-post NP_HW${i} --title HW_3 --content Ask!<br>Is NP HW_3 Released?" Enter
	#Create post successfully. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-post NP${i}" Enter
	#Board is not exist. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-post NP_HW${i}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-post NP_HW${i} ##HW_2" Enter
	# ID	Title           Author          Date
	# 1     About NP HW2    Bob             04/14
	sleep $SLEEP_TIME
done
echo "Read test"
for i in $(seq 0 3)
do
	let "index=i*2+1"
	tmux send-keys -t ${i} "read 888" Enter
	#Post is not exist.
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "read ${index}" Enter
	sleep $SLEEP_TIME
	let "index=i*2+2"
	tmux send-keys -t ${i} "read ${index}" Enter
done
echo "Update-post test"
for i in $(seq 0 3)
do
	tmux send-keys -t ${i} "update-post 888 --title NP HW_2" Enter
	#Post is not exist. 
	sleep $SLEEP_TIME
	let "index=i*2+1"
	tmux send-keys -t ${i} "whoami" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "update-post ${index} --title NP HW_2" Enter
	#Update successfully. 
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "read ${index}" Enter
	sleep $SLEEP_TIME

	let "index=(i*2+3)%8"
	tmux send-keys -t ${i} "update-post ${index} --title HA HA HA I change it" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-all-post" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "read ${index}" Enter
	sleep $SLEEP_TIME
done

echo "Switch user"

index2=0
index3=0
for i in $(seq 0 3)
do
	let "index=(i+1)%4"
	let "index2=i*2+1"
	let "index3=index*2+1"
	tmux send-keys -t ${index} "logout" Enter
	#Bye, Bob.
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "login user${i} user${i}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "whoami" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "update-post ${index2} --content Ha!<br>ha!<br>ha!" Enter	
	sleep $SLEEP_TIME
	
	tmux send-keys -t ${index} "comment 888 Ha! ha! ha!" Enter
	#Post is not exist.
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "comment ${index2} Ha! ha! ha!" Enter
	#Comment successfully.
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "comment ${index3} Ha! ha! ha!" Enter
	#Comment successfully.
	sleep $SLEEP_TIME

	tmux send-keys -t ${index} "read ${index2}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "read ${index3}" Enter
	sleep $SLEEP_TIME
	
	tmux send-keys -t ${index} "whoami" Enter
	sleep $SLEEP_TIME

	tmux send-keys -t ${index} "delete-post ${index2}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "delete-post ${index3}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "delete-post 8" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "list-all-post" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "read ${index2}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "read ${index3}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "create-board Hello${index}" Enter
	#Create board successfully.
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "list-board" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${index} "logout" Enter
	sleep $SLEEP_TIME
done

echo "Send mail test"
mail1=0
mail2=0
mail3=0
for i in $(seq 0 3)
do
	let "mail1=(i+1)%4"
	let "mail2=(i+2)%4"	
	let "mail3=(i+3)%4"
	tmux send-keys -t ${i} "login user${i} user${i}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-mail" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "delete-mail 1" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 1" Enter
	sleep $SLEEP_TIME
done

for i in $(seq 0 3)
do
        let "mail1=(i+1)%4"
        let "mail2=(i+2)%4"
        let "mail3=(i+3)%4"
	tmux send-keys -t ${i} "mail-to TT --subject Hi TT --content Hi <br> TT!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "mail-to user${mail1} --subject Hi user${mail1} --content Hi <br> user${mail1} HAHA!!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "mail-to user${mail1} --subject 87? user${mail1} --content 8787ha <br> user${mail1} HAHA!!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "mail-to user${mail2} --subject Hi user${mail2} --content Hi <br> user${mail2} HAHA!!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "mail-to user${mail2} --subject 87? user${mail2} --content 8787ha <br> user${mail2} HAHA!!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "mail-to user${mail3} --subject Hi user${mail3} --content Hi <br> user${mail3} HAHA!!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "mail-to user${mail3} --subject 87? user${mail3} --content 8787ha <br> user${mail3} HAHA!!" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "logout" Enter
	sleep $SLEEP_TIME
done

echo "List and retr mail test"
for i in $(seq 0 3)
do
	let "index=(i+1)%4"
	tmux send-keys -t ${i} "login user${index} user${index}" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "whoami" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-mail" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 188" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 1" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 2" Enter
	sleep $SLEEP_TIME	
	tmux send-keys -t ${i} "retr-mail 3" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 4" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "delete-mail 233" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "delete-mail 2" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "delete-mail 1" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "delete-mail 3" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-mail" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 1" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 2" Enter
	sleep $SLEEP_TIME	
	tmux send-keys -t ${i} "retr-mail 3" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "retr-mail 4" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "delete-mail 1" Enter
	sleep $SLEEP_TIME
	tmux send-keys -t ${i} "list-mail" Enter
	sleep $SLEEP_TIME	
	tmux send-keys -t ${i} "logout" Enter
	sleep $SLEEP_TIME	
	tmux send-keys -t ${i} "exit" Enter
	sleep $SLEEP_TIME	
done

echo "Show result"
sleep 3
tmux attach-session -t $SESSION
