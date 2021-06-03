/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/15 17:40:23 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/03 16:56:03 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.h"
#include "ServerResponse.hpp"
#include "ClientRequest.hpp"

class Cgi {

    public:
        Cgi();
        ~Cgi();
        typedef std::map<std::string, std::list<std::string> >  t_content_map;


        void build_env(ServerResponse &serv_resp, t_content_map &cli_conf) {

            char tmp[] = "-1";
            char *_env[17] { "AUTH_TYPE=Basic", //0
                        "CONTENT_LENGTH=", //1
                        "CONTENT_TYPE=", //2
                        "GATEWAY_INTERFACE=CGI/1.1", //3
                        "PATH_INFO=", //4
                        "PATH_TRANSLATED=", //5
                        "QUERY_STRING=", //6
                        "REMOTE_ADDR=", //7
                        "REMOTE_IDENT=", //8
                        "REMOTE_USER=", //9
                        "REQUEST_METHOD=", //10
                        "SCRIPT_NAME=", //11
                        "SERVER_NAME=", //12
                        "SERVER_PORT=", //13
                        "SERVER_PROTOCOL=HTTP/1.1", //14
                        "SERVER_SOFTWARE=webserv/1.0"}; //15 /* LAST ONE SHOULD BE NULL*/
            _env[16] = NULL;

            //"AUTH_TYPE=Basic",
            
            //"CONTENT_LENGTH=",
            if (cli_conf.find("content-length") != cli_conf.end())
                _env[1] = ft_strjoin(_env[1], (*cli_conf["content-length"].begin()).c_str());
            else
                _env[1] = ft_strjoin(_env[1], tmp);

            //"CONTENT_TYPE=",
            _env[2] = ft_strjoin(_env[2], serv_resp.get_mime_type(serv_resp._extension).c_str());
          
            //"GATEWAY_INTERFACE=CGI/1.1 ",
                        
            //"PATH_INFO=",
            _env[4] = ft_strjoin(_env[4], (*cli_conf["file"].begin()).substr(serv_resp.i + 1).c_str());
            
            //"PATH_TRANSLATED=",
            _env[5] = ft_strjoin(_env[5], serv_resp._resource_path.c_str());
            
            //"QUERY_STRING=",
            _env[6] = ft_strjoin(_env[6], serv_resp._query.c_str());
            
            //I don't believe we get these: (these are info about the client)
            //"REMOTE_ADDR=",
            //"REMOTE_IDENT=",
            //"REMOTE_USER=",

            //"REQUEST_METHOD=",
            _env[10] = ft_strjoin(_env[10], (*cli_conf["method"].begin()).c_str());

            //"SCRIPT_NAME=",
            _env[11] = ft_strjoin(_env[11], serv_resp._resource_path.c_str());

            //"SERVER_NAME=",
            _env[12] = ft_strjoin(_env[12], (*serv_resp.get_serv_info()["server_name"].begin()).c_str());
            
            //"SERVER_PORT=",
            _env[13] = ft_strjoin(_env[13], (*serv_resp.get_serv_info()["server_port"].begin()).c_str());

            //"SERVER_PROTOCOL=HTTP/1.1",
            //"SERVER_SOFTWARE=webserv/1.0";
        };
        
        void launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf) {
            build_env(serv_resp, cli_conf);
        };


    private:


        char *file_path; // to send as first argument
        
        char _env[];
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