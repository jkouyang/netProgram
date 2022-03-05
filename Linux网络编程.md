# 0. 随笔记

## Linux常用命令

| ****                                                         |
| :----------------------------------------------------------- |
| **pwd** 查看当前所在目录                                     |
| **ls, ll** 查看当前目录下的文件和文件夹                      |
| **cd** xxx 打开目录xxx                                       |
| **cd ..** 返回上一级，**cd .. ..** 返回上两级                |
| **g ++ -g  xxx -0 xxx** g++编译命令                          |
| **su** 切换到root用户，su xxx 切换到xxx用户， **exit**退出该用户 |
| **vim xxx**  编辑xxx文件，i进入编辑模式，Esc退出该模式， **wq,  q,   wq!,  q!** |
| **mkdir  xxx** 创建一个新文件夹(目录)                        |
| **shutdown  –h now** 关机                                    |
| **ps** 查看进程                                              |
| **kill -9 -pid** 杀死进程，pid为进程对应的进程编号           |
| **netstat -a\|grep 5188** 查看端口信息(可以自己选择要查看的端口号) |

## _exit()、exit(0)、exit(1)的区别

_exit()和exit()，主要区别是一个退出进程会清理I/O缓冲区，一个直接结束进程进入到内核中

所以exit()将缓冲区的数据写完后才能退出来，所以调用exit()函数后程序并不会马上退出，这就是有些出现的僵尸程序，而_exit是直接退出进入到内核中去。

```c++
//_exit(0)与exit(0)区别
//下面程序输出的结果是：is ok
int mian(){
    printf("is ok\n");
    printf("I am a chinese people");
    _exit(0);
}

//下面程序输出的结果是：is ok I am a chinese people
int mian(){
    printf("is ok\n");
    printf("I am a chinese people");
    exit(0);
}
```

 exit(1)表示异常退出，这个1是返回给操作系统的
 exit(0)表示正常退出 

## perror()函数

perror()函数显示标准错误输出流stderr中的错误信息，该函数的格式为：

```c++
void perror( const char *message );
```

## **strcat()函数**

```c++
#include<stdlib.h>
stract(str1,str2);//将str2拼接在str1后面
stract(str1,str2,n);//将str2的前n各字符拼接在str1后面
//str未字符数组，stract_s()函数更安全
```

## **标准输入输出**

fets()输入遇到'\n'结束，但会把'\n'写入，cin遇到'\n'结束，但不会把'\n'写入

```c++
//fets()输入遇到'\n'结束，但会把'\n'读入
# include <stdio.h>
char *fgets(char *s, int size, FILE *stream);
char aaa[20]; 
fgets(aaa, 7, stdin); 
```

## dup()、dup2()





# 1. socket套接字

![socket层](img\socket层.png)

![socketliuc](img\socket流程.png)

![scoket函数常见值](img\scoket函数常见值.png)

![Posix规范数据类型](img\Posix规范数据类型.png)

## 1.0 预备知识

### IP 地址结构

**ipv4_addr:**

![ipv4_addr](img\ipv4_addr.png)



**ipv4_通用 addr:**

![ipv4_通用addr](img\ipv4_通用addr.png)



### 字节序转换函数，大端-小端

![字节序转换函数](img\字节序转换函数.png)

### ip地址转换函数，点分十进制-二进制

```c++
#include<arpa/inet.h>
int inet_aton(const char* strptr, struct in_addr* addrptr);
	//字符串有效返回1，无效返回0，将strptr字符串转换成32位二进制地址，通过指针addrptr来存储

in_addr_t inet_addr(const char *strptr);
	//若字符串有效返回32位二进制的ipv4地址，出错返回INADDR_NONE
	//出错返回INADDR_NONE常值(通常全是1)，可能被视为255.255.255.255(广播地址)
	//已弃用

char* inet_ntoa(struct in_addr inaddr);
	//返回一个指向点分十进制数串的指针
```

```c++
#include<arpa/inet.h>
//ipv4和ipv6都可以
int inet_pton(int family, const char* strptr,  void* addrptr);
const char* inet_ntop(int family, const void* addrptr, char* strptr, size_t len);
```

## 1.1 socket()

![socket](img\socket.png)

```c++
//1.创建服务端的socket
int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//在UNIX系统中，一切输入输出设备皆文件，socket()函数的返回值其本质是一个文件描述符，是一个整数。
```

## 1.2 bind()

![bind](img\bind.png)

```c++
//2.把服务端用于通信的地址和端口绑定到socket上
bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
```



## 1.3 listen()

![listen](img\listen.png)

```c++
//3.把socket设置为监听模式
listen(listenfd,MAXSIZE);
```

![listen+](img\listen+.png)

![listen++](img\listen++.png)



## 1.4 accept()

![accept](img\accept.png)

```c++
//4.接受客户端的连接
int connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen))
```



## 1.5 connect()

![connect](img\connect.png)

```c++
//客户端
//先创建socket和服务器的sockaddr_in(服务器IP地址)
int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//连接
connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))
```

## 1.6 read()&write()&send()&recv()

为每个文件描述符创建一个缓冲区，缓冲区之间通过socket通信，利用read()，write()函数完成对缓冲区的读写。写入时，将数据写入缓冲区；读出时，将数据读出并在缓冲区中清空该数据的这一块位置。**缓冲区的内存一般大于nbyte，所以我们不能一次将缓冲区读完或者写满。**

**1）当要写入的数据大于write()的nbyte时，会分次写**

**2）当缓冲区的数据大于read()的nbyte时，会分次读**

**这样会造成无法区分是一条连续的较长的信息，还是几条非连续的短信息**

**read()、write()不负责每次将数据填满至nbyte再返回(在可填满的情况下)，可能中途就返回了，因此通常利用循环不断的填入数据至结束**——引出readn()，writen()

![文件描述符读写原理](img\文件描述符读写原理.png)

### >read()

从文件描述符fd的缓存中最多读取nbyte个字节到buf中

```c++
include <unistd.h>
ssize_t read(int fd, void *buf, size_t nbyte);
//从文件描述符fd的缓存中最多读取nbyte个字节到buf中
```

**读取成功时，返回实际所读的字节数**

**失败时，返回-1**，表示出现了错误，如果错误为EINTR说明读是由中断引起的，如果是ECONNREST表示网络连接出了问题。

**若返回的值是0，表示已经读到文件的结束了**

### >write()

将buf的最多nbyte个字节写入文件描述符fd的缓存中

```c++
include <unistd.h>
ssize_t write(int fd, const void*buf,size_t nbytes);
//将buf的最多nbyte个字节写入文件描述符fd的缓存中
```

**成功时返回写的字节数**

**失败时返回-1**并设置errno变量，若错误为EINTR：写的时候出现中断错误，若错误为EPIPE：网络连接出现问题(对方已经关闭了连接)。

在网络程序中，当我们向套接字文件描述符写时有两可能：1）write的返回值大于0，表示写了部分或者是全部的数据，这样我们用一个while循环来不停的写入，但是循环过程中的buf参数和nbyte参数得由我们来更新。也就是说，**网络写函数是不负责将全部数据写完之后在返回的**。2）返回的值小于0，此时出现了错误，我们要根据错误类型来处理。

### >send()&rcev()

recv和send函数提供了和read和write差不多的功能，不过它们提供了第四个参数来控制读写操作

```c++
include <sys/types.h>
include <sys/socket.h>
int recv(int sockfd,void *buf,int len,int flags);
int send(int sockfd,void *buf,int len,int flags); 
```

前面的三个参数和一样，第四个参数通常置0，或者是以下的组合：

```
MSG_DONTROUTE	不查找表
MSG_OOB			接受或者发送带外数据
MSG_PEEK		查看数据,并不从系统缓冲区移走数据
MSG_WAITALL		等待所有数据
```

## 1.7 readn()&writen()&readline()

​	字节流套接字（TCP套接字）上的read和write函数所表现的行为不同于通常的文件I/O。字节流套接字调用read或write输入或输出的字节数可能比请求的数量少，然而这并不是出错的状态。这是因为内核中用于套接字的缓冲区可能达到了极限。通常这种情况出现在一次读多于4096个字节或write在非阻塞情况下返回不足字节数。为了不多次调用read或防止write返回不足字节数，我们用下面的两个函数来替代read和write。

### >readn()

