/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 14:44:40 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/22 16:14:15 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
        typedef std::map<std::string, std::list<std::string> >  t_content_map;

        typedef struct          s_conf {
            t_content_map               serv_info;
            // std::list<std::string>      server_host;
            // std::list<std::string>      server_port;
            // std::list<std::string>      server_name;
            // std::list<std::string>      client_max_body_size;
            //------------------------------------------------------------------------

            std::list < t_content_map > locations;
            //     std::list<std::string>      path;
            //     std::list<std::string>      extensions;
            //     std::list<std::string>      accept_methods;
            //     std::list<std::string>      root;
            //     std::list<std::string>      autoindex;
            //     std::list<std::string>      default_error;
            //------------------------------------------------------------------------
        }                       t_conf;

        Server();
        ~Server();

        void init_server();

        Server::t_conf              &get_conf() { return _conf; }
        Server::t_content_map       &get_serv_info() { return _conf.serv_info; }
        std::list < t_content_map > &get_locations() {return _conf.locations;}

        void        print_server_info();
        void        print_server_locations(); 

        int         &get_server_fd()    { return _server_fd; }
        sockaddr_in &get_address()      { return _address; }
        socklen_t   &get_address_sz()   { return _address_sz; }

    private:
        Server::t_conf  _conf;

        int             _server_fd;
        int             _reuse;
        sockaddr_in     _address;
        socklen_t       _address_sz;

};

#endif
