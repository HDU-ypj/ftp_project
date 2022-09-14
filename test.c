/**************************************************
>	作	  者：杭电羊皮卷
>	邮	  箱：2997675141@qq.com
>   文件名称: test.c
>   创建日期：2022年09月13日
>   描    述：
**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define handle_error(msg)\
	do{perror(msg);exit(EXIT_FAILURE);}while(0)
typedef struct sockaddr* SP;

int main(int argc,const char* argv[])
{
	int cmd_sock = socket(AF_INET,SOCK_STREAM,0);
	if(0 > cmd_sock)	handle_error("socket");
	struct sockaddr_in svr_addr;
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(21);
	svr_addr.sin_addr.s_addr = inet_addr("192.168.1.120");
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if(0 > connect(cmd_sock,(SP)&svr_addr,addrlen))
		handle_error("connect");

	char buf[4096];
	size_t buf_size = sizeof(buf);

	recv(cmd_sock,buf,buf_size,0);
	if(0 != strncmp("220",buf,3))
	{
		printf("连接失败\n");
		return EXIT_FAILURE;
	}
	char name[20];
	printf("请输入用户名:");
	scanf("%s",name);
	sprintf(buf,"USER %s\n",name);//FTP以\n为结束标志
	send(cmd_sock,buf,strlen(buf),0);

	recv(cmd_sock,buf,buf_size,0);
	if(0 != strncmp("331",buf,3))
	{
		printf("用户名有误\n");
		return EXIT_FAILURE;
	}
	printf("%s\n",buf);

	char pass[20];
	printf("请输入密码：");
	scanf("%s",pass);
	sprintf(buf,"PASS %s\n",pass);
	send(cmd_sock,buf,strlen(buf),0);
	recv(cmd_sock,buf,buf_size,0);
	printf("%s\n",buf);
	if(0 != strncmp("230",buf,3))
	{
		printf("登录失败\n");
		return EXIT_FAILURE;
	}
	printf("登录成功\n");

	unsigned char ip1,ip2,ip3,ip4,port1,port2;
	sprintf(buf,"PASV\n");
	send(cmd_sock,buf,strlen(buf),0);
	recv(cmd_sock,buf,buf_size,0);
	printf("%s\n",buf);
	if(0 != strncmp("227",buf,3))
	{
		printf("数据通道打开失败\n");
		return EXIT_FAILURE;
	}
	sscanf(buf,)


	return 0;
}
