# TCP Chatroom
The TCP Chatroom is created using the concept of socket programming in C language. It is based on stream sockets i.e. use TCP sockets. The two macros in server specify the max clients which can connect and chat and the other one specifies max length of messages than can be sent. Since TCP is a connection oriented protocol multithreading is used to handle many clients at same time.

## Features
- The server can serve MAX_CLIENT clients.
- All client messages are displayed on server with name for monitoring.
- Server can also undergo input mode.
- In input mode, server can remove user from list of users or send a message to specific user (message visible only to that user).
- While in input mode, all messages from clients will be queued.


## Working
- On server startup, a thread is created for input mode.
- A TCP listening socket is created on main thread.
- On client connection request, chatroom is checked for user limit.
- If user limit is reached, client is notified about the same and is asked to try again later.  
- On successful connection, a separate thread is assigned to client and client name received as first message and is stored in array.
- On receiving a message from client, the same message is sent to every other client connected to server.
- On client exit, server is notified about the same.
- On server exit, all clients are exited with a message of server shutdown.
- Server can enter into input mode by typing input_mode.
