/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/05 12:25:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/15 22:24:25 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi()
{
	_headers.push_back("host");
	_headers.push_back("accept-charset");
	_headers.push_back("accept-language");
	_headers.push_back("authorization");
	_headers.push_back("referer");
	_headers.push_back("user-agent");
	_headers.push_back("content-type");
	_headers.push_back("content-length");
	_headers.push_back("transfer-encoding");
	_headers.push_back("port");
	_headers.push_back("method");
	_headers.push_back("protocol");
	_headers.push_back("file");
	_headers.push_back("body");
}

void Cgi::reinit_cgi() {

    s_env.clear();
    _file[0] = NULL;
    _file[1] = NULL;
    _fd[0] = -1;
    _fd[1] = -1;
    _cgi_body.clear();
}

void Cgi::build_env(ServerResponse &serv_resp, t_content_map &cli_conf) {

    //"AUTH_TYPE=Basic",
    // std::cout << s_env[0] << std::endl;

    s_env.push_back("AUTH_TYPE=Basic");

    //"CONTENT_LENGTH=",
    if (cli_conf.find("content-length") != cli_conf.end())
        s_env.push_back(std::string("CONTENT_LENGTH=") + (*cli_conf["content-length"].begin()));
    else
        s_env.push_back(std::string("CONTENT_LENGTH=-1"));

    //"CONTENT_TYPE=",
    s_env.push_back(std::string("CONTENT_TYPE=") + serv_resp.get_mime_type(serv_resp._extension));

    //"GATEWAY_INTERFACE=CGI/1.1",
    s_env.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));

    //"PATH_INFO=",
	if (serv_resp.i >= (*cli_conf["file"].begin()).size())
		s_env.push_back(std::string("PATH_INFO=") + (*cli_conf["file"].begin()));
	else
		s_env.push_back(std::string("PATH_INFO=") + (*cli_conf["file"].begin()).substr(serv_resp.i));

    //"PATH_TRANSLATED=",
    s_env.push_back(std::string("PATH_TRANSLATED=") + serv_resp._resource_path);

    //"QUERY_STRING=",
    s_env.push_back(std::string("QUERY_STRING=") + serv_resp._query);

    //I don't believe we get these: (these are info about the client)
    //"REMOTE_ADDR=",
    //"REMOTE_IDENT=",
    //"REMOTE_USER=",

    //"REQUEST_METHOD=",
    s_env.push_back(std::string("REQUEST_METHOD=") + (*cli_conf["method"].begin()));

    //"SCRIPT_NAME=",
    s_env.push_back(std::string("SERVER_SOFTWARE=webserv/1.0"));
    //"SERVER_NAME=",
    s_env.push_back(std::string("SERVER_NAME=") + (*serv_resp.get_serv_info()["server_name"].begin()));

    //"SERVER_PORT=",
    s_env.push_back(std::string("SERVER_PORT=") + (*serv_resp.get_serv_info()["server_port"].begin()));

    //"SERVER_PROTOCOL=HTTP/1.1",
    s_env.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    //"SERVER_SOFTWARE=webserv/1.0";
	std::string		topush;
	int				lev;
	for(t_content_map::iterator it = cli_conf.begin() ; it != cli_conf.end() ; ++it)
	{
		lev = 0;
		for (size_t j = 0 ; j < _headers.size() ; ++j)
		{
			if (_headers[j] == (*it).first)
				lev = 1;
		}
		if (lev == 0)
		{
			topush = format_env((*it).first);
			std::cout << "FIRST = " << (*it).first << std::endl;
			topush += "=";
			topush += *(*it).second.begin();
			std::cout << "SCD = " << *(*it).second.begin() << std::endl;
			s_env.push_back(topush);
		}
	}

    // _env[100] est une methode bourrin il faudrait allouer dynamiquement
	size_t	count = 0;
    for (; count < s_env.size(); ++count)
	{
        _env[count] = const_cast<char*>(s_env[count].c_str());
		if (count + 1 == 99)
			break ;
	}
    _env[count] = NULL;
};