```c++
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
ssize_t readn(int fd,void *vptr,size_t n){
    size_t nleft;
    ssize_t nread;
    char *ptr;
    ptr=vptr;
    nleft=n;
    while(nleft > 0){
        if((nread = read(fd,ptr,nleft)) < 0){
            if(errno == EINTR) //被信号中断，重读
                nread = 0;
            else //出错
                return -1;
        }
        else if(nread == 0) //EOF
            break;
        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}
```

### >writen()

```c++
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
ssize_t safe_write(int fd, const void *vptr, size_t n){
    size_t  nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = n;
    while(nleft > 0){
        if((nwritten = write(fd, ptr, nleft)) <= 0){
            if(nwritten < 0 && errno == EINTR) //被信号中断，重写
                nwritten = 0;
            else //error
                return -1;
        }
        nleft -= nwritten;
        ptr   += nwritten;
     }
    return(n);
}
```

上面介绍了两个安全读写函数，但是并不意味着这两个函数在任何地方都完全适用，所以不要强迫自己使用。**需要注意阻塞、效率等问题，当你只是读写少量字节时，就没必要使用了。**

### >readline()

**按行读取(读取到\n，也可以解决粘包问题)**

```c++
ssize_t readline(int fd, void* vptr, size_t maxlen){
	ssize_t n, rc;
	char	c, * ptr;
	ptr = (char*)vptr;
	for (n = 1; n < maxlen; n++) {
	again:
		if ((rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n') {
				break;
			}
			else if (rc == 0) {
				*ptr = 0;
				return (n - 1);
			}
			else {
				if (errno == EINTR) {
					goto again;
				}
				return (-1);
			}
		}
	}
	*ptr = 0;
	return (n);
}
```

## 1.8 close()

```c++
#include<unistd.h>
int close(int sockfd);//成功返回0，出错返回-1
```

描述符引用计数：

并发服务器中关闭已连接套接字只是导致相应的文件描述符引用计数减1，当描述引用计数>0，该close不会引起TCP连接终结

## 1.9 socket通信完整代码

```c++
//Server.cpp 
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
 int connfd; 
 if((connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen))<0) 
 cout<<"accept error!"<<endl; 
 //5.data transform 
 char rec[1024]; 
 int iconnect = 0; 
 while(1){ 
     memset(rec,0,sizeof(rec)); 
     int set = read(connfd,rec,sizeof(rec)); 
     cout<<rec<<endl; 
     write(connfd,rec,set); 
     iconnect++; 
     if(iconnect>5)break; 
 } 
 close(listenfd); 
 close(connfd); 
 cout<<"over"<<endl; 
 return 0; 
} 
```

```c++
//Client.cpp 
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
```

## 解决问题——端口复用

上述Server端关闭后，一段时间内会出现TIME_WAIT状态
REUSEADDR 端口复用来解决	setsockopt

```c++
//解决Server端关闭后，一段时间内会出现TIME_WAIT状态
//server.cpp中bind()之前加入下面这代码
int on = 1;
if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
	cout<<"setsockopt reuseaddr error!"<<endl;
```

```c++
//main()代码，//解决Server端关闭后，一段时间内会出现TIME_WAIT状态
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
	servaddr.sin_addr.s_addr = inet_addr("192.168.177.128");//my(seirver) addr
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//本机的任意地址
	int on = 1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		cout<<"setsockopt reuseaddr error!"<<endl;
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cout<<"bind error!"<<endl;

	//3.lnsten
		...
	//4.accept
		...
	//5.data transfer
        ...
}
```

## 解决问题——TCP粘包问题

**粘包问题的本质就是数据读取边界错误所致**



![tcp粘包1](img\tcp粘包1.png)

如图1所示，当前的socket缓存中已经有6个数据分组到达，其大小如图中数字。而应用程序在对数据进行收取时(如图2)，read()函数采用300字节的要求去读取，则会误将pkg1和pkg2一起收走当做一个包来处理。而实际上，很可能pkg1是一个文本文件的内容，而pkg2则可能是一个音频内容，显然有失妥当。严重时可能因为丢了pkg2而导致软件陷入异常分支产生乌龙事件。

![tcp粘包2](img\tcp粘包2.png)

解决方法：**1.定长发送	2.尾部标记序列	3.头部标记分步接收**

![解决粘包问题](img\解决粘包问题.png)

**基于上面的问题，将write()改为writen()，read()改为readline()解决粘包问题**

![tcp粘包3](img\tcp粘包3.png)

**TCP是基于字节流传输的，只维护发送出去多少，确认了多少，没有维护消息与消息的边界，因而可能导致粘包问题。**

**粘包问题的解决方式就是在应用层维护消息边界。**

**write()、writen()会写入‘\n'，但不会写入‘\0’（遇见认为字符串结束）**

# 1++ 一些函数

### getsockname()、getpeername()

```c++
//获取套接字的本地地址：getsockname()
struct sockaddr_in localaddr;//初始化结构体localaddr
int getsockname(int sockfd, struct sockaddr* localaddr, socklen_t addrlen);
//注意，先初始化结构体localaddr，调用getsockname()函数将本地地址信息存储在localaddr结构体中
//失败返回-1
```

```c++
//获取套接字的对方地址(套接字连接后)：getpeername()
struct sockaddr_in peeraddr;//初始化结构体peeraddr
int getpeername(int sockfd, struct sockaddr* peeraddr, socklen_t addrlen);
//注意，先初始化结构体peeraddr，调用getpeername()函数将对方地址信息存储在peeraddr结构体中
//失败返回-1
```

### gethostname()、gethostbyname()

```c++
//获取主机名：gethostname()
char host[100] = {0};//初始化host
int gethostname(char* name,size_t len);
//获取主机名，存储在host中
//失败返回-1
```

```c++
//通过主机名获取ip地址：gethostbyname()
#include<netdb.h>
struct hostent* gethostbyname(const char* name);
//ip地址保存在hostent的char** h_addr_list中
struct hostent* hp;
int i = 0;
//hp->h_addr_list[i], 保存的是ip地址的结构体
while(h_addr_list[i]!=NULL){//指针为空时退出
    cout<<inet_ntoa(*(struct in_addr*)hp->h_addr_list[i])<<endl;//转换成点分十进制的ip地址，利用i遍历
	i++;
}
```

### shutdown()

终止网络连接的通常方法是调用close()函数。不过close有两个限制：

（1）close把文件描述符引用计数减1，仅当该计数为0时才关闭套接字

（2）close终止读和写两方面的数据传送，不能只终止其中一方面

利用shutdown()可以解决上面两个问题，shutdown不够引用计数为多少，之间关闭，且可单向关闭

```c++
#include<sys/sock.h>
int shutdown(int sockfd,int howto);//成功返回0，出错返回-1
	sockfd 套接字的文件描述符
	howto参数：
		SHUT_RD 关闭读——套接字不再能接收数据，且接收缓冲区中的数据被丢弃
		SHUT_WR 关闭写——称为半关闭，套接字不再能写数据，但当前发送缓冲区中的数据将被发送
        SHUT_RDWR 读写都关闭
```

### fcntl()

fcntl系统调用可以用来对已打开的文件描述符进行各种控制操作以改变已打开文件的的各种属性

```c++
#include<unistd.h>
#include<fcntl.h>
int fcntl(int fd, int cmd);
int fcntl(int fd, int cmd, long arg);
int fcntl(int fd, int cmd ,struct flock* lock);
```

fcntl函数功能依据cmd的值的不同而不同。参数对应功能如下：

<img src="img\fcntl函数的参数.png" alt="fcntl函数的参数" style="zoom: 80%;" />

（1）F_DUPFD

与dup函数功能一样，复制由fd指向的文件描述符，调用成功后返回新的文件描述符，与旧的文件描述符共同指向同一个文件。

（2）F_GETFD

读取文件描述符close-on-exec标志

（3）F_SETFD

将文件描述符close-on-exec标志设置为第三个参数arg的最后一位

（4）F_GETFL

获取文件打开方式的标志，标志值含义与open调用一致

（5）F_SETF

设置文件打开方式为arg指定方式

（6）F_SETLK

此时fcntl函数用来设置或释放锁。当short_l_type为F_RDLCK为读锁，F_WDLCK为写锁，F_UNLCK为解锁。

如果锁被其他进程占用，则返回-1;

这种情况设的锁遇到锁被其他进程占用时，会立刻停止进程。

（7）F_SETLKW

此时也是给文件上锁，不同于F_SETLK的是，该上锁是阻塞方式。当希望设置的锁因为其他锁而被阻止设置时，该命令会等待相冲突的锁被释放。

（8）F_GETLK

