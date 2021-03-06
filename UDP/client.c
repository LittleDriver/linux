#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>

static void usage(const char* proc)
{
	printf("Usage : %s [ip] [port]\n",proc);
}

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		return 1;
	}

	int sock = socket(AF_INET,SOCK_DGRAM,0);
	if(sock < 0)
	{
		perror("socket");
		return 2;
	}

	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(atoi(argv[2]));
	remote.sin_addr.s_addr = inet_addr(argv[1]);

	int done = 0;
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);
	char buf[1024];

	while(!done)
	{
		printf("Please Enter: ");
		fflush(stdout);
		ssize_t _s = read(0,buf,sizeof(buf)-1);
		if(_s > 0)
		{
			buf[_s-1] = '\0';
			sendto(sock,buf,sizeof(buf),0,(struct sockaddr*)&remote,sizeof(remote));

			memset(buf,'\0',sizeof(buf));
			recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&peer,&len);
			printf("server echo %s,socket: %s:%d\n",buf,inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));
		}
	}
	return 0;
}