int Cgi::launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf) {
    std::cout << "IN CGI" << std::endl;
    build_env(serv_resp, cli_conf);
    // std::cout << "  ENV   " << std::endl;
    // for (int i = 0; i < 13; i++)
    //     std::cout << _env[i] << std::endl;

	int	saveIn = dup(STDIN_FILENO);
	int	saveOut = dup(STDOUT_FILENO);
    _file[0] = tmpfile();
    _file[1] = tmpfile();
    _fd[0] = fileno(_file[0]);
    _fd[1] = fileno(_file[1]);

    int pid;
    int status = 0;

	write(_fd[0], serv_resp._cli_body.c_str(), serv_resp._cli_body.size());
	lseek(_fd[0], 0, SEEK_SET);
    if ((pid = fork()) < 0)
	{
		dup2(saveIn, STDIN_FILENO);
		dup2(saveOut, STDOUT_FILENO);
		fclose(_file[0]);
        fclose(_file[1]);
        close(_fd[0]);
        close(_fd[1]);
		return (serv_resp.build_error_response(500));
	}
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
        //std::cout << "PARENT BEFORE WAIT" << std::endl;
		waitpid(pid, &status, 0);
        // if (WIFEXITED(status))
		// 	serv_resp._error = WEXITSTATUS(status);
        //if error, return serv_resp._error
        //std::cout << "PARENT AFTER WAIT" << std::endl;
        char buf[_MAXLINE] = {0};
		std::string	str;
        lseek(_fd[1], 0, SEEK_SET);
        //std::cout << "PARENT BEF READ" << std::endl;

        int i = 1;
		//int	lev = 1;
		serv_resp._body.clear();
		while (i > 0) {
            //std::cout << "PARENT IN READ" << std::endl;
			memset(buf, 0, _MAXLINE);
            i = read(_fd[1], buf, _MAXLINE - 1);
            //std::cout << "[" << buf << "]" << std::endl;
			/*if (lev-- == 1 && i > 0)
			{
				if ((str = parse_content_type(serv_resp, buf)) != "\r\n\r\n")
				{
					memset(buf, 0, _MAXLINE);
					for (size_t c = 0 ; c < str.size() ; ++c)
					{
						buf[c] = str[c];
					}
					i = (int)str.size();
				}
				std::cout << "[" << buf << "]" << std::endl;
			}*/
            for (int j = 0 ; j < i ; ++j)
			{
                serv_resp._body.push_back(buf[j]);
			}
            //std::cout << "serv_resp in cgi [" << serv_resp._body << "]" << std::endl;
            //PUT PRINTS HERE TO UNDRSTAND WHATS READ
        }
		parse_content_type(serv_resp);
		/*if (serv_resp._body.size() && serv_resp._body.substr(0, 11) != "Status: 200") {
            fclose(_file[0]);
            fclose(_file[1]);
            close(_fd[0]);
            close(_fd[1]);
            return (serv_resp.build_error_response(ft_stoi(serv_resp._body.substr(8, 3))));
        }*/
		dup2(saveIn, STDIN_FILENO);
		dup2(saveOut, STDOUT_FILENO);
		fclose(_file[0]);
        fclose(_file[1]);
        close(_fd[0]);
        close(_fd[1]);
    }
	std::cout << "BEFORE RETURN IN PARENT" << std::endl;
    return 0;
};

void	Cgi::parse_content_type(ServerResponse &serv_resp)
{
	std::vector<std::string>	vec;
	size_t						body = -1;
	int							err = 0;
	size_t						start = 0;
	size_t						found = 0;
	int							lev[2];
    std::string                 cpy_body = serv_resp._body;
    size_t                      body_start;

	lev[0] = 1;
	lev[1] = 1;
	if (cpy_body.find("\r\n") == std::string::npos)
		return ;
	vec = split_crlf(serv_resp._body, &body, &err);
	if (err)
		return ;

	if ((body_start = serv_resp._body.find("\r\n\r\n")) != std::string::npos)
	{
		std::cout << "BODY START [" << serv_resp._body.substr(0, body_start) << "]" << std::endl;
		serv_resp._body = serv_resp._body.substr(body_start + 4);
	}
	for (size_t i = start ; i < 2 ; i++)
	{
		if (i != body - 1 && (found = vec[i].find(':')) != std::string::npos && is_alpha(vec[i][found - 1]))
		{
			std::vector<std::string>	vec2 = split(vec[i], ':', 1);
			if (vec2[0] == "Content-Type" && lev[0]--)
				serv_resp._extension = vec2[1];
			if (vec2[0] == "Status" && lev[1]--)
				serv_resp._error = ft_stoi(vec2[1]);
		}
	}
}
