/**************************************************
>	作	  者：杭电羊皮卷
>	邮	  箱：2997675141@qq.com
>   文件名称: main.c
>   创建日期：2022年09月13日
>   描    述：
**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "function.h"
#include "tools.h"

int main(int argc,const char* argv[])
{
	int cmd_sock = init_connect("127.0.0.1");
	sign_in(cmd_sock);
	run(cmd_sock);
	return 0;
}
