#!/bin/bash

SERVER_IP=$1
SERVER_PORT=$2
SESSION="test"
SLEEP_TIME=1.5
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

echo "Connection..."
tmux send-keys "python3 client.py ${SERVER_IP} ${SERVER_PORT}" Enter
sleep $SLEEP_TIME

tmux send-keys "register a0 0 0" Enter
sleep $SLEEP_TIME

tmux send-keys "login a0 0" Enter
sleep $SLEEP_TIME

echo "Sub Board"
tmux send-keys "subscribe --board HAHA --keyword ooppp" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --board HAHA --keyword ooppp" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --board HeHe --keyword aaaaaa" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --board HeHe --keyword bbbbbb" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --board HeHe --keyword cccccc" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --board HeHe --keyword ddddd" Enter
sleep $SLEEP_TIME

tmux send-keys "list-sub" Enter
sleep $SLEEP_TIME

echo "Sub author"
tmux send-keys "subscribe --author Timm --keyword 11111" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --author Timm --keyword 22222" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --author Jack --keyword kkkkk" Enter
sleep $SLEEP_TIME

tmux send-keys "subscribe --author Jack --keyword ooooo" Enter
sleep $SLEEP_TIME

tmux send-keys "list-sub" Enter
sleep $SLEEP_TIME

echo "Unsubscribe test"
tmux send-keys "unsubscribe --board HAHA" Enter
sleep $SLEEP_TIME

tmux send-keys "list-sub" Enter
sleep $SLEEP_TIME

tmux send-keys "unsubscribe --author Jack" Enter
sleep $SLEEP_TIME

tmux send-keys "list-sub" Enter
sleep $SLEEP_TIME

tmux send-keys "exit" Enter
sleep $SLEEP_TIME

echo "Show result"
sleep 3
tmux attach-session -t $SESSION