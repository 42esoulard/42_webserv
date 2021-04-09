/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:20:03 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/09 17:56:15 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
#define CLIENTREQUEST_HPP

#include "webserv.h"

/*
    We must handle the following fields:

    ◦ Accept-Charsets: character sets that are acceptable.
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

        void parse_request(ServerResponse &serv_response);

        //getter for _read
        char *get_read();

    private:
        char        _read[_MAXLINE];//to store our initial reading of the received request

        //configuration info
        std::map<std::string, std::string> _conf;
        // ^^^ this will contain this vvv

        // //first line of the request contains:
        // std::string _method;
        // std::string _file;
        // std::string _protocol;
        
        // std::string _auth; //credentials
        // std::string _host; //our server address

        // std::string _body; //message body

        // //DO WE CARE ABOUT THOSE OR CAN WE IGNORE THEM?!
        // std::string _referer;
        // std::string _user_agent;
        // std::string _accept_char;
        // std::string _accept_lang;

        //FOR CHECKS, DO WE NEED A REFERENCE OF POSSIBLE VALUES FOR EACH FIELD? 
        // this will depend on what nginx says in case of bad value. Maybe it doesnt check.

};

#endif