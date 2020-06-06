#!/bin/bash

SERVER_IP=$1
SERVER_PORT=$2
SESSION="test"
SLEEP_TIME=2
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

sleep 2.5

echo "Connection..."
for i in $(seq 0 3)
do
	tmux send-keys -t ${i} "python3 client.py ${SERVER_IP} ${SERVER_PORT}" Enter
	sleep $SLEEP_TIME
done

echo "Begin sub test"

tmux send-keys -t 1 "register Paul paul@cs.nctu.edu.tw 12345" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "register Brad brad@cs.nctu.edu.tw 12345" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "register Gary gary@cs.nctu.edu.tw 12345" Enter
sleep $SLEEP_TIME

tmux send-keys -t 1 "login Paul 12345" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "login Brad 12345" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "login Gary 12345" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "subscribe --board HW4_Board --keyword Project" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "subscribe --author Jason --keyword hw4" Enter
sleep $SLEEP_TIME

tmux send-keys -t 1 "create-board HW4_Board" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "subscribe --board HW4_Board --keyword Project" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "subscribe --author Paul --keyword HW" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "subscribe --author Paul --keyword post" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "subscribe --author Bryant --keyword exam" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "unsubscribe --author Brad" Enter
sleep $SLEEP_TIME

tmux send-keys -t 1 "create-post HW4_Board --title About Project --content HW4…" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "list-board ##HW" Enter
sleep $SLEEP_TIME

tmux send-keys -t 1 "create-post HW4_Board --title About HW and Exam --content blablabla" Enter
sleep $SLEEP_TIME

tmux send-keys -t 2 "unsubscribe --author Bryant" Enter
sleep $SLEEP_TIME

tmux send-keys -t 3 "list-sub" Enter
sleep $SLEEP_TIME

echo "Show result"
sleep 3
tmux attach-session -t $SESSION