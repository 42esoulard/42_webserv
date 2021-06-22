/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/05 12:25:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/22 17:34:49 by rturcey          ###   ########.fr       */
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
	if (serv_resp.i >= (*cli_conf["file"].begin()).size())
		s_env[4] = std::string("PATH_INFO=") + (*cli_conf["file"].begin());
	else
		s_env[4] = std::string("PATH_INFO=") + (*cli_conf["file"].begin()).substr(serv_resp.i);

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

    _file[0] = tmpfile();
    _file[1] = tmpfile();
    _fd[0] = fileno(_file[0]);
    _fd[1] = fileno(_file[1]);

    int pid;
    int status = 0;

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
        if (dup2(_fd[0], 0) == -1 || dup2(_fd[1], 1) == -1) {
            std::cerr << "CHILD DUP2 FAIL" << std::endl;
            std::cerr << strerror(errno) << std::endl;
            // serv_resp.build_error_response(500);

            // std::cout << serv_resp._payload << std::endl;
            exit(500);
        }
        std::cerr << "CHILD AFT DUP2" << std::endl;
        //close(_pipe[0]);
        char *args[3];

        if (getcwd(serv_resp._abs_resource_path, PATH_MAX)) {

            std::cerr << "CHILD BEFORE EXECVE: [" << serv_resp._abs_resource_path << "][" << serv_resp._resource_path << "]" << std::endl;
            std::string _cgi_abs_path(serv_resp._abs_resource_path);
            _cgi_abs_path += "/" + serv_resp._resource_path;

            // if (!(args[0] = (char *)malloc(sizeof(char) * _cgi_abs_path.size() + 1)))
            //     return -1;
            args[0] = ft_strdup((*(*serv_resp._location)["cgi_bin"].begin()).c_str());
            args[1] = ft_strdup(_cgi_abs_path.c_str());
            args[2] = NULL;
            std::cerr << "Gonna exec [" << args[0] << "with file " << args[1] << "]" << std::endl;
            execve(args[0], args, _env);
            free(args[0]);
            free(args[1]);
        }
        std::cerr << "CHILD AFTER EXECVE errno[" << strerror(errno) << "]" << std::endl;
		// serv_resp.build_error_response(500);
        // std::cout << serv_resp._payload << std::endl;
        exit(500);
	}
	else {
        std::cout << "PARENT BEFORE WAIT" << std::endl;
		waitpid(pid, &status, 0);
        // if (WIFEXITED(status))
		// 	serv_resp._error = WEXITSTATUS(status);
        //if error, return serv_resp._error
        std::cout << "PARENT AFTER WAIT" << std::endl;
        char buf[_MAXLINE] = {0};
        lseek(_fd[1], 0, SEEK_SET);
        std::cout << "PARENT BEF READ" << std::endl;

        int i = 1;
		while (i > 0) {
            std::cout << "PARENT IN READ" << std::endl;
            i = read(_fd[1], buf, _MAXLINE);
            //std::cout << "[" << buf << "]" << std::endl;
            for (int j = 0 ; j < i ; ++j)
                serv_resp._body.push_back(buf[j]);
			memset(buf, 0, _MAXLINE);
			//PUT PRINTS HERE TO UNDRSTAND WHATS READ
		}
		parse_content_type(serv_resp);
        std::cout << "PARENT AFT READ" << std::endl;

        // if (serv_resp._error != 200 && serv_resp._error != 201 && serv_resp._error != 204) {
        //     serv_resp._body
        //     return -1;
        // }
        fclose(_file[0]);
        fclose(_file[1]);
        close(_fd[0]);
        close(_fd[1]);
    }
    return 0;
};

void	Cgi::parse_content_type(ServerResponse &serv_resp)
{
	std::vector<std::string>	vec;
	size_t						body = -1;
	int							err = 0;
	size_t						found;
	size_t						start = 0;
	size_t						k = 0;
	size_t						j = 0;
	size_t						l = 0;

	if (serv_resp._body.find("\r\n") == std::string::npos)
		return ;
	vec = split_crlf(serv_resp._body, &body, &err);
	if (err)
		return ;
	for (size_t i = start ; i < vec.size() ; i++)
	{
		if (i > 0 && i < body - 1 && vec[i].size() > _MAXHEADERFIELD)
			return ;
		found = 0;
		if (i != body - 1 && (found = vec[i].find(':')) != std::string::npos && is_alpha(vec[i][found - 1]))
		{
			std::vector<std::string>	vec2 = split(vec[i], ':', 1);
			if (vec2[0] == "Content-Type")
			{
				serv_resp._extension = vec2[1];
				k = serv_resp._body.find("Content-Type:");
				j = serv_resp._body.find("\r\n", k);
				l = j;
				j = serv_resp._body.find("\r\n\r\n", j);
				if (j != l)
					return ;
				std::string		body1 = serv_resp._body.substr(0, k);
				std::string		body2 = serv_resp._body.substr(j + 4);
				serv_resp._body = body1 + body2;
				return ;
			}
		}
		else if (found && found != std::string::npos && is_space(vec[i][found - 1]))
			return ;
		else if (i == body - 1)
			return ;
	}
}
