/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tuto_medium.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/01 15:05:44 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/01 17:45:28 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

typedef struct s_sockaddr_in 
{ 
    __uint8_t         sin_len; 
    sa_family_t       sin_family; 
    in_port_t         sin_port; 
    struct in_addr    sin_addr; 
    char              sin_zero[8]; 
}              t_sockaddr_in;

/*
** sockaddr_in is a struct defined in netinet/in.h, used for 
** IP networking, contains all elements that make sense for that 
** specific communication type
*/

int main() {

   

    /*
    ** 1) CREATE A SOCKET
    **
    ** We create a socket with socket(domain, type, protocol)
    ** domain = communication domain. For IPv4, AF_INET
    ** type = type of service. SOCK_STREAM (virtual circuit service), SOCK_DGRAM (datagram service), SOCK_RAW (direct IP service). Depends on our address family
    ** protocol = in case the type chosen offers several protocols to chose from. But virtual circuit service offers only one form
    */

    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        strerror(errno);
        return 0; 
    }

    /*
    ** 2) NAME A SOCKET
    ** = assigning a transport address (= port) to the socket, the address with which we will access it.
    ** This address is defined in a structure which is different depending on the communication type used. It contains info.
    ** For IP, we use a sockaddr_in structure (netinet/in.h), which contains:
    **  - sin_family: the address family (here, IPv4)
    **  - sin_port: the transport address/port. Servers communicate the port to which clients can connect, clients just put 0.
    **  - sin_addr: system address, leave it to default INADDR_ANY
    */

    const int PORT = 8080; // default port
    t_sockaddr_in address;
    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET; // the IPv4 domain we used to open the socket
    address.sin_addr.s_addr = htonl(INADDR_ANY); // htonl() = long int to network
    address.sin_port = htons(PORT); // htons() = short int to network
    address.sin_len = sizeof(t_sockaddr_in); // just the size of the struct

    // We bind our open socket with its new address

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        strerror(errno);
        return 0; 
    };
    return 0;
}