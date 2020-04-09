#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define STDIN 0
#define MAX_QUE 20
#define BUFFER_SIZE 1024
#define ERR1 "Usage: register <username> <email> <password>\n"
#define ERR2 "Username or Email is already used\n"
#define ERR3 "Usage: login <username> <password>\n"
#define ERR4 "Login failed.\n"
#define ERR5 "Please logout first.\n"
#define ERR6 "Please login first.\n"
#define ERR7 "Wrong Command.\n"
#define ERR8 "Board is already exist.\n"
#define ERR9 "Board is not exist.\n"
#define ERR10 "Usage: create-board <name>\n"
#define ERR11 "Usage: create-post <board-name> --title <title> --content <content>\n"


#define SUC0 "********************************\n** Welcome to the BBS server. **\n********************************\n"
#define SUC1 "Register successfully.\n"
#define SUC2 "Welcome, "
#define SUC3 "Bye, "
#define SUC4 "Create board successfully.\n"

void* conn(void *arg);
void fix_endline(char fixed[]){
	char temp[100] = "";
	int flag = 0;
        for(int l = 0; l < 100; l++){
        	if(fixed[l] == '\n' || fixed[l] == ' '){
     	   		strncpy(temp, fixed, l-1);
			flag = 1;
			break;
                }
        }
	if(flag == 0) strcpy(temp, fixed);
	strcpy(fixed, temp);
	//return fixed;
}
void substr(char *dest, char *src, int start, int cnt){
	strncpy(dest, src + start, cnt);
	dest[cnt] = 0;
}

typedef struct Data Data;
typedef struct Board Borad;

struct Data{
	int regis, login;
	char name[100], email[100], password[100];
};
struct Data database[20];

struct Board{
	char name[100], title[1024], content[1024], moderator[100];
	int num;
};
struct Board allboard[51];

int acc_num = 0;
int acc_board = 0;

