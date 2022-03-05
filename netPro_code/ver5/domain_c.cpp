#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<stddef.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>
#include"wrap.c"
using namespace std;

int main(){
	//创建unix流式套接字
	int cfd=socket(AF_UNIX,SOCK_STREAM,0);

	//如果不绑定就是隐式绑定
	unlink("sock.c");
	struct sockaddr_un cliaddr;
	cliaddr.sun_family=AF_UNIX;
	strcpy(cliaddr.sun_path,"sock.c");
	bind(cfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	//连接
	struct sockaddr_un seraddr;
        seraddr.sun_family=AF_UNIX;
        strcpy(seraddr.sun_path,"sock.s");
	connect(cfd,(struct sockaddr*)&seraddr,sizeof(seraddr));

	//读写
	char sendbuf[128]={0};
	while(1){
		fgets(sendbuf,sizeof(sendbuf),stdin);
		write(cfd,sendbuf,strlen(sendbuf));//注意这个地方用strlen，不要用sizeof
		memset(sendbuf,0,sizeof(sendbuf));
	}
	//关闭
	close(cfd);
}


