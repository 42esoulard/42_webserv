/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/05 12:25:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/05 15:39:24 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

void Cgi::build_env(ServerResponse &serv_resp, t_content_map &cli_conf) {    

    //"AUTH_TYPE=Basic",
    // std::cout << s_env[0] << std::endl;
    s_env[0] = "AUTH_TYPE=Basic";

    //"CONTENT_LENGTH=",
    if (cli_conf.find("content-length") != cli_conf.end())
        s_env[1] = std::string("CONTENT_LENGTH=") + (*cli_conf["content-length"].begin());
    else
        s_env[1] = std::string("CONTENT_LENGTH=-1");

    //"CONTENT_TYPE=",
    s_env[2] = std::string("CONTENT_TYPE=") + serv_resp.get_mime_type(serv_resp._extension);
  
    //"GATEWAY_INTERFACE=CGI/1.1",
    s_env[3] = std::string("GATEWAY_INTERFACE=CGI/1.1");

    //"PATH_INFO=",
    s_env[4] = std::string("PATH_INFO=") + (*cli_conf["file"].begin()).substr(serv_resp.i + 1);
            
    //"PATH_TRANSLATED=",
    s_env[5] = std::string("PATH_TRANSLATED=") + serv_resp._resource_path;
            
    //"QUERY_STRING=",
    s_env[6] = std::string("QUERY_STRING=") + serv_resp._query;
            
    //I don't believe we get these: (these are info about the client)
    //"REMOTE_ADDR=",
    //"REMOTE_IDENT=",
    //"REMOTE_USER=",

    //"REQUEST_METHOD=",
    s_env[7] = std::string("REQUEST_METHOD=") + (*cli_conf["method"].begin());

    //"SCRIPT_NAME=",
    s_env[8] = std::string("SCRIPT_NAME=") + serv_resp._resource_path;

    //"SERVER_NAME=",
    s_env[9] = std::string("SERVER_NAME=") + (*serv_resp.get_serv_info()["server_name"].begin());
            
    //"SERVER_PORT=",
    s_env[10] = std::string("SERVER_PORT=") + (*serv_resp.get_serv_info()["server_port"].begin());

    //"SERVER_PROTOCOL=HTTP/1.1",
    s_env[11] = std::string("SERVER_PROTOCOL=HTTP/1.1");
    //"SERVER_SOFTWARE=webserv/1.0";
    s_env[12] = std::string("SERVER_SOFTWARE=webserv/1.0");

    // DO s_env to env
    for (int i = 0; i < 13; i++) 
        _env[i] = const_cast<char*>(s_env[i].c_str());
    _env[13] = NULL;
};
        
void Cgi::launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf) {
    std::cout << "IN CGI" << std::endl;
    build_env(serv_resp, cli_conf);
    // std::cout << "  ENV   " << std::endl;
    // for (int i = 0; i < 13; i++)
    //     std::cout << _env[i] << std::endl;
};