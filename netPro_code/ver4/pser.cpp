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
#include<poll.h>
#include"wrap.h"
using namespace std;

#define OPEN_MAX 100


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
	
	//4.poll
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in clientaddr;
	socklen_t clientlen = sizeof(clientaddr);
	memset(&clientaddr,0,clientlen);

	client[0].fd=listenfd;
	client[0].events=POLLIN;

	for(int i =1;i<OPEN_MAX;i++){
		client[i].fd=-1;//初始化,fd=-1的位置不监听
	}
	int maxi=0;//client[]中有效位置的最大下标

	char rec[1024]={0};
	memset(rec,0,sizeof(rec));
	while(1){
		int cnum=poll(client,maxi+1,-1);
		cout<<"cnum:"<<cnum<<endl;
		if(client[0].revents&POLLIN){//有客户连接，为什么用&搞不懂
			int connfd = Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
			char rback[]="Connect successly!";
			write(connfd,rback,sizeof(rback));
			memset(rback,0,sizeof(rback));
			cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
			cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
			memset(&clientaddr,0,clientlen);
			//将新连接的connfd加入client[]中
			for(int i=1;i<OPEN_MAX;i++){
				if(client[i].fd==-1){//找到一个最近的空位置，将新的连接放入
					client[i].fd=connfd;
					client[i].events=POLLIN;
					maxi=maxi>i?maxi:i;
					cout<<"maxi:"<<maxi<<endl;
					break;
				}
			}
			if(--cnum<=0)continue;//如果只有linstenfd响应，后面的就跳过
		}
		for(int i=1;i<=maxi;i++){
			cout<<"轮询："<<i<<endl;
			if(client[i].fd==-1)continue;
			if(client[i].revents&POLLIN){
				int readlen = Readline(client[i].fd,rec,sizeof(rec));
				if(readlen<0){
					cout<<"read error!"<<endl;
					if(--cnum<=0) break;//若只有这一件事响应，则执行完退出轮询
					continue;//此处事件为读异常，后面的读事件不用执行
				}
				if(readlen==0){//接收的数据长度为0时，退出子进程
					Close(client[i].fd);
					client[i].fd=-1;
					cout<<"close a client"<<endl;
					if(--cnum<=0) break;//若只有这一件事响应，则执行完退出轮询
					continue;//此次响应为客户端断开，后面的读事件不用执行
				}
				getpeername(client[i].fd, (struct sockaddr*)&clientaddr,&clientlen);
				cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                    		cout<<" port:"<<ntohs(clientaddr.sin_port);
				cout<<"  "<<rec<<endl;
				memset(rec,0,sizeof(rec));	
				memset(&clientaddr,0,sizeof(clientaddr));			
				if(--cnum<=0) break;//若只有这一件事响应，则执行完退出轮询
			}
		}

	}
	Close(listenfd);
	return 0;
}
