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
using namespace std;

int main(int argc,char* argv[]){
	//1.make socket
	int sockfd;
	if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cout<<"make socket error!"<<endl;
	
	//2.make ip addr and connect
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	//void *memset(void *s,int c,size_t n)
	//总的作用：将已开辟内存空间 s 的首 n 个字节的值设为值 c。
	servaddr.sin_family= AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("192.168.177.128");

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cout<<"connect error!"<<endl;
	
	//3.after connect	
	char sendbuf[1024]= {0};
	char recvbuf[1024]= {0};
	while(1){
		cin>>sendbuf;
		cout<<strlen(sendbuf)<<endl;
		write(sockfd,sendbuf,strlen(sendbuf));
		read(sockfd,recvbuf,sizeof(recvbuf));
		cout<<recvbuf<<endl;
		memset(sendbuf,0,sizeof(sendbuf));
		memset(recvbuf,0,sizeof(recvbuf));
	}
	close(sockfd);
	return 0;
}



