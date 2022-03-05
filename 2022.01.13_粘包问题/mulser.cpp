#include<iostream>
#include<string>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<signal.h>
#include<sys/wait.h>
#include"wrap.h"
using namespace std;

void handle_sigchld(int sig){
	pid_t pid;
	int stat;
	while((pid = waitpid(-1,&stat,WNOHANG))>0){};
	//while(waitpid(-1,NULL,WNOHANG)>0){};这样也也可
	return;
}

int main(int argc, char *argv[]){
	//signal(SIGCHLD,SIG_IGN);//直接忽视
	signal(SIGCHLD,handle_sigchld);
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
	
	//4.accept，等待连接(阻塞)
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
		//socklen_t是一种数据类型，和int差不多
		//32位下,size_t和int的长度相同,都是32 bits,但64位下,size_t(32bits)和int(64 bits)长度不同
		//socket编程中的accept函数的第三个参数的长度必须和int的长度相同。于是有了socklen_t类型。
	int connfd;
	pid_t pid;
	while(1){
		connfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
        	cout<<"New Connect"<<"\tClient ip:"<<inet_ntoa(clientaddr.sin_addr);
		cout<<"\tClient port:"<<ntohs(clientaddr.sin_port)<<endl;
		pid = fork();
		if(pid==0){//子进程
			close(listenfd);
			// 5.data transfer
			sleep(2);
        		char rec[1024];
        		int iconnect = 0;
        		while(1){
                		memset(rec,0,sizeof(rec));
				char ack[]="accept successly!";
                		int readlen = Readline(connfd,rec,sizeof(rec));//readlen 接收字符串的长度
				//int readlen = read(connfd,rec,5);
				//cout<<"接收的数据长度："<<readlen<<endl;
				//cout<<"rec[8]"<<rec[8]<<endl;
				//cout<<"rec[9]"<<rec[9]<<endl;
                		if(readlen==0){//接收的数据长度为0时，退出子进程
					cout<<"Close"<<"\tClient ip:"<<inet_ntoa(clientaddr.sin_addr);
                			cout<<"\tClient port:"<<ntohs(clientaddr.sin_port)<<endl;
					close(connfd);
					exit(0);//退出子进程
				}	
				cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                                        cout<<" port:"<<ntohs(clientaddr.sin_port)<<"  ";
				cout<<rec<<endl;
                		//Writen(connfd,ack,sizeof(ack));
        		}
		}
		else if(pid>0){//父进程
			close(connfd);
		}
		else cout<<"fork error!"<<endl;//pid<0,进程创建失败
	}
	
	close(listenfd);
	close(connfd);
	cout<<"over"<<endl;
	return 0;
}
