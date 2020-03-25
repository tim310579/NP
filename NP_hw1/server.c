#include <stdio.h>  
#include <stdlib.h>  
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/select.h>
#define BACKLOG 5     //完成三次握手但没有accept的队列的长度  
#define CONCURRENT_MAX 20   //应用层同时可以处理的连接  
#define SERVER_PORT 11332  
#define BUFFER_SIZE 1024  
#define QUIT_CMD ".quit"  
typedef struct Data Data;
struct Data{
	int regis, login;
	char name[100], email[100], password[100];
};
int client_fds[CONCURRENT_MAX];  
int main(int argc, const char * argv[])  
{  
	struct Data database[20];
	int i, acc_num = 0;
	
	for(i = 0; i < 20; i++){
		//Data* database[7] = (Data*)malloc(sizeof(Data));
		database[i].regis = 0;
		database[i].login = 0;
		strcpy(database[i].name, "");
		strcpy(database[i].email, "");
		strcpy(database[i].password, "");
	}
    char input_msg[BUFFER_SIZE];  
    char recv_msg[BUFFER_SIZE];  
    //本地地址  
    struct sockaddr_in server_addr;  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(atoi(argv[1]));  
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  
    bzero(&(server_addr.sin_zero), 8);  
    //创建socket  
    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);  
    if(server_sock_fd == -1)  
    {  
        perror("socket error");  
        return 1;  
    }  
    //绑定socket  
    int bind_result = bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));  
    if(bind_result == -1)  
    {  
        perror("bind error");  
        return 1;  
    }  
    //listen  
    if(listen(server_sock_fd, BACKLOG) == -1)  
    {  
        perror("listen error");  
        return 1;  
    }  
    //fd_set  
    fd_set server_fd_set;  
    int max_fd = -1;  
    struct timeval tv;  //超时时间设置  
    while(1)  
    {  
	for(int u = 0; u < 20; u++){
		send(client_fds[u], "% ", 2, 0);
	}
        tv.tv_sec = 20;  
        tv.tv_usec = 0;  
        FD_ZERO(&server_fd_set);  
        FD_SET(STDIN_FILENO, &server_fd_set);  
        if(max_fd <STDIN_FILENO)  
        {  
            max_fd = STDIN_FILENO;  
        }  
        //printf("STDIN_FILENO=%d\n", STDIN_FILENO);  
    //服务器端socket  
        FD_SET(server_sock_fd, &server_fd_set);  
       // printf("server_sock_fd=%d\n", server_sock_fd);  
        if(max_fd < server_sock_fd)  
        {  
            max_fd = server_sock_fd;  
        }  
    //客户端连接  
        for(int i =0; i < CONCURRENT_MAX; i++)  
        {  
            //printf("client_fds[%d]=%d\n", i, client_fds[i]);  
            if(client_fds[i] != 0)  
            {  
                FD_SET(client_fds[i], &server_fd_set);  
                if(max_fd < client_fds[i])  
                {  
                    max_fd = client_fds[i];  
                }  
            }  
        }   
	int ret = select(max_fd + 1, &server_fd_set, NULL, NULL, &tv);
        if(ret < 0)
        {
            //perror("select 出错\n");
            continue;
        }
        else if(ret == 0)
        {
            //printf("select 超时\n");
            continue;
        }
	else{
            //ret 为未状态发生变化的文件描述符的个数  
            if(FD_ISSET(STDIN_FILENO, &server_fd_set))  
            {  
                //printf("发送消息：\n");  
                bzero(input_msg, BUFFER_SIZE);  
                fgets(input_msg, BUFFER_SIZE, stdin);  
                //输入“.quit"则退出服务器  
		printf("input message %s iqiqiqi\n", input_msg);
                if(strcmp(input_msg, "quit\n") == 0)  
                { 
		       printf("8787");	
                    exit(0);  
                }  
                for(int i = 0; i < CONCURRENT_MAX; i++)  
                {  
                    if(client_fds[i] != 0)  
                    {  
                        printf("client_fds[%d]=%d\n", i, client_fds[i]);  
                        send(client_fds[i], input_msg, BUFFER_SIZE, 0);  
                    }  
                }  
            }  
            if(FD_ISSET(server_sock_fd, &server_fd_set))  
            {  
                //有新的连接请求  
                struct sockaddr_in client_address;  
                socklen_t address_len;  
                int client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_address, &address_len);  
                printf("new connection client_sock_fd = %d\n", client_sock_fd);  
                if(client_sock_fd > 0)  
                {  
                    int index = -1;  
                    for(int i = 0; i < CONCURRENT_MAX; i++)  
                    {  
                        if(client_fds[i] == 0)  
                        {  
                            index = i;  
                            client_fds[i] = client_sock_fd;  
                            break;  
                        }  
                    }  
                    if(index >= 0)  
                    {  
			    //success
                        //printf("新客户端(%d)加入成功 %s:%d\n", index, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                    }  
                    else  
                    {  
                        bzero(input_msg, BUFFER_SIZE);  
                        strcpy(input_msg, "服务器加入的客户端数达到最大值,无法加入!\n");  
                        send(client_sock_fd, input_msg, BUFFER_SIZE, 0);  
                        printf("客户端连接数达到最大值，新客户端加入失败 %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                    }  
                }  
            }  
            for(int i = 0; i < CONCURRENT_MAX; i++)  
            {  
                if(client_fds[i] !=0)  
                {  
                    if(FD_ISSET(client_fds[i], &server_fd_set))  
                    {  
                        //处理某个客户端过来的消息  
                        bzero(recv_msg, BUFFER_SIZE);  
                        long byte_num = recv(client_fds[i], recv_msg, BUFFER_SIZE, 0);  
                        if (byte_num > 0)  
                        {  
                            if(byte_num > BUFFER_SIZE)  
                            {  
                                byte_num = BUFFER_SIZE;  
                            }  
                            //recv_msg[byte_num] = '\0';  
                            //printf("客户端(%d):%s", i, recv_msg);
                            /*转发数据back*/                         
                            //for(int i = 0; i < CONCURRENT_MAX; i++)  
                            //{ 
			    if(!strncmp(recv_msg, "show", 4)){
				for(int k = 0; k < acc_num; k++){
					printf("%d  %s  %s  %s\n", database[k].regis, database[k].name, database[k].email, database[k].password);
				}
			    }
			    if(!strncmp(recv_msg, "register", 8)){
				printf("begin %s\n", recv_msg);
				char *delim = " ";
				char *pch;
				pch = strtok(recv_msg, delim);
				int tmp = 0;
				char tmp0[100]="", tmp1[100]="", tmp2[100]="", tmp3[100]="";
				//strcpy(tmp_str[tmp], pch);
				//tmp_str[0] = strtok(recv_msg, delim);
				
				while(pch != NULL){
					
					//printf("%s\n", pch);
					//pch = strtok(NULL, delim);
					//sprintf(tmp_str[tmp], pch);
					//if(tmp > 0) tmp_str[tmp] = strtok(tmp_str[tmp-1], delim);
					if(tmp >= 4) break;
					switch(tmp){
						case 0:
							strcpy(tmp0, pch);
						case 1:
							strcpy(tmp1, pch);
						case 2:
							strcpy(tmp2, pch);
						case 3:
							strcpy(tmp3, pch);
					}	
					tmp++;
					pch = strtok(NULL, delim);
					//tmp++;
				}
				    printf("%d num\n", tmp);
				    int reg_yn = 1;
				    char error1[200] = "Usage: register <username> <email> <password>\n";
				    if(tmp != 4) {
					    send(client_fds[i], error1, sizeof(error1), 0);
				            reg_yn = 0;
				    }char error2[200] = "Username or Email is already used\n";
				    for(int j = 0; j < acc_num; j++){
					if(!strcmp(tmp1, database[j].name) || !strcmp(tmp2, database[j].email)){
						send(client_fds[i], error2, sizeof(error2), 0);
						reg_yn = 0;
					}
				    }
				    if(reg_yn == 1){
				    	database[acc_num].regis = 1;
					strcpy(database[acc_num].name, tmp1);
					strcpy(database[acc_num].email, tmp2);
					strcpy(database[acc_num].password, tmp3);
					acc_num ++;
				    }
				    
			    } 
                                if(client_fds[i] != 0)  
                                {   
                                    //send(client_fds[i], recv_msg, sizeof(recv_msg), 0);  
                                }  
                            //}    
                            /*结束转发内容*/
                        }  
                        else if(byte_num < 0)  
                        {  
                            printf("从客户端(%d)接受消息出错.\n", i);  
                        }  
                        else  
                        {  
                            FD_CLR(client_fds[i], &server_fd_set);  
                            client_fds[i] = 0;  
                            printf("客户端(%d)退出了\n", i);  
                        }  
                    }  
                }  
            }  
    	}   
    }  
    return 0;  
} 
