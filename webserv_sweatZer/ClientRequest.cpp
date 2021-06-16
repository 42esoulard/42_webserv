/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:46:45 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/16 17:45:04 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest(): _vecRead(), _vecChunked(), _conf()
{
	memset(_read, 0, _MAXLINE);
	_sread = std::string("");
	_headers[0] = "host";
	_headers[1] = "accept-charset";
	_headers[2] = "accept-language";
	_headers[3] = "authorization";
	_headers[4] = "referer";
	_headers[5] = "user-agent";
	_headers[6] = "content-type";
	_headers[7] = "content-length";
	_headers[8] = "transfer-encoding";
	_headers[9] = "";
}

ClientRequest::~ClientRequest() {};

char	*ClientRequest::get_read()
{
	return _read;
}

void ClientRequest::set_read() {
	memset(_read, 0, _MAXLINE);
	for (size_t i = 0; i < _sread.size(); i++) {
		_read[i] = _sread[i];
	}
}

std::string &ClientRequest::get_sread()
{
	return _sread;
}

bool	   ClientRequest::is_method(std::string &str)
{
	std::string	methods[9];
	methods[0] = "GET";
	methods[1] = "POST";
	methods[2] = "HEAD";
	methods[3] = "PUT";
	methods[4] = "DELETE";
	methods[5] = "CONNECT";
	methods[6] = "OPTIONS";
	methods[7] = "TRACE";
	methods[8] = "PATCH";
	for (int i = 0 ; i < 9 ; i++)
	{
		if (!str.compare(methods[i]))
		{
			_conf["method"].push_back(methods[i]);
			return (1);
		}
	}
	return (0);
}

// parsing the whole request, handling errors using serv_response & RFC's error codes
// split_crlf splits the whole buffer into vector<string> _vecRead, using "\r\n" as charset
// int body is telling us which element of _vecRead is the body
int		ClientRequest::parse_request(ServerResponse &serv_response, int socket)
{
	std::string				toRead(_read);
	int						error;
	size_t					body = -1;

	_vecRead = split_crlf(toRead, &body);
	if (_vecRead.empty())
		return (serv_response.error(400));
	if ((error = parse_method()) || (error = parse_headers(body, socket)))
		return (serv_response.error(error));
	if (parse_host())
		return (serv_response.error(400));
	parse_charset();
	parse_language();
	print_map(_conf);
	return (0);
}

// split_sp splits the method line into vector<string> vec, using spaces as charset
// then we check if method & protocal are correct and well formated
// URI size limit = 2000 bytes
int		ClientRequest::parse_method()
{
	std::vector<std::string>	 vec = split_sp(_vecRead[0]);
	
	if (vec.size() != 3)
		return (400);
	else if (!is_method(vec[0]))
		return (501);
	else if (vec[1].size() > 2000)
		return (414);
	_conf["file"].push_back(vec[1]);
	if (vec[2].substr(0, 5).compare("HTTP/"))
		return (400);
	else if (vec[2].compare("HTTP/1.1"))
		return (505);
	_conf["protocol"].push_back(vec[2]);
	return (0);
}

// each header is parsed until we encounter an error
// we allow bullshit only if it's RFC compliant
// for example, spaces before colons are not allowed
int		ClientRequest::parse_headers(size_t body, int socket)
{
	size_t					found;
	int						error;

	for (size_t i = 1 ; i < _vecRead.size() ; i++)
	{
		found = 0;
		if (i != body - 1 && (found = _vecRead[i].find(':')) != std::string::npos && is_alpha(_vecRead[i][found - 1]))
		{
			if ((error = save_header(_vecRead[i])))
				return (error);
		}
		else if (found && found != std::string::npos && is_space(_vecRead[i][found - 1]))
			return (400);
		else if (i == body - 1)
			return (parse_body(i, socket));
	}
	return (0);
}

// saving each header into our t_content_map
// we're checking if there are multiple hosts or content-length (and if it's value isn't correct)
// if the name of the header isn't correct, we juste ignore it
int	ClientRequest::save_header(std::string &str)
{
	std::vector<std::string>	vec = split(str, ':', 1);

	lower(vec[0]);
	for (size_t i = 0 ; _headers[i] != "" ; i++)
	{
		if (vec[0] == "host" && _conf.find("host") != _conf.end())
			return (400);
		if (vec[0] == "content-length" && (_conf.find("content-length") != _conf.end() || !is_num(vec[1])))
			return (400);
		if (vec[0] == "transfer-encoding" && vec[1].compare("chunked"))
			return (501);
		if (!vec[0].compare(_headers[i]))
		{
			_conf[vec[0]].push_back(vec[1]);
			return (0);
		}
		if (is_space(vec[0][vec[0].size() - 1]))
			return (400);
	}
	return (0);
}

