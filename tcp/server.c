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

static void usage(const char* proc)
{
	printf("usage: %s [ip] [port]\n",proc);
}

void* thread_run(void* arg)
{
	printf("create a new thread\n");
	int fd = (int)arg;
	char buf[1024];
	while(1)
	{
		memset(buf,'\0',sizeof(buf));
		ssize_t _s = read(fd,buf,sizeof(buf) - 1);
		if(_s > 0)
		{
			buf[_s] = '\0';
			printf("client# %s\n",buf);
			write(fd,buf,strlen(buf));
		}
		else if(_s == 0)
		{
			printf("client close...\n");
			break;
		}
		else
		{
			printf("read done...\n");
			break;
		}
	}

	return (void*)0;
}

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}

//创建监听套接字
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock < 0)
	{
		perror("socket");
		return 1;
	}
//填充本地信息
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2])); //argv[2]
	local.sin_addr.s_addr = inet_addr(argv[1]); //argv[1]

//绑定ip,端口号
	if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		return 2;
	}

//监听
	listen(listen_sock,5);


	//4.accept
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);
	while(1)
	{	
		int fd = accept(listen_sock,(struct sockaddr*)&peer,&len);
		if(fd < 0)
		{
			perror("accept");
			return 3;
		}
	
		printf("get a new link,socket -> %s:%d\n",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));

//使用线程的方法处理请求
		pthread_t id;
		pthread_create(&id,NULL,thread_run,(void*)fd);


		pthread_detach(id);
		
		
		//使用进程的方式处理请求，
	//	pid_t id = fork();
	//	if(id == 0)
	//	{
	//		char buf[1024];
	//		while(1)
	//		{
	//			memset(buf,'\0',sizeof(buf));
	//			ssize_t _s = read(fd,buf,sizeof(buf) - 1);
	//			if(_s > 0)
	//			{
	//			 	buf[_s] = '\0';
	//				printf("client# %s\n",buf);
	//				write(fd,buf,strlen(buf));
	//			}
	//			else
	//			{
	//			   	printf("read done...\n");
	//				break;
	//			}
	//		}
	//	}
	//	else //father
	//	{
	//		waitpid(-1,NULL,WNOHANG);
	//	}
	
	
//只能处理一种请求
//    	char buf[1024];
//		while(1)
//		{
//			memset(buf,'\0',sizeof(buf));
//			ssize_t _s = read(fd,buf,sizeof(buf) - 1);
//			if(_s > 0)
//			{
//		    	buf[_s] = '\0';
//				printf("client# %s\n",buf);
//				write(fd,buf,strlen(buf));
//			}
//			else
//			{
//		    	printf("read done...\n");
//				break;
//			}
//		}
	}
	return 0;
}
