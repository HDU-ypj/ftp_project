#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr* SP;

#define BUFF_CLEAR stdin->_IO_read_ptr = stdin->_IO_read_end	//清空缓存区

#define handle_error(msg)\
	do{perror(msg);exit(EXIT_FAILURE);}while(0)
	
//一定要记得buf里面是以\n结束的
#define send_cmd_data(sockfd,buf)\
	send(sockfd,buf,strlen(buf),0)
	
typedef struct
{
	char cmd[20];
	void (*func)(int sockfd);
}Func_map;

typedef struct
{
	char cmd[20];
	void (*func)(int sockfd,const char* path);
}Func2_map;


char *fgets_t(char*str,int len);	//fgets的完善，将'\n''\0'合并为'\0'

//从服务器接收数据，并与code比对，如果比对不成功，打印tips,返回-1,成功返回0
int recv_cmd_data(int cmd_fd,const char* code,const char* error_tips);

//开启文件传输socket
int open_sock(int cmd_sock);

#endif
