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
	//make socket
	int sockfd;
	if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cout<<"make socket error!"<<endl;
	
	//make ip addr and connect
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family= AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("119.91.33.136");

	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cout<<"connect error!"<<endl;
	
	//after connect	
	char sendbuf[1024]= {0};
	char recvbuf[1024]= {0};
	cin>>sendbuf;
	int iconnect = 0;
	while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL){
		write(sockfd,sendbuf,strlen(sendbuf));
		read(sockfd,recvbuf,sizeof(recvbuf));
		cout<<recvbuf<<endl;
		memset(sendbuf,0,sizeof(sendbuf));
		memset(recvbuf,0,sizeof(recvbuf));
		iconnect++;
		if(iconnect>5)break;
	}
	close(sockfd);
	return 0;
}




