/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 10:16:47 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/01 10:44:40 by esoulard         ###   ########.fr       */
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

        Cluster(std::string &config): _mime_types(SimpleHashTable(65)), _error_codes(SimpleHashTable(42)) { init_cluster(config); };
        ~Cluster() {};

        void init_cluster(std::string &config);

        std::list<Server> server_list;

        //CONFIG PARSING AND TRANSFER
        void        set_mime();
        void        set_error();
        void        parse_config(std::string &config_file);
        std::string get_conf_token(char *line, int &index);
        void        parse_field(std::string &field, std::string &config);
        void        parse_values(std::string &field, std::string &config);
        void        check_conf(std::string &config);
        void        print_config(); //DEV UTIL

        // Request reception :
        void handle_connection();
        void parse_request();
        void save_chunk(std::vector<std::string> *_vecChunk, std::string &chunk);
        bool check_body_end(std::string &s_tmp, std::string *s_read_ptr, ServerResponse &serv_response);
        bool handle_chunk(std::string &s_tmp, std::string *_sread_ptr, ServerResponse &serv_response);
        
        void send_response(std::string &response);

    private:

        std::vector<ClientRequest> _cli_request;
        
        // socket handling
        std::string _config_file;
        int         _config_fd;

        fd_set      _active_fd_set;
        fd_set      _read_fd_set;
        fd_set      _clients_fd_set;

        int         _new_socket;
        int         _cur_socket;

        char        buf[_MAXLINE];

        // config parsing utils
        char    *_line;
        int     _index;
        bool    _in_location;
        bool    _in_server;
        int     _line_nb;

        // reference hashtables
        SimpleHashTable _mime_types;
        SimpleHashTable _error_codes;
        
};

#endif
