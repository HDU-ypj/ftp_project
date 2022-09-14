#include <getch.h>
#include "tools.h"
#include "function.h"


int init_connect(const char* ip)
{
	//创建tcp连接通道
	int cmd_sock = socket(AF_INET,SOCK_STREAM,0);
	if(0 > cmd_sock)
		handle_error("socket");
	

	struct sockaddr_in svr_addr = {};
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(21);
	svr_addr.sin_addr.s_addr = inet_addr(ip);
	socklen_t addrlen = sizeof(svr_addr);

	//连接服务器
	if(connect(cmd_sock,(SP)&svr_addr,addrlen))
		handle_error("connect");
	
	if(recv_cmd_data(cmd_sock,"220","连接服务器失败"))
		exit(EXIT_FAILURE);
	return cmd_sock;
}

void sign_in(int cmd_sock)
{
	char buf[256];
	char temp[21];
	
	printf("请输入用户名:");
	fgets_t(temp,20);
	sprintf(buf,"USER %s\n",temp);	// FTP命令以\n为结束标志
	send_cmd_data(cmd_sock,buf);
	if(recv_cmd_data(cmd_sock,"331","用户名有误!"))
		exit(EXIT_FAILURE);

	printf("请输入密码:");
	int flag=0;
	int ch;
	while((ch = getch()) != '\n')
	{
		if(ch == 127 && flag!=0)		//读取到退格，并且还有位置可以退
		{
			flag--;
			printf("\b \b");
		}else if(flag<20 && ch!=127)	//限制写入位数为20位以下，并且没有读到退格键
		{
			temp[flag++] = ch;
			putchar('*');
		}
	}
	puts("\0");
	temp[flag] = '\0';					//完善字符串
	//fgets_t(temp,20);
	
	sprintf(buf,"PASS %s\n",temp);
	send_cmd_data(cmd_sock,buf);
	if(recv_cmd_data(cmd_sock,"230","密码有误!"))
		exit(EXIT_FAILURE);
		
	printf("登录成功!\n");
}

void pwd(int cmd_sock)
{
	send_cmd_data(cmd_sock,"PWD\n");
	if(recv_cmd_data(cmd_sock,"257",NULL))
	{
		printf("断开连接!\n");
		close(cmd_sock);
	}
}

void cd(int cmd_sock,const char* path)
{
	char buf[100];
	sprintf(buf,"CWD %s\n",path);
	send_cmd_data(cmd_sock,buf);
	recv_cmd_data(cmd_sock,"250","目录打开失败!");
}

void ls(int cmd_sock)
{
	char buf[4096];
	size_t buf_size = sizeof(buf);
	int ret;
	
	//创建数据socket连接
	int src_sock = open_sock(cmd_sock);
	send_cmd_data(cmd_sock,"LIST -al\n");
	recv_cmd_data(cmd_sock,"150",NULL);
	//接收并打印ls
	while(ret = read(src_sock,buf,buf_size))
	{
		//printf("%s",buf);
		write(1,buf,ret);
		//fflush(stdout);
	}
	close(src_sock);
	recv_cmd_data(cmd_sock,"226",NULL);
}

//put 	上传
void put(int cmd_sock,const char* path)
{
	char buf[4096];
	size_t buf_size = sizeof(buf);
	int fd = open(path,O_RDONLY);
	int ret;
	if(0 > fd)
	{
		printf("文件不存在，请检查!\n");
		return;
	}
	
	int pasv_sock = open_sock(cmd_sock);
	//recv_cmd_data(cmd_sock,"150","数据发送有误!")
	char cmd[50];
	sprintf(cmd,"STOR %s\n",path);
	send_cmd_data(cmd_sock,cmd);
	if(recv_cmd_data(cmd_sock,"150","数据发送有误!"))
		return;
		
	while(ret = read(fd,buf,buf_size))
	{
		send(pasv_sock,buf,ret,0);	
	}
	close(fd);
	close(pasv_sock);

	if(recv_cmd_data(cmd_sock,"226","数据发送有误!"))
		return;
	
	printf("文件上传成功!\n");
}
//get	下载
void get(int cmd_sock,const char* path)
{
	int ret;
	char buf[4096];
	size_t buf_size = sizeof(buf);
	
	int fd = open(path,O_CREAT|0644|O_EXCL);
	if(0 > fd)
	{
		printf("文件存在，是否覆盖？y/n\n");
		char ch;
		ch = getchar();
		if(ch != 'Y' && ch != 'y')
		{
			printf("文件下载失败\n");
			return;
		}
		else
		{
			fd = open(path,O_CREAT|0644|O_TRUNC);
		}
	}
	int pasv_sock = open_sock(cmd_sock);
	sprintf(buf,"RETR %s\n",path);
	send_cmd_data(cmd_sock,buf);
	if(recv_cmd_data(cmd_sock,"150","下载失败"))
	{
		close(fd);
		close(pasv_sock);
		return;
	}
	
	while(ret = read(pasv_sock,buf,buf_size))
	{
		write(fd,buf,ret);	
	}
	close(fd);
	close(pasv_sock);
	
	if(recv_cmd_data(cmd_sock,"226",NULL))
		return;
	printf("文件下载成功!\n");
}

void bye(int cmd_sock)
{
	send_cmd_data(cmd_sock,"QUIT\n");
	recv_cmd_data(cmd_sock,"221","注销失败!");
	close(cmd_sock);
}

//mkdir
void mkdir_t(int cmd_sock,const char* path)
{
	char buf[256];
	sprintf(buf,"MKD %s\n",path);
	send_cmd_data(cmd_sock,buf);
	recv_cmd_data(cmd_sock,"257","创建失败!");
}
//rmdir
void rmdir_t(int cmd_sock,const char* path)
{
	char buf[256];
	sprintf(buf,"RMD %s\n",path);
	send_cmd_data(cmd_sock,buf);
	recv_cmd_data(cmd_sock,"250","删除失败!");
}

void othercmd(int cmd_sock)
{
	printf("非法命令\n");
}


void run(int cmd_sock)
{
	char buf[30];
	char cmd[2][20]={};
	const Func_map map[]=
	{
		{"bye",bye},
		{"quit",bye},
		{"pwd",pwd},
		{"ls",ls},
	};
	const Func2_map map2[]=
	{
		{"mkdir",mkdir_t},
		{"rmdir",rmdir_t},
		{"cd",cd},
		{"put",put},
		{"get",get},
	};
	while(1)
	{
		int flag=0;
		printf("ftp>");
		fflush(stdout);
		fgets_t(buf,20);
		
		//遍历功能列表
		for(int i=0; i<sizeof(map)/sizeof(map[0]); i++)
		{
			if(0 == strcmp(map[i].cmd,buf))
			{
				flag=1;
				map[i].func(cmd_sock);
				if(map[i].func == bye)
				{
					exit(0);
				}
			}
		}
		if(!flag)
		{
			sscanf(buf,"%s %s",cmd[0],cmd[1]);
			for(int i=0; i<sizeof(map2)/sizeof(map2[0]); i++)
			{
				if(0 == strcmp(cmd[0],map2[i].cmd))
				{
					flag=1;
					map2[i].func(cmd_sock,cmd[1]);
				}
			}
		}
		if(!flag && 0 != strcmp("\n",buf))
		{
			othercmd(cmd_sock);
		}
	}
}
