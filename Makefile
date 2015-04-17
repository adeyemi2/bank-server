COMPILER = gcc
CCFLAGS = -Wall -m32 -pedantic -g -std=c99

client: client.c
	$(COMPILER) $(CCFLAGS) client.c -o client

server: server.c
	$(COMPILER) $(CCFLAGS) server.c -o server

all: client server

clean:
	rm -f client server
