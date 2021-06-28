/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/15 17:40:23 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/28 11:40:10 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.h"
#include "ServerResponse.hpp"
class ServerResponse;

class Cgi {

    public:
		Cgi();
        ~Cgi() {};
        typedef std::map<std::string, std::list<std::string> >  t_content_map;

        void build_env(ServerResponse &serv_resp, t_content_map &cli_conf);
        int launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf);
		void	parse_content_type(ServerResponse &serv_resp);

    private:

       // char *file_path; // to send as first argument

        char        *_env[100];
		std::vector<std::string>	_headers;
        std::vector<std::string>	s_env;
        FILE        *_file[2];
        int         _fd[2];
        std::string _cgi_body;
        /* CONTENT OF THE ENV SHOULD BE THE FOLLOWING, IN A FORMAT
           such as "key=value" : */
        /*
            AUTH_TYPE
            CONTENT_LENGTH
            CONTENT_TYPE
            GATEWAY_INTERFACE
            PATH_INFO
            PATH_TRANSLATED
            QUERY_STRING
            REMOTE_ADDR
            REMOTE_IDENT
            REMOTE_USER
            REQUEST_METHOD
            REQUEST_URI
            SCRIPT_NAME
            SERVER_NAME
            SERVER_PORT
            SERVER_PROTOCOL
            SERVER_SOFTWARE
        */


};

#endif
