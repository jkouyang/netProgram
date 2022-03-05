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
	//1.make socket
	int listenfd;
	if((listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cout<<"make socket error!"<<endl;

	//2.make ipaddr and bind
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("192.168.177.128");//local any addr,my(server) addr
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cout<<"bind error!"<<endl;
	
	//3.lnsten
	if(listen(listenfd,5)<0)
		cout<<"listen error!"<<endl;
	
	//4.accept
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
	int conn;
	if((conn=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen))<0)
		cout<<"accept error!"<<endl;

	//5.data transform
	char rec[1024];
	int iconnect = 0;
	while(1){
		memset(rec,0,sizeof(rec));
		int set = read(conn,rec,sizeof(rec));
		cout<<rec<<endl;
		write(conn,rec,set);
		iconnect++;
		if(iconnect>5)break;
	}
	close(listenfd);
	close(conn);
	cout<<"over"<<endl;
	return 0;
}
