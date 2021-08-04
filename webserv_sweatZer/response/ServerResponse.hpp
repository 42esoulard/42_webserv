/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 16:27:00 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/04 18:44:54 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP

#include "webserv.h"
#include "Server.hpp"
#include "Cgi.hpp"
class Cgi;

class ServerResponse {

    public:

        ServerResponse(SimpleHashTable &mime_table, SimpleHashTable &error_codes, std::list<Server> &server_list);
        ~ServerResponse();

        typedef std::map<std::string, std::list<std::string> >  t_content_map;

        void        reinit_serv_response();

        //put all our method functions in a map, this way we can just parse
        // the request and call e.g. (ServerResponse.get_method("GET"))()
        void init_methods_list() {

            _methods["GET"] = &ServerResponse::method_get;
            _methods["HEAD"] = &ServerResponse::method_head;
            _methods["POST"] = &ServerResponse::method_post;
            _methods["PUT"] = &ServerResponse::method_put;
            _methods["DELETE"] = &ServerResponse::method_delete;
        };

        typedef void (ServerResponse::*method_func)();
        method_func get_method(std::string &method) {
            return _methods[method];
        };

        std::string &get_payload() { return _payload; }
        int const   &get_error() const { return _error; };
        std::string get_mime_type(std::string &extension);
        std::string get_next_token(std::string &line, size_t &index);

        std::list < t_content_map > &get_locations() {return _server_conf->locations;}
        Server::t_content_map       &get_serv_info() { return _server_conf->serv_info; }
        Server::t_conf              *get_server_conf_by_name(std::string &searched_name, std::string &searched_port);
        Server::t_conf              *get_server_conf_by_address(std::string &searched_host, std::string &searched_port);
        int                         identify_server(t_content_map &cli_conf);
        unsigned long               identify_location(std::string &file, std::string &extension);

        void    set_conf(std::string &key, std::string &val) { _conf[key] = val; };

        int     build_response(t_content_map &cli_conf);
        bool    check_path_lvl(std::string &path);
        int     file_to_body(void);
        int     make_index(void);
        int     check_auth(std::string &tmp);

		int		error(int code);
        int     build_error_response(int code);
		int		check_server_location(std::string &requested_path);
		int		check_body_size(t_content_map &cli_conf);
		int		check_stats_file(t_content_map &cli_conf);
		int		check_allowed_method(t_content_map &cli_conf);
		int		check_authorization(t_content_map &cli_conf);
		int		check_cgi(t_content_map &cli_conf, std::string &requested_path);
		void	format_error_response();
        int     no_host_response();
        int     slow_loris_response();
        int     serv_loc_not_found_response(int code);

    private:

        //references
        SimpleHashTable     *_mime_types;
        SimpleHashTable     *_error_codes;
        void                method_get();
        void                method_head();
        void                method_post();
        void                method_put();
        void                method_delete();
        std::map< std::string, method_func >    _methods;

        //from cluster .conf
        std::map<std::string, std::string>      _conf;//cluster conf
        std::list<Server>   _server_list;//full cluster server list
        Server::t_conf      *_server_conf;//specific server conf
        t_content_map       *_location;//specific location conf
        size_t              _max_body;

        //resource utils
        std::string         _resource_path;
        char                _abs_resource_path[PATH_MAX];
        std::string         _extension;
        std::string         _query;

        //cli info
        std::string         _method;
        std::string         _cli_body;
        std::string         _body;
        std::string         _payload;

        //error utils
        int                 _error;
        std::string         _s_error;
        std::string         *_p_error_msg;
        std::string         _s_error_msg;
        std::string         _sp;

        //utils
        char			    _buf[4096];
        unsigned long       i;

        //CGI
        friend class        Cgi;
        Cgi                 *_cgi;
        bool                _cgi_on;

};

#endif
