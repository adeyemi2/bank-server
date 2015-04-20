#Assignment 4: Multithreaded Banking System

### Shreyas Jaganmohan & Will Newman

##Overview

###Server
Our server.c program uses a manager-worker type system to handle sessions. A thread is created on initialization which creates an initial socket and allows incoming connections in the **createSessionAcceptorThread** function. Every connection that comes in is put on its own thread in the **createClientServiceThread** function.

**createClientServiceThread** is also the function which handlies incoming messages from the client, and runs the appropriate command.

Explain how the thread synchronization stuff works.


###Client

The **client.c** program runs two threads: **server_listener** and **client_listener**.

**server_listener** continuously reads from the socket that the client is connected to, and outputs any response received to the user. This thread also will check for SIGINTs received by the client in the case that the server shuts down. 

**client_listener** continuously reads from standard input. It sends any commands received to the server over the socket connect client is connected to. 

The client handles SIGINTs from the user by first sending a message to the server. The server closes any session that the user initiated and then closes the socket