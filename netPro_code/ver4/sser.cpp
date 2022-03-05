#include<iostream>
#include<string>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#include<sys/times.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include"wrap.h"
using namespace std;


int main(int argc, char *argv[]){
	//1.创建套接字socket
	int listenfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	//2.创建ipv4地址，并和socket绑定 
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("192.168.177.128");//my(seirver) addr
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//本机的任意地址
	int on = 1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		cout<<"setsockopt reuseaddr error!"<<endl;
	Bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	//3.监听
	Listen(listenfd,10);
	
	//4.初始化要检测的文件描述符集合
	fd_set readset,tempset;
	FD_ZERO(&readset);//清理	
	FD_SET(listenfd,&readset);//将listenfd添加到读集合中
	int nfds = listenfd+1;//监控的文件描述符集里最大文件描述符

	struct sockaddr_in clientaddr;//客户端地址
	socklen_t clientlen =sizeof(clientaddr);	
	char rec[1024]={0};
	while(1){
		tempset=readset;
		int cnum = select(nfds,&tempset,NULL,NULL,NULL);//响应的个数
		for(int i=listenfd;i<nfds;i++){//遍历集合
			cout<<"轮询文件描述符:"<<i<<endl;
			if(i==listenfd&&FD_ISSET(listenfd,&tempset)){//listenfd在集合中
				int connfd = Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
				char rback[]="Connect successly!";
				write(connfd,rback,sizeof(rback));
				memset(rback,0,sizeof(rback));
				cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
				cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
				FD_SET(connfd,&readset);
				nfds = connfd+1>nfds?connfd+1:nfds;
				memset(&clientaddr,0,sizeof(clientaddr));
				if(--cnum<=0)continue;//如果只有listenfd响应了，后面的步骤跳过
			}
			else{
				if(FD_ISSET(i,&tempset)){
					int readlen = Readline(i,rec,sizeof(rec));
					if(readlen==0){//接收的数据长度为0时，退出子进程
						FD_CLR(i,&readset);
						Close(i);
						cout<<"close a client"<<endl;
						continue;
					}
					getpeername(i, (struct sockaddr*)&clientaddr,&clientlen);
					cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                                        cout<<" port:"<<ntohs(clientaddr.sin_port);
					cout<<"  "<<rec<<endl;
					memset(rec,0,sizeof(rec));	
					memset(&clientaddr,0,sizeof(clientaddr));
				}
			}
		}	
	}

	return 0;
}
