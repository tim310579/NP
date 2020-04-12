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
#include <time.h>

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
#define ERR12 "Usage: list-post <board-name> ##<key>\n"
#define ERR13 "Post is not exist.\n"
#define ERR14 "Usage: read <post-id>\n"
#define ERR15 "Not the post owner.\n"
#define ERR16 "Usage: delete-post <post-id>\n"
#define ERR17 "No data now.\n"
#define ERR18 "Usage: update-post <post-id> --title/content <new>\n"
#define ERR19 "Usage: comment <post-id> <comment>\n"

#define SUC0 "********************************\n** Welcome to the BBS server. **\n********************************\n"
#define SUC1 "Register successfully.\n"
#define SUC2 "Welcome, "
#define SUC3 "Bye, "
#define SUC4 "Create board successfully.\n"
#define SUC5 "Create post successfully.\n"
#define SUC6 "Delete successfully.\n"
#define SUC7 "Store data success.\n"
#define SUC8 "Load data success.\n"
#define SUC9 "Update successfully.\n"
#define SUC10 "Comment successfully.\n"

void* conn(void *arg);

void fix_lines(char fixed[]){
	char temp[1000] = "";
        int flag = 0;
        for(int l = 0; l < 1000; l++){
                if(fixed[l] == '\n'){
                        strncpy(temp, fixed, l);
                        flag = 1;
                        break;
                }
        }

        if(flag == 0) strcpy(temp, fixed);
        strcpy(fixed, temp);
}
void fix_endline(char fixed[]){
	char temp[100] = "";
	int flag = 0;
        for(int l = 0; l < 100; l++){
        	if(fixed[l] == '\n' || fixed[l] == ' '){
     	   		if(fixed[l] == '\n') strncpy(temp, fixed, l-1);
			else strncpy(temp, fixed, l);
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

void strrpc(char str[], char oldstr[], char newstr[]){
	char bstr[strlen(str)];//buffer
	memset(bstr,0,sizeof(bstr));
	for(int i = 0;i < strlen(str);i++){
        	if(!strncmp(str+i,oldstr,strlen(oldstr))){//查詢目標字串
	            strcat(bstr,newstr);
        	    i += strlen(oldstr) - 1;
        	}
		else{
        	    strncat(bstr,str + i,1);//儲存一位元組進緩衝區
	    	}
    	}
	strcpy(str, bstr);
}

void fix_content(char fixed[]){
	char old[10] = "<br>";
	char new[10] = "\n     ";
	strrpc(fixed, old, new);
}

typedef struct Data Data;
typedef struct Board Borad;
typedef struct Post Post;

struct Data{
	int regis, login;
	char name[100], email[100], password[100];
};

struct Board{
	char name[100], moderator[100];
	int num;
	
};

struct Post{
	int id, exist, comment_cnt;
	char title[1024], author[100], datey[100], datem[100], bname[100], content[1024];
	char **comments;
};

struct Data database[100];
struct Board allboard[101];
struct Post posts[101];

int acc_num = 0;
int acc_board = 0;
int acc_post = 0;
int main(int argc, const char * argv[])
{
	
//	struct Data database[20];
	pthread_t t[20];
	void *ret[20];
	
	
	int j;


	for(j = 0; j < 100; j++){
		//database[j] = (Data*)malloc(sizeof(Data));
		database[j].regis = 0;
		database[j].login = 0;
		//database[j].name = (char*)malloc(sizeof(char)*100);
		strcpy(database[j].name, "");
		strcpy(database[j].email, "");
		strcpy(database[j].password, "");
	}
	for(j = 0; j < 101; j++){
		allboard[j].num = 0;
		strcpy(allboard[j].name, "");
		strcpy(allboard[j].moderator, "");

		posts[j].id = 0;
		posts[j].exist = 0;
		posts[j].comment_cnt = 0;
		strcpy(posts[j].title, "");
		strcpy(posts[j].author, "");
		strcpy(posts[j].datey, "");
		strcpy(posts[j].datem, "");
		strcpy(posts[j].bname, "");
		strcpy(posts[j].content, "");
		posts[j].comments = (char**)malloc(sizeof(char*)*101);
		for(int k = 0; k <= 100; k++){
			posts[j].comments[k] = (char*)malloc(sizeof(char)*100);
		}
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
						break;
					case 1:
						strcpy(tmp1, pch);
						break;
					case 2:
						strcpy(tmp2, pch);
						break;
					case 3:
						strcpy(tmp3, pch);
						break;
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
						break;
					case 1:
						strcpy(tmp1, pch);
						break;
					case 2:
						strcpy(tmp2, pch);
						break;
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
					for(int l = 0; l <= acc_board; l++){ //check repeated name
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
					if(strstr(recv_msg, " --title ") == NULL || strstr(recv_msg, " --content ") == NULL){
						send(fd, ERR11, sizeof(ERR11), 0);
					}
					else{
						char *name = strdup(recv_msg + 12);
						char *title = strstr(recv_msg, "--title");
						int len = strlen(name) - strlen(title);
						char bname[100];
						substr(bname, name, 0, len-1);
						
						int exist = 0;
					
						for(int l = 1; l <= acc_board; l++){
							if(!strcmp(bname, allboard[l].name)) exist = 1;
						}
						char title0[1024]; 
						strcpy(title0, title + 8);
						char *content = strstr(recv_msg, "--content");
						int len2 = strlen(title0) - strlen(content);
						char titlename[1024];
						substr(titlename, title0, 0, len2-1);

						char real_content[1024];
						strcpy(real_content, content + 10);
						//printf("%s\n%s\n%s\n", bname, titlename, real_content);


						if(exist == 0){	//not exist
							send(fd, ERR9, sizeof(ERR9), 0);
						}
						else{
							acc_post ++;
							posts[acc_post].id = acc_post;
							posts[acc_post].exist = 1;
							strcpy(posts[acc_post].bname, bname);
							strcpy(posts[acc_post].title, titlename);
							strcpy(posts[acc_post].author, login_name);
							fix_content(real_content);
							strcpy(posts[acc_post].content, real_content);
							time_t p;
							struct tm *tp;
							time(&p);
							tp = localtime(&p);
							char temp_time[100];
							sprintf(temp_time, "%04d-%02d-%02d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday);
							strcpy(posts[acc_post].datey, temp_time);

							sprintf(temp_time, "%02d/%02d", tp->tm_mon+1, tp->tm_mday);
							strcpy(posts[acc_post].datem, temp_time);


							send(fd, SUC5, sizeof(SUC5), 0);
						}
					}
				}
				else {
					send(fd, ERR11, sizeof(ERR11), 0);
				}
			}	
		}
		
		else if(!strncmp(recv_msg, "list-post", 9)){
			if(!strncmp(recv_msg, "list-post ", 10)){
				if(strstr(recv_msg, " ##") != NULL){
					char *tmp_bname = strdup(recv_msg + 10);
					char *key = strstr(recv_msg, "##");
					int len = strlen(tmp_bname) - strlen(key);
					char board_name[100];
					substr(board_name, tmp_bname, 0, len-1);
					char real_key[100];
					strcpy(real_key, key + 2);
					fix_endline(real_key);
					int exist = 0;
					for(int l = 1; l <= acc_board; l++){
						if(!strcmp(board_name, allboard[l].name)) exist = 1;
					}
					if(exist == 0){	//not exist
						send(fd, ERR9, sizeof(ERR9), 0);
					}
					else{
						char send0[100] = "";
						sprintf(send0, "    Id               Title            Author           Date\n");
						send(fd, send0, sizeof(send0), 0);
						for(int l = 0; l <= acc_post; l++){
							char send_msg[4000] = "";
							//printf("%s||%s||\n", board_name, posts[l].bname);
							if(!strcmp(board_name, posts[l].bname) && strstr(posts[l].title, real_key) != NULL && posts[l].exist > 0){	//bname is right && key matched && existed

								sprintf(send_msg, "    %-17d%-17s%-17s%-17s\n", posts[l].id, posts[l].title, posts[l].author, posts[l].datem);
								send(fd, send_msg, sizeof(send_msg), 0);
							}
						}
					}
				}
				else{
					char board_name[100];
					strcpy(board_name, recv_msg + 10);
					//printf("%s||\n", board_name);
					fix_endline(board_name);
					//printf("%s||\n", board_name);
					int exist = 0;
                                        for(int l = 1; l <= acc_board; l++){
                                                if(!strcmp(board_name, allboard[l].name)) exist = 1;
                                        }
                                        if(exist == 0){ //not exist
                                                send(fd, ERR9, sizeof(ERR9), 0);
                                        }
                                        else{
						char send0[100] = "";
						sprintf(send0, "    Id               Title            Author           Date\n");
						send(fd, send0, sizeof(send0), 0);
						for(int l = 0; l <= acc_post; l++){
							char send_msg[4000] = "";
							//printf("%s||%s||\n", board_name, posts[l].bname);
							if(!strcmp(board_name, posts[l].bname) && posts[l].exist > 0){

								sprintf(send_msg, "    %-17d%-17s%-17s%-17s\n", posts[l].id, posts[l].title, posts[l].author, posts[l].datem);
								send(fd, send_msg, sizeof(send_msg), 0);
							}
						}
                                        }

				}			
				
			}
			else{
				send(fd, ERR12, sizeof(ERR12), 0);
			}
		}
		else if(!strncmp(recv_msg, "read", 4)){
			if(!strncmp(recv_msg, "read ", 5)){
				char tmp_id[10];
				strcpy(tmp_id, recv_msg + 5);
				fix_endline(tmp_id);
				int real_id = atoi(tmp_id);
				if(posts[real_id].exist == 0){	//not exist
					send(fd, ERR13, sizeof(ERR13), 0);
				}
				else{
					char send_author[200], send_title[1040], send_date[200], send_content[1100], send_comment[120];
					sprintf(send_author, "    Author    :%s\n", posts[real_id].author);
					sprintf(send_title, "    Title     :%s\n", posts[real_id].title);
					sprintf(send_date, "    Date      :%s\n", posts[real_id].datey);
					sprintf(send_content, "    --\n     %s    --\n", posts[real_id].content);
					
					send(fd, send_author, strlen(send_author), 0);
					send(fd, send_title, strlen(send_title), 0);
					send(fd, send_date, strlen(send_date), 0);
					send(fd, send_content, strlen(send_content), 0);
					for(int k = 1; k <= posts[real_id].comment_cnt; k++){
						sprintf(send_comment, "     %s", posts[real_id].comments[k]);
						send(fd, send_comment, strlen(send_comment), 0);
					}

				}
			}
			else{
				send(fd, ERR14, sizeof(ERR14), 0);
			}
		}
		else if(!strncmp(recv_msg, "delete-post", 11)){
			if(!strncmp(recv_msg, "delete-post ", 12)){
				char tmp_id[10];
				strcpy(tmp_id, recv_msg + 12);
                                fix_endline(tmp_id);
                                int real_id = atoi(tmp_id);
				if(login_yn == 0){	//login first
					send(fd, ERR6, sizeof(ERR6), 0);
				}
				else if(posts[real_id].exist == 0){  //not exist
                                        send(fd, ERR13, sizeof(ERR13), 0);
                                }
                                else{	//exist
					if(!strcmp(login_name, posts[real_id].author)){	//is owner
						posts[real_id].exist = 0;
						send(fd, SUC6, sizeof(SUC6), 0);
					}
					else{	//not owner
						send(fd, ERR15, sizeof(ERR15), 0);
					}
				}
			}	
			else{
				send(fd, ERR16, sizeof(ERR16), 0);
			}
		}
		else if(!strncmp(recv_msg, "update-post", 11)){
			if(!strncmp(recv_msg, "update-post ", 12)){
				if(login_yn == 0){
                                        send(fd, ERR6, sizeof(ERR6), 0);
                                }
				else if(strstr(recv_msg, " --title ") == NULL && strstr(recv_msg, " --content ") == NULL){
					send(fd, ERR18, sizeof(ERR18), 0);
				}
				else{
					char *tmp_id = strdup(recv_msg + 12);
					if(strstr(recv_msg, "--title")){	//update title
						char *title = strstr(recv_msg, "--title");
						int len = strlen(tmp_id) - strlen(title);
						char real_id[10];
						substr(real_id, tmp_id, 0, len-1);
						int true_id = atoi(real_id);
						if(posts[true_id].exist == 0){
							send(fd, ERR13, sizeof(ERR13), 0);
						}
						else{
							if(!strcmp(posts[true_id].author, login_name)){
								char real_title[1024];
                                                        	strcpy(real_title, title + 8);
								fix_lines(real_title);
                                             	           	strcpy(posts[true_id].title, real_title);
                                                	        send(fd, SUC9, sizeof(SUC9), 0); //is owner
							}
							else{
								send(fd, ERR15, sizeof(ERR15), 0); //not owner
							}
						}
					}
					else if(strstr(recv_msg, "--content")){	//update content
						char *content = strstr(recv_msg, "--content");
                                                int len = strlen(tmp_id) - strlen(content);
                                                char real_id[10];
                                                substr(real_id, tmp_id, 0, len-1);
                                                int true_id = atoi(real_id);
                                                if(posts[true_id].exist == 0){
                                                        send(fd, ERR13, sizeof(ERR13), 0);
                                                }
                                                else{
                                                        if(!strcmp(posts[true_id].author, login_name)){
                                                                char real_content[1024];
                                                                strcpy(real_content, content + 10);
                                                            	fix_content(real_content);
							    	strcpy(posts[true_id].content, real_content);
                                                                send(fd, SUC9, sizeof(SUC9), 0); //is owner
                                                        }
                                                        else{
                                                                send(fd, ERR15, sizeof(ERR15), 0); //not owner
                                                        }
                                                }
					}
					
				}
			}
			else{
				send(fd, ERR18, sizeof(ERR18), 0);
			}
			
		}
		else if(!strncmp(recv_msg, "comment", 7)){
			if(!strncmp(recv_msg, "comment ", 8)){
				if(login_yn == 0){
					send(fd, ERR6, sizeof(ERR6), 0);
				}
				else{
					char *delim = " ";
					char *pch;
					pch = strtok(recv_msg, delim);
					pch = strtok(NULL, delim);
					//printf("%s\n", pch);
					int real_id = atoi(pch);
					if(posts[real_id].exist == 0){	//not exist
						send(fd, ERR13, sizeof(ERR13), 0);
					}
					else{
						char pch0[100];
						int len = strlen(pch) + 8 + 2;
						strcpy(pch0, recv_msg + len);
						fix_content(pch0);
						char tmp_comment[100];
						strcpy(tmp_comment, login_name);
						strcat(tmp_comment, ": ");
						strcat(tmp_comment, pch0);
						posts[real_id].comment_cnt ++;
						posts[real_id].comments[posts[real_id].comment_cnt] = strdup(tmp_comment);
						send(fd, SUC10, sizeof(SUC10), 0);
					}
				}	
			}
			else{
				send(fd, ERR19, sizeof(ERR19), 0);
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
		else if(!strncmp(recv_msg, "store", 5)){
			FILE *outfp;
			outfp = fopen("data.txt", "w");
			fprintf(outfp, "user:\n%d\n", acc_num);
			for(int j = 0; j < acc_num; j++){
				fprintf(outfp, "%d %d %s %s %s\n", database[j].regis, database[j].login, database[j].name, database[j].email, database[j].password);
			}

			fprintf(outfp, "board:\n%d\n", acc_board);
			for(int j = 1; j <= acc_board; j++){
				fprintf(outfp, "%d %s %s\n", allboard[j].num, allboard[j].name, allboard[j].moderator);
			}
			fprintf(outfp, "post:\n%d\n", acc_post);
			for(int j = 1; j <= acc_post; j++){
				fprintf(outfp, "%s\n%d\n%d\n%s\n%s\n%s\n%s\n%scontent is over\n%d\n", posts[j].bname, posts[j].id, posts[j].exist, posts[j].author, posts[j].title, posts[j].datey, posts[j].datem, posts[j].content, posts[j].comment_cnt);
				for(int k = 1; k <= posts[j].comment_cnt; k++){
					fprintf(outfp, "%s", posts[j].comments[k]);
				}
			}
			
			send(fd, SUC7, sizeof(SUC7), 0);
			fclose(outfp);
		}
		else if(!strncmp(recv_msg, "load", 4)){
			FILE *infp;
			infp = fopen("data.txt", "r");
			if(infp == NULL){
				send(fd, ERR17, sizeof(ERR17), 0);
			}
			else{
				char tmp_buf[1024];
				char tmp_num[10];
				fgets(tmp_buf, 1024, infp);
				fgets(tmp_num, 10, infp);
				int real_num = atoi(tmp_num);
				int i = 0;
				char *delim = " ";
				char *pch;
				for(int i = 0; i < real_num; i++){
					acc_num ++;
					fgets(tmp_buf, 1024, infp);
					pch = strtok(tmp_buf, delim);
					for(int j = 0; j < 5; j++){
						switch(j){
							case 0:	
								database[i].regis = atoi(pch);
								break;
							case 1: 
								database[i].login = atoi(pch);
								break;
							case 2: 
								strcpy(database[i].name, pch);
								break;
							case 3: 
								strcpy(database[i].email, pch);
								break;
							case 4:{
								       strncpy(database[i].password, pch, strlen(pch)-1);
								       break;
							       }
						}
						pch = strtok(NULL, delim);
					}	
					//printf("%s\n", pch);
					//pch = strtok(NULL, delim);
				}
				fgets(tmp_buf, 1024, infp);	//read boards
				fgets(tmp_num, 10, infp);
				real_num = atoi(tmp_num);
				for(int i = 1; i <= real_num; i++){
					acc_board++;
					fgets(tmp_buf, 1024, infp);
					pch = strtok(tmp_buf, delim);
					for(int j = 0; j < 3; j++){
						switch(j){
							case 0:
								allboard[i].num = atoi(pch);
								break;
							case 1:
								strcpy(allboard[i].name, pch);
								break;
							case 2:
								strncpy(allboard[i].moderator, pch, strlen(pch)-1);	//delete '\n'
								break;

						}
						//printf("%s\n", pch);
						pch = strtok(NULL, delim);
					}

				}
				fgets(tmp_buf, 1024, infp);	//read posts
                                fgets(tmp_num, 10, infp);
                                real_num = atoi(tmp_num);
				for(int i = 1; i <= real_num; i++){
					acc_post ++;
					fgets(tmp_buf, 1024, infp);	//board name
					fix_lines(tmp_buf);
					strcpy(posts[i].bname, tmp_buf);
				
					fgets(tmp_buf, 1024, infp);	//id
                                        fix_lines(tmp_buf);
                                        posts[i].id = atoi(tmp_buf);

					fgets(tmp_buf, 1024, infp);	//existed
                                        fix_lines(tmp_buf);
                                        posts[i].exist = atoi(tmp_buf);

				//printf("here%s\n", tmp_buf);

					fgets(tmp_buf, 1024, infp);     //author
                                        fix_lines(tmp_buf);
                                        strcpy(posts[i].author, tmp_buf);

				//printf("here%s\n", tmp_buf);

					fgets(tmp_buf, 1024, infp);     //title
                                        fix_lines(tmp_buf);
                                        strcpy(posts[i].title, tmp_buf);

				//printf("here%s\n", tmp_buf);
					fgets(tmp_buf, 1024, infp);     //datey
                                        fix_lines(tmp_buf);
                                        strcpy(posts[i].datey, tmp_buf);
				//printf("here%s\n", tmp_buf);

					fgets(tmp_buf, 1024, infp);     //datem
                                        fix_lines(tmp_buf);
                                        strcpy(posts[i].datem, tmp_buf);
					
				//printf("here%s\n", tmp_buf);
					while(1){	//content
						fgets(tmp_buf, 1024, infp);
						//printf("here%s\n", tmp_buf);
						if(!strncmp(tmp_buf, "content is over", 15)) break;
						strcat(posts[i].content, tmp_buf);
					}

					fgets(tmp_buf, 1024, infp);     //comment_cnt
                                        fix_lines(tmp_buf);
					int comment_cnt = atoi(tmp_buf);
					posts[i].comment_cnt = comment_cnt;
					for(int k = 1; k <= comment_cnt; k++){
						fgets(tmp_buf, 1024, infp);	//comments
						posts[i].comments[k] = strdup(tmp_buf);
					}


				}

			}
			send(fd, SUC8, sizeof(SUC8), 0);
			fclose(infp);
		}

		else{
			//signal(SIGPIPE,SIG_IGN);
			send(fd, ERR7, sizeof(ERR7), 0);
		}

	}
	}
}
