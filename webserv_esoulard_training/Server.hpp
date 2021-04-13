/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 14:44:40 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/13 15:36:28 by esoulard         ###   ########.fr       */
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

        Config _conf;

        //     std::map <std::string, std::list <std::string > >               serv_info;
        //     // std::string                 server_host;
        //     // std::string                 server_port;
        //     // std::list<std::string>      server_name;
        //     //------------------------------------------------------------------------

        //     std::list <std::map <std::string, std::list <std::string > > >  locations;
        //     //     std::list<std::string>      path;
        //     //     std::list<std::string>      extensions;
        //     //     std::list<std::string>      accept_methods;
        //     //     std::list<std::string>      root;
        //     //     std::list<std::string>      autoindex;
        //     //     std::list<std::string>      default_error;
        //     //------------------------------------------------------------------------
        //     long int client_max_body_size;
 
        
        // ESTELLE FROM THE FUTURE: 
        // - add another function that'll run at the end of parse_config to check if parameter number is correct for each field,
        // and when possible if value formatting seems ok
        // - save mime.types in a map somewhere
        // - replace default values in init_server with the proper _conf ones
};

#endif