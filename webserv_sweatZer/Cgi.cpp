/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/05 12:25:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/06 16:07:43 by esoulard         ###   ########.fr       */
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
        
int Cgi::launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf) {
    std::cout << "IN CGI" << std::endl;
    build_env(serv_resp, cli_conf);
    // std::cout << "  ENV   " << std::endl;
    // for (int i = 0; i < 13; i++)
    //     std::cout << _env[i] << std::endl;

    FILE    *file_in = tmpfile();
    FILE    *file_out = tmpfile();
    int     fd_in = fileno(file_in);
    int     fd_out = fileno(file_out);

    int pid;
    int status = 0;
    if (pipe(_pipe) == -1)
		return (serv_resp.build_error_response(500));
    if ((pid = fork()) < 0)
		return (serv_resp.build_error_response(500));
	else if (pid == 0)
	{
        std::cout << "IN CHILD" << std::endl;
		// if (sh->obj->prev && sh->obj->prev->pip == IS_PIPE)
        // {
        //     if (dup2(sh->obj->prev->tube[0], 0) == -1)
        //         return (-1);
        //     if (sh->obj->prev->tube[1] != -1)
        //         close(sh->obj->prev->tube[1]);
        // }
        // if (sh->obj->pip == IS_PIPE)
        // {
        std::cout << "CHILD BEF DUP2" << std::endl;
        if (dup2(fd_in, 0) == -1 || dup2(fd_out, 1) == -1) {
            std::cerr << "CHILD DUP2 FAIL" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            // serv_resp.build_error_response(500);

            // std::cout << serv_resp._payload << std::endl;
            exit(500);
        }
        std::cerr << "CHILD AFT DUP2" << std::endl;
        //close(_pipe[0]);
        char **args = NULL;
        
        
        
        
        if (getcwd(serv_resp._abs_resource_path, PATH_MAX)) {
            std::cerr << "CHILD BEFORE EXECVE: [" << serv_resp._abs_resource_path << "][" << serv_resp._resource_path << "]" << std::endl;
            std::string _cgi_abs_path(serv_resp._abs_resource_path);
            _cgi_abs_path += "/" + serv_resp._resource_path;
            std::cerr << "Gonna exec [" << _cgi_abs_path << "]" << std::endl;
            execve(_cgi_abs_path.c_str(), args, _env);
        }
        std::cerr << "CHILD AFTER EXECVE errno[" << strerror(errno) << "]" << std::endl;
		// serv_resp.build_error_response(500);
        // std::cout << serv_resp._payload << std::endl;
        exit(500);
	}
	else { 
        std::cout << "PARENT BEFORE WAIT" << std::endl;
		waitpid(pid, &status, 0);
        if (WIFEXITED(status))
			serv_resp._error = WEXITSTATUS(status);
        //if error, return serv_resp._error
        std::cout << "PARENT AFTER WAIT" << std::endl;
        char buf[_MAXLINE] = {0};
        std::cout << "PARENT BEF READ" << std::endl;
        close(_pipe[1]);
		while (read(fd_out, buf, _MAXLINE) > 0) {
            std::cout << "PARENT IN READ" << std::endl;
			memset(buf, 0, _MAXLINE);
			serv_resp._body += buf;
		}
        std::cout << "PARENT AFT READ" << std::endl;
        
        // if (serv_resp._error != 200 && serv_resp._error != 201 && serv_resp._error != 204) {
        //     serv_resp._body
        //     return -1;
        // }
        fclose(file_in);
        fclose(file_out);
        close(fd_in);
        close(fd_out);
    }
    return 0;
};