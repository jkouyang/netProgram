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

	Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	//3.after connect	
	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
	int iconnect =0;
	while(1){
		if(++iconnect>2)break;
		//cout<<"cin:";
		char sendbuf_1[]="this is a txt.\n";
		char sendbuf_2[]="this is a struct.\n";
		//memset(sendbuf_1,0,sizeof(sendbuf_1));
		//memset(sendbuf_2,0,sizeof(sendbuf_2));
		cin>>sendbuf_1;
		//cin>>sendbuf_2;
		//cout<<strlen(sendbuf_1)<<endl;
		//cout<<sizeof(sendbuf_1)<<endl;
		write(sockfd,sendbuf_1,strlen(sendbuf_1));
		Writen(sockfd,sendbuf_2,strlen(sendbuf_2));
		//write(sockfd,sendbuf,strlen(sendbuf));
		//read(sockfd,recvbuf,sizeof(recvbuf));
		//cout<<recvbuf<<endl;
		memset(sendbuf_1,0,sizeof(sendbuf_1));
		memset(sendbuf_2,0,sizeof(sendbuf_2));
		//memset(sendbuf,0,sizeof(sendbuf));
		//memset(recvbuf,0,sizeof(recvbuf));
	}
	Close(sockfd);
	return 0;
}



