/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 10:16:04 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/18 19:15:13 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

void Cluster::init_cluster(std::string &config) {

    //set all the tables we'll use for comparison here, we'll use them for the whole program
    for( int i = 0; i < _MAXCLIENTS; ++i)
        _cli_request.push_back(ClientRequest());

    set_mime();
    set_error();

    this->parse_config(config);

    FD_ZERO (&this->_active_fd_set);

    std::list<Server>::iterator it;

    for (it = server_list.begin(); it != server_list.end(); it++) {
        it->init_server();
        FD_SET (it->get_server_fd(), &this->_active_fd_set);
    }
};

void Cluster::set_mime() {

    int mime_fd;
    if ((mime_fd = open(MIME_TYPES, O_RDONLY)) < 0)
        throw Exception("Couldn't open mime types file " + std::string(MIME_TYPES));

    char *line;

    while (get_next_line(mime_fd, &line) > 0) {
        _mime_types.add_entry(line);
        free (line);
    }
    free(line);
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

void Cluster::set_error() {

    int err_fd;

    if ((err_fd = open(ERROR_CODES, O_RDONLY)) < 0)
        throw Exception("Couldn't open error codes file " + std::string(ERROR_CODES));

    char *line;

    while (get_next_line(err_fd, &line) > 0) {
        _error_codes.add_entry_w_spaces(line);
        free (line);
    }
    free(line);
    close (err_fd);
};

void Cluster::parse_config(std::string &config) {

    if ((_config_fd = open(config.c_str(), O_RDONLY)) < 0)
        throw Exception("Couldn't open configuration file " + config);

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
    free(_line);
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
        if (tmp.size() > 1 && (tmp[tmp.size() - 1] == '*' || tmp[tmp.size() - 1] == '/'))
            tmp = tmp.substr(0, tmp.find_last_of('/'));
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
                    fcntl(this->_new_socket, F_SETFL, O_NONBLOCK);
                    FD_SET (this->_new_socket, &this->_active_fd_set);
                    return ;
                }
                ++server_it;
            }
            /* Data arriving on an already-connected socket. */
            this->parse_request();
        }
    }
};

void Cluster::parse_request() {

    /*
    ** 5) SEND AND RECEIVE MESSAGES
    ** The same read and write system calls that work on files also work on sockets.
    */

    memset(buf, 0, _MAXLINE);

    int ret = recv(this->_cur_socket, buf, _MAXLINE - 1, 0);
    if (ret <= 0) {
		close(this->_cur_socket);
        FD_CLR (this->_cur_socket, &this->_active_fd_set);
        _cli_request[_cur_socket] = ClientRequest(); //reinit client request for this socket

		if (ret == 0)
			std::cout << "\rConnection was closed by client.\n" << std::endl;
		else
			std::cout << "\rRead error, closing connection.\n" << std::endl;
		return ;
	}

    ServerResponse serv_response(_mime_types, _error_codes, server_list);

    std::string *_sread_ptr = &(_cli_request[this->_cur_socket].get_sread());
    std::string s_tmp = (*_sread_ptr) + std::string(buf);

    if (s_tmp.find("\r\n\r\n") == std::string::npos) { //check if headers are over

        _cli_request[this->_cur_socket].get_sread() += std::string(buf);
        _cli_request[this->_cur_socket].set_read();
        return; //headers not over
    }
	else { //check if body is over

        if (!check_body_end(s_tmp, _sread_ptr, serv_response)) //body not over
            return ;
	}

    _cli_request[_cur_socket].parse_request(serv_response, this->_cur_socket);
    std::cout << "++++++++++++++++++++++++++++++++ COCO L'ASTICOT +++++++++++++++++++++++++++++++++++++++" << _cli_request[_cur_socket].get_read() << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++ COCO L'ASTICOBIS ++++++++++++++++++++++++++++++++++++++" << std::endl;

    serv_response.build_response(_cli_request[_cur_socket].get_conf());
    this->send_response(serv_response.get_payload());

    _cli_request[_cur_socket] = ClientRequest(); //reinitializing client request for this socket
};

// 0 to return and take another recv to complete the request
// 1 to go to client parse req
bool Cluster::check_body_end(std::string &s_tmp, std::string *_sread_ptr, ServerResponse &serv_response) {

 // if theres a content length, check if body size matches
    if (s_tmp.find("Content-Length: ") != std::string::npos) {

        _cli_request[this->_cur_socket].get_sread() += std::string(buf);
        _cli_request[this->_cur_socket].set_read();

        size_t cl_start = s_tmp.find("Content-Length: ") + 16;
        size_t cl_end = s_tmp.find("\r\n", cl_start);
        size_t len = ft_stoi(s_tmp.substr(cl_start, cl_end - cl_start));

        if (s_tmp.size() - (s_tmp.find("\r\n\r\n") + 4) < len) {
            return 0; //body isn't over
        }
    }
    else if (s_tmp.find("Transfer-Encoding: chunked") != std::string::npos) {

        return handle_chunk(s_tmp, _sread_ptr, serv_response);
    }
    else {
        _cli_request[this->_cur_socket].get_sread() += std::string(buf);
        _cli_request[this->_cur_socket].set_read();
    }

    return 1;
}

