/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tuto_medium.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/01 15:05:44 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/04 16:06:06 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** This is a realisation of this tutorial, showing how to build
** a simple HTTP server in C.
** https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
**
** THIS IS SERVER SIDE.
** The steps are : 
** 1) opening a socket (an access to the network)
** 2) giving this socket an address (a port, with which we bind it)
** 3) listening for a connection
** 4) accepting a connection and creating a new socket for it
** 5) read client message, send a message back
** 6) close the communication socket
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#define PORT 8080 // default port

/*
** struct in_addr {
**     unsigned long s_addr;
** };
** 
** typedef struct sockaddr_in 
** { 
**     __uint8_t         sin_len; 
**     sa_family_t       sin_family; 
**     in_port_t         sin_port; 
**     struct in_addr  sin_addr; 
**     char              sin_zero[8]; 
** }              sockaddr_in;
**
** sockaddr_in is a struct defined in netinet/in.h, used for 
** IP networking, contains all elements that make sense for that 
** specific communication type
*/

int main() {

    /*
    ** 1) CREATE A SOCKET
    **
    ** int socket(int domain, int type, int protocol);
    ** domain = communication domain. For IPv4, AF_INET
    ** type = type of service. SOCK_STREAM (virtual circuit service), SOCK_DGRAM (datagram service), SOCK_RAW (direct IP service). Depends on our address family
    ** protocol = in case the type chosen offers several protocols to chose from. But virtual circuit service offers only one form
    */

    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        strerror(errno);
        return 0; 
    }

    // to allow for fast restart, otherwise binding fails because port is still in use from previous attempt
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    /*
    ** 2) NAME A SOCKET
    ** = assigning a transport address (= port) to the socket, the address with which we will access it.
    **
    ** This address is defined in a structure which is different depending on the communication type used. It contains info.
    ** For IP, we use a sockaddr_in structure (netinet/in.h), which contains:
    **  - sin_family: the address family (here, IPv4)
    **  - sin_port: the transport address/port. Servers communicate the port to which clients can connect, clients just put 0.
    **  - sin_addr: system address, leave it to default INADDR_ANY
    ** int bind(int socket, const struct sockaddr *address, socklen_t address_len);
    */

    sockaddr_in address;
    memset((char *)&address, 0, sizeof(address));
    int addrlen = sizeof(address);
    address.sin_family = AF_INET; // the IPv4 domain we used to open the socket
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT); // htons() = short int to network
    address.sin_len = addrlen; // just the size of the struct
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    // We bind our open socket with its new address

    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    /*
    ** 3) WAIT FOR A CONNECTION
    ** int listen(int socket, int backlog)
    ** Tells a socket that it should be capable of accepting incoming connections
    ** backlog: maximum number of pending connections that can be queued up before connections are refused
    */

    if (listen(server_fd, 3) < 0) {
       strerror(errno);
       exit(EXIT_FAILURE);
    }

    /*
    ** 4) ACCEPT A CONNECTION AND CREATE A NEW SOCKET FOR IT
    ** int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
    ** accept system call grabs the first connection request on the queue of pending connections (set up in listen) 
    ** and creates a new socket for that connection.
    */

    int new_socket;

    while (1) {
        std::cout << std::endl << "[--- WAITING FOR NEW CONNECTION ---]" << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            strerror(errno);            
            exit(EXIT_FAILURE);        
        }

        /*
        ** 5) SEND AND RECEIVE MESSAGES
        ** The same read and write system calls that work on files also work on sockets.
        */

        // I think we'll have to use GNL?
        char buffer[1024] = {0};
        int valread = read(new_socket , buffer, 1024); 
        std::cout << "[CLIENT MSG] " << buffer << std::endl;
        if(valread < 0) 
            std::cout << "[No bytes are there to read]" << std::endl;
        // we are gonna send a message with a proper HTTP header format (or the browser wouldn't accept it)
        char hello[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";//IMPORTANT! WE WILL GET TO IT
        write(new_socket , hello , strlen(hello));
        std::cout << "[--- HELLO MSG SENT ---]" << std::endl;
        
        /*
        ** 6) CLOSE THE SOCKET
        ** The same close() that we use for files
        */

        close(new_socket);
    }

    return 0;
}