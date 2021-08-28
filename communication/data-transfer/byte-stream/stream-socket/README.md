# Notes for socket types
> Socket seperate to two kinds of **socket domains** that depend on the applied domain.
> - Unix domain (same host, localhost, between process - process)
> - Internet domain (use for difference host/machine to host/machine, over internet LAN, WAN...).

- Each **socket domains** has two kinds of data transfer types
  - STREAM (using byte by byte, preserve by sequence, but does not preserve message boundaries)
  - DATAGRAM (using by sequence, and also preserve message boundaries)
  
- [This is reference for socket types](https://stackoverflow.com/questions/13953912/difference-between-unix-domain-stream-and-datagram-sockets)

## 1. STREAM SOCKET.
> Theory : The operation of stream sockets can be explained by analogy with the telephone system:
> 1. The **socket()** system call, which creates a socket, is the equivalent of installing a telephone. In order for two applications to communicate, each of them must create a socket.
> 2. Communication via a stream socket is analogous to a telephone call. One application must connect its socket to another application’s socket before communication can take place. Two sockets are connected as follows:
>> - One application calls **bind()** in order to bind the socket to a well-known address, and then calls **listen()** to notify the kernel of its willingness to accept incoming connections. This step is analogous to having a known telephone number and ensuring that our telephone is turned on so that people can call us.
>> - The other application establishes the connection by calling **connect()**, specifying the address of the socket to which the connection is to be made. This is analogous to dialing someone’s telephone number.
>> - The application that called **listen()** then accepts the connection using **accept()**. This is analogous to picking up the telephone when it rings. If the **accept()** is performed before the peer application calls **connect()**, then the **accept()** blocks (“waiting by the telephone”).
> 3. Once a connection has been established, data can be transmitted in both directions between the applications (analogous to a two-way telephone conversation) until one of them closes the connection using close(). Communication is performed using the conventional **read()** and **write()** system calls or via a number of socketspecific system calls (such as **send()** and **recv()**) that provide additional functionality.



> - Overview of system calls used with stream sockets
> 
>![Image](_repo\Stream-socket-connection-flow.JPG "system calls used with stream sockets")

> - The listen() system call marks the stream socket referred to by the file descriptor sockfd as passive. The socket will subsequently be used to accept connections from other (active) sockets.
> ```
> #include <sys/socket.h> 
> int listen(int sockfd, int backlog);
> 
>                                Returns 0 on success, or –1 on error
> ```
> - We can’t apply ***listen()*** to a connected socket—that is, a socket on which a ***connect()*** has been successfully performed or a socket returned by a call to ***accept()***. 
> - To understand the purpose of the backlog argument, we first observe that the client may call ***connect()*** before the server calls ***accept()***. This could happen, for example, because the server is busy handling some other client(s). This results in a pending connection, as illustrated in Figure 56-2.

> -  A pending socket connection
>
>![Image](_repo\pending-socket-connection.JPG "A pending socket connection")