COMPILER = gcc
CCFLAGS = -Wall -m32 -pedantic -g -std=c99

client: client.c
	$(COMPILER) $(CCFLAGS) client.c -o client

server: server.c
	$(COMPILER) $(CCFLAGS) server.c -o server

acctest: account_test.c
	$(COMPILER) $(CCFLAGS) account_test.c -o acc_test

all: client server

clean:
	rm -rf client server *.dSYM
