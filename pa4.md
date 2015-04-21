#Assignment 4: Multithreaded Banking System

### Shreyas Jaganmohan & Will Newman

##Overview

###Server
Our server.c program uses a manager-worker type system to handle sessions. A thread is created on initialization which creates an initial socket and allows incoming connections in the **createSessionAcceptorThread** function. Every connection that comes in is put on its own thread in the **createClientServiceThread** function.

**createClientServiceThread** is also the function which handlies incoming messages from the client, and runs the appropriate command.

######Thread Synchronization:
The object that is used continuously in memory by the majority of functions is the ACCOUNTS global variable. This stores the collection of accounts, and also is susceptible to having the program not be thread safe. What this means is that if a certain function changes a piece of information in one of the accounts in the AccountStoragePtr object, another function at the same time in another thread may be trying to use that same account for printing the information. This can cause errors since the information printed would be out of date from the actual, newly changed information. In order to make sure the thread is synchronized, we create a mutual exclusion on the ACCOUNTS variable inside the thread that prints all accounts onto the server every twenty seconds. This locks any other thread trying to access the ACCOUNTS variable (specifically any client service thread), and ensures that all the information is up to date. The functions we specifically use to intiialize POSIX standard mutexes are pthread_mutex_lock, pthread_mutex_unlock and the pthread_mutex_init. 

###Client

The **client.c** program runs two threads: **server_listener** and **client_listener**.

**server_listener** continuously reads from the socket that the client is connected to, and outputs any response received to the user. This thread also will check for SIGINTs received by the client in the case that the server shuts down. 

**client_listener** continuously reads from standard input. It sends any commands received to the server over the socket connect client is connected to. 

The client handles SIGINTs from the user by first sending a message to the server. The server closes any session that the user initiated and then closes the socket
