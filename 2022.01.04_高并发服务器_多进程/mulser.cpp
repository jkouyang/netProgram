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
using namespace std;

int main(int argc, char *argv[]){
	//1.创建套接字socket
	int listenfd;
	if((listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cout<<"make socket error!"<<endl;

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
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cout<<"bind error!"<<endl;
	
	//3.监听
	if(listen(listenfd,10)<0)
		cout<<"listen error!"<<endl;
	
	//4.accept，等待连接(阻塞)
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
		//socklen_t是一种数据类型，和int差不多
		//32位下,size_t和int的长度相同,都是32 bits,但64位下,size_t(32bits)和int(64 bits)长度不同
		//socket编程中的accept函数的第三个参数的长度必须和int的长度相同。于是有了socklen_t类型。
	int connfd;
	pid_t pid;
	while(1){
		if((connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen))<0)
                	cout<<"accept error!"<<endl;
        	cout<<"New Connect"<<"\tClient ip:"<<inet_ntoa(clientaddr.sin_addr);
		cout<<"\tClient port:"<<ntohs(clientaddr.sin_port)<<endl;
		pid = fork();
		if(pid==0){//子进程
			close(listenfd);
			// 5.data transfer
        		char rec[1024];
        		int iconnect = 0;
        		while(1){
                		memset(rec,0,sizeof(rec));
                		int readlen = read(connfd,rec,sizeof(rec));//set 接收字符串的长度
				//cout<<"接收的数据长度："<<set<<endl;
                		if(readlen==0){//接收的数据长度为0时，退出子进程
					cout<<"Close"<<"\tClient ip:"<<inet_ntoa(clientaddr.sin_addr);
                			cout<<"\tClient port:"<<ntohs(clientaddr.sin_port)<<endl;
					close(connfd);
					exit(0);//退出子进程
				}	
				cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                                        cout<<" port:"<<ntohs(clientaddr.sin_port)<<"  ";
				cout<<rec<<endl;
                		write(connfd,rec,readlen);
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
