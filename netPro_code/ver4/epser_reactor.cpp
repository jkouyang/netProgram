#include<iostream>
#include<errno.h>
#include<string>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
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

//epoll reactor 测试epoll反应堆 服务器端

#define MAX_EVENTS 1024 //监听数上限
#define BUFLEN 1024 //buf[]数组长度，缓冲区大小

struct myevent{
	int fd;
	int events;//要处理的时间
	void *arg;//指向自己结构体指针
	void (*call_back)(int fd,void *arg);//回调函数，函数指针
	int status;//0：未上树，1：已上树
	char buf[BUFLEN];
	int len;
	long last_active;
};

//全局变量
int efd;//红黑树树根
struct myevent myevs[MAX_EVENTS+1];//与epoll_event对应的自定义结构体数组，用来调用回调函数 

void recvdata(int fd,void *arg);
void senddata(int fd,void *arg);

//创建一个myevent结构体
void set_myevent(struct myevent *myev,int fd,void (*call_back)(int fd,void *arg),int events){
	myev->fd=fd;
        myev->events=events;
        myev->arg=myev;//指向自己结构体本身的指针
        myev->call_back=call_back;
        myev->status=0;//0表示未上树
        memset(myev->buf,0,sizeof(myev->buf));//清空buf
	myev->len=0;//长度置0
        myev->last_active=time(NULL);
}

//构造一个对应myevent的内核结构体epoll_event，并将epoll_event上树
void eventadd(int efd,struct myevent *myev){
	struct epoll_event ev;
	ev.data.ptr = myev;//ev的ptr指针指向myev结构体
	ev.events=myev->events;
	if(myev->status==0){//如果myev的status为0，表示未上树
		if(epoll_ctl(efd,EPOLL_CTL_ADD,myev->fd,&ev)<0)//将ev上树
			cout<<"Add ev to efd error!"<<endl;
		else	cout<<"Add ev successly!"<<endl;
	}
	myev->status=1;
}

//从epoll红黑树上删除一个epoll_event结构体，下树，并修改对应的myevent
void eventdel(int efd,struct myevent *myev){
	struct epoll_event ev;
	if(myev->status!=1) return;//myev并没所有上树，不处理
	ev.data.ptr=NULL;
	myev->status=0;
	if(epoll_ctl(efd,EPOLL_CTL_DEL,myev->fd,NULL)<0)
		cout<<"Delete ev error!"<<endl;
	else	cout<<"Delete ev successly!"<<endl;
}

//当listenfd文件描述符就绪, 调用该函数完成与客户端的连接
void acceptconn(int lfd,void *arg){
	struct sockaddr_in clientaddr;
	socklen_t len =sizeof(clientaddr);
	int connfd;
	connfd=Accept(lfd,(struct sockaddr*)&clientaddr,&len);
	int i;
	//从下标0开始寻找一个未上树的结构体myevent
	for(i=0;i<MAX_EVENTS;i++){
		if(myevs[i].status==0)break;
	}
	if(i==MAX_EVENTS){
		cout<<"over max connect limit"<<endl;
		return;
	}
	fcntl(connfd,F_SETFL,O_NONBLOCK);
	set_myevent(&myevs[i],connfd,recvdata,EPOLLIN|EPOLLET);//创建myevent结构体
	eventadd(efd,&myevs[i]);//上树
	char rback[]="connect successly!";
	write(connfd,rback,sizeof(rback));
	cout<<rback<<endl;
	cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
	cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
	memset(&clientaddr,0,sizeof(clientaddr));
	memset(rback,0,sizeof(rback));
}

//当connfd文件描述符就绪，且为读事件，调用此函数完成读
void recvdata(int fd,void *arg){
	struct myevent *myev = (struct myevent*)arg;
	int readlen=Readline(fd,myev->buf,sizeof(myev->buf));
	cout<<"readlen:"<<readlen<<endl;
	struct sockaddr_in clientaddr;
        socklen_t clientlen =sizeof(clientaddr);
        getpeername(myev->fd, (struct sockaddr*)&clientaddr,&clientlen);
	if(readlen<0){
		eventdel(efd,myev);
		Close(myev->fd);
		return;
	}
	if(readlen==0){
                eventdel(efd,myev);
                Close(myev->fd);
		cout<<"Client Close:";
		cout<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
		cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
		memset(&clientaddr,0,sizeof(clientaddr));
                return;
        }
	cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
        cout<<" port:"<<ntohs(clientaddr.sin_port);
	cout<<"  "<<myev->buf<<endl;
	memset(&clientaddr,0,sizeof(clientaddr));
}
//当connfd文件描述符就绪，且为写事件，调用此函数完成写
void senddata(int fd,void *arg){

}

int main(int argc, char *argv[]){
	//1.创建套接字socket
	int listenfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

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
	fcntl(listenfd, F_SETFL, O_NONBLOCK);//设置非阻塞
	Bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	//3.监听
	Listen(listenfd,100);

	//epoll 反应堆
	efd=epoll_create(MAX_EVENTS+1);
	struct epoll_event evs[MAX_EVENTS + 1];//用来接收epoll_wait返回的事件

	//构建listenfd对应的myevent结构体，回调函数为acceptconn
	set_myevent(&myevs[MAX_EVENTS],listenfd,acceptconn,EPOLLIN|EPOLLET);
	//将listenfd对应的myevent对应到epoll_event，再将其上树
	eventadd(efd,&myevs[MAX_EVENTS]);
	while(1){
		cout<<"wait..."<<endl;
		int cnum=epoll_wait(efd,evs,MAX_EVENTS+1,-1);
		cout<<"wait successly, cnum:"<<cnum<<endl;
		for(int i=0;i<cnum;i++){
			struct myevent *myev = (struct myevent*)evs[i].data.ptr;
			if(evs[i].events & EPOLLIN){
				myev->call_back(myev->fd,myev);
			}
			if(evs[i].events & EPOLLOUT){
				myev->call_back(myev->fd,myev);
			}
			cout<<endl;
		}
	}
	Close(listenfd);
	return 0;
}
