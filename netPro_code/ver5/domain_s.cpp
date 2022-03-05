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
	int lfd=socket(AF_UNIX,SOCK_STREAM,0);

	//绑定
	unlink("sock.s");
	struct sockaddr_un seraddr;
	seraddr.sun_family=AF_UNIX;
	strcpy(seraddr.sun_path,"sock.s");
	bind(lfd,(struct sockaddr*)&seraddr,sizeof(seraddr));

	//监听
	listen(lfd,128);

	//提取
	struct sockaddr_un cliaddr;
	memset(&cliaddr,0,sizeof(cliaddr));
	socklen_t cliaddrlen=sizeof(cliaddr);
	int cfd=accept(lfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
	cout<<"New connect file: "<<cliaddr.sun_path<<endl;

	//读写
	char buf[128]={0};
	while(1){
		int n=Readline(cfd,buf,sizeof(buf));
		cout<<"n:"<<n<<endl;
		if(n<=0){
			cout<<"Close file: "<<cliaddr.sun_path<<endl;
			break;
		}
		else{
			cout<<"From "<<cliaddr.sun_path<<": "<<buf<<endl;
			memset(buf,0,sizeof(buf));
		}
	}
	//关闭
	close(cfd);
	close(lfd);
}


