#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/select.h>

#define _FD_NUM_ 512
#define _FD_DEFAULT_VAL_ -1

typedef struct _select_fd
{
	int max_fd;
	int fd_arr[_FD_NUM_];
}select_fd_t;

void init_select_set(select_fd_t* select_fd,int listen_fd)
{
	select_fd->max_fd = listen_fd;
	select_fd->fd_arr[0] = listen_fd;
	int i = 1;
	for(;i<_FD_NUM_;++i)
	{
		select_fd->fd_arr[i] = _FD_DEFAULT_VAL_;
	}
}


static void usage(const char* proc)
{
	printf("%s [ip] [port]\n",proc);
}

static void add_read_fd(fd_set* set,select_fd_t* select_fd)
{
	int i = 0;
	for(;i<_FD_NUM_;++i)
	{
		if(select_fd->fd_arr[i] != _FD_DEFAULT_VAL_)
		{
			FD_SET(select_fd->fd_arr[i],set);
			if(select_fd->fd_arr[i] > select_fd->max_fd)
			{
				select_fd->max_fd = select_fd->fd_arr[i];
			}
		}
	}
}

static int add_new_fd(select_fd_t* select_fd,int new_fd)
{
	//当数组满了就会添加失败
	int i = 1;
	for(;i<_FD_NUM_;++i)
	{
		if(select_fd->fd_arr[i] == _FD_DEFAULT_VAL_)
		{
			select_fd->fd_arr[i] = new_fd;
			return 0;
		}
	}
	return -1;
}

static void delete_new_fd(select_fd_t* select_fd,int fd)
{
	int i = 1;
	for(;i<_FD_NUM_;++i)
	{
		if(select_fd->fd_arr[i] == fd)
		{
			select_fd->fd_arr[i] = _FD_DEFAULT_VAL_;
		}
	}
}


static int read_data_show(int new_fd)
{
	char buf[1024];
	ssize_t sz = 0;
	int total = 0;
	while((sz = read(new_fd,buf+total,64)) > 0)
	{
		total += sz;
	}

	if(sz == 0)//success
	{
		buf[total] = '\0';
		printf("client# : %s\n",buf);
		return 0;
	}
	else if(sz < 0)
	{
		perror("read");
		return -1;
	}
	else
	{
		//do nothing
	}
}

int start(const char* ip,int port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("sock");
		exit(1);
	}

	//本地信息填充
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);//点分十进制转化为short int

	//绑定
	if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		exit(1);
	}

	//监听链接
	if(listen(sock,5) < 0)
	{
		perror("listen");
		exit(1);
	}

	return sock;
}

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		usage(argv[1]);
		exit(1);
	}
	int listen_sock = start(argv[1],atoi(argv[2]));

	if(listen_sock < 0)
	{
		perror("start");
		exit(2);
	}

	select_fd_t select_set;
	init_select_set(&select_set,listen_sock);
	fd_set r_set;//可读文件描述符集合

	while(1)
	{
		FD_ZERO(&r_set);
		add_read_fd(&r_set,&select_set);
		struct timeval timeout = {5,0};
		switch(select(select_set.max_fd+1,&r_set,NULL,NULL,&timeout))
		{
			case 0://timeout
				perror("timeout");
				break;
			case -1://error
				perror("select");
				break;
			default:
				{
					int i = 0;
					for(;i<_FD_NUM_;i++)
					{
						int fd = select_set.fd_arr[i];
						if(i == 0 && FD_ISSET(fd,&r_set))//listen_sock
						{
						struct sockaddr_in client;
							socklen_t len = sizeof(client);
							int new_fd = accept(fd,(struct sockaddr*)&client,&len);
							if(new_fd == -1)
							{
							perror("accept");
								continue;
							}
							if( 0 == add_new_fd(&select_set,new_fd))
							{

							}
							else
							{
								close(fd);//处理不了就直接关闭链接
							}
							continue;
						}

						if(fd != _FD_DEFAULT_VAL_ && FD_ISSET(fd,&r_set))
						{
							if(0 == read_data_show(fd))//read success
							{
								delete_new_fd(&select_set,fd);
								close(fd);
							}
							else
							{
								//do nothing
							}
						}
					}
				}

				break;
		}
	}
	return 0;
}
