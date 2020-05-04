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

tmux send-keys "register Brad bb@cs.nctu.edu.tw 12345" Enter
sleep $SLEEP_TIME

tmux send-keys "register Brad bb@cs.nctu.edu.tw 12345" Enter
sleep $SLEEP_TIME

tmux send-keys "register V v@cs.nctu.edu.tw bt21" Enter
sleep $SLEEP_TIME

tmux send-keys "login Brad 12345" Enter
sleep $SLEEP_TIME

tmux send-keys "whoami" Enter 
sleep $SLEEP_TIME

tmux send-keys "create-board NP_HW" Enter
sleep $SLEEP_TIME

tmux send-keys "create-board NP_HW" Enter
sleep $SLEEP_TIME

tmux send-keys "list-board" Enter
sleep $SLEEP_TIME

tmux send-keys "list-board ##HW" Enter
sleep $SLEEP_TIME

tmux send-keys "create-post NP_HW --title NP_HW3 --content Err...<br>Ha!" Enter
sleep $SLEEP_TIME

tmux send-keys "create-post NCTU --title NP_HW3 --content Uh..." Enter
sleep $SLEEP_TIME

tmux send-keys "create-post NP_HW --title NP_HW4 --content Wow..." Enter
sleep $SLEEP_TIME

tmux send-keys "list-post NP" Enter
sleep $SLEEP_TIME

tmux send-keys "list-post NP_HW" Enter
sleep $SLEEP_TIME

tmux send-keys "list-post NP_HW ##HW3" Enter
sleep $SLEEP_TIME

tmux send-keys "read 888" Enter
sleep $SLEEP_TIME

tmux send-keys "read 1" Enter
sleep $SLEEP_TIME

tmux send-keys "update-post 888 --title NP HW_4" Enter
sleep $SLEEP_TIME

tmux send-keys "update-post 1 --title NP HW_4" Enter
sleep $SLEEP_TIME

tmux send-keys "read 1" Enter
sleep $SLEEP_TIME

tmux send-keys "update-post 1 --content Yeah!" Enter
sleep $SLEEP_TIME

tmux send-keys "read 1" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "whoami" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "login V bt21" Enter
sleep $SLEEP_TIME

tmux send-keys "create-post NP_HW --title Hello --content I am<br><br>V." Enter
sleep $SLEEP_TIME

tmux send-keys "update-post 1 --content Ha!<br>ha!" Enter
sleep $SLEEP_TIME

tmux send-keys "delete-post 1" Enter
sleep $SLEEP_TIME

tmux send-keys "comment 888 Ha ha!" Enter
sleep $SLEEP_TIME

tmux send-keys "comment 1 Ha ha!" Enter
sleep $SLEEP_TIME

tmux send-keys "read 1" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to TT --subject Hi TT --content Hi <br> TT!" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to Brad --subject Hi Brad --content Hi <br> Brad! " Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to Brad --subject Hey --content Hey <br>Brad!" Enter
sleep $SLEEP_TIME

tmux send-keys "list-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "delete-mail 1" Enter
sleep $SLEEP_TIME

tmux send-keys "retr-mail 1" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "login Brad 12345" Enter
sleep $SLEEP_TIME

tmux send-keys "list-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "retr-mail 1" Enter
sleep $SLEEP_TIME

tmux send-keys "mail-to V --subject Hi V --content WoW V!" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "login V bt21" Enter
sleep $SLEEP_TIME

tmux send-keys "list-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME


tmux send-keys "login Brad 12345 " Enter
sleep $SLEEP_TIME

tmux send-keys "list-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "delete-mail 1" Enter
sleep $SLEEP_TIME

tmux send-keys "list-mail" Enter
sleep $SLEEP_TIME

tmux send-keys "logout" Enter
sleep $SLEEP_TIME

tmux send-keys "exit" Enter
sleep $SLEEP_TIME

echo "Show result"
sleep 3
tmux attach-session -t $SESSION

