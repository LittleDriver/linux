.PHONY:all
all:server client

server:server.c
	gcc -o server server.c -lpthread -g
client:client.c
	gcc -o client client.c -g

.PHONY:clean
clean:
	rm -f server client
