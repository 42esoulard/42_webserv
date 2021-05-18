/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:46:45 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/18 11:00:55 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest()
{
    _read[0] = '\0';
    _headers[0] = "host";
    _headers[1] = "accept-charset";
    _headers[2] = "accept-language";
    _headers[3] = "authorization";
    _headers[4] = "referer";
    _headers[5] = "user-agent";
    _headers[6] = "content-type";
    _headers[7] = "content-length";
    _headers[8] = "body";
    _headers[9] = "transfer-encoding";
    _headers[10] = "";
}

ClientRequest::~ClientRequest() {};

char    *ClientRequest::get_read()
{
    return _read;
}

bool       ClientRequest::is_method(std::string &str)
{
    std::string    methods[9];
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

int       ClientRequest::parse_method()
{
    std::vector<std::string>     vec = split_sp(_vecRead[0]);
    if (vec.size() != 3)
        return (400);
    else if (!is_method(vec[0]))
        return (501);
	else if (vec[1].size() > 2000)
		return (414);
    // should we parse file now ?
    _conf["file"].push_back(vec[1]);
    if (vec[2].compare("HTTP/1.1"))
        return (400);
    _conf["protocol"].push_back(vec[2]);
    return (0);
}

int    ClientRequest::save_header(std::string &str)
{
    // lowercase
    std::vector<std::string>    vec = split(str, ':', 1);
    for (size_t i = 0 ; i < vec[0].size() ; i++)
    {
        if (vec[0][i] >= 'A' && vec[0][i] <= 'Z')
            vec[0][i] += 32;
    }
    // if we don't handle this header, or if it's bullshit, we just ignore it
    for (size_t i = 0 ; _headers[i] != "" ; i++)
    {
        if (vec[0] == "host" && _conf.find("host") != _conf.end())
			return (400);
		if (vec[0] == "content-length" && _conf.find("content-length") != _conf.end())
			return (400);
		if (vec[0] == "transfer-encoding" && vec[1].compare("chunked"))
			return (501);
		else if (vec[0] == "content-length")
		{
			for (size_t i = 0 ; i < vec[1].size() ; i++)
			{
				if (vec[1][i] < '0' || vec[1][i] > '9')
					return (400);
			}
		}
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

bool    ClientRequest::parse_host()
{
    std::map<std::string, std::list<std::string> >::iterator    it = _conf.find("host");
	if (it == _conf.end())
		return (1);
	if ((*it).second.front().find('/') > -1)
		return (1);
	size_t	i = (*it).second.front().find(':');
	size_t	j = 0;
	if (i == std::string::npos)
		_conf["port"].push_back("80");
	else
	{
		while ((*it).second.front()[i] >= '0' && (*it).second.front()[i] <= '9')
		{
			i++;
			j++;
		}
		_conf["port"].push_back((*it).second.front().substr(i - j, j));
	}
    return (0);
}

bool    ClientRequest::parse_language()
{
    std::map<std::string, std::list<std::string> >::iterator    it = _conf.find("accept-language");

    if (it == _conf.end())
        return (1);
    std::vector<std::string>					vec;
    std::string									str;
    std::string									tmp;
    std::vector<float>							ft;
	std::list<std::pair<float, std::string> >	_language;
    size_t          i = 0;
    size_t          j = 0;
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
        //  will have to deal with '*'
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

bool    ClientRequest::parse_charset()
{
    std::map<std::string, std::list<std::string> >::iterator	it = _conf.find("accept-charset");
    if (it == _conf.end())
        return (1);
    std::vector<std::string>					vec;
    std::string									str;
    std::string									tmp;
    std::vector<float>							ft;
	std::list<std::pair<float, std::string> >	_charset;
    size_t          i = 0;
    size_t          j = 0;
    vec = split((*it).second.front(), ',');
    while (j < vec.size())
    {
        i = 0;
        pass_spaces(vec[j], i);
        while (is_alnum(vec[j][i]))
        {
            str = cap_alphanum(vec[j], i);
            tmp += str;
            if (i < vec[j].size() && vec[j][i] == '-')
            {
                ++i;
                tmp += "-";
            }
        }
        //  will have to deal with '*'
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
}

/*bool    ClientRequest::parse_authorization()
{

}*/

void    ClientRequest::parse_request(ServerResponse &serv_response) {
    //check if request format is good
    //first line will init _method, _file and _protocol
    //then each header field will init each of our ClientRequest attributes.
    (void)serv_response;
    std::string     toRead(_read);
    int             error;
	size_t			body = -1;
    _vecRead = split_crlf(toRead, &body);
    if ((error = parse_method()))
    {
        // serv_response.error(error);
        std::cout << "ERROR " << error <<std::endl;
        return ;
    }
    size_t  found;
    // saving all headers in _conf
    for (size_t i = 1 ; i < _vecRead.size() ; i++)
    {
		found = 0;
        if (i != body - 1 && (found = _vecRead[i].find(':')) && is_alpha(_vecRead[i][found - 1]))
		{
            if ((error = save_header(_vecRead[i])))
			{
				// serv_response.error(error);
       			std::cout << "ERROR " << error <<std::endl;
        		return ;
			}
		}
        else if (found && is_space(_vecRead[i][found - 1]))
        {
			// serv_response.error(400);
            std::cout << "ERROR 400" <<std::endl;
            return ;
        }
        else if (i == body - 1)
        {
			std::map<std::string, std::list<std::string> >::iterator	itc;
			if ((itc = _conf.find("content-length")) == _conf.end())
			{
				// serv_response.error(411);
            	std::cout << "ERROR 411" <<std::endl;
           		return ;
			}
			// incomplete body
            if (_vecRead[i].size() < (size_t)ft_stoi((*itc).second.front()))
			{
				// serv_response.error(400);
            	std::cout << "ERROR 400" <<std::endl;
           		return ;
			}
			_vecRead[i] = _vecRead[i].substr(0, (size_t)ft_stoi((*itc).second.front()));
            _conf["body"].push_back(_vecRead[i]);
            if (_conf.find("content-type") == _conf.end())
                _conf["content-type"].push_back("application/octet-stream");
            break ;
        }
    }

    // checkinf if there is a "host"
    if (parse_host())
    {
        // serv_response.error(400);
        std::cout << "ERROR 400" <<std::endl;
        return ;
    }
    parse_charset();
    parse_language();
    print_map(_conf);
};
