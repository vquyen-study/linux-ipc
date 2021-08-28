
> The operation of datagram sockets can be explained by analogy with the postal system:
> 
> 1. The **socket()** system call is the equivalent of setting up a mailbox. (Here, we assume a system like the rural postal service in some countries, which both picks up letters from and delivers letters to the mailbox.) Each application that wants to send or receive datagrams creates a datagram socket using **socket()**.
> 2. In order to allow another application to send it datagrams (letters), an application uses **bind()** to bind its socket to a well-known address. Typically, a server binds its socket to a well-known address, and a client initiates communication by sending a datagram to that address. (In some domains—notably the UNIX domain—the client may also need to use **bind()** to assign an address to its socket if it wants to receive datagrams sent by the server.)
> 3. To send a datagram, an application calls **sendto()**, which takes as one of its arguments the address of the socket to which the datagram is to be sent. This is analogous to putting the recipient’s address on a letter and posting it.
> 4. In order to receive a datagram, an application calls **recvfrom()**, which may block
if no datagram has yet arrived. Because **recvfrom()** allows us to obtain the address of the sender, we can send a reply if desired. (This is useful if the sender’s socket is bound to an address that is not well known, which is typical of a client.) Here, we stretch the analogy a little, since there is no requirement that a posted letter is marked with the sender’s address.
> 5. When the socket is no longer needed, the application closes it using** close()**.


> - Datagram systemcall flows
> 
>![Image](_repo\UDP-socket-systemcall-flows.JPG "Datagram systemcall flows")