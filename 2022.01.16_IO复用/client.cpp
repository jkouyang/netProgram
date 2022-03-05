#include<iostream>
#include<string>
#include<string.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include"wrap.h"
using namespace std;

int main(int argc,char* argv[]){
	//1.make socket
	int sockfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	//2.make ip addr and connect
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	//void *memset(void *s,int c,size_t n)
	//总的作用：将已开辟内存空间 s 的首 n 个字节的值设为值 c。
	servaddr.sin_family= AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("192.168.177.128");
	cout<<"Connect ..."<<endl;
	Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	char rback[1024]={0};
	read(sockfd,rback,sizeof(rback));
	cout<<rback<<endl;
	//3.after connect	
	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
	int iconnect =0;
	while(1){
		fgets(sendbuf,sizeof(sendbuf),stdin);
		Writen(sockfd,sendbuf,strlen(sendbuf));
		memset(sendbuf,0,sizeof(sendbuf));
	}
	Close(sockfd);
	return 0;
}



