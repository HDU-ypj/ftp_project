#include "tools.h"


char *fgets_t(char*str,int len)
{
	stdin->_IO_read_ptr = stdin->_IO_read_end;//清空缓存区
	fgets(str,len,stdin);
	stdin->_IO_read_ptr = stdin->_IO_read_end;//清空缓存区

	for(int i=1;i<len;i++)
	{
		if(str[i]=='\n')
		{
			str[i] = '\0';
		}
	}
	return str;
}


int recv_cmd_data(int cmd_fd,const char* code,const char* error_tips)
{
	char buf[4096] = {};
	size_t buf_size = sizeof(buf);

	if(0 >= recv(cmd_fd,buf,buf_size,0))
		handle_error("recv");
	//打印提示信息
	printf("%s\n",buf);
	
	if(0 != strncmp(code,buf,strlen(code)))
	{
		if(NULL != error_tips)
		{
			printf("%s\n",error_tips);
		}
		return -1;
	}
	return 0;
}

int open_sock(int cmd_sock)
{
	char buf[100] = {};
	unsigned char ip1,ip2,ip3,ip4,port1,port2;
	size_t buf_size = sizeof(buf);
	
	send_cmd_data(cmd_sock,"PASV\n");
	if(0 >= recv(cmd_sock,buf,buf_size,0))
		handle_error("recv");
	printf("%s\n",buf);
	if(0 != strncmp("227",buf,3))
	{
		printf("数据通道打开失败!\n");
		return -1;
	}
	sscanf(buf,"227 Entering Passive Mode (%hhu,%hhu,%hhu,%hhu,%hhu,%hhu)",&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	char ip[16] = {};
	sprintf(ip,"%hhu.%hhu.%hhu.%hhu",ip1,ip2,ip3,ip4);
	unsigned short port = port1*256+port2;
	
	int pasv_sock = socket(AF_INET,SOCK_STREAM,0);
	if(0 > pasv_sock) 
	{
		perror("socket");
		return -1;
	}
	
	struct sockaddr_in svr_addr = {};
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(port);
	svr_addr.sin_addr.s_addr = inet_addr(ip);
	
	socklen_t addrlen = sizeof(svr_addr);

	if(connect(pasv_sock,(SP)&svr_addr,addrlen))
	{
		printf("连接数据通道失败\n");
		return -1;
	}
	
	return pasv_sock;
}
