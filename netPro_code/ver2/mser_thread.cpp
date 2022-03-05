#include<iostream>
#include<string>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include"wrap.h"
using namespace std;
//高并发--多线程实现
struct cli_info{
	int connfd;
	struct sockaddr_in clientaddr;
};

void* do_work(void* arg){
	struct cli_info *info = (struct cli_info*)arg;
	int connfd=info->connfd;
	struct sockaddr_in clientaddr=info->clientaddr;
	char rec[256]={0};
	while(1){
		int readlen=read(connfd,rec,sizeof(rec));
        	if(readlen==0){//接收的数据长度为0时，退出
                	cout<<"Close"<<"\tClient ip:"<<inet_ntoa(clientaddr.sin_addr);
                        cout<<"\tClient port:"<<ntohs(clientaddr.sin_port)<<endl;
                        close(connfd);
			break;
                }
               	cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
               	cout<<" port:"<<ntohs(clientaddr.sin_port)<<"  ";
                cout<<rec<<endl;
                write(connfd,rec,readlen);
	}
}

int main(int argc, char *argv[]){
	//1.创建套接字socket
	int listenfd;
	listenfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	//2.创建ipv4地址，并和socket绑定 
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("192.168.109.128");//my(seirver) addr
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//本机的任意地址
	int on = 1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		cout<<"setsockopt reuseaddr error!"<<endl;
	Bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	//3.监听
	Listen(listenfd,10);
	
	//4.accept，等待连接(阻塞)
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
	int connfd;
	cli_info cliInfo;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	while(1){
		connfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
        	cout<<"New Connect"<<"\tClient ip:"<<inet_ntoa(clientaddr.sin_addr);
		cout<<"\tClient port:"<<ntohs(clientaddr.sin_port)<<endl;
		cliInfo.connfd=connfd;
		cliInfo.clientaddr=clientaddr;
		pthread_t tid;
		pthread_create(&tid,&attr,do_work,&cliInfo);
	}
	
	close(listenfd);
	return 0;
}