int main(int argc, const char * argv[])
{
	
//	struct Data database[20];
	pthread_t t[20];
	void *ret[20];
	
	
	int j;


	for(j = 0; j < 20; j++){
		//Data* database[7] = (Data*)malloc(sizeof(Data));
		database[j].regis = 0;
		database[j].login = 0;
		strcpy(database[j].name, "");
		strcpy(database[j].email, "");
		strcpy(database[j].password, "");
	}
	for(j = 0; j < 51; j++){
		allboard[j].num = 0;
		strcpy(allboard[j].name, "");
		strcpy(allboard[j].title, "");
		strcpy(allboard[j].content, "");
		strcpy(allboard[j].moderator, "");
	}

	char recv_msg[BUFFER_SIZE];  
 	char input_msg[BUFFER_SIZE];   
	struct sockaddr_in local_sockaddr;
    	int socketfd;	//socket
    	int clientfd;	//client
    	pid_t pid;
    if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0)//build socket link
    {
        perror("socket");
        //exit(1);
    }
    //printf("Socket id = %d\n",socketfd);
    local_sockaddr.sin_family = AF_INET;
    local_sockaddr.sin_port    = htons(atoi(argv[1]));
    //local_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //printf("server ip =%s\n",inet_ntoa(local_sockaddr.sin_addr));	//ip
    memset(local_sockaddr.sin_zero,0,8);
    int i = 1;
    signal(SIGPIPE, SIG_IGN);
    //setsockopt(socketfd,SOL_SOCKET,SO_NOSIGPIPE,&i,sizeof(i));
    setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));
    if(bind(socketfd,(struct sockaddr *)&local_sockaddr,sizeof(struct sockaddr))<0)
    {
        perror("bind");
        //exit(1);
    }
    //printf("Bind success!\n");
    if(listen(socketfd,MAX_QUE)<0)	//listen passively
    {
        //perror("listen");
        //exit(1);
    }
    //printf("%d listen\n", listen(socketfd,MAX_QUE));
    int c_num = 0;

    while(1){
        
	//printf("%d\n", clientfd);
	    if((clientfd=accept(socketfd,(struct sockaddr*)NULL,NULL)) < 0)	//accept(), wait for client
        {
		//bzero(input_msg, BUFFER_SIZE);  
                //fgets(input_msg, BUFFER_SIZE, stdin);
		//if(!strncmp(input_msg, "quit", 4)) exit(0);
		//printf("herre  %s||\n", input_msg);
		//recv(clientfd, recv_msg, BUFFER_SIZE, 0);
		//send(clientfd, recv_msg, sizeof(recv_msg), 0);
            perror("accept");
            //exit(1);
        }
        printf("New connection.\n");
	pthread_create(&t[c_num], NULL, conn, &clientfd);
	c_num ++;
	
	//printf("%d listen\n", listen(socketfd,MAX_QUE));
		
		
			//send(clientfd, recv_msg, sizeof(recv_msg), 0);
			//close(socketfd);//关闭socket连接
			
                  struct sockaddr_in server, client;
            char server_ip[20];
                char client_ip[20];
            socklen_t server_len = sizeof(server);
                socklen_t client_len = sizeof(client);
                getpeername(clientfd, (struct sockaddr *)&client, &client_len);
                inet_ntop(AF_INET, &client.sin_addr, client_ip, sizeof(client_ip));
            //printf("client ip:%s\tport:%d\n", client_ip, ntohs(client.sin_port));
            //close(clientfd);
            //exit(0);
	    
//		}
  //      }
        //close(clientfd);
    }
}
void* conn(void *arg){
	pthread_detach(pthread_self());

	int fd = *(int*)arg;

	int login_yn = 0, login_num = 0;
	char login_name[100];
	//int fd = (int) arg;
	//printf("%d num\n", fd);
	char recv_msg[BUFFER_SIZE];
	//bzero(recv_msg, BUFFER_SIZE);
	//long byte_num = recv(fd, recv_msg, BUFFER_SIZE, 0); 
	send(fd, SUC0, sizeof(SUC0), 0);
	while(1){
                send(fd, "% ", 2, 0);
                //printf("%d\n\n", clientfd);
                bzero(recv_msg, BUFFER_SIZE);
                long tmpp = recv(fd, recv_msg, BUFFER_SIZE, 0);
	if(tmpp > 0){
                if(!strncmp(recv_msg, "ls", 2)){
                        printf("------------------------------------------------------------\n");
                        for(int k = 0; k < acc_num; k++){
                                printf("%d  %d  %s  %s  %s||\n", database[k].regis, database[k].login, database[k].name, database[k].email, database[k].password);
                        }
			printf("------------------------------------------------------------\n");
                }
		else if(!strncmp(recv_msg, "register", 8)){
			//printf("begin %s\n", recv_msg);
			char *delim = " ";
			char *pch;
			pch = strtok(recv_msg, delim);
			int tmp = 0;
			char tmp0[100] = "", tmp1[100] = "", tmp2[100] = "", tmp3[100] = "";
			while(pch != NULL){
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
				if(tmp >= 4) break;
				pch = strtok(NULL, delim);
				//tmp++;
			}
			int flag = 0;
			char temp[100] = "";
			for(int l = 0; l < 100; l++){
				if(tmp3[l] == '\n'){
					strncpy(temp, tmp3, l-1);	//delete '\n'
					flag = 1;
					break;
					//tmp3[l] = '\0';
					//del(tmp3, l, 1);
					//printf("here %c||\n", tmp3[l]);
				}
			}
			if(flag == 0) strcpy(temp, tmp3);
			strcpy(tmp3, temp);
			//printf("||%s||%s||\n", temp, tmp3);
			int reg_yn = 1;
		    	if(tmp != 4) {
				send(fd, ERR1, sizeof(ERR1), 0);
		            	reg_yn = 0;
			}
			for(int j = 0; j < acc_num; j++){
				if(!strcmp(tmp1, database[j].name) || !strcmp(tmp2, database[j].email)){
					send(fd, ERR2, sizeof(ERR2), 0);
					reg_yn = 0;
				}
			}
			if(reg_yn == 1){
			   	database[acc_num].regis = 1;
				strcpy(database[acc_num].name, tmp1);
				strcpy(database[acc_num].email, tmp2);
				strcpy(database[acc_num].password, tmp3);
				send(fd, SUC1, sizeof(SUC1), 0);
				acc_num ++;
			}
				    
		}
		else if(!strncmp(recv_msg, "login", 5)){
			char *delim = " ";
			char *pch;
			pch = strtok(recv_msg, delim);
			int tmp = 0;
			char tmp0[100]="", tmp1[100]="", tmp2[100]="";
			while(pch != NULL){
				if(tmp >= 3) break;
				switch(tmp){
					case 0:
						strcpy(tmp0, pch);
					case 1:
						strcpy(tmp1, pch);
					case 2:
						strcpy(tmp2, pch);
				}
				tmp++;
				if(tmp >= 3) break;
				pch = strtok(NULL, delim);
			}
			//if(tmp2[sizeof(tmp2)] == '\n'){ //last one is '\n'
                          //      tmp2[sizeof(tmp2)] = '\0';
                        //}
			char temp[100] = "";
			int flag = 0;
                        for(int l = 0; l < 100; l++){
                                if(tmp2[l] == '\n'){
                                        strncpy(temp, tmp2, l-1);
					flag = 1;
					break;
					//del(tmp2, l, 1);
                                        //printf("here %c||\n", tmp3[l]);
                                }
                        }
			if(flag == 0) strcpy(temp, tmp2);
			strcpy(tmp2, temp);
			//printf("%s||%s||\n", temp, tmp2);
			int log_yn = 1;
			if(tmp != 3){
				send(fd, ERR3, sizeof(ERR3), 0);
			        log_yn = 0;
			}
			else{
				int find = 0;
				if(login_yn == 1){	//already login, logout first
					send(fd, ERR5, sizeof(ERR5), 0);
				}
				else{
					for(int j = 0; j < acc_num; j++){
						if(!strcmp(tmp1, database[j].name)){	//find name
							find = 1;
							if(!strcmp(tmp2, database[j].password)){
								char suc[100] = "Welcome, ";
								strcat(suc, tmp1);
								strcat(suc, ".\n");
								send(fd, suc, sizeof(suc), 0);
								database[j].login = 1;
								login_yn = 1;
								strcpy(login_name, tmp1);
								login_num = j;
								//right password
							}
							else{
								log_yn = 0;
								//send(fd, ERR4, sizeof(ERR4), 0);
								//wrong password
							}
						}
					}
					if(find = 0 || login_yn == 0){	//wrong name or password
						send(fd, ERR4, sizeof(ERR4), 0);
					}
				}
			}
		}
		else if(!strncmp(recv_msg, "logout", 6)){
			if(login_yn == 0){	//login first
				send(fd, ERR6, sizeof(ERR6), 0);
			}
			else{	//cam logout
				char suc[100] = "Bye, ";
				strcat(suc, login_name);
				strcat(suc, ".\n");
				send(fd, suc, sizeof(suc), 0);
				login_yn = 0;
				database[login_num].login = 0;
			}
		}
		else if(!strncmp(recv_msg, "whoami", 6)){
			if(login_yn == 0){	//login first
				send(fd, ERR6, sizeof(ERR6), 0);
			}
			else{
				char tmp[100] = "";
				strcpy(tmp, login_name);
				strcat(tmp, "\n");
				send(fd, tmp, sizeof(tmp), 0);
			}
		}
		else if(!strncmp(recv_msg, "exit", 4)){
			close(fd);
			//exit(0);
		}
		else if(!strncmp(recv_msg, "create-board", 12)){
			if(login_yn == 0){
				send(fd, ERR6, sizeof(ERR6), 0);
			}
			else{
				if(!strncmp(recv_msg, "create-board ", 13)){
					int tmp = 0;
					char tmp0[100] = "", tmp1[100] = "";
					strncpy(tmp0, recv_msg, 12);
					strcpy(tmp1, recv_msg+13);
					fix_endline(tmp1);
					int repeat_name = 0;
					for(int l = 0; l < acc_board; l++){//check repeated name
						if(!strcmp(tmp1, allboard[l].name)){
							repeat_name = 1;
						}
					}
					if(!strcmp(tmp1, "")){
						send(fd, ERR10, sizeof(ERR10), 0);
					}
					else if(repeat_name == 1){
						send(fd, ERR8, sizeof(ERR8), 0);
					}
					else if(repeat_name == 0){
						acc_board ++;
						strcpy(allboard[acc_board].name, tmp1);
						allboard[acc_board].num = acc_board;
						strcpy(allboard[acc_board].moderator, login_name);
						send(fd, SUC4, sizeof(SUC4), 0);
					}	
				}
				else{
					send(fd, ERR10, sizeof(ERR10), 0);
				}	
			}
		}
		else if(!strncmp(recv_msg, "list-board ##", 13)){
			char search[100];
			strncpy(search, recv_msg + 13, 100);
			fix_endline(search);
			char send0[100] = "";
			sprintf(send0, "    Index      Name       Moderator\n");
			send(fd, send0, sizeof(send0), 0);
			for(int l = 1; l <= acc_board; l++){
				char send_msg[1024] = "";
				if(strstr(allboard[l].name, search) != NULL || strstr(allboard[l].moderator, search) != NULL){
					sprintf(send_msg, "    %-11d%-11s%-11s\n", allboard[l].num, allboard[l].name, allboard[l].moderator);
					send(fd, send_msg, sizeof(send_msg), 0);
				}
			}
		}
		else if(!strncmp(recv_msg, "list-board", 10)){
			char send0[100] = "";
			sprintf(send0, "    Index      Name       Moderator\n");
                        send(fd, send0, sizeof(send0), 0);
			for(int l = 1; l <= acc_board; l++){
				char send_msg[1024] = "";
				sprintf(send_msg, "    %-11d%-11s%-11s\n", allboard[l].num, allboard[l].name, allboard[l].moderator);
				send(fd, send_msg, sizeof(send_msg), 0);
			}
		}
		else if(!strncmp(recv_msg, "create-post", 11)){
			if(login_yn == 0){
				send(fd, ERR6, sizeof(ERR6), 0);
			}
			else{
				if(!strncmp(recv_msg, "create-post ", 12)){
					char *name = strdup(recv_msg + 12);
					char *title = strstr(recv_msg, "--title");
					int len = strlen(name)- strlen(title);
					char newname[100];
					substr(newname, name, 0, len-1);
					printf("%slll\n", newname);
				}

			}	
		}
		else if(!strncmp(recv_msg, "adddata", 7)){
			
			for(int h = acc_num; h < acc_num + 10; h++){
				char temp[10] = "a";
				database[h].regis = 1;
				database[h].login = 0;
				char tmp[10] = "";
				sprintf(tmp, "%d", h);
				strcat(temp, tmp);
				strcpy(database[h].name, temp);
				strcat(temp, tmp);
				strcpy(database[h].email, temp);
				strcpy(database[h].password, tmp);
			}
			acc_num += 10;
		}
		else{
			//signal(SIGPIPE,SIG_IGN);
			send(fd, ERR7, sizeof(ERR7), 0);
		}
	}
	}
}
