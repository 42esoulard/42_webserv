/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/15 17:40:23 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/16 11:56:13 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "ClientRequest.hpp"

class Cgi {

    public:
        Cgi();
        ~Cgi();


        void build_env(ClientRequest::t_content_map &cli_conf) {
            
            _env[17] = NULL;
            ft_strjoin(_env[0], (*cli_conf["_auth"].begin()).c_str());


        };
        void launch_cgi();


    private:


        char *file_path; // to send as first argument
        
        char *_env[18] { "AUTH_TYPE="
                        "CONTENT_LENGTH="
                        "CONTENT_TYPE="
                        "GATEWAY_INTERFACE="
                        "PATH_INFO="
                        "PATH_TRANSLATED="
                        "QUERY_STRING="
                        "REMOTE_ADDR="
                        "REMOTE_IDENT="
                        "REMOTE_USER="
                        "REQUEST_METHOD="
                        "REQUEST_URI="
                        "SCRIPT_NAME="
                        "SERVER_NAME="
                        "SERVER_PORT="
                        "SERVER_PROTOCOL="
                        "SERVER_SOFTWARE="}; /* LAST ONE SHOULD BE NULL*/

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
       

}