第3个参数lock指向一个希望设置的锁的属性结构，如果锁能被设置，该命令并不真的设置锁，而是只修改lock的l_type为F_UNLCK,然后返回该结构体。如果存在一个或多个锁与希望设置的锁相互冲突，则fcntl返回其中的一个锁的flock结构。


# 2. 并发服务器-进程线程实现

## 2.1 fork()

```c++
pid_t fork(void);//创建一个进程，返回一个文件描述符
//文件描述符<0(或者==-1)，进程创建失败
//文件描述符==0，表示为子进程
//文件描述符>0，表示为父进程
```

```c++
int main ()
{
    pid_t fpid; //fpid表示fork函数返回的值
    int count = 0;
    fpid = fork();
    if (fpid < 0)
        printf("进程创建失败");
    else if (fpid == 0){//子进程
        printf("我是子进程/n");
        count++;
    }
    else{//fpid>0，父进程
        printf("我是父进程/n");
        count++;
    }
    printf("统计结果是: %d/n", count);
    return 0;
}
//运行结果：
我是子进程
统计结果是：1
我是父进程
统计结果是：1
/*注意哦
    两个进程变量不共用(count最后结果为1)，相当于完全复制了一份*/
```

**在语句fpid=fork()之前，只有一个进程在执行这段代码，但在这条语句之后，就变成两个进程在执行了，这两个进程的几乎完全相同，将要执行的下一条语句都是if(fpid<0)……**

为什么两个进程的fpid不同呢，这与fork函数的特性有关。

fork调用的一个奇妙之处就是它仅仅被调用一次，却能够返回两次，它可能有三种不同的返回值：
    1）在父进程中，fork返回新创建子进程的进程ID；**对应父进程中fork返回值>0**
    2）在子进程中，fork返回0；**对应子进程中fork返回值==0**
    3）如果出现错误，fork返回一个负值；

在fork函数执行完毕后，如果创建新进程成功，则出现两个进程，一个是子进程，一个是父进程。在子进程中，fork函数返回0，在父进程中，fork返回新创建子进程的进程ID。

**利用fork创建多个进程，实现并发服务器。父进程用于客户端的请求连接，所以关闭父进程的connfd，子进程用于服务器客户端的通信，因此子进程关闭listenfd**

![fork创建进程](img\fork创建进程.png)

### server.cpp

```c++
//server.cpp
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
			//5.data transfer
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
```

### client.cpp

```c++
//client.cpp
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
```

## 2.2 thread()

多进程并发服务器：主线程处理listenfd事件，每有一个新的连接创建一个子进程去处理。

多线程并发服务器：主线程处理listenfd事件，每有一个新的连接创建一个子线程去处理。

区别：

**每个进程都有自己的资源空间(不共用)，在创建子进程时会先自动把父进程的资源全拷贝过来。**

**子线程和主线程共用一个资源空间，每一次新来的连接都会覆盖客户端信息，所以每个子线程因该自己开辟一个空间存储对应的用户信息(在被覆盖前，从主线程拷贝过来)**



<img src="img\多线程并发服务器.png" alt="多线程并发服务器" style="zoom:67%;" />

```c++
//pthread_create是UNIX环境创建线程的函数
#include <pthread.h>
int pthread_create(pthread_t* tid,const pthread_attr_t* attr,void* (*start_rtn)(void*),void* arg);
    tidp：事先创建好的pthread_t类型的参数。成功时tid指向的内存单元被设置为新创建线程的线程ID
    attr：用于定制各种不同的线程属性,通常直接设为NULL
    start_rtn：新创建线程从此函数开始运行,无参数是arg设为NULL即可
    arg：start_rtn函数的参数。无参数时设为NULL即可。有参数时输入参数的地址。当多于一个参数时应当使用结构体传入。
    返回值：成功返回0，否则返回错误码。
//例子
pthread_t tid;
void do_work()；//函数
pthread_create(&tid, NULL, do_work, (void*)&ts);
//线程号tid，调用do_work()函数，将数据ts传给do_work()函数
```

```c++
//针对pthread_create()中第二个参数设置线程分离属性，这样在线程结束时，回收线程资源
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
//设置线程分离属性，将设置好的attr传入，在线程结束时，回收线程资源
pthread_create(&tid,&attr,do_work,&cliInfo);
```

由于pthread库不是Linux系统默认的库，连接时需要使用库libpthread.a,所以在使用pthread_create创建线程时，在编译中要加-lpthread参数:

```linux
对‘pthread_create’未定义的引用
g++ -g mser_thread.cpp wrap.c -lpthread -o mser
```

### server.cpp

```c++
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
struct cli_info{//pthread_create()只能传一个参数，所以报要传的数据写出一个结构体
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
```

# 3. C/S模型通信过程

## 3.1 TCP的11种状态

**11种状态：LISTEN、SYN_SENT、SYN_RCVD、ESTABLISHED、CLOSING、FIN_WAIT_1、CLOSE_WAIT、FIN_WAIT_2、LAST_ACK、TIME_WAIT、CLOSED**

**过程：CLOSEED->LISTEN->SYN_SENT->SYN_RCVD->ESTABLISHED->...->FIN_WAIT_1->CLOSE_WAIT->FIN_WAIT_2->LAST_ACK->TIME_WAIT->CLOSED**

![TCP的11种状态](img\TCP的11种状态.png)

**CLOSED**：初始状态，表示TCP连接是“关闭着的”或“未打开的”。

**LISTEN** ：表示服务器端的某个SOCKET处于监听状态，可以接受客户端的连接。

**SYN_RCVD** ：表示服务器接收到了来自客户端请求连接的SYN报文。在正常情况下，这个状态是服务器端的SOCKET在建立TCP连接时的三次握手会话过程中的一个中间状态，很短暂，基本上用netstat很难看到这种状态，除非故意写一个监测程序，将三次TCP握手过程中最后一个ACK报文不予发送。当TCP连接处于此状态时，再收到客户端的ACK报文，它就会进入到ESTABLISHED 状态。

**SYN_SENT** ：这个状态与SYN_RCVD 状态相呼应，当客户端SOCKET执行connect()进行连接时，它首先发送SYN报文，然后随即进入到SYN_SENT 状态，并等待服务端的发送三次握手中的第2个报文。SYN_SENT 状态表示客户端已发送SYN报文。

**ESTABLISHED** ：表示TCP连接已经成功建立。

**FIN_WAIT_1** ：这个状态得好好解释一下，其实FIN_WAIT_1 和FIN_WAIT_2 两种状态的真正含义都是表示等待对方的FIN报文。而这两种状态的区别是：FIN_WAIT_1状态实际上是当SOCKET在ESTABLISHED状态时，它想主动关闭连接，向对方发送了FIN报文，此时该SOCKET进入到FIN_WAIT_1 状态。而当对方回应ACK报文后，则进入到FIN_WAIT_2 状态。当然在实际的正常情况下，无论对方处于任何种情况下，都应该马上回应ACK报文，所以FIN_WAIT_1 状态一般是比较难见到的，而FIN_WAIT_2 状态有时仍可以用netstat看到。

**FIN_WAIT_2** ：**半关闭**，上面已经解释了这种状态的由来，实际上FIN_WAIT_2状态下的SOCKET表示半连接，即有一方调用close()主动要求关闭连接。注意：FIN_WAIT_2 是没有超时的（不像TIME_WAIT 状态），这种状态下如果对方不关闭（不配合完成4次挥手过程），那这个 FIN_WAIT_2 状态将一直保持到系统重启，越来越多的FIN_WAIT_2 状态会导致内核crash。

**TIME_WAIT** ：表示收到了对方的FIN报文，并发送出了ACK报文。 TIME_WAIT状态下的TCP连接会等待2*MSL（Max Segment Lifetime，最大分段生存期，指一个TCP报文在Internet上的最长生存时间），然后即可回到CLOSED 可用状态了。如果FIN_WAIT_1状态下，收到了对方同时带FIN标志和ACK标志的报文时，可以直接进入到TIME_WAIT状态，而无须经过FIN_WAIT_2状态。（这种情况应该就是四次挥手变成三次挥手的那种情况）

**CLOSING** ：这种状态在实际情况中应该很少见，属于一种比较罕见的例外状态。正常情况下，当一方发送FIN报文后，按理来说是应该先收到（或同时收到）对方的ACK报文，再收到对方的FIN报文。但是CLOSING 状态表示一方发送FIN报文后，并没有收到对方的ACK报文，反而却也收到了对方的FIN报文。什么情况下会出现此种情况呢？那就是当双方几乎在同时close()一个SOCKET的话，就出现了双方同时发送FIN报文的情况，这是就会出现CLOSING 状态，表示双方都正在关闭SOCKET连接。

