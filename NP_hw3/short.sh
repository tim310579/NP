#!/bin/bash

SERVER_IP=$1
SERVER_PORT=$2
SESSION="test"
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

echo "Connection..."
tmux send-keys "python3 client.py ${SERVER_IP} ${SERVER_PORT}" Enter
sleep 1

tmux send-keys "register a0 0 0" Enter
sleep $SLEEP_TIME

tmux send-keys "register a1 1 1" Enter
sleep $SLEEP_TIME

tmux send-keys "login a0 0" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject 0487 --content HAHAHA!!<br>hehe" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject 51487 --content HAkak!!<br>hking" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject oqoq --content aaa" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject oaffafq --content aajjja" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject aaa --content attttt" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "login a1 1" Enter
sleep $SLEEP_TIME

tmux send-keys "list-all-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "delete-mail 2" Enter
sleep $SLEEP_TIME

tmux send-keys "list-all-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "delete-mail 3" Enter
sleep $SLEEP_TIME

tmux send-keys "retr-mail 1" Enter
sleep $SLEEP_TIME

tmux send-keys "retr-mail 2" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "login a0 0" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject HHHH --content AAA<br>al" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to a1 --subject oGGGGq --content PPP" Enter
sleep $SLEEP_TIME

echo "Show result"
sleep 3
tmux attach-session -t $SESSION

