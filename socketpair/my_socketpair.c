#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
//#include<arpa/inet.h>
//#include<netinet/in.h>

int main()
{
	int fd[2];
	int ret = socketpair(AF_LOCAL,SOCK_STREAM,0,fd);
	if(ret != 0)
	{
		perror("socketpair()");
		//return ret;
		return 1;
	}

	pid_t id = fork();
	if(id < 0)
	{
		perror("fork()");
		//return id;
		return 2;
	}
	else if(id == 0)//child
	{
		close(fd[0]);
		char buf[512];
		while(1)
		{
			memset(buf,0,sizeof(buf));
			strcpy(buf,"hello world");
			write(fd[1],buf,sizeof(buf));
			read(fd[1],buf,sizeof(buf));
			sleep(1);
			printf("father#: %s\n",buf);
		}
	}
	else // father
	{
		close(fd[1]);
		char buf[512];
		while(1)
		{
			memset(buf,0,sizeof(buf));
			read(fd[0],buf,sizeof(buf));
			printf("child#: %s\n",buf);
			strcpy(buf,"hello world too!");
			write(fd[0],buf,sizeof(buf));
		}
	}
	return 0;
}
