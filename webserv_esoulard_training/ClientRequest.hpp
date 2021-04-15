/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:20:03 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/15 15:23:30 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
#define CLIENTREQUEST_HPP

#include "webserv.h"

/*
    We must handle the following fields:

    ◦ Accept-Charset: character sets that are acceptable.
                [Accept-Charset: utf-8]
    ◦ Accept-Language: list of acceptable human languages for response. 
                [Accept-Language: en-US]
    ◦ Authorization: authentication credentials for HTTP authentication.
                [Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==] >>> Basic means the following is a simple base64 encryption of login:password. THIS IS NOT SECURE
    ◦ Host: domain name of the server + the TCP port number on which the server is listening (or no port if default)
                [Host: en.wikipedia.org:8080]
                [Host: en.wikipedia.org]
    ◦ Referer: the address of the previous web page from which a link to the currently requested page was followed.
                [Referer: http://en.wikipedia.org/wiki/Main_Page]
    ◦ User-Agent: The user agent string of the user agent. 	
                [User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0]
*/

class ClientRequest {
    
    public:
        ClientRequest();
        ~ClientRequest();

        void    parse_request(ServerResponse &serv_response);
        char    *get_read(); //getter for _read
        bool    is_method(std::string &str);
        int     parse_method();
        void    save_header(std::string &str);

    private:
        char                _read[_MAXLINE];//to store our initial reading of the received request
        std::string         _headers[6];
        std::vector<std::string> _vecRead;

        //configuration info
        std::map<std::string, std::string> _conf;

        //FOR CHECKS, DO WE NEED A REFERENCE OF POSSIBLE VALUES FOR EACH FIELD? 
        // this will depend on what nginx says in case of bad value. Maybe it doesnt check.

};

#endif