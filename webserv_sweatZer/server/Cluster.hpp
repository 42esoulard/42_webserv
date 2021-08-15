/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 10:16:47 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/07 18:05:28 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

 #include "webserv.h"
 #include "Server.hpp"
 #include "ClientRequest.hpp"
 #include "ServerResponse.hpp"

class Cluster {

    public:

        Cluster(std::string &config): _mime_types(SimpleHashTable(65)), _error_codes(SimpleHashTable(42)) { 
            init_cluster(config); 
            _serv_response = new ServerResponse(_mime_types, _error_codes, server_list);
            (*_serv_response).reinit_serv_response();
        };
        
        ~Cluster() { delete _serv_response; };

        void init_cluster(std::string &config);

        std::list<Server> server_list;

        //CONFIG PARSING AND TRANSFER
        void        set_mime();
        void        set_error();
        void        parse_config(std::string &config_file);
        std::string get_conf_token(char *line, int &index);
        int         parse_field(std::string &field, std::string &config);
        int         parse_values(std::string &field, std::string &config);
        void        check_conf(std::string &config);
        void        print_config(); 

        // Request reception :
        void handle_connection();
        void parse_request();
        void save_chunk(std::vector<std::string> &_vecChunk, std::string &chunk);
        bool check_body_end(std::string &s_tmp, ServerResponse &serv_response);
        bool handle_chunk(std::string &s_tmp, ServerResponse &serv_response);

        void send_response(std::string &response);

        bool get_sigint() { return _sigint; };
        
    private:

        std::vector<ClientRequest>  _cli_request;
        ServerResponse              *_serv_response;

        // socket handling
        std::string _config_file;
        int         _config_fd;

        fd_set      _active_fd_set;
        fd_set      _read_fd_set;
        fd_set      _write_fd_set;
        fd_set      _clients_fd_set;

        int         _nb_clients;
        int         _maxed_out_fds;

        struct timeval	_timeout;

        int         _new_socket;
        int         _cur_socket;

        // config parsing utils

        std::string     _sline;
        char            *_line;
        std::string     field;
        int             _index;
        bool            _in_location;
        bool            _in_server;
        int             _line_nb;
        std::string     _tmp;
        std::string		buf;

        std::list<Server>::iterator _serv_it;

        std::string _error_serv_unavailable;
        std::string _error_req_timeout;
        std::string _error_req_timeout_sl;

        // reference hashtables
        SimpleHashTable _mime_types;
        SimpleHashTable _error_codes;

        bool            _sigint;

};

#endif
