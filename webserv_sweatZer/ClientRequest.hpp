/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:20:03 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/13 16:31:38 by esoulard         ###   ########.fr       */
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

class	ServerResponse;

class	ClientRequest {

	public:
		ClientRequest();
		~ClientRequest();
		ClientRequest(int index);

		int		parse_request(ServerResponse &serv_response, int socket);
		int	 	allocate_read(size_t max_body);
		char	*get_read(); //getter for _read
		void 	set_read();
		std::string	&get_sread(); //getter for _read
		std::vector<std::string>	&get_vecChunked() { return _vecChunked; };
		bool	is_method(std::string &str);
		int		parse_method();
		int		parse_headers(size_t body, int socket);
		int		save_header(std::string &str);
		int		parse_body(size_t i, int socket);
		int		parse_body_chunked(std::string str, int socket);
		bool	parse_host();
		bool	parse_language();
		bool	parse_charset();

		typedef std::map<std::string, std::list<std::string> >  t_content_map;

		t_content_map &get_conf() { return _conf; };

		bool check_timeout();
		void set_timeout();
		int sock_nb;

	private:
		time_t 						_last_request;
		// char						*_read;
		std::string					_sread;
		std::vector<std::string>	_vecRead;
		std::vector<std::string>	_vecChunked;
		t_content_map				_conf;

		// In our map, we got :
		// accept-language
		// accept-charset
		// host
		// protocol
		// content-length
		// content-size
		// body
		// transfer-encoding
		// authorization
		// user-agent
		// referer
		// method
};

#endif
