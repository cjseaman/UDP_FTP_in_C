
CC	= gcc
CFLAGS	= -g
PROGS	= server client

all: $(PROGS)

clean:;	$(RM) $(PROGS) *.o core

server: server.h server.c 
	$(CC) -o server server.c
client: client.h client.c 
	$(CC) -o client client.c 
