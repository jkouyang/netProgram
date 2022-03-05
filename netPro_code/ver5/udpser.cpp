#include<iostream>
#include<string>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <ctype.h>
using namespace std;

#define MAXLINE 128

int main(void){
	struct sockaddr_in servaddr;
	int sockfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.109.128");
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5000);
	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len = sizeof(cliaddr);
	printf("Accepting connections ...\n");
	int n;
	while (1){
		n=recvfrom(sockfd,buf,MAXLINE,0,(struct sockaddr *)&cliaddr, &cliaddr_len);
		if(n == -1) perror("recvfrom error");
		cout<<"New Connect"<<"  Client ip:"<<inet_ntoa(cliaddr.sin_addr);
		cout<<"  Client port:"<<ntohs(cliaddr.sin_port);
		cout<<"  "<<buf<<endl;
		n=sendto(sockfd, buf, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
		if(n == -1) perror("sendto error");
		memset(buf,0,sizeof(buf));
		memset(&cliaddr,0,sizeof(cliaddr_len));
	}
	close(sockfd);
	return 0;
}