// we check if content-type is defined. If not, default value
// if chunked, parse_body_chunked()
// else, we checkf if there is a contnt-length (if not, error 411)
// then if the size of the body is correct, we save it
int		ClientRequest::parse_body(size_t i, int socket)
{
	(void) socket;
	t_content_map::iterator	itc;

	if (_conf.find("content-type") == _conf.end())
		_conf["content-type"].push_back("application/octet-stream");
	if ((itc = _conf.find("transfer-encoding")) != _conf.end())
		return (parse_body_chunked(_vecRead[i], socket));
	if ((itc = _conf.find("content-length")) == _conf.end())
		return (411);
	if (_vecRead[i].size() < (size_t)ft_stoi((*itc).second.front()))
		return (400);
	_vecRead[i] = _vecRead[i].substr(0, (size_t)ft_stoi((*itc).second.front()));
	_conf["body"].push_back(_vecRead[i]);
	return (0);
}

// parsing chunked body : size(in hexa)\r\nbody
// until size is 0 or incorrect
// if the size of the body is < to chunk_size, error
// if the chunk_size is incorrect, error
// we read until body.size() > LIMIT or until chunk_size == 0
int		ClientRequest::parse_body_chunked(std::string str, int socket)
{
	(void) socket;
	
	_conf["body"].push_back(str);
	return (0);
}

// parsing host & port. Some bullshit is allowed but we don't accept '/'
bool	ClientRequest::parse_host()
{
	t_content_map::iterator	it = _conf.find("host");
	size_t					j = 0;

	if (it == _conf.end())
		return (1);
	
	if ((*it).second.front().find('/') > -1)
		return (1);
	size_t	i = (*it).second.front().find(':');
	if (i++ == std::string::npos)
		_conf["port"].push_back(ft_itos(PORT));
	else
	{
		while (i < (*it).second.front().size() && (*it).second.front()[i] >= '0' 
			&& (*it).second.front()[i] <= '9')
		{
			i++;
			j++;
		}
		_conf["port"].push_back((*it).second.front().substr(i - j, j));
		(*it).second.front() = (*it).second.front().substr(0, i - j - 1);
	}
	return (0);
}

// parsing language according to RFC, even if it seems pretty useless
bool	ClientRequest::parse_language()
{
	t_content_map::iterator						it = _conf.find("accept-language");
	std::vector<std::string>					vec;
	std::string									str;
	std::string									tmp;
	std::vector<float>							ft;
	size_t										i = 0;
	size_t										j = 0;
	std::list<std::pair<float, std::string> >	_language;

	if (it == _conf.end())
		return (1);
	vec = split((*it).second.front(), ',');
	while (j < vec.size())
	{
		i = 0;
		pass_spaces(vec[j], i);
		while (is_alpha(vec[j][i]))
		{
			str = cap_alpha(vec[j], i);
			if (str.size() > 8)
			{
				j++;
				continue;
			}
			else
				tmp += str;
			if (i < vec[j].size() && vec[j][i] == '-')
			{
				++i;
				tmp += "-";
			}
		}
		if (!(tmp.empty()))
		{
			if (i + 3 < vec[j].size() && vec[j].substr(i, 3) == ";q=" && vec[j][i + 3] == '0')
			{
				_language.push_back(std::pair<float, std::string>(atof(vec[j].substr(i + 3).c_str()), tmp));
			}
			else
				_language.push_back(std::pair<float, std::string>(1.0, tmp));
			tmp.clear();
		}
		j++;
	}
	_language.sort(comp_float);
	(*it).second.clear();
	for (std::list<std::pair<float, std::string> >::iterator ti = _language.begin() ; ti != _language.end() ; ++ti)
	{
		(*it).second.push_back((*ti).second);
	}
	return (0);
}

// parsing charset according to RFC, even if it seems pretty useless
bool	ClientRequest::parse_charset()
{
	t_content_map::iterator						it = _conf.find("accept-charset");
	std::vector<std::string>					vec;
	std::string									str;
	std::string									tmp;
	std::vector<float>							ft;
	size_t										i = 0;
	size_t										j = 0;
	std::list<std::pair<float, std::string> >	_charset;

	if (it == _conf.end())
		return (1);
	vec = split((*it).second.front(), ',');
	while (j < vec.size())
	{
		i = 0;
		pass_spaces(vec[j], i);
		while (is_alpha(vec[j][i]))
		{
			str = cap_alpha(vec[j], i);
			if (str.size() > 8)
			{
				j++;
				continue;
			}
			else
				tmp += str;
			if (i < vec[j].size() && vec[j][i] == '-')
			{
				++i;
				tmp += "-";
			}
		}
		if (!(tmp.empty()))
		{
			if (i + 3 < vec[j].size() && vec[j].substr(i, 3) == ";q=" && vec[j][i + 3] == '0')
			{
				_charset.push_back(std::pair<float, std::string>(atof(vec[j].substr(i + 3).c_str()), tmp));
			}
			else
				_charset.push_back(std::pair<float, std::string>(1.0, tmp));
			tmp.clear();
		}
		j++;
	}
	_charset.sort(comp_float);
	(*it).second.clear();
	for (std::list<std::pair<float, std::string> >::iterator ti = _charset.begin() ; ti != _charset.end() ; ++ti)
	{
		(*it).second.push_back((*ti).second);
	}
	return (0);
};