void Cluster::save_chunk(std::vector<std::string> *_vecChunk, std::string &chunk) {

    // //previous chunk not over
    // if ((*_vecChunk).size() > 1 && ((uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) > (*_vecChunk)[(*_vecChunk).size() - 1].size()
    //     || ((*_vecChunk)[(*_vecChunk).size() - 2] == "0" && (*_vecChunk)[(*_vecChunk).size() - 1] == ""))) {
    //     (*_vecChunk)[(*_vecChunk).size() - 1] += chunk;
    //     return;
    // }
    // if (chunk.find("\r\n") == std::string::npos) {
    //     (*_vecChunk).push_back(chunk);
    //     (*_vecChunk).push_back(std::string(""));
    // }
    // else {
    //     (*_vecChunk).push_back(chunk.substr(0, chunk.find("\r\n")));
    //     if (chunk.find("\r\n") + 2 < chunk.size())
    //         (*_vecChunk).push_back(chunk.substr(chunk.find("\r\n") + 2));
    //     else
    //         (*_vecChunk).push_back(std::string(""));
    // }
    size_t index = 0;
    size_t next_CRLF;
    size_t missing_chars;
    size_t chunk_left;

    std::cout << "IN SAVE CHUNK [" <<  chunk << "]" << std::endl;

    while (index < chunk.size() && index != std::string::npos) {

        while (index != std::string::npos && index == chunk.find("\r\n") && index < chunk.size()) 
            index += 4;
        if (index == std::string::npos || index >= chunk.size())
            return;
        
        chunk = chunk.substr(index);
        next_CRLF = chunk.find("\r\n");

        std::cout << "IN SAVE CHUNK LOOP chunk [" << chunk << "]" << std::endl;

        if ((*_vecChunk).size() > 1 && (uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) < 0)
            return; //bad hex value for next vector

        if ((*_vecChunk).size() > 1 && (uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) > (*_vecChunk)[(*_vecChunk).size() - 1].size()) {
            
            missing_chars = (uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) - (*_vecChunk)[(*_vecChunk).size() - 1].size();
            if (next_CRLF == std::string::npos)
                chunk_left = chunk.size() - index;
            else
                chunk_left = next_CRLF - index;

            if (missing_chars < chunk_left) {
                (*_vecChunk)[(*_vecChunk).size() - 1] += chunk.substr(index, missing_chars); // or next_crlf -1 ?
                index += missing_chars;
            }
            else if (missing_chars >= chunk_left) {
                (*_vecChunk)[(*_vecChunk).size() - 1] += chunk.substr(index, chunk_left); // or next_crlf -1 ?
                index = next_CRLF;
            }
            continue;
        }
        else if (next_CRLF == std::string::npos) {
            (*_vecChunk).push_back(chunk);
            (*_vecChunk).push_back(std::string(""));
            return;
        }
        else {
            (*_vecChunk).push_back(chunk.substr(0, next_CRLF));
            (*_vecChunk).push_back(std::string(""));
            index = next_CRLF;
            std::cout << "IN LAST ELSE vec size " <<(*_vecChunk).size() << " [" << (*_vecChunk)[0] << "] [" << (*_vecChunk)[1] << "]" << std::endl;
        }
    }
};

bool Cluster::handle_chunk(std::string &s_tmp, std::string *_sread_ptr, ServerResponse &serv_response) {

    //input stopped before chunk
    if ((s_tmp.find("\r\n\r\n") + 5) >= s_tmp.size()) {
        if (_cli_request[this->_cur_socket].get_sread().size() < s_tmp.size()) {
            _cli_request[this->_cur_socket].get_sread() += std::string(buf);
            _cli_request[this->_cur_socket].set_read();
        }
        return 0;
    }

    std::string chunk = s_tmp.substr(s_tmp.find("\r\n\r\n") + 4);

    //if the end of the header was just sent with the chunk, we update _sread with the end of the headers
    if ((*_sread_ptr).find("\r\n\r\n") == std::string::npos) {
        std::string sbuf(buf);
        _cli_request[this->_cur_socket].get_sread() += sbuf.substr(0, sbuf.find("\r\n\r\n") + 4);
        _cli_request[this->_cur_socket].set_read();
    }

    //save all chunks in a vector alternating size and content
    std::vector<std::string> *_vecChunked_ptr = &(_cli_request[this->_cur_socket].get_vecChunked());
    save_chunk(_vecChunked_ptr, chunk);
    int chunk_len = ft_stoi_hex((*_vecChunked_ptr)[(*_vecChunked_ptr).size() - 2]);

    // std::cout << " AFTER SAVE CHUNK LAST CHUNK LEN = " << chunk_len << std::endl;

    if (chunk_len == 0) {
        // we got the last chunk, we can now add all the chunks together to _sread
        for (size_t i = 0; i < (*_vecChunked_ptr).size(); i++) {
            if ((*_vecChunked_ptr)[i] == "0")
                break;

            size_t len = ft_stoi((*_vecChunked_ptr)[i]);

            //bad chunk len || no chunk content || incomplete chunk || too big chunk ?!
            if (len < 0 || i == (*_vecChunked_ptr).size() - 1 || len > (*_vecChunked_ptr)[i + 1].size()
                || len < (*_vecChunked_ptr)[i + 1].size())
                serv_response.error(400);

            i++;
            _cli_request[this->_cur_socket].get_sread() += (*_vecChunked_ptr)[i];
        }
        _cli_request[this->_cur_socket].set_read();
    }
    else //not the last chunk, we saved it to our vector and will add it later
        return 0;

    return 1;
}

void Cluster::send_response(std::string &response) {

    write(this->_cur_socket , response.c_str() , response.size());
    std::cout << "[--- MSG SENT ---]" << std::endl;
    std::cout << "[" << response << "]" << std::endl << "SIZE{ " << response.size() << "}" << std::endl;
};
