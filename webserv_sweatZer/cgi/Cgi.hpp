/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/15 17:40:23 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/22 17:13:34 by esoulard         ###   ########.fr       */
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

        void    reinit_cgi();

        void    build_env(ServerResponse &serv_resp, t_content_map &cli_conf);
        void    str_to_env(t_content_map &cli_conf);
        int     launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf);
	  	  
        void    parse_content_type(ServerResponse &serv_resp);

    private:
        
        std::vector<std::string>    _headers;
        std::string                 _cgi_body;

        //execve utils
        std::vector<std::string>    s_env;
        char                        *_env[100];
        char                        *_args[3];
        FILE                        *_file[2];
        int                         _fd[2];
        int                         _save[2];
        int                         _pid;
        int                         _status;
        
        //utils
        char                        _buf[_MAXLINE];
        std::string                 _tmp;
        std::vector<std::string>  	_vec;
        std::vector<std::string>    _vec2;

};

#endif