**CLOSE_WAIT** ：表示正在等待关闭。怎么理解呢？当对方close()一个SOCKET后发送FIN报文给自己，你的系统毫无疑问地将会回应一个ACK报文给对方，此时TCP连接则进入到CLOSE_WAIT状态。接下来呢，你需要检查自己是否还有数据要发送给对方，如果没有的话，那你也就可以close()这个SOCKET并发送FIN报文给对方，即关闭自己到对方这个方向的连接。有数据的话则看程序的策略，继续发送或丢弃。简单地说，当你处于CLOSE_WAIT 状态下，需要完成的事情是等待你去关闭连接。

**LAST_ACK** ：当被动关闭的一方在发送FIN报文后，等待对方的ACK报文的时候，就处于LAST_ACK 状态。当收到对方的ACK报文后，也就可以进入到CLOSED 可用状态了。

```c++
netstat -a|grep 端口号//查询tcp的状态
```

![TCP状态查询](img\TCP状态查询.png)

**当服务器的子进程终止时，给父进程发送一个SIGCHLD信号，而父进程的默认行为是忽略该信号。既然父进程未处理，那么该子进程就会进入僵尸状态。可通过ps查询进程状态验证。**

## 3.2 信号处理

信号(signal)就是告知进程发生了某个事件的通知，有时也称为软件中断。信号通常是异步发生的，就是说进程预先不会知道信号发生的准确时刻。信号：一个进程发给另一个进程(或本身)、内核发给某个进程。

### signal函数

```c++
signal()函数有两个参数，第一个参数是信号名，第二个参数是函数指针，或者常值SIG_IGN(忽略),SIG_DFL(默认)
```

### wait()&waitpid()

```c++
#include<sys/wait.h>
pid_t wait(int* statloc);
pid_t waitpid(pid_t pid,int* statloc,int options);
//均返回:若成功则返回进程ID，或出错则返回0或-1
```

若调用wait()函数时没有已终止的子进程，不过有一个多多个子进程在执行中，那么**wait()将阻塞到现有的子进程第一个终止为止**。

**waitpid()函数就等待哪个进程以及是否阻塞给了我们更多的控制**。首先，pid参数允许指定想要等待的进程ID，值-1表示等待第一个终止的子进程。其次，options参数允许指定附加项，最常用的附加项是WNOHANG,它告知内核在没有已终止的子进程时不要阻塞。

**建立一个信号处理函数并在其中调用wait()并不足以防止出现僵死进程。**本问题在：所有的信号都在信号处理函数执行之前产生，而信号处理函数只执行一次，因为Unix信号一般是不排队的。更严重的是，本问题是不确定的。在我们刚刚运行的例子中，客户与胀务器在同一个主机！信号处理函数执行 1次。但是如果我们在不同的主机，运行客户和服务器，那么信号处理函效般执行2次：一次是第1个产生的信号引起的，另外的信号在信号处理函教第一次执行时发生，因此该处理函数仅仅再被调川一次，那么信号处理函效般执行2次。

**正确的解决办法是在一循环内调用waitpid()**而不是wait()。这个版本竹用的原因在于：我们在一个循环内调用 wditpid()，以获取所有己终止子进程的状态。我们必须指定WNOHANG选项，它告知waitpid()在有尚未终止的了进程在运行时不要阻塞。我们不能在循环内调用wait()，因为没有办法防止wait()在正运行的子进程尚有未终止时阻塞。

## 解决问题，僵尸进程

对于高并发服务器，每个客户端都会对应一个进程，**客户端关闭后该进程变成僵尸进程，占用系统资源**

![僵尸进程](img\僵尸进程.png)

利用**signal**解决僵尸进程的问题

```c++
#include<signal.h>
signal(SIGCHLD,SIG_IGN);//放在main()函数下面第一句即可
//不建议直接使用这个，建议用signal(SIGCHLD,handle_sigchld);有更大的操作空间
```

![signal解决僵尸进程](img\signal解决僵尸进程.png)

如果一个进程把SIGCHLD的处置设定为SIG_IGN，它的子进程就不会变成僵尸进程，但这种方式只适用于System V和Unix 98，而POSIX明确表示没有规定这样做，**处理僵尸进程的可移植方法就是捕获SIGCHLD，并调用wait或waitpid来处理。**

```c++
void handle_sigchld(int sig){//自己写的，处理僵尸进程
	pid_t pid;
	int stat;
	while((pid = waitpid(-1,&stat,WNOHANG))>0)//while(waitpid(-1,NULL,WNOHANG)>0)这样也也可
		;
	return;
}
int main(){
    signal(SIGCHLD,handle_sigchld);//调用自己写的函数来处理
    ...
}
```

## 解决问题：客户端异常终止

如果客户端和服务器连接后，客户端异常终止(例如客户端断网)，服务器不知道客户端断开，该客户端对应的服务器子进程会一直占用资源。——利用心跳包解决(服务器隔一段时间向客户端发一个消息，若未收到回复则认为客户端已断开)

```c++
int keepAlive =1;
setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&keepAlive,sizeof(keepAlive));
//与端口复用是同一个函数
//该心跳包每2h发送一次
//若自己要求(要求1分钟确认一次是否断开)，一般心跳包都是在应用层自己来写
```



**总结**：回射服务器，在编写过程中我们遇到粘包问题，端口复用，僵尸进程。以上问题已提出解决方案。

还遇见以下等问题：客户端异常终止(心跳包解决)、accpet返回前连接终止、服务器进程终止、服务器主机崩溃、服务器主机崩溃后重启、服务器主机关机——（见UNIX网络编程卷1）

下一个问题：当服务器进程终止时，客户进程没有被告知。其实客户的TCP确实被告知了(服务器进程向客户进程发送FIN)，但是客户进程由于正阻塞于等待用户输入而未收到该通知。将在下面的部分使用select、poll来处理这种情形，它们等待描述符中的任何一个就绪而不是阻塞于单个描述符。

# 4. I/O复用：select、poll、epoll

## 4.1 I/O模型

### 阻塞等待

**多进程、多线程实现的并发，消耗资源，cpu未充分利用**

系统为每个并发的客户分配一个进程，该进程只处理该客户信息。由于IO事件的时间较慢，所以进程通常都是阻塞在IO处，浪费系统资源，cpu未充分利用。

<img src="img\阻塞等待.png" alt="阻塞等待" style="zoom:60%;" />



### 非阻塞等待(轮询)

只有一个进程，依次轮询每个连接套接字connfd的状态。状态发生改变时，应用层进行处理。**浪费cpu(轮询)**

<img src="img\非阻塞等待_轮询.png" alt="非阻塞等待_轮询" style="zoom:60%;" />



### 多路I/O复用

只有一个进程，所有套接字(包括listenfd和connfd)都交给内核监听，套接字状态发生时，通知应用层处理。

内核监听的函数通常有：select，poll，epoll

<img src="img\多路IO复用.png" alt="多路IO复用" style="zoom: 50%;" />



## 4.2 select

### 原理

1. select 能监听的文件描述符个数受限于 FD_SETSIZE，一般为 1024

2. 解决 1024 以下客户端时使用 select 是很合适的，但如果链接客户端过多，select 采用的是轮询模型，会大 大降低服务器响应效率，不应在 select 上投入更多精力。

3. ​    **（1）单进程可以打开fd有限制，1024，内核FD_SETSIZE决定；**

   ​    **（2）对文件描述符进行扫描时是线性扫描，即采用轮询的方法，效率较低；**

   ​    **（3）用户空间和内核空间的复制非常消耗资源；**

4. 如果开启大量客户端连接后，任何又关闭，nfds将被撑大，轮询时间变长(可用文件描述符重定向解决 dup2())

5. 大量并发，少量活跃，效率低

