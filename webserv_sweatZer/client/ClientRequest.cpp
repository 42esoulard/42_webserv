/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:46:45 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/27 20:23:13 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest(): _sread(""), _vecRead(), _vecChunked(), _conf()
{
	set_timeout();
}

void ClientRequest::reinit_cli() {

	set_timeout();
	_sread.clear();
	_vecRead.clear();
	_vecChunked.clear();
	_conf.clear();
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
int		ClientRequest::parse_request(ServerResponse &serv_response)
{
	int						error;
	size_t					body = -1;
	int						err = 0;

	_vecRead = split_crlf(_sread, &body, &err);
	if (err)
		return (serv_response.error(413));
	if (_vecRead.empty())
		return (serv_response.error(400));
	if ((error = parse_method()))
	{
		parse_headers(body);
		if (parse_host()) {
			return (serv_response.error(666));
		}
		return (serv_response.error(error));
	}
	if ((error = parse_headers(body)))
	{
		if (parse_host())
			return (serv_response.error(666));
		return (serv_response.error(error));
	}
	if (parse_host())
		return (serv_response.error(666));

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
	_vec = split_sp(_vecRead[0]);

	if (_vec.size() != 3)
		return (400);
	else if (!is_method(_vec[0]))
		return (501);
	else if (_vec[1].size() > _MAXURI)
		return (414);
	if (_vec[1].find('/') == std::string::npos)
		return (400);
	_conf["file"].push_back(_vec[1]);
	if (_vec[2].substr(0, 5).compare("HTTP/"))
		return (400);
	else if (_vec[2].compare("HTTP/1.1"))
		return (505);
	_conf["protocol"].push_back(_vec[2]);
	return (0);
}

// each header is parsed until we encounter an error
// we allow bad input only if it's RFC compliant
// for example, spaces before colons are not allowed
int		ClientRequest::parse_headers(size_t body)
{
	size_t					found;
	int						error;
	size_t					start = 1;

	if (_vecRead[1].empty())
		start = 0;
	for (size_t i = start ; i < _vecRead.size() ; i++)
	{
		if (i > 0 && i < body - 1 && _vecRead[i].size() > _MAXHEADERFIELD)
			return (413);
		found = 0;
		if (i != body - 1 && (found = _vecRead[i].find(':')) != std::string::npos && is_alpha(_vecRead[i][found - 1]))
		{
			if ((error = save_header(_vecRead[i])))
				return (error);
		}
		else if (found && found != std::string::npos && is_space(_vecRead[i][found - 1]))
			return (400);
		else if (i == body - 1)
			return (parse_body(i));
	}
	return (0);
}

// saving each header into our t_content_map
// we're checking if there are multiple hosts or content-length (and if it's value isn't correct)
// if the name of the header isn't correct, we juste ignore it
int	ClientRequest::save_header(std::string &str)
{
	_vec = split(str, ':', 1);

	lower(_vec[0]);
	if (_vec[0] == "host" && _conf.find("host") != _conf.end())
		return (400);
	if (_vec[0] == "content-length" && (_conf.find("content-length") != _conf.end() || !is_num(_vec[1])))
		return (400);
	if (_vec[0] == "transfer-encoding" && _vec[1].compare("chunked"))
		return (501);
	if (is_space(_vec[0][_vec[0].size() - 1]))
		return (400);
	_conf[_vec[0]].push_back(_vec[1]);
	return (0);
}

// we check if content-type is defined. If not, default value
// if chunked, parse_body_chunked()
// else, we check if there is a content-length (if not, error 411)
// then if the size of the body is correct, we save it
int		ClientRequest::parse_body(size_t i)
{
	if (_conf.find("content-type") == _conf.end())
		_conf["content-type"].push_back("application/octet-stream");
	if ((_map_it = _conf.find("transfer-encoding")) != _conf.end())
		return (parse_body_chunked(_vecRead[i]));
	if ((_map_it = _conf.find("content-length")) == _conf.end())
		return (411);
	if (_vecRead[i].size() < (size_t)ft_stoi((*_map_it).second.front()))
		return (400);
	_vecRead[i] = _vecRead[i].substr(0, (size_t)ft_stoi((*_map_it).second.front()));
	_conf["body"].push_back(_vecRead[i]);
	return (0);
}

// parsing chunked body : size(in hexa)\r\nbody
// until size is 0 or incorrect
// if the size of the body is < to chunk_size, error
// if the chunk_size is incorrect, error
// we read until body.size() > LIMIT or until chunk_size == 0
int		ClientRequest::parse_body_chunked(std::string str)
{
	_conf["body"].push_back(str);
	return (0);
}

// parsing host & port. Some bullshit is allowed but we don't accept '/'
bool	ClientRequest::parse_host()
{
	_map_it = _conf.find("host");
	size_t					j = 0;

	if (_map_it == _conf.end())
		return (1);

	if ((*_map_it).second.front().find('/') > -1)
		return (1);
	size_t	i = (*_map_it).second.front().find(':');
	if (i++ == std::string::npos)
		_conf["port"].push_back(ft_itos(PORT));
	while (i < (*_map_it).second.front().size() && (*_map_it).second.front()[i] == 32)
		++i;
	if (i != std::string::npos)
	{
		while (i < (*_map_it).second.front().size() && (*_map_it).second.front()[i] >= '0'
			&& (*_map_it).second.front()[i] <= '9')
		{
			i++;
			j++;
		}
		_conf["port"].push_back((*_map_it).second.front().substr(i - j, j));
		(*_map_it).second.front() = (*_map_it).second.front().substr(0, (*_map_it).second.front().find(':'));
	}
	return (0);
}

// parsing language according to RFC
bool	ClientRequest::parse_language()
{
	_map_it = _conf.find("accept-language");

	size_t	i = 0;
	size_t	j = 0;

	_language.clear();
	if (_map_it == _conf.end())
		return (1);
	_vec = split((*_map_it).second.front(), ',');
	while (j < _vec.size())
	{
		i = 0;
		pass_spaces(_vec[j], i);
		while (is_alpha(_vec[j][i]))
		{
			_tmpb = cap_alpha(_vec[j], i);
			if (_tmpb.size() > 8)
			{
				j++;
				continue;
			}
			else
				_tmpa += _tmpb;
			if (i < _vec[j].size() && _vec[j][i] == '-')
			{
				++i;
				_tmpa += "-";
			}
		}
		if (!(_tmpa.empty()))
		{
			if (i + 3 < _vec[j].size() && _vec[j].substr(i, 3) == ";q=" && _vec[j][i + 3] == '0')
			{
				_language.push_back(std::pair<float, std::string>(atof(_vec[j].substr(i + 3).c_str()), _tmpa));
			}
			else
				_language.push_back(std::pair<float, std::string>(1.0, _tmpa));
			_tmpa.clear();
		}
		j++;
	}
	_language.sort(comp_float);
	(*_map_it).second.clear();
	for (std::list<std::pair<float, std::string> >::iterator ti = _language.begin() ; ti != _language.end() ; ++ti)
	{
		(*_map_it).second.push_back((*ti).second);
	}
	return (0);
}

// parsing charset according to RFC
bool	ClientRequest::parse_charset()
{
	_map_it = _conf.find("accept-charset");
	size_t										i = 0;
	size_t										j = 0;

	_charset.clear();

	if (_map_it == _conf.end())
		return (1);
	_vec = split((*_map_it).second.front(), ',');
	while (j < _vec.size())
	{
		i = 0;
		pass_spaces(_vec[j], i);
		while (is_alpha(_vec[j][i]))
		{
			_tmpb = cap_alpha(_vec[j], i);
			if (_tmpb.size() > 8)
			{
				j++;
				continue;
			}
			else
				_tmpa += _tmpb;
			if (i < _vec[j].size() && _vec[j][i] == '-')
			{
				++i;
				_tmpa += "-";
			}
		}
		if (!(_tmpa.empty()))
		{
			if (i + 3 < _vec[j].size() && _vec[j].substr(i, 3) == ";q=" && _vec[j][i + 3] == '0')
			{
				_charset.push_back(std::pair<float, std::string>(atof(_vec[j].substr(i + 3).c_str()), _tmpa));
			}
			else
				_charset.push_back(std::pair<float, std::string>(1.0, _tmpa));
			_tmpa.clear();
		}
		j++;
	}
	_charset.sort(comp_float);
	(*_map_it).second.clear();
	for (std::list<std::pair<float, std::string> >::iterator ti = _charset.begin() ; ti != _charset.end() ; ++ti)
	{
		(*_map_it).second.push_back((*ti).second);
	}
	return (0);
};

bool ClientRequest::check_timeout() {

	gettimeofday(&_tv, NULL);
	_cur_time = _tv.tv_sec;
	_diff = _cur_time - _last_request;

	if (_diff >= _TIMEOUT) {
		return true;
	}
	return false;
}

void ClientRequest::set_timeout() {

	gettimeofday(&_tv, NULL);
	_last_request = _tv.tv_sec;
}
