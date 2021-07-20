/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tuto_medium_client.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/02 14:05:12 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/13 12:07:29 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Client side C/C++ program to demonstrate Socket programming
// from Medium tutorial : https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int sock = 0; 
    long valread;
    struct sockaddr_in serv_addr;
    char hello[] = "Hello from client";
    char buffer[1024] = {0};

    // client opens his socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        strerror(errno);
        return -1;
    }
    
    // Initialize the sockaddr structure 
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 address from text to binary form
    // and put it in sin_addr. I think the IP is the dest server IP.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        strerror(errno);
        return -1;
    }
    
    // equivalent of bind() but when u want to connect to a remote address (a server) 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        strerror(errno);
        return -1;
    }

    // initiate transmission of a message from the specified socket to its peer.
    send(sock , hello, strlen(hello)  , 0 );
    std::cout << "[Hello message sent]" << std::endl;
    valread = read( sock , buffer, 1024);
    std::cout << buffer << std::endl;
    return 0;
}