```c++
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    nfds:		监控的文件描述符集里最大文件描述符加 1，因为此参数会告诉内核检测前多少个文件描述符的状态
    readfds:	监控有读数据到达文件描述符集合，传入传出参数
    writefds:	监控写数据到达文件描述符集合，传入传出参数
    exceptfds:	监控异常发生达文件描述符集合,如带外数据到达异常，传入传出参数
    timeout:	定时阻塞监控时间，3 种情况
        1.NULL，永远等下去
        2.设置 timeval，等待固定时间
        3.设置 timeval 里时间均为 0，检查描述字后立即返回，轮询

struct timeval {
    long tv_sec; /* seconds */
    long tv_usec; /* microseconds */
};

int FD_ISSET(int fd, fd_set *set);	//文件描述符集合里 fd 是否置1  (判断fd是否在文件描述符集合中)
void FD_CLR(int fd, fd_set *set); 	//文件描述符集合里 fd 位置0	(把fd从文件描述符中删除)
void FD_SET(int fd, fd_set *set); 	//文件描述符集合里 fd 位置1	(把fd添加到文件描述符集合中)
void FD_ZERO(fd_set *set); 			//文件描述符集合里所有位置 0   (把文件描述符集合清空)
```

![select流程](img\select流程.png)

### sever.cpp

```c++
#include<iostream>
#include<string>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#include<sys/times.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include"wrap.h"
using namespace std;

int main(int argc, char *argv[]){
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
	
	//4.初始化要检测的文件描述符集合
	fd_set readset,tempset;
	FD_ZERO(&readset);//清理	
	FD_SET(listenfd,&readset);//将listenfd添加到读集合中
	int nfds = listenfd+1;//监控的文件描述符集里最大文件描述符

	struct sockaddr_in clientaddr;//客户端地址
	socklen_t clientlen =sizeof(clientaddr);	
	char rec[1024]={0};
	while(1){
		tempset=readset;
		int cnum = select(nfds,&tempset,NULL,NULL,NULL);//响应的个数
		for(int i=listenfd;i<nfds;i++){//遍历集合
			cout<<"轮询文件描述符:"<<i<<endl;
			if(i==listenfd&&FD_ISSET(listenfd,&tempset)){//listenfd在集合中
				int connfd = Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
				char rback[]="Connect successly!";
				write(connfd,rback,sizeof(rback));
				memset(rback,0,sizeof(rback));
				cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
				cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
				FD_SET(connfd,&readset);
				nfds = connfd+1>nfds?connfd+1:nfds;
				memset(&clientaddr,0,sizeof(clientaddr));
				if(--cnum<=0)continue;//如果只有listenfd响应了，后面的步骤跳过
			}
			else{
				if(FD_ISSET(i,&tempset)){
					int readlen = Readline(i,rec,sizeof(rec));
					if(readlen==0){//接收的数据长度为0时，退出子进程
						FD_CLR(i,&readset);
						Close(i);
						cout<<"close a client"<<endl;
						continue;
					}
					getpeername(i, (struct sockaddr*)&clientaddr,&clientlen);
					cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                    cout<<" port:"<<ntohs(clientaddr.sin_port);
					cout<<"  "<<rec<<endl;
					memset(rec,0,sizeof(rec));	
					memset(&clientaddr,0,sizeof(clientaddr));
				}
			}
		}	
	}

	return 0;
}
```

## 4.3 poll

### 原理

```c++
#include <poll.h>
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
	fds:		监听数组(存放pollfd的结构体)的首元素地址
    nfds:		数组有效元素的最大下标+1	监控数组中有多少文件描述符需要被监控
    timeout:	超时时间 毫秒级等待
				0:  立即返回，不阻塞进程
    			-1: 阻塞, 检测的fd有变化解除阻塞
    			>0: 阻塞时长, 单位毫秒
返回值:
    -1: 失败
    >0(n): 检测的集合中有n个文件描述符发生状态变化

struct pollfd {
    int fd; /* 文件描述符 */
    short events; /* 监控的事件 */
    short revents; /* 监控事件中满足条件返回的事件 */
};
    // events事件
    - POLLIN -> 检测读
    - POLLOUT -> 检测写
```

```c++
struct pollfd myfd;
myfd.fd = 5;
myfd.events = POLLIN | POLLOUT; // 检测读写
struct pollfd myfd[100];
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

![poll的事件信息](img\poll的事件信息.png)

<img src="img\poll流程.png" alt="poll流程" style="zoom:60%;" />

**调用过程和select类似**

相较于 select 而言，poll 的优势：

1. 传入、传出事件分离。无需每次调用时，重新设定监听事件。
2. 采用链表的方式替换原有fd_set数据结构，而使其**没有连接数的上限**，能监控的最大上限数可使用配置文件调整。

### server.cpp

```c++
#include<iostream>
#include<string>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#include<sys/times.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<poll.h>
#include"wrap.h"
using namespace std;

#define OPEN_MAX 100


int main(int argc, char *argv[]){
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
	Listen(listenfd,100);
	
	//4.poll
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in clientaddr;
	socklen_t clientlen = sizeof(clientaddr);
	memset(&clientaddr,0,clientlen);

	client[0].fd=listenfd;
	client[0].events=POLLIN;

	for(int i =1;i<OPEN_MAX;i++){
		client[i].fd=-1;//初始化,fd=-1的位置不监听
	}
	int maxi=0;//client[]中有效位置的最大下标

	char rec[1024]={0};
	memset(rec,0,sizeof(rec));
	while(1){
		int cnum=poll(client,maxi+1,-1);
		cout<<"cnum:"<<cnum<<endl;
		if(client[0].revents&POLLIN){//有客户连接，为什么用&搞不懂
			int connfd = Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
			char rback[]="Connect successly!";
			write(connfd,rback,sizeof(rback));
			memset(rback,0,sizeof(rback));
			cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
			cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
			memset(&clientaddr,0,clientlen);
			//将新连接的connfd加入client[]中
			for(int i=1;i<OPEN_MAX;i++){
				if(client[i].fd==-1){//找到一个最近的空位置，将新的连接放入
					client[i].fd=connfd;
					client[i].events=POLLIN;
					maxi=maxi>i?maxi:i;
					cout<<"maxi:"<<maxi<<endl;
					break;
				}
			}
			if(--cnum<=0)continue;//如果只有linstenfd响应，后面的就跳过
		}
		for(int i=1;i<=maxi;i++){
			cout<<"轮询："<<i<<endl;
			if(client[i].fd==-1)continue;
			if(client[i].revents&POLLIN){
				int readlen = Readline(client[i].fd,rec,sizeof(rec));
				if(readlen<0){
					cout<<"read error!"<<endl;
					if(--cnum<=0) break;//若只有这一件事响应，则执行完退出轮询
					continue;//此处事件为读异常，后面的读事件不用执行
				}
				if(readlen==0){//接收的数据长度为0时，退出子进程
					Close(client[i].fd);
					client[i].fd=-1;
					cout<<"close a client"<<endl;
					if(--cnum<=0) break;//若只有这一件事响应，则执行完退出轮询
					continue;//此次响应为客户端断开，后面的读事件不用执行
				}
				getpeername(client[i].fd, (struct sockaddr*)&clientaddr,&clientlen);
				cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                cout<<" port:"<<ntohs(clientaddr.sin_port);
				cout<<"  "<<rec<<endl;
				memset(rec,0,sizeof(rec));	
				memset(&clientaddr,0,sizeof(clientaddr));			
				if(--cnum<=0) break;//若只有这一件事响应，则执行完退出轮询
			}
		}
	}
	Close(listenfd);
	return 0;
}
```

## 4.4 epoll

int **epoll_create**(int size);

struct **epoll_event**;

typedef union **epoll_data**;

int **epoll_ctl**(int epfd, int op, int fd, struct epoll_event *event);

int **epoll_wait**(int epfd, struct epoll_event *events, int maxevents, int timeout);

### 原理

```c++
步骤：
#include<sys/epoll.h>
(1)创建红黑树模型
	int epoll_create(int size);
	参数	size:要监听的文件描述符上限，2.6版本后写1 即可，可自动扩展
	返回	成功，返回树的句柄，失败: -1

(2)上树，下树，修改节点
    struct epoll_event {
        uint32_t events; /* Epoll events */
        epoll_data_t data; /* User data variable */
    };
    typedef union epoll_data {
        void *ptr;
        int fd; // 常用的一个成员
        uint32_t u32;
        uint64_t u64;
    } epoll_data_t;
    - events:
        - EPOLLIN: 读事件, 检测文件描述符的读缓冲区, 检测有没有数据
        - EPOLLOUT: 写事件, 检测文件描述符的写缓冲区, 检测是不是可写(有内存空间就可写)
    - data.fd 为 epoll_ctl() 第三个参数的的fd的值
    
	int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    参数:
        - epfd: epoll_create() 函数的返回值, 找到epoll的实例
        - op:
            - EPOLL_CTL_ADD: 添加新节点
            - EPOLL_CTL_MOD: 修改已经添加到树上的节点是属性，比如原来检测的是 读事件, 可以修改为写事件
            - EPOLL_CTL_DEL: 将节点从树上删除
        - fd: 要操作的文件描述符
        - event:上述的结点，设置要检测的文件描述符的什么事件，结点与文件描述符绑定
    返回值:成功: 0 失败: -1

