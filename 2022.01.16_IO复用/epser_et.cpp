#include<iostream>
#include<errno.h>
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
#include<fcntl.h>
#include"wrap.h"
using namespace std;
/*
epoll 测试 服务器端
ET 边沿工作模式
	发生变化时只调用一次epoll_wait
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
	ev.events=EPOLLIN|EPOLLET;
	ev.data.fd=listenfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	
	//(3)监测
	struct epoll_event evs[1024];//用于接收epoll监听的返回值
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
	char rec[4]={0};	
	while(1){
		cout<<"epoll_wait()..."<<endl;
		int cnum=epoll_wait(epfd,evs,1024,-1);
		cout<<"epoll_wait()完成 "<<"cnum:"<<cnum<<endl;
		for(int i=0;i<cnum;i++){
			//cout<<"i:"<<i<<endl;
			while(1){
				if(!(evs[i].events&EPOLLIN))break;//若不是读事件忽略
				if(evs[i].data.fd==listenfd){//lfd响应，新客户连接
					int connfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
        				//因为文件描述符行为默认是阻塞的, 因此要追加非阻塞行为
					//fcntl系统调用:对已打开的文件描述符进行控制操作,改变已打开文件的各种属性
					int flag=fcntl(connfd,F_GETFL);//获取文件描述符的flag属性
					flag |= O_NONBLOCK;//给flag追加非阻塞
					fcntl(connfd,F_SETFL,flag);//将新的flag属性设置到文件描述符中
					ev.data.fd=connfd;
        				ev.events=EPOLLIN|EPOLLET;
        				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
					char rback[]="Connect successly!";
					write(connfd,rback,sizeof(rback));
					cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
					cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
					memset(&clientaddr,0,sizeof(clientaddr));
					memset(rback,0,sizeof(rback));
					break;
				}	
				else{//connfd响应的读事件
					cout<<"read... ";
					int readlen = Readline(evs[i].data.fd,rec,sizeof(rec));
					cout<<"readlen="<<readlen<<endl;
					if(readlen<0){
						//缓冲区读干净了，若设置阻塞套接字时read会阻塞在这里
						//设置非阻塞套接字，read会返回错误信息EAGAIN，此时跳出循环
						if(errno==EAGAIN) break;
						//若为其他错误，关闭套接字，下树，跳出循环
						cout<<"read error!";
						epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
                                                Close(evs[i].data.fd);
						break;
					}
					if(readlen==0){//接收的数据长度为0时，退出子进程
						epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
						Close(evs[i].data.fd);
						cout<<"close a client"<<endl;
						break;
					}
					getpeername(evs[i].data.fd, (struct sockaddr*)&clientaddr,&clientlen);
					cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                	    		cout<<" port:"<<ntohs(clientaddr.sin_port);
					cout<<"  "<<rec<<endl;
					memset(rec,0,sizeof(rec));
					memset(&clientaddr,0,sizeof(clientaddr));
				}
			}
			cout<<endl;
		}
	} 
	Close(listenfd);
	return 0;
}
