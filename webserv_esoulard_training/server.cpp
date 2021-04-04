/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 11:28:30 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/04 16:06:05 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

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
**     struct in_addr    sin_addr; 
**     char              sin_zero[8]; 
** }              sockaddr_in;
**
** sockaddr_in is a struct defined in netinet/in.h, used for 
** IP networking, contains all elements that make sense for that 
** specific communication type
*/

typedef struct      s_server { 
    
    int         server_fd;
    int         reuse;
    sockaddr_in address;
    int         addrlen;

    fd_set      active_fd_set; 
    fd_set      read_fd_set;
    int         new_socket;
    int         cur_socket;

}                   t_server;

void init_server(t_server *ws) {

    /*
    ** 1) CREATE A SOCKET
    **
    ** int socket(int domain, int type, int protocol);
    ** domain = communication domain. For IPv4, AF_INET
    ** type = type of service. SOCK_STREAM (virtual circuit service), SOCK_DGRAM (datagram service), SOCK_RAW (direct IP service). Depends on our address family
    ** protocol = in case the type chosen offers several protocols to chose from. But virtual circuit service offers only one form
    */
   
    if ((ws->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw Exception("socket error");

    // to allow for fast restart, otherwise binding fails because port is still in use from previous attempt
    ws->reuse = 1;
    if (setsockopt(ws->server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&ws->reuse, sizeof(ws->reuse)) < 0)
        throw Exception("setsockopt error");
  
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

    memset((char *)&ws->address, 0, sizeof(ws->address));
    ws->addrlen = sizeof(ws->address);
    ws->address.sin_family = AF_INET; // the IPv4 domain we used to open the socket
    ws->address.sin_addr.s_addr = INADDR_ANY; 
    ws->address.sin_port = htons(PORT); // htons() = short int to network
    ws->address.sin_len = ws->addrlen; // just the size of the struct
    memset(ws->address.sin_zero, '\0', sizeof(ws->address.sin_zero));

    // We bind our open socket with its new address

    if (bind(ws->server_fd, (struct sockaddr *)&ws->address, ws->addrlen) < 0)
        throw Exception("bind error");

    /*
    ** 3) WAIT FOR A CONNECTION
    ** int listen(int socket, int backlog)
    ** Tells a socket that it should be capable of accepting incoming connections
    ** backlog: maximum number of pending connections that can be queued up before connections are refused
    */

    if (listen(ws->server_fd, 3) < 0) 
        throw Exception("listen error");
    /*
    ** Initialize the set of active sockets. 
    */

    FD_ZERO (&ws->active_fd_set);
    FD_SET (ws->server_fd, &ws->active_fd_set);
}

void parse_request(t_server *ws) {

    /*
    ** 5) SEND AND RECEIVE MESSAGES
    ** The same read and write system calls that work on files also work on sockets.
    */
    // I think we'll have to use GNL?
    // char buffer[1024] = {0};
    // int valread = read(ws->cur_socket , buffer, 102); 
    // std::cout << "[CLIENT MSG] " << buffer << std::endl;
    // if(valread < 0) 
    //     std::cout << "[No bytes are there to read]" << std::endl; 
   

}

void format_response(t_server *ws) {
    // we are gonna send a message with a proper HTTP header format (or the browser wouldn't accept it)
    char hello[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    write(ws->cur_socket , hello , strlen(hello));
    std::cout << "[--- HELLO MSG SENT ---]" << std::endl;
}

void connection_loop(t_server *ws) {

    std::cout << std::endl << "[--- WAITING FOR NEW CONNECTION ---]" << std::endl;
    ws->read_fd_set = ws->active_fd_set;
    if (select(FD_SETSIZE, &ws->read_fd_set, NULL, NULL, NULL) < 0)
        throw Exception("select error");

    for (ws->cur_socket = 0; ws->cur_socket < FD_SETSIZE; ++ws->cur_socket) {
        if (FD_ISSET (ws->cur_socket, &ws->read_fd_set)) {
            if (ws->cur_socket == ws->server_fd) {
                /* Connection request on original socket. */
                if ((ws->new_socket = accept(ws->server_fd, (struct sockaddr *)&ws->address, (socklen_t*)&ws->addrlen)) < 0)
                    throw Exception("accept error");
                std::cerr << "Server: connect from host " << inet_ntoa (ws->address.sin_addr) << ", port " <<  ntohs (ws->address.sin_port) << std::endl;
                FD_SET (ws->new_socket, &ws->active_fd_set);    
            }
            else {
                /* Data arriving on an already-connected socket. */
                    
                parse_request(ws);
                format_response(ws);
                    
                /*
                ** 6) CLOSE THE SOCKET
                ** The same close() that we use for files
                */

                close(ws->cur_socket);
                FD_CLR (ws->cur_socket, &ws->active_fd_set);
            }
        }
    }
}

int main(void) {

    try {
        t_server ws;
        init_server(&ws);
        while (1)
            connection_loop(&ws);  
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}