(3)监听
	// 这是一个阻塞函数
	// 委托内核检测epoll树上的文件描述符状态, 如果没有状态变化, 该函数默认一直阻塞
	// 有满足条件的文件描述符被检测到, 函数返回
    int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    参数:
    - epfd epoll_create() 函数的返回值, 找到epoll的实例，理解为树根结点
    - events: 传出参数, 里边记录了当前这轮检测epoll模型中有状态变化的文件描述符信息
    	- 这个参数是一个结构体数组的地址
    - maxevents: 指定第二个参数 events 数组的容量
    - timeout: 超时时长, 单位 ms, 和poll是一样的
    - -1: 委托内核检测epoll树上的文件描述符状态, 如果没有状态变化, 该函数默认一直阻塞,有满足条件的文件描述符被检测到, 函数返回
    - 0: epoll_wait() 调用之后, 函数马上返回
    - >0: 委托内核检测epoll树上的文件描述符状态, 如果没有状态变化,但是timeout时间到达了,函数被强制解除阻塞
    返回值:成功: 有多少文件描述符发生了状态变化

(4)返回监听变化，应用层处理
	应用层对返回的 events[](有状态变化的文件描述符信息) 进行处理
```

<img src="img\epoll流程图_1.png" alt="epoll流程图_1" style="zoom:67%;" />

![epoll流程图_2](img\epoll流程图_2.png)

跨平台: 不支持, 只支持linux

检测的连接数 和内存有关系 

检测方式和效率 树状( 红黑树 )模型, 检测效率很高 内部处理是基于事件的, 和libevent是对应的

委托epoll检测的文件描述符集合用户和内核使用的是同一块内存, 没有数据的拷贝 使用了共享内存 

传出的信息的量: 有多少文件描述符发送变化了 -> 返回值 可以精确的知道到底是哪个文件描述符发生了状态变化

### server.cpp	LT模式

```c++
#include<iostream>
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
#include"wrap.h"
using namespace std;
/*
epoll 测试 服务器端
LT 水平工作模式
	工作模式默认是水平模式，缓冲区有数据就调用一次epoll_wait
*/

int main(int argc, char *argv[]){
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
	Listen(listenfd,100);
	
	//4.epoll
	//(1)创建epoll模型，创建红黑树
	int epfd = epoll_create(1);
	
	//(2)入树，将要检测的文件描述符入树
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=listenfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	
	//(3)监测
	struct epoll_event evs[1024];//用于接收epoll监听的返回值
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
	char rec[1024]={0};	
	while(1){
		cout<<"epoll_wait()..."<<endl;
		int cnum=epoll_wait(epfd,evs,1024,-1);
		cout<<"epoll_wait()完成 "<<"cnum:"<<cnum<<endl;
		for(int i=0;i<cnum;i++){
			//cout<<"i:"<<i<<endl;
			if(!(evs[i].events&EPOLLIN))continue;//若不是读事件忽略
			if(evs[i].data.fd==listenfd){//lfd响应，新客户连接
				int connfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
        		ev.data.fd=connfd;
        		ev.events=EPOLLIN;//工作模式默认是水平模式，缓冲区有数据就调用一次epoll_wait
        		epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
				char rback[]="Connect successly!";
                write(connfd,rback,sizeof(rback));
				cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
				cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
				memset(&clientaddr,0,sizeof(clientaddr));
				memset(rback,0,sizeof(rback));
				continue;
			}
			else{
				cout<<"read... ";
				int readlen = Readline(evs[i].data.fd,rec,sizeof(rec));
				cout<<"readlen="<<readlen<<endl;
				if(readlen<0){cout<<"read error!";continue;}
				if(readlen==0){//接收的数据长度为0时，退出子进程
					epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
					Close(evs[i].data.fd);
					cout<<"close a client"<<endl;
					continue;
				}
				getpeername(evs[i].data.fd, (struct sockaddr*)&clientaddr,&clientlen);
				cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                cout<<" port:"<<ntohs(clientaddr.sin_port);
				cout<<"  "<<rec<<endl;
				memset(rec,0,sizeof(rec));	
				memset(&clientaddr,0,sizeof(clientaddr));	
			}
			cout<<endl;
		}
	} 
	Close(listenfd);
	return 0;
}
```

### epoll 的工作模式

两种工作模式：**水平模式(LT)，边沿模式(ET)**

- **水平模式**, 默认的工作的模式是水平模式

  - LT(level triggered)

  - **阻塞和非阻塞的套接字都是支持的**

  - 阻塞指定的接收和发送数据的状态
    - read/recv
    - write/send

- 边沿模式

  - ET(edge-triggered)
  - **效率高, 只支持非阻塞的套接字**

**LT模式** 

LT(level triggered)是**缺省的工作方式**，并且**同时支持block和no-block socket**。

内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的fd进行IO操作。如果你不作任何操作，内核还是会继续通知你的。

**epoll默认是水平模式 LT**

```c++
/*
- 读事件: 
	在这种场景下, 只要是epoll_wait检测到读缓冲区有数据, 就会通知用户一次
		- 不管数据有没有读完, 只要有数据就通知
		- 通知就是 epoll_wait() 函数返回, 我们就可以处理传出参数中的文件描述符的状态
- 写事件:
	检测写缓冲区是否可用(是否有容量), 只要是可写(有容量)epoll_wait()就会返回
*/
```

下面是客户端发送1234567890和abcdefg两次数据，服务器端接收rec[4] (一次最多接收4个字符)，LT模式下可以看到多次调用epoll_wait()，文件描述符的读缓冲区有数据就会一直调用epoll_wait()，直至读完

![epoll水平模式](img\epoll水平模式.png)

**ET模式**

ET(edge-triggered)是**高速工作方式**，**只支持no-block socket**。在这种模式下，当描述符从未就绪变为就绪时，内核通过epoll**通知一次**

ET模式在很大程度上减少了epoll事件被重复触发的次数，因此效率要比LT模式高。epoll工作在ET模式的时候，必须使用非阻塞套接口，以避免由于一个文件句柄的阻塞读/阻塞写操作把处理多个文件描述符的任务饿死。

```c++
/*
特点: epoll_wait()检测的次数变少了, 效率变高了(有满足条件的新状态才会通知)
- 读事件: 
	- 接收端每次收到一条(***新的***)数据, epoll_wait() 只会通知一次，不管你有没有将数据读完
- 写事件:
	- 检测写缓冲区是否可用(是否有容量)，通知一次
	- 写缓冲区原来是不可用(满了), 后来缓冲区可用(不满), epoll_wait()检测到之后通知一次(唯一)
*/
```

下面是客户端发送1234567890和abcdefg两次数据，服务器端接收rec[4] (一次最多接收4个字符)，ET模式下可以看到两次调用epoll_wait()，因为客户端只发送两两次数据，对应文件描述符发生两次变化

![epoll边沿模式](img\epoll边沿模式.png)

**如何设置边沿模式?**

```c++
// 在struct epoll_event 结构体的成员变量 events 事件中额外设置 EPOLLET
// 往epoll模型上添加新节点
int cfd = accept(lfd, NULL, NULL);
// cfd 添加到检测的原始集合中
ev.events = EPOLLIN | EPOLLET; // 设置文件描述符的边沿模式
ev.data.fd = cfd;
epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
```

通过测试如果epoll_wait()只通知一次, 并且接收端接收数据的缓存比较小, 导致服务器端通信的文件描述符中的数据越来越多, 数据如果不能全部读出, 就无法处理客户端请求, 如果解决这个问题?

```c++
// 解决方案, 在epoll_wait() 通知的这一次中, 将客户端发送的数据全部读出
    - 循环的进行数据接收
        - 需要使用这种方案, 但是有问题, 会导致服务器端程序的阻塞在read()
            while(1)
            {
                int len = read(cfd, buf, sizeof(buf));
                // 读完之后需要跳出循环
                // 如果客户端和服务器的连接还保持着, 如果数据接收完毕, read函数阻塞
                // 服务器端程序的单线程/进程的, read阻塞会导致整个服务器程序阻塞
            }
		- 解决上述的问题: 将数据的接收动作修改为非阻塞
            - read()/recv(), write()/send()阻塞是函数行为, 还是操作的文件描述符导致的?
            - 调用这些函数都是去检测操作的文件描述符的读写缓冲区 => 是文件描述符导致的
