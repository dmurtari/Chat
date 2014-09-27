# Chat
## Domenic Murtari

Basic chat system using sockets in C++


### Compilation Instructions

The program will successfully compile with:

* `make` to compile.
* `make clean` to get rid of compiled files.


### How to Run

Run the coordinator with `./chat_coordinator`, which will print out a port
number. Then, run a client with `./chat_client 127.0.0.1 port_number`. From 
there, you can enter commands into the chat client.

### Current State of the Program

Currently, most aspects of the original assignment should be implemented. The
chat client can start a new named chat session, which the coordinator forks off
into its own server. Other clients can search for and join existing servers. 
Clients can submit messages to the server, and get the next message or get all
messages that they have not read. Once clients are done chatting, they can leave
the chat or exit the program altogether. 

The only aspect that isn't working is the termination -- while the server will
terminate if it stays idle for 60 seconds, it does not pass on the termination
signal to the coordinator so the coordinator will not remove the server from 
its internal list of chat servers. 

### Known Bugs and Limitations

As mentioned above, the chat coordinator does not know when a server that it
spawned terminates. 

Also, there are some problems with multiple clients communicating with the same
chat server. Messages may not be being kept track of properly for each client. 

The code isn't the prettiest to look at, but it is functional. 