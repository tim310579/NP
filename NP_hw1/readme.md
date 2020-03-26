testcode

server端
-----------------------------------
//compile

gcc -o server server.c -lpthread

//execute

./server 1234   //can be any number

-------------------------------------
client端

telnet 127.0.0.1 1234

