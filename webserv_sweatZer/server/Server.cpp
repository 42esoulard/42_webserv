/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 14:51:46 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/19 20:31:22 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {};
Server::~Server() {};

void Server::init_server() {

    /*
    ** 1) CREATE A SOCKET
    **
    ** int socket(int domain, int type, int protocol);
    ** domain = communication domain. For IPv4, AF_INET
    ** type = type of service. SOCK_STREAM (virtual circuit service), SOCK_DGRAM (datagram service), SOCK_RAW (direct IP service). Depends on our address family
    ** protocol = in case the type chosen offers several protocols to chose from. But virtual circuit service offers only one form
    */

    if ((this->_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw Exception("socket error");

    // to allow for fast restart, otherwise binding fails because port is still in use from previous attempt
    this->_reuse = 1;
    if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&this->_reuse, sizeof(this->_reuse)) < 0)
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

    memset((char *)&this->_address, 0, sizeof(this->_address));
    this->_address.sin_family = AF_INET; // the IPv4 domain we used to open the socket
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(ft_stoi(*(_conf.serv_info["server_port"].begin()))); // htons() = short int to network
    //this->_address.sin_len = sizeof(this->_address); // just the size of the struct
    this->_address_sz = sizeof(this->_address); //for ubuntu compatibility since it doesn have sin_len
    memset(this->_address.sin_zero, '\0', sizeof(this->_address.sin_zero));

    // We bind our open socket with its new address
    if (bind(this->_server_fd, (struct sockaddr *)&this->_address, this->_address_sz) < 0)
        throw Exception("bind error");

    /*
    ** 3) WAIT FOR A CONNECTION
    ** int listen(int socket, int backlog)
    ** Tells a socket that it should be capable of accepting incoming connections
    ** backlog: maximum number of pending connections that can be queued up before connections are refused
    */

    if (listen(this->_server_fd, __FD_SETSIZE) < 0) // backlog 3? 10?
        throw Exception("listen error");
    /*
    ** Initialize the set of active sockets.
    */

};

//prints general info on a specific server block
void Server::print_server_info() {

    std::cout << "----SERVER GENERAL INFO----"<< std::endl;

    t_content_map::iterator serv_info_it;
    std::list<std::string>::iterator content_it;

    //printing server content
    serv_info_it = _conf.serv_info.begin();
    while (serv_info_it != _conf.serv_info.end()) {
        content_it = serv_info_it->second.begin();
        std::cout << "KEY [" << serv_info_it->first << "]" << std::endl;
        while (content_it != serv_info_it->second.end()) {
            std::cout << "VALUE [" << *content_it << "]" << std::endl;
            ++content_it;
        }
        ++serv_info_it;
    }
}

//prints all the contents of _conf
void Server::print_server_locations() {

    std::cout << "----SERVER LOCATIONS INFO----"<< std::endl;

    std::list <t_content_map>::iterator loc_it;
    t_content_map::iterator             loc_info_it;
    std::list<std::string>::iterator    content_it;

    int index = 0;
    //printing locations content
    loc_it = _conf.locations.begin();
    while (loc_it != _conf.locations.end()) {
        loc_info_it = (*loc_it).begin();

        std::cout << "----LOCATION " << index++ << "----" << std::endl;

        while (loc_info_it != (*loc_it).end()) {
            content_it = loc_info_it->second.begin();
            std::cout << "KEY [" << loc_info_it->first << "]" << std::endl;

            while (content_it != loc_info_it->second.end()) {
                std::cout << "VALUE [" << *content_it << "]" << std::endl;
                ++content_it;
            }
            ++loc_info_it;
        }
        ++loc_it;
    }
}
