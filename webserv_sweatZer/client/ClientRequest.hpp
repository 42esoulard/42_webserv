/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:20:03 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/22 17:24:26 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
#define CLIENTREQUEST_HPP

#include "webserv.h"
#include "ServerResponse.hpp"

class	ServerResponse;

class	ClientRequest {

	public:
		ClientRequest();
		~ClientRequest() {};
		
		typedef std::map<std::string, std::list<std::string> >  t_content_map;

		void 		reinit_cli();

		int			parse_request(ServerResponse &serv_response);
		
		bool		is_method(std::string &str);
		int			parse_method();
		int			parse_headers(size_t body);
		int			save_header(std::string &str);
		int			parse_body(size_t i);
		int			parse_body_chunked(std::string str);
		bool		parse_host();
		bool		parse_language();
		bool		parse_charset();

		bool 		check_timeout();
		void 		set_timeout();

		std::string					&get_sread() 		{ return _sread; };
		std::vector<std::string>	&get_vecChunked() 	{ return _vecChunked; };
	
		t_content_map 				&get_conf() 		{ return _conf; };

	private:
		//timeout
		time_t 						_last_request;
		struct timeval				_tv;
		time_t						_cur_time;
		time_t						_diff;

		//request reading
		std::string					_sread;
		std::vector<std::string>	_vecRead;
		std::vector<std::string>	_vecChunked;

		//parsed client info
		t_content_map				_conf;

		//utils
		std::vector<std::string>					_vec;
		t_content_map::iterator 					_map_it;
		std::string									_tmpa;
		std::string									_tmpb;
		std::vector<float>							ft;
		std::list<std::pair<float, std::string> >	_charset;		
		std::list<std::pair<float, std::string> >	_language;

};

#endif