```

**如何设置文件描述符的非阻塞?**

```c++
// 使用fcntl函数设置文件描述符的非阻塞
#include <fcntl.h>
int fcntl(int fd, int cmd, ... /* arg */ );
// 因为文件描述符行为默认是阻塞的, 因此要追加非阻塞行为
// 获取文件描述符的flag属性
int flag = fcntl(cfd, F_GETFL);
// 给flag追加非阻塞
flag = flag | O_NONBLOCK; // 或 flag |= O_NONBLOCK;
// 将新的flag属性设置到文件描述符中
fcntl(cfd, F_SETFL, flag);
```

**在非阻塞模式下读完数据时遇到的错误，此时应该跳出循环**

```c++
// recv error: Resource temporarily unavailable -> 资源不可用, 因为内存中没有数据了
// 错误出现的原因:
#include<errno.h>
	while(1){
        int raadlen = read();
        if(readlen==-1&&errno = EAGAIN)//read()本来阻塞在这里，这时应该跳出循环
    }
循环的读数据, 当通信的文件描述符对应读缓冲区数据被读完, recv/read 不会阻塞, 继续读缓冲区
但是缓冲区中没有数据, 这时候read/recv 调用就失败了, 返回 -1, 这时候错误号 errno的值为:
 errno = EAGAIN or EWOULDBLOCK , 一般情况下使用 EAGAIN 判断就可以了
```

### server.cpp ET模式

```c++
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
/*
epoll 测试 服务器端
ET 边沿工作模式
	发生变化时只调用一次epoll_wait
*/

int main(int argc, char *argv[]){
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
	Listen(listenfd,100);
	
	//4.epoll
	//(1)创建epoll模型，创建红黑树
	int epfd = epoll_create(1);
	
	//(2)入树，将要检测的文件描述符入树
	struct epoll_event ev;
	ev.events=EPOLLIN|EPOLLET;
	ev.data.fd=listenfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	
	//(3)监测
	struct epoll_event evs[1024];//用于接收epoll监听的返回值
	struct sockaddr_in clientaddr;
	socklen_t clientlen =sizeof(clientaddr);
	char rec[4]={0};	
	while(1){
		cout<<"epoll_wait()..."<<endl;
		int cnum=epoll_wait(epfd,evs,1024,-1);
		cout<<"epoll_wait()完成 "<<"cnum:"<<cnum<<endl;
		for(int i=0;i<cnum;i++){
			//cout<<"i:"<<i<<endl;
			while(1){
				if(!(evs[i].events&EPOLLIN))break;//若不是读事件忽略
				if(evs[i].data.fd==listenfd){//lfd响应，新客户连接
					int connfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
        			//因为文件描述符行为默认是阻塞的, 因此要追加非阻塞行为
					//fcntl系统调用:对已打开的文件描述符进行控制操作,改变已打开文件的各种属性
					int flag=fcntl(connfd,F_GETFL);//获取文件描述符的flag属性
					flag |= O_NONBLOCK;//给flag追加非阻塞
					fcntl(connfd,F_SETFL,flag);//将新的flag属性设置到文件描述符中
					ev.data.fd=connfd;
        			ev.events=EPOLLIN|EPOLLET;
        			epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
					char rback[]="Connect successly!";
					write(connfd,rback,sizeof(rback));
					cout<<"New Connect"<<" ip:"<<inet_ntoa(clientaddr.sin_addr);
					cout<<" port:"<<ntohs(clientaddr.sin_port)<<endl;
					memset(&clientaddr,0,sizeof(clientaddr));
					memset(rback,0,sizeof(rback));
					break;
				}	
				else{//connfd响应的读事件
					cout<<"read... ";
					int readlen = Readline(evs[i].data.fd,rec,sizeof(rec));
					cout<<"readlen="<<readlen<<endl;
					if(readlen<0){
						//缓冲区读干净了，若设置阻塞套接字时read会阻塞在这里
						//设置非阻塞套接字，read会返回错误信息EAGAIN，此时跳出循环
						if(errno==EAGAIN) break;
						//若为其他错误，关闭套接字，下树，跳出循环
						cout<<"read error!";
						epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
                        Close(evs[i].data.fd);
						break;
					}
					if(readlen==0){//接收的数据长度为0时，退出子进程
						epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
						Close(evs[i].data.fd);
						cout<<"close a client"<<endl;
						break;
					}
					getpeername(evs[i].data.fd, (struct sockaddr*)&clientaddr,&clientlen);
					cout<<"From ip:"<<inet_ntoa(clientaddr.sin_addr);
                	cout<<" port:"<<ntohs(clientaddr.sin_port);
					cout<<"  "<<rec<<endl;
					memset(rec,0,sizeof(rec));
					memset(&clientaddr,0,sizeof(clientaddr));
				}
			}
			cout<<endl;
		}
	} 
	Close(listenfd);
	return 0;
}
```

## 4.5 client.cpp

客户端代码，select、poll、epoll相同

```c++
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
```



## 4.6 select、poll、epoll总结

#### 时间复杂度

| select、poll、epoll时间复杂度                                |
| ------------------------------------------------------------ |
| **select==>时间复杂度O(n)**<br/>它仅仅知道了，有I/O事件发生了，却并不知道是哪那几个流（可能有一个，多个，甚至全部），我们只能**无差别轮询**所有流，找出能读出数据，或者写入数据的流，对他们进行操作。所以select具有O(n)的无差别轮询复杂度，**同时处理的流越多，无差别轮询时间就越长**。 |
| **poll==>时间复杂度O(n)**<br/>poll本质上和select没有区别，它将用户传入的数组拷贝到内核空间，然后查询每个fd对应的设备状态， 但是它**没有最大连接数的限制**，原因是它是基于链表来存储的. |
| **epoll==>时间复杂度O(1)**<br/>**epoll可以理解为event poll**，不同于忙轮询和无差别轮询，epoll会把哪个流发生了怎样的I/O事件通知我们。所以我们说epoll实际上是事件驱动（每个事件关联上fd）的，此时我们对这些流的操作都是有意义的。（复杂度降低到了O(1)） |

#### 同步I/O

select，poll，epoll都是IO多路复用的机制。I/O多路复用就通过一种机制，可以监视多个描述符，一旦某个描述符就绪(一般是读就绪或者写就绪)，能够通知程序进行相应的读写操作。但**select，poll，epoll本质上都是同步I/O，因为他们都需要在读写事件就绪后自己负责进行读写，也就是说这个读写过程是阻塞的，而异步I/O则无需自己负责进行读写，异步I/O的实现会负责把数据从内核拷贝到用户空间。**

#### 如何选择

1、epoll是Linux所特有，select则应该是POSIX所规定，windows、linux都有

2、多连接，少活跃，选择epoll

3、在连接数少并且连接都十分活跃的情况下，select和poll的性能可能比epoll好，因为epoll的通知机制需要很多函数回调。

4、select低效是因为每次它都需要轮询，但低效也是相对的，视情况而定，也可通过良好的设计改善

## 4.7 epoll反应堆

### 原理

前面的epoll对应每个事件的发生，都需要去处理(如果是IO事件，处理较慢，相当于阻塞)，epoll反应堆的思想就是当某个时间发生了，自动的去处理这 个事件(先从内核区拷贝到用户区，后面可以用户再处理)

这样的思想对我们的编码来说就是设置回调，将文件描述符，对应的事件，和事件产生时的处理函数封装 到一起，这样当某个文件描述符的事件发生了，回调函数会自动被触发，这就是所谓的反应堆思想。

![epoll反应堆原理](img\epoll反应堆原理.png)

**epoll反应堆流程**

![epoll反应堆流程](img\epoll反应堆流程.png)

一般我们先会给epoll_event中的data进行赋值，然后再epoll_wait返回后取出来进行使用

**切记不要同时给data.fd和data.ptr赋值，会覆盖，先赋值的那个将无意义，data.fd和data.ptr只能用其中一个**

```c++
//**epoll反应堆流程**
epoll_create(); // 创建监听红黑树
epoll_ctl(); // 向书上添加监听fd
epoll_wait(); // 监听
有客户端连接上来--->lfd调用acceptconn()--->将cfd挂载到红黑树上监听其读事件--->
epoll_wait()返回cfd--->cfd回调recvdata()--->将cfd摘下来监听写事件--->...--->
epoll_wait()返回cfd--->cfd回调senddata()--->将cfd摘下来监听读事件--->...--->
//目前的代码是只监听读事件
```

### epoll反应堆 server.cpp

```c++
//自定义结构体
struct myevent{
	int fd;
	int events;//要处理的时间
	void *arg;//指向自己结构体指针
	void (*call_back)(int fd,void *arg);//回调函数，函数指针
	int status;//0：未上树，1：已上树
	char buf[BUFLEN];
	int len;
};
//创建一个myevent结构体
void set_myevent(struct myevent *myev,int fd,void (*call_back)(int fd,void *arg),int events);
//构造一个对应myevent的内核结构体epoll_event，并将epoll_event上树
void eventadd(int efd,struct myevent *myev);
//从epoll红黑树上删除一个epoll_event结构体，下树，并修改对应的myevent
void eventdel(int efd,struct myevent *myev);
//当listenfd文件描述符就绪, 调用该函数完成与客户端的连接
void acceptconn(int lfd,void *arg);
//当connfd文件描述符就绪，且为读事件，调用此函数完成读
void recvdata(int fd,void *arg);
//当connfd文件描述符就绪，且为写事件，调用此函数完成写
void senddata(int fd,void *arg);
```

```c++
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
    //data.fd和data.ptr只能用其中一个,不要两个都赋值了
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
```

上述代码参考：https://blog.csdn.net/daaikuaichuan/article/details/83862311?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164294855916780265490444%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164294855916780265490444&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-83862311.first_rank_v2_pc_rank_v29&utm_term=epoll%E5%8F%8D%E5%BA%94%E5%A0%86&spm=1018.2226.3001.4187

## 4.8 epoll线程池

线程池是一个抽象概念，可以简单的认为若干线程在一起运行，线程不退出，有任务时线程执行任务，无任务时等待。

 **为什么要有线程池？**

1. 利用多线程处理高并发时，对于多个请求每次都去建立线程，这样线程的创建和销毁也会有很大的系统开销，使用上效率很低。 
2.  创建线程并非多多益善，所以提前创建好若干个线程，不退出，等待任务的产生，去接收任务处理后等待下一个任务。 

**线程池如何实现？**需要思考 2 个问题？

1. 假设线程池创建了，线程们如何去协调接收任务并且处理？ 
2. 线程池上的线程如何能够执行不同的请求任务？ 

上述**问题 1 解决思路是操作系统资源分配思路，助互斥锁和条件变量来搞定。**就很像我们之前学过的生产者和消费者模型，客户端对应生产者，服务器端这边的线程池对应消费者，再用互斥锁和条件变量来搞定。 **问题 2 解决思路就是利用回调机制**，我们同样可以借助结构体的方式，对任务进行封装，比如任务的数据和任 务处理回调都封装在结构体上，这样线程池的工作线程拿到任务的同时，也知道该如何执行了。

<img src="img\epoll线程池原理.png" alt="epoll线程池原理" style="zoom: 50%;" />

**任务队列中的任务包含了要执行的事件，及其回调函数等信息。**

# 5. UDP

### 原理

```
TCP：传输控制协议 安全可靠 丢包重传 面向连接
UDP：用户数据报协议 不安全不可靠 丢包不重传 快 不面向连接
```

```
TCP通信流程：
服务器：创建流式套接字 绑定 监听 提取连接 读写 关闭
客户端：创建流式套接字 连接 读写 关闭
收发数据
read recv
write send
```

```c++
UDP通信流程：
服务器：创建报式套接字 绑定 读写 关闭
客户端：创建报式套接字 读写 关闭
发数据
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
    dest_addr 	目标地址信息
    addrlen		结构体大小
