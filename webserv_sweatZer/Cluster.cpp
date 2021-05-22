/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 10:16:04 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/22 15:59:35 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

void Cluster::set_mime() {

    int mime_fd;
    //FD_ZERO (&mime_fd);
    if ((mime_fd = open(MIME_TYPES, O_RDONLY)) < 0)
        throw Exception("Couldn't open mime types file " + std::string(MIME_TYPES));

    // fd_set active_fd_set;
    // FD_SET (mime_fd, &active_fd_set);
    // if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0)
    //     throw Exception("select error");
    char *line;

    //_mime_types = SimpleHashTable(65);

    while (get_next_line(mime_fd, &line) > 0) {
        _mime_types.add_entry(line);
        free (line);
    }
    close (mime_fd);

    // //TEST
    // std::cout << "----------THIS IS A MIME TEST-------------" << std::endl;

    // std::string key = "jpg";
    // std::string *value = g_mime_types.get_value(key);
    // if (value)
    //     std::cout << *value << std::endl;
    // key = "cco";
    // value = g_mime_types.get_value(key);
    // if (value)
    //     std::cout << *value << std::endl;

    // key = "asf";
    // value = g_mime_types.get_value(key);
    // if (value)
    //     std::cout << *value << std::endl;

    // key = "mng";
    // value = g_mime_types.get_value(key);
    // if (value)
    //     std::cout << *value << std::endl;

    // key = "html";
    // value = g_mime_types.get_value(key);
    // if (value)
    //     std::cout << *value << std::endl;

    // key = "bullshit";
    // value = g_mime_types.get_value(key);
    // if (value)
    //     std::cout << *value << std::endl;

    // std::cout << "----------THIS WAS A MIME TEST-------------" << std::endl;

};

void Cluster::init_cluster(std::string &config) {

    set_mime();
    //set all the tables we'll use for comparison here, we'll use them for the whole program (make them global)

    this->parse_config(config);

    FD_ZERO (&this->_active_fd_set);

    std::list<Server>::iterator it;

    for (it = server_list.begin(); it != server_list.end(); it++) {
        it->init_server();
        FD_SET (it->get_server_fd(), &this->_active_fd_set);
    }
};

void Cluster::parse_config(std::string &config) {

   // FD_ZERO (&_config_fd);
    if ((_config_fd = open(config.c_str(), O_RDONLY)) < 0)
        throw Exception("Couldn't open configuration file " + config);

    // fd_set active_fd_set;
    // FD_SET (_config_fd, &active_fd_set);
    // if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0)
    //     throw Exception("select error");

    _in_location = false;
    _in_server = false;
    _line_nb = 0;
    std::string field;
    while (get_next_line(_config_fd, &_line) > 0) {

        ++_line_nb;
        _index = 0;

        parse_field((field = get_conf_token(_line, _index)), config);
        parse_values(field, config);

        free (_line);
    }
    close (_config_fd);
    print_config(); //prints all the contents of _conf
    check_conf(config);
};

//prints all the contents of _conf
void Cluster::print_config() {

    std::cout << "----CONFIG----"<< std::endl;
    std::list<Server>::iterator it = server_list.begin();
    int index = 0;

    while (it != server_list.end()) {
        std::cout << "------------------------SERVER " << index++ << "------------------------" << std::endl;
        //printing server content
        it->print_server_info();

        //printing locations content
        it->print_server_locations();
        ++it;
    }
    std::cout << "-----------------------" << std::endl;
}

void Cluster::parse_field(std::string &field, std::string &config) {
    if (field == "server") {

            if (get_conf_token(_line, _index) != "{" || get_conf_token(_line, _index) != "")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
            if (_in_server)
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + ": already in a server context!");
            Server new_server;
            server_list.push_back(new_server);
            _in_server = true;
    }
    else if (field == "location") {

        if (_in_location)
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + ": already in a location context!");

        Server::t_content_map new_loc;
        server_list.back().get_locations().push_back(new_loc);
        _in_location = true;

        std::string tmp;
        if ((tmp = get_conf_token(_line, _index)) == "" || tmp == "{")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + "location needs a path");
        server_list.back().get_locations().back()["path"].push_back(tmp);
        while ((tmp = get_conf_token(_line, _index)) != "{" && tmp != "")
            server_list.back().get_locations().back()["extensions"].push_back(tmp);
        if (tmp != "{" || get_conf_token(_line, _index) != "")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
    }
    else if (field == "}") {

        if (_in_location)
            _in_location = false;
        else if (_in_server)
            _in_server = false;
        if (get_conf_token(_line, _index) != "")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
    }
}

// simply parses spaces and returns the next non space character sequence
std::string Cluster::get_conf_token(char *line, int &index) {

    pass_spaces(line, index);
    if (line && !line[index])
        return "";

    int start = index;
    while (line && line[index] && !is_space(line[index]) && line[index] != ';')
        ++index;

    return std::string(&line[start], index - start);
}

void Cluster::parse_values(std::string &field, std::string &config) {

    while (_line && _line[_index] && _line[_index] != ';') {

        if (_in_server && _in_location)
            server_list.back().get_locations().back()[field].push_back(get_conf_token(_line, _index));
        else if (_in_server)
            server_list.back().get_serv_info()[field].push_back(get_conf_token(_line, _index));
        else
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + ": fields must be in a context");
        if (_line[_index] == ';' && get_conf_token(_line, _index) != "")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
    }
}

