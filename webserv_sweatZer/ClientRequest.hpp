/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:20:03 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/16 11:56:18 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
#define CLIENTREQUEST_HPP

#include "webserv.h"
#include "ServerResponse.hpp"

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

/*
    MAYBE in the case of POST requests from a client, we might receive a chunked request
    in Transfer-encoding header. We'll need to read from the client until content length is 0,
    and save all the read content properly (=unchunk it).
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
        bool    parse_host();
        bool    parse_language();
		bool    parse_charset();

        typedef std::map <std::string, std::list <std::string > >  t_content_map;

        t_content_map get_conf() { return _conf; };

    private:
        char                                        _read[_MAXLINE];//to store our initial reading of the received request
        std::string                                 _headers[7];
        std::vector<std::string> _vecRead;

        //configuration info
        std::map<std::string, std::list<std::string> > _conf;
         // ^^^ this will contain this vvv

        // //first line of the request contains:
        // std::string _method;
        // std::string _file;
        // std::string _protocol;
        // std::string _auth; //credentials
        // std::string _host; //our server address
		// std::string _port;
        // std::string _body; //message body
        // std::string _referer;
        // std::string _user_agent;
        // std::string _accept_char;
        // std::string _accept_lang;

        //FOR CHECKS, DO WE NEED A REFERENCE OF POSSIBLE VALUES FOR EACH FIELD?
        // this will depend on what nginx says in case of bad value. Maybe it doesnt check.

        // Server *requested_server;

};

#endif