收数据
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
	dest_addr 	对方地址信息
    addrlen		结构体大小
```

**udp因为无连接，所以每个发送消息都需要包含目的地址 sendto()中 dest_addr 	目标地址信息**

**每个接受的消息都要提取出对方的地址sendto() 中 dest_addr 	对方地址信息**

### udp server.cpp

```c++
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
```

### udp client.cpp

```c++
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <ctype.h>

#define MAXLINE 128

int main(int argc, char *argv[]){
	struct sockaddr_in servaddr;
	int sockfd, n;
	char buf[MAXLINE];
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.109.128", &servaddr.sin_addr);
	servaddr.sin_port = htons(5000);
	while (fgets(buf, MAXLINE, stdin)!=NULL) {
		n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if(n == -1)perror("sendto error");
		n = recvfrom(sockfd, buf, MAXLINE, 0, NULL, 0);
		if (n == -1)perror("recvfrom error");
		write(STDOUT_FILENO, buf, n);
	}
	close(sockfd);
	return 0;
}
```

# 6. 本地套接字

### 原理

socket API 原本是为网络通讯设计的，但后来在 socket 的框架上发展出一种 IPC 机制，就是 UNIX Domain Socket。 虽然网络 socket 也可用于同一台主机的进程间通讯（通过 loopback 地址 127.0.0.1），但是 UNIX Domain Socket 用于 IPC 更有效率：不需要经过网络协议栈，不需要打包拆包、计算校验和、维护序号和应答等，只是将应用层数据从一 个进程拷贝到另一个进程。

```c++
网络套接字地址结构
struct sockaddr_in {
    _kernel_sa_family_t sin_family; /* Address family */ 地址结构类型
    _be16 sin_port; /* Port number */ 端口号
    struct in_addr sin_addr; /* Internet address */ IP 地址
};
本地套接字地址结构：
struct sockaddr_un {
    _kernel_sa_family_t sun_family; /* AF_UNIX */ 地址结构类型
    char sun_path[UNIX_PATH_MAX]; /* pathname */ socket 文件名(含路径)
};
```

![本地套接字](img\本地套接字.png)

**创建本地套接字->绑定->监听->提取->读写->关闭**

```c++
//创建本地套接字
int socket(int domain, int type, int protocol);
	domian:AF_UNIX
	type:SOCK_STREAM(TCP)
	protoclo:0
//绑定
int bind(int sockfd,struct sockaddr *addr,socklen_t addrlen);
	sockfd:本地套接字
	*addr:本地套接字结构，sockaddr_un
	addrlen:sockaddr_un大小
//后面的类比	
```

### domian_s.cpp

```c++
#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<stddef.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>
#include"wrap.c"
using namespace std;

int main(){
	//创建unix流式套接字
	int lfd=socket(AF_UNIX,SOCK_STREAM,0);

	//绑定
	unlink("sock.s");
	struct sockaddr_un seraddr;
	seraddr.sun_family=AF_UNIX;
	strcpy(seraddr.sun_path,"sock.s");
	bind(lfd,(struct sockaddr*)&seraddr,sizeof(seraddr));

	//监听
	listen(lfd,128);

	//提取
	struct sockaddr_un cliaddr;
	memset(&cliaddr,0,sizeof(cliaddr));
	socklen_t cliaddrlen=sizeof(cliaddr);
	int cfd=accept(lfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
	cout<<"New connect file: "<<cliaddr.sun_path<<endl;

	//读写
	char buf[128]={0};
	while(1){
		int n=Readline(cfd,buf,sizeof(buf));
		cout<<"n:"<<n<<endl;
		if(n<=0){
			cout<<"Close file: "<<cliaddr.sun_path<<endl;
			break;
		}
		else{
			cout<<"From "<<cliaddr.sun_path<<": "<<buf<<endl;
			memset(buf,0,sizeof(buf));
		}
	}
	//关闭
	close(cfd);
	close(lfd);
}
```

### domian_c.cpp

```c++
#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<stddef.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>
#include"wrap.c"
using namespace std;

int main(){
	//创建unix流式套接字
	int cfd=socket(AF_UNIX,SOCK_STREAM,0);

	//如果不绑定就是隐式绑定
	unlink("sock.c");
	struct sockaddr_un cliaddr;
	cliaddr.sun_family=AF_UNIX;
	strcpy(cliaddr.sun_path,"sock.c");
	bind(cfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	//连接
	struct sockaddr_un seraddr;
        seraddr.sun_family=AF_UNIX;
        strcpy(seraddr.sun_path,"sock.s");
	connect(cfd,(struct sockaddr*)&seraddr,sizeof(seraddr));

	//读写
	char sendbuf[128]={0};
	while(1){
		fgets(sendbuf,sizeof(sendbuf),stdin);
		write(cfd,sendbuf,strlen(sendbuf));//注意这个地方用strlen，不要用sizeof
		memset(sendbuf,0,sizeof(sendbuf));
	}
	//关闭
	close(cfd);
}
```

