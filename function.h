#ifndef FUNCTION_H
#define FUNCTION_H

//输入服务器ip地址，创建通信socket同时建立连接，返回tcp通道
int init_connect(const char* ip);

//sign in
void sign_in(int cmd_sock);

//pwd
void pwd(int cmd_sock);

//mkdir
void mkdir_t(int cmd_sock,const char* path);

//rmdir
void rmdir_t(int cmd_sock,const char* path);

//cd
void cd(int cmd_sock,const char* path);

//ls
void ls(int cmd_sock);

//put 	上传
void put(int cmd_sock,const char* path);

//get	下载
void get(int cmd_sock,const char* path);

//bye	退出登录
void bye(int cmd_sock);

//other "非法命令"
void othercmd(int cmd_sock);

void run(int cmd_sock);

#endif