void    Cluster::check_conf(std::string &config) {

    std::list<Server>::iterator                 server_it = server_list.begin();
    std::list<Server::t_content_map>::iterator  locations_it;

    Server::t_content_map::iterator             serv_info_it;
    Server::t_content_map::iterator             loc_info_it;

    while (server_it != server_list.end()) { //iterate on each server block

        serv_info_it = (*server_it).get_serv_info().begin();
        while (serv_info_it != (*server_it).get_serv_info().end()) {
            // check that except in the case of server_name, we don't accept more than 1 param
            // MAYBE SEVERAL PORTS ARE OK, NOT SURE. IN THIS CASE, ADD ANOTHER EXCEPTION FOR server_port
            if (serv_info_it->second.size() != 1 && serv_info_it->first != "server_name")
                throw Exception("Config file " + config + ": " + serv_info_it->first + "wrong number of parameters");
            ++serv_info_it;
        }

        locations_it = (*server_it).get_locations().begin();
        while (locations_it != (*server_it).get_locations().end()) {
            loc_info_it = (*locations_it).begin();
            while (loc_info_it != (*locations_it).end()) {
                if (loc_info_it->second.size() != 1 && loc_info_it->first != "accept_methods")
                    throw Exception("Config file " + config + ": " + loc_info_it->first + ": wrong number of parameters");
                ++loc_info_it;
            }
            ++locations_it;
        }
        ++server_it;
    }
    // WE WILL STILL NEED TO CHECK EACH PARAMETER'S VALIDITY UPON USING IT.
};

void Cluster::handle_connection(){

    std::cout << std::endl << "[--- WAITING FOR NEW CONNECTION ---]" << std::endl;
    this->_read_fd_set = this->_active_fd_set;

    if (select(FD_SETSIZE, &this->_read_fd_set, NULL, NULL, NULL) < 0)
        throw Exception("select error");

    for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
        if (FD_ISSET (this->_cur_socket, &this->_read_fd_set)) {

            std::list<Server>::iterator server_it = server_list.begin();
            while (server_it != server_list.end()) {
                if (this->_cur_socket == server_it->get_server_fd()) {
                    /* Connection request on original socket. */
                    if ((this->_new_socket = accept(server_it->get_server_fd(), (struct sockaddr *)&(server_it->get_address()), &server_it->get_address_sz())) < 0) {
                        std::cout << "accept fail ret " << this->_new_socket << " errno " << errno << std::endl;
                        throw Exception("accept error");
                    }
                    std::cerr << "Server: connect from host " << inet_ntoa (server_it->get_address().sin_addr) << ", port " <<  ntohs (server_it->get_address().sin_port) << std::endl;
                    FD_SET (this->_new_socket, &this->_active_fd_set);
                    return ;
                }
                ++server_it;
            }
            /* Data arriving on an already-connected socket. */

            this->parse_request();
            this->format_response();

            /*
            ** 6) CLOSE THE SOCKET
            ** The same close() that we use for files
            */

            close(this->_cur_socket);
            FD_CLR (this->_cur_socket, &this->_active_fd_set);
        }
    }
};

void Cluster::parse_request() {

    /*
    ** 5) SEND AND RECEIVE MESSAGES
    ** The same read and write system calls that work on files also work on sockets.
    */
    ClientRequest cli_request;
    read(this->_cur_socket, cli_request.get_read(), _MAXLINE);
    ServerResponse serv_response(_mime_types, server_list);
    cli_request.parse_request(serv_response, this->_cur_socket);// should also take 
    // std::cout << "[CLIENT MSG] " << cli_request.get_read() << std::endl;

    //I NEED TO DO TESTS WITH NGINX TO SEE WHAT MATTERS: ARE ERRORS BEYOND FIRST LINE IMPORTANT? ARE THEY TREATED BEFORE 1ST LINE PARSING?
};

void Cluster::format_response() {

    // we are gonna send a message with a proper HTTP header format (or the browser wouldn't accept it)
    char hello[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    write(this->_cur_socket , hello , strlen(hello));
    std::cout << "[--- HELLO MSG SENT ---]" << std::endl;
};

// // find a server with one of its names, NOT TESTED YET
// Server::t_conf *Cluster::get_server_conf_by_name(std::string &searched_name) {

//     std::list<Server>::iterator server_it = server_list.begin();
//     std::list<std::string>::iterator    content_it;

//     while (server_it != server_list.end()) {

//         content_it = (*server_it).get_serv_info()["server_name"].begin();
//         while (content_it != (*server_it).get_serv_info()["server_name"].end()) {

//             if (*content_it == searched_name)
//                 return &((*server_it).get_conf());
//             ++content_it;
//         }
//         ++server_it;
//     }
//     return NULL;
// };

// Server::t_conf  *Cluster::get_server_conf_by_address(std::string &searched_host, std::string &searched_port) {

//     std::list<Server>::iterator server_it = server_list.begin();
//     std::list<std::string>::iterator    host_it;
//     std::list<std::string>::iterator    port_it;

//     while (server_it != server_list.end()) {

//         host_it = (*server_it).get_serv_info()["server_host"].begin();
//         while (host_it != (*server_it).get_serv_info()["server_host"].end()) {

//             if (*host_it == searched_host) {
//                 port_it = (*server_it).get_serv_info()["server_port"].begin();
//                 while (port_it != (*server_it).get_serv_info()["server_port"].end()) {

//                     if (*port_it == searched_port)
//                         return &((*server_it).get_conf());
//                 }
//             }
//             ++host_it;
//         }
//         ++server_it;
//     }
//     return NULL;
// };
