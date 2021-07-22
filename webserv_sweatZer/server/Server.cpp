/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 14:51:46 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/22 16:32:38 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {};
Server::~Server() {};

void Server::init_server() {

    //create socket
    if ((this->_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw Exception("socket error");

    // to allow for fast restart, otherwise binding fails because port is still in use from previous attempt
    this->_reuse = 1;
    if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&this->_reuse, sizeof(this->_reuse)) < 0)
        throw Exception("setsockopt error");

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

    //wait for a connection
    if (listen(this->_server_fd, __FD_SETSIZE) < 0) // backlog 3? 10?
        throw Exception("listen error");

};

//prints general info on a specific server block
void Server::print_server_info() {

    std::cerr << "----SERVER GENERAL INFO----"<< std::endl;

    t_content_map::iterator serv_info_it;
    std::list<std::string>::iterator content_it;

    //printing server content
    serv_info_it = _conf.serv_info.begin();
    while (serv_info_it != _conf.serv_info.end()) {
        content_it = serv_info_it->second.begin();
        std::cerr << "KEY [" << serv_info_it->first << "]" << std::endl;
        while (content_it != serv_info_it->second.end()) {
            std::cerr << "VALUE [" << *content_it << "]" << std::endl;
            ++content_it;
        }
        ++serv_info_it;
    }
}

//prints all the contents of _conf
void Server::print_server_locations() {

    std::cerr << "----SERVER LOCATIONS INFO----"<< std::endl;

    std::list <t_content_map>::iterator loc_it;
    t_content_map::iterator             loc_info_it;
    std::list<std::string>::iterator    content_it;

    int index = 0;
    //printing locations content
    loc_it = _conf.locations.begin();
    while (loc_it != _conf.locations.end()) {
        loc_info_it = (*loc_it).begin();

        std::cerr << "----LOCATION " << index++ << "----" << std::endl;

        while (loc_info_it != (*loc_it).end()) {
            content_it = loc_info_it->second.begin();
            std::cerr << "KEY [" << loc_info_it->first << "]" << std::endl;

            while (content_it != loc_info_it->second.end()) {
                std::cerr << "VALUE [" << *content_it << "]" << std::endl;
                ++content_it;
            }
            ++loc_info_it;
        }
        ++loc_it;
    }
}
