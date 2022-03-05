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
#include<sys/epoll.h>
#include"wrap.h"
using namespace std;
/*
epoll 测试 服务器端
LT 水平工作模式
	工作模式默认是水平模式，缓冲区有数据就调用一次epoll_wait
*/

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
	Listen(listenfd,100);
	
	//4.epoll
	//(1)创建epoll模型，创建红黑树
	int epfd = epoll_create(1);
	
	//(2)入树，将要检测的文件描述符入树
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=listenfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	
	//(3)监测
	struct epoll_event evs[1024];//用于接收epoll监听的返回值
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
	char rec[1024]={0};	
	while(1){
		cout<<"epoll_wait()..."<<endl;
		int cnum=epoll_wait(epfd,evs,1024,-1);
		cout<<"epoll_wait()完成 "<<"cnum:"<<cnum<<endl;
		for(int i=0;i<cnum;i++){
			//cout<<"i:"<<i<<endl;
			if(!(evs[i].events&EPOLLIN))continue;//若不是读事件忽略
			if(evs[i].data.fd==listenfd){//lfd响应，新客户连接
				int connfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
        			ev.data.fd=connfd;
        			ev.events=EPOLLIN;//工作模式默认是水平模式，缓冲区有数据就调用一次epoll_wait
        			epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
				char rback[]="Connect successly!";
                                write(connfd,rback,sizeof(rback));
				cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
				cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
				memset(&clientaddr,0,sizeof(clientaddr));
				memset(rback,0,sizeof(rback));
				continue;
			}
			else{
				cout<<"read... ";
				int readlen = Readline(evs[i].data.fd,rec,sizeof(rec));
				cout<<"readlen="<<readlen<<endl;
				if(readlen<0){cout<<"read error!";continue;}
				if(readlen==0){//接收的数据长度为0时，退出子进程
					epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
					Close(evs[i].data.fd);
					cout<<"close a client"<<endl;
					continue;
				}
				getpeername(evs[i].data.fd, (struct sockaddr*)&clientaddr,&clientlen);
				cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                    		cout<<" port:"<<ntohs(clientaddr.sin_port);
				cout<<"  "<<rec<<endl;
				memset(rec,0,sizeof(rec));	
				memset(&clientaddr,0,sizeof(clientaddr));	
			}
			cout<<endl;
		}

	} 
	Close(listenfd);
	return 0;
}
