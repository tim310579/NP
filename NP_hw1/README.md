# Introduce

### server端

` gcc -o server server.c -lpthread`


` ./server 1234`

(1234 can be any number)

當有新的client連線時，會顯示"New connection"

### client端

` telnet 127.0.0.1 1234`

連線成功時，會顯示如下

` ********************************`     
` ** Welcome to the BBS server. **`     
` ********************************`     


#### example:

`ls`    
(see all database)

`register a aaa 111`

`register b bbb 222`

` adddata`    
(can add many data)

` login a 111`

` whoami`

` logout`

` exit`
