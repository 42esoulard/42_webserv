/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 14:44:40 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/11 12:23:06 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#define _MAXLINE 655360
#define PORT 8080 // default port

#include "webserv.h"

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

class Server {

    public:
        Server(std::string &config);
        ~Server();

        void init_server(std::string &config);
        void parse_config(std::string &config);

        void parse_request();
        void format_response();
        void handle_connection();

    private:
        std::string _config_file;

        int         _server_fd;
        int         _reuse;
        sockaddr_in _address;

        //socket utils
        fd_set      _active_fd_set; 
        fd_set      _read_fd_set;
        int         _new_socket;
        int         _cur_socket;

        //configuration info
        std::map<std::string, std::list<std::string> > _conf;
        // ^^^ this will contain this vvv
        
        // std::string _server_host;
        // std::string _server_port;
        // std::string _default_error_file;
        // std::string _max_client_body;
        // std::string _accept_methods;
        // std::string _search_dir;
        // std::string _dir_listing;
        // std::string _cgi_ext;
};

#endif