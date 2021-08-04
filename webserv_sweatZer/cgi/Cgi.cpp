/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/05 12:25:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/04 17:36:43 by esoulard         ###   ########.fr       */
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

void Cgi::str_to_env(t_content_map &cli_conf) {
	
	//formatting str env
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
			topush += "=";
			topush += *(*it).second.begin();
			s_env.push_back(topush);
		}
	}

    // str env to const char* env
	size_t	count = 0;
    for (; count < s_env.size(); ++count)
	{
        _env[count] = const_cast<char*>(s_env[count].c_str());
		if (count + 1 == 99)
			break ;
	}
    _env[count] = NULL;
}

void Cgi::build_env(ServerResponse &serv_resp, t_content_map &cli_conf) {

    //"AUTH_TYPE=Basic",
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
    _tmp = *cli_conf["file"].begin();
	if (serv_resp.i >= _tmp.size())
		s_env.push_back(std::string("PATH_INFO=") + _tmp);
	else
		s_env.push_back(std::string("PATH_INFO=") + _tmp.substr(serv_resp.i));

    //"PATH_TRANSLATED=",
    s_env.push_back(std::string("PATH_TRANSLATED=") + serv_resp._resource_path);

    //"QUERY_STRING=",
    s_env.push_back(std::string("QUERY_STRING=") + serv_resp._query);

    //"REQUEST_METHOD=",
    s_env.push_back(std::string("REQUEST_METHOD=") + (*cli_conf["method"].begin()));

    //"SERVER_SOFTWARE=webserv/1.0";
    s_env.push_back(std::string("SERVER_SOFTWARE=webserv/1.0"));
    
    //"SERVER_NAME=",
    s_env.push_back(std::string("SERVER_NAME=") + (*serv_resp.get_serv_info()["server_name"].begin()));

    //"SERVER_PORT=",
    s_env.push_back(std::string("SERVER_PORT=") + (*serv_resp.get_serv_info()["server_port"].begin()));

    //"SERVER_PROTOCOL=HTTP/1.1",
    s_env.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    
};

int Cgi::launch_cgi(ServerResponse &serv_resp, t_content_map &cli_conf) {
    
    std::cerr << "----------------- LAUNCH CGI..." << std::endl;
    build_env(serv_resp, cli_conf);
	str_to_env(cli_conf);

	_save[0] = dup(STDIN_FILENO);
	_save[1] = dup(STDOUT_FILENO);
    _file[0] = tmpfile();
    _file[1] = tmpfile();
    _fd[0] = fileno(_file[0]);
    _fd[1] = fileno(_file[1]);

    _status = 0;

	write(_fd[0], serv_resp._cli_body.c_str(), serv_resp._cli_body.size());
	lseek(_fd[0], 0, SEEK_SET);

    if ((_pid = fork()) < 0) {
		dup2(_save[0], STDIN_FILENO);
		dup2(_save[1], STDOUT_FILENO);
		fclose(_file[0]);
        fclose(_file[1]);
        close(_save[0]);
        close(_save[1]);
		return (serv_resp.build_error_response(500));
	}
	else if (_pid == 0) {

        if (dup2(_fd[0], 0) == -1 || dup2(_fd[1], 1) == -1) {
            std::cerr << "ERROR: CGI CHILD: dup2 fail" << std::endl;
            exit(500);
        }
        
        if (getcwd(serv_resp._abs_resource_path, PATH_MAX)) {

            std::string _cgi_abs_path(serv_resp._abs_resource_path);
            _cgi_abs_path += "/" + serv_resp._resource_path;

            _args[0] = ft_strdup((*(*serv_resp._location)["cgi_bin"].begin()).c_str());
            _args[1] = ft_strdup(_cgi_abs_path.c_str());
            _args[2] = NULL;
            std::cerr << "----------------- CGI GONNA EXEC [" << _args[0] << "] with file [" << _args[1] << "]..." << std::endl;
            execve(_args[0], _args, _env);
            ft_memdel((void**)&(_args[0]));
            ft_memdel((void**)&(_args[1]));
        }
        std::cerr << "ERROR: CGI CHILD: execve fail" << std::endl;
        exit(500);
	}
	else {
		waitpid(_pid, &_status, 0);
        
        memset(_buf, 0, _MAXLINE);		
        lseek(_fd[1], 0, SEEK_SET);

        int i = 1;
		serv_resp._body.clear();
		while (i > 0) {

			memset(_buf, 0, _MAXLINE);
            i = read(_fd[1], _buf, _MAXLINE - 1);
            for (int j = 0 ; j < i ; ++j)
                serv_resp._body.push_back(_buf[j]);
        }
		parse_content_type(serv_resp);

		dup2(_save[0], STDIN_FILENO);
		dup2(_save[1], STDOUT_FILENO);
		fclose(_file[0]);
        fclose(_file[1]);
        close(_save[0]);
        close(_save[1]);
  
    }
    return 0;
};

void	Cgi::parse_content_type(ServerResponse &serv_resp)
{
	
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
        
    _vec.clear();
	_vec = split_crlf(serv_resp._body, &body, &err);
	if (err)
		return ;

	if ((body_start = serv_resp._body.find("\r\n\r\n")) != std::string::npos)
		serv_resp._body = serv_resp._body.substr(body_start + 4);
	
	for (size_t i = start ; i < 2 ; i++)
	{
		if (i != body - 1 && (found = _vec[i].find(':')) != std::string::npos && is_alpha(_vec[i][found - 1]))
		{
            _vec2.clear();
			_vec2 = split(_vec[i], ':', 1);
			if (_vec2[0] == "Content-Type" && lev[0]--)
				serv_resp._extension = _vec2[1];
			if (_vec2[0] == "Status" && lev[1]--)
				serv_resp._error = ft_stoi(_vec2[1]);
		}
	}
}
