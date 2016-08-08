#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<pthread.h>

static usage(const char* proc)
{
	printf("usage: %s [ip] [port]\n",proc);
}

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}
	
	
//创建一个基于iPv4协议和流式服务的套接字
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		return 2;
	}

//填充本地消息
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(atoi(argv[2]));
	remote.sin_addr.s_addr = inet_addr(argv[1]);

//连接
	if(connect(sock,(struct sockaddr*)&remote,sizeof(remote))< 0)
	{
		perror("connect");
		return 3;
	}


	char buf[1024];
	while(1)
	{
		memset(buf,'\0',sizeof(buf));
		printf("Please Enter: ");
		fflush(stdout);
		ssize_t _s = read(0,buf,sizeof(buf) - 1);
		if(_s > 0)
		{
			buf[_s - 1] ='\0';
			write(sock,buf,strlen(buf));
			_s = read(sock,buf,sizeof(buf));
			if(_s > 0)
			{
				buf[_s] = '\0';
				printf("%s\n",buf);
			}
		}
	}
	return 0;
}
