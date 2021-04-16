/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:46:45 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/16 12:34:19 by rturcey          ###   ########.fr       */
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
            _conf["method"] = methods[i];
            return (1);
        }
    }
    return (0);
}

int       ClientRequest::parse_method()
{
    std::vector<std::string>     vec = split_sp(_vecRead[0]);
    if (vec.size() != 3 || !is_method(vec[0]))
        return (400);
    // should we parse file now ?
    _conf["file"] = vec[1];
    if (vec[2].compare("HTTP/1.1"))
        return (400);
    _conf["protocol"] = vec[2];
    return (0);
}

void    ClientRequest::save_header(std::string &str)
{
    // lowercase
    std::vector<std::string>    vec = split(str, ':', 1);
    for (size_t i = 0 ; i < vec[0].size() ; i++)
    {
        if (vec[0][i] >= 'A' && vec[0][i] <= 'Z')
            vec[0][i] += 32;
    }
    // if we don't handle this header, or if it's bullshit, we just ignore it
    for (size_t i = 0 ; i < _headers[i].size() ; i++)
    {
        if (!vec[0].compare(_headers[i]))
        {
            _conf[vec[0]] = vec[1];
            return ;
        }
    }      
}

bool    ClientRequest::parse_host()
{
    std::map<std::string, std::string>::iterator    it = _conf.find("host");
    if (it == _conf.end())
        return (1);
    if ((*it).second.find('/') > -1)
        return (1);
    return (0);
}

bool    ClientRequest::parse_language()
{
    
}

void    ClientRequest::parse_request(ServerResponse &serv_response) {
    //check if request format is good
    //if not >>> should we stop parsing here and start writing error response ? or keep parsing and recording the info we need.
    //first line will init _method, _file and _protocol
    //then each header field will init each of our ClientRequest attributes. 
    // std::string field("_protocol");
    // std::string protocol("HTTP/1.1");
    // serv_response.set_conf(field, protocol);
    (void)serv_response;
    std::string     toRead(_read);
    _vecRead = split(toRead, '\n');
    if (parse_method())
    {
        // serv_response.error(400);
        return ;
    }
    size_t  found;
    // saving all headers in _conf
    for (size_t i = 1 ; i < _vecRead.size() ; i++)
    {
        if ((found = _vecRead[i].find(':')) && is_alpha(_vecRead[i][found - 1]))
            save_header(_vecRead[i]);
    }
    // checkinf if there is a "host"
    if (parse_host())
    {
        // serv_response.error(400);
        return ;
    }
    // now, parsing each header
    // parse_charset();
    // parse_language();
    // parse_authorization();
    // parse_referer();
    // parse_userAgent();
    print_map(_conf);
};
