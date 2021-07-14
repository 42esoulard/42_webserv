/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 10:16:04 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/14 11:34:11 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

void Cluster::init_cluster(std::string &config) {

    //set all the tables we'll use for comparison here, we'll use them for the whole program
    for( int i = 0; i < __FD_SETSIZE; ++i)
        _cli_request.push_back(ClientRequest());

    _nb_clients = 0;
    
    set_mime();
    set_error();

    this->parse_config(config);

    FD_ZERO (&this->_active_fd_set);
    FD_ZERO (&this->_clients_fd_set);

    std::list<Server>::iterator it;

    for (it = server_list.begin(); it != server_list.end(); it++) {
        it->init_server();
        FD_SET (it->get_server_fd(), &this->_active_fd_set);
    }

    _error_serv_unavailable = std::string("HTTP/1.1 503 Service Unavailable\r\nContent-Type: text/html\r\nContent-Length: 47\r\n\r\nSorry, we're a bit busy but we'll be back ASAP!");
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

int g_socket = -1;
fd_set *g_active_fd_set = NULL;
fd_set *g_clients_fd_set = NULL;
bool g_sigpipe = false;

void		sighandler(int num)
{
    std::cout << "in sighandler" << std::endl;
    if (num == SIGPIPE) {
        std::cout << "in sighandler sigpipe" << std::endl;
        getchar();
        close(g_socket);
        FD_CLR (g_socket, g_active_fd_set);
        FD_CLR (g_socket, g_clients_fd_set);
        g_sigpipe = true;
    }
}

void Cluster::handle_connection(){

    std::cout << std::endl << "[--- WAITING FOR NEW CONNECTION ---]" << std::endl;

    this->_read_fd_set = this->_active_fd_set;
    int ret = 0;
    while (ret <= 0) {
        if ((ret = select(FD_SETSIZE, &this->_read_fd_set, NULL, NULL, NULL)) < 0)
            std::cout << "select fail but we're ok" << std::endl;
            //throw Exception("select error");
        for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
            if (FD_ISSET(this->_cur_socket, &this->_clients_fd_set) && !FD_ISSET(this->_cur_socket, &this->_read_fd_set) 
                && this->_cli_request[_cur_socket].check_timeout()) {
                std::cout << "-------------------------TIMEOUT in select loop sock ["<< _cur_socket <<"]---------------------------" << std::endl;
                getchar();
                _cli_request[_cur_socket] = ClientRequest();
                close(this->_cur_socket);
                FD_CLR (this->_cur_socket, &this->_clients_fd_set);
                FD_CLR (this->_cur_socket, &this->_active_fd_set);
                --_nb_clients;
            }
        }
    }

    for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
        if (FD_ISSET (this->_cur_socket, &this->_read_fd_set)) {

            std::list<Server>::iterator server_it = server_list.begin();
            while (server_it != server_list.end()) {
                if (this->_cur_socket == server_it->get_server_fd()) {
                    /* Connection request on original socket. */

                    if (_nb_clients >= _MAXCLIENTS) {
                        std::cout << "socket " << this->_new_socket << " | " << _nb_clients << "> MAXCLIENTS" << _MAXCLIENTS <<"; sending error 503" << std::endl;
                        this->send_response(_error_serv_unavailable);
                        return;
                    }
                    if ((this->_new_socket = accept(server_it->get_server_fd(), (struct sockaddr *)&(server_it->get_address()), &server_it->get_address_sz())) < 0) {
                        std::cout << "accept fail ret " << this->_new_socket << " errno " << errno << std::endl;
                        return;
                        //throw Exception("accept error");
                    }
                    ++_nb_clients;
                    this->_cli_request[_cur_socket].set_timeout();

                    std::cerr << "Server: connect from host " << inet_ntoa (server_it->get_address().sin_addr) << ", port " <<  ntohs (server_it->get_address().sin_port) << "cur_socket [" << this->_cur_socket << "]" << std::endl;
                    fcntl(this->_new_socket, F_SETFL, O_NONBLOCK);
                    FD_SET (this->_new_socket, &this->_active_fd_set);
                    FD_SET (this->_new_socket, &this->_clients_fd_set);
                    // if (this->_new_socket >= _MAXCLIENTS) {
                    //     std::cout << "socket " << this->_new_socket << "> MAXCLIENTS" << _MAXCLIENTS <<"; sending error 503" << std::endl;
                    //     this->send_response(_error_serv_unavailable);
                    // }
                    return ;
                }
                ++server_it;
            }
            /* Data arriving on an already-connected socket. */
            g_socket = _cur_socket;
            g_active_fd_set = &this->_active_fd_set;
            g_clients_fd_set = &this->_clients_fd_set;
            this->parse_request();
        }
        //MOVE THIS TO SELECT LOOP?
        // if (FD_ISSET (this->_cur_socket, &this->_clients_fd_set) 
        //     && this->_cli_request[_cur_socket].check_timeout()) {
        //     std::cout << "-------------------------TIMEOUT sock ["<< _cur_socket <<"]---------------------------" << std::endl;
        //     getchar();
        //     close(this->_cur_socket);
        //     FD_CLR (this->_cur_socket, &this->_clients_fd_set);
        //     FD_CLR (this->_cur_socket, &this->_active_fd_set);
        //     this->_cur_socket = 0;
        // }
    }
};

void Cluster::parse_request() {

    /*
    ** 5) SEND AND RECEIVE MESSAGES
    ** The same read and write system calls that work on files also work on sockets.
    */

    memset(buf, 0, _MAXLINE);

    _cli_request[_cur_socket].set_timeout();
    
    int ret = recv(this->_cur_socket, buf, _MAXLINE - 1, 0);
    // if (ret <= 0) {
    if (ret <= 0) {
		
        _cli_request[_cur_socket] = ClientRequest(); //reinit client request for this socket

        // std::cout << "closing connection [" << _cur_socket << "] ret " << ret << std::endl;
		// if (ret == 0)
		// 	std::cout << "\rConnection was closed by client.\n" << std::endl;
		// else {
        if (ret < 0) {
            close(this->_cur_socket);
            FD_CLR (this->_cur_socket, &this->_clients_fd_set);
            FD_CLR (this->_cur_socket, &this->_active_fd_set);
            --_nb_clients;
        	std::cout << "\rRead error, closing connection.\n" << std::endl;
        }
        else {
            FD_CLR (this->_cur_socket, &this->_clients_fd_set);
            FD_CLR (this->_cur_socket, &this->_active_fd_set);
        }
		return ;
	}
    // usleep(500);

    ServerResponse serv_response(_mime_types, _error_codes, server_list);

    std::string *_sread_ptr = &(_cli_request[this->_cur_socket].get_sread());
    std::string s_tmp = (*_sread_ptr) + std::string(buf);
    // std::cout << "after rescv buf [" << buf << "]" <<std::endl;

    if (s_tmp.find("\r\n\r\n") == std::string::npos) { //check if headers are over

        _cli_request[this->_cur_socket].get_sread() += std::string(buf);
        return; //headers not over
    }
	else { //check if body is over

        if (!check_body_end(s_tmp, serv_response)) //body not over
            return ;
	}

    _cli_request[_cur_socket].parse_request(serv_response, this->_cur_socket);
    if (_cli_request[_cur_socket].get_sread().size() < 10000)
        std::cout << "++++++++++++++++++++++++++++++++ COCO L'ASTICOT +++++++++++++++++++++++++++++++++++++++" << _cli_request[_cur_socket].get_sread() << std::endl;
    else
        std::cout << "++++++++++++++++++++++++++++++++ COCO L'ASTICOT +++++++++++++++++++++++++++++++++++++++" << _cli_request[_cur_socket].get_sread().substr(0, 10000) << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++ COCO L'ASTICOBIS ++++++++++++++++++++++++++++++++++++++" << std::endl;
    serv_response.build_response(_cli_request[_cur_socket].get_conf());
	this->send_response(serv_response.get_payload());
    _cli_request[_cur_socket] = ClientRequest(); //reinitializing client request for this socket
};

// 0 to return and take another recv to complete the request
// 1 to go to client parse req
bool Cluster::check_body_end(std::string &s_tmp, ServerResponse &serv_response) {

    // if theres a content length, check if body size matches
    // std::cout << "IN CHECK BODY END s_tmp [" << s_tmp << "]" << std::endl << "_sread_ptr [" << *_sread_ptr << "]" << std::endl;
    if (s_tmp.find("Content-Length: ") != std::string::npos) {

        _cli_request[this->_cur_socket].get_sread() += std::string(buf);

        size_t cl_start = s_tmp.find("Content-Length: ") + 16;
        size_t cl_end = s_tmp.find("\r\n", cl_start);
        size_t len = ft_stoi(s_tmp.substr(cl_start, cl_end - cl_start));

        if (s_tmp.size() - (s_tmp.find("\r\n\r\n") + 4) < len) {
            return 0; //body isn't over
        }
    }
    else if (s_tmp.find("Transfer-Encoding: chunked") != std::string::npos) {
        // _sread_ptr = &(_cli_request[this->_cur_socket].get_sread());
        return handle_chunk(s_tmp, serv_response);
    }
    else {
        _cli_request[this->_cur_socket].get_sread() += std::string(buf);
    }

    return 1;
}

void Cluster::save_chunk(std::vector<std::string> &_vecChunk, std::string &chunk) {

    size_t index = 0;
    size_t one_CRLF;
    size_t two_CRLF;
    size_t expected_size;
    size_t missing_chars;
    size_t end_of_tmp_chunk;
    std::string tmp;
    size_t len;

    // std::cout << "IN SAVE CHUNK [" <<  chunk << "] size "<< chunk.size() << std::endl;

    while (index < chunk.size() && index != std::string::npos) {
        std::cout << "-----------------save chunk loop, cur vector " << _vecChunk.size() - 1 << " index " << index << "---------------------" << std::endl;
        chunk = chunk.substr(index);
        // std::cout << "after substr chunk is [" << chunk << "]" << std::endl;
        index = 0;

        tmp = chunk;
        one_CRLF = tmp.find("\r\n");
        two_CRLF = std::string::npos;

        // SIZE IS INCOMPLETE
        if (_vecChunk.size() % 2 != 0) {
            std::cout << "0 Gonna complete incomplete size [" << _vecChunk[_vecChunk.size() - 1] << "]" << std::endl;
            tmp = _vecChunk[_vecChunk.size() - 1] + chunk;
            one_CRLF = tmp.find("\r\n");

            if (one_CRLF == std::string::npos || one_CRLF + 2 >= tmp.size()) {
                _vecChunk[_vecChunk.size() - 1] += chunk;
                return;
            }

            // std::cout << "gonna look for two crlf in substr [" << tmp.substr(one_CRLF + 2) << "] from str [" << tmp << "]" << std::endl;
            two_CRLF = tmp.substr(one_CRLF + 2).find("\r\n");
            if (two_CRLF != std::string::npos)
                two_CRLF += one_CRLF + 2;
                
            if (one_CRLF == 0) {
                // std::cout << "here 1" << std::endl;
                len = two_CRLF;
                if (two_CRLF != std::string::npos) {
                    // std::cout << "two crlf isnt npos! it's " << two_CRLF << std::endl;
                    len = two_CRLF - (one_CRLF + 2);
                }
                // std::cout << "GONNA SUBSTR from " << one_CRLF + 2 << " for len " << len << std::endl;
                _vecChunk[_vecChunk.size() - 1] = tmp.substr(one_CRLF + 2, len);
                // std::cout << "now vec with size contains [" << (*_vecChunk)[(*_vecChunk).size() - 1] << "] len [" << (*_vecChunk)[(*_vecChunk).size() - 1].size() << "]" << std::endl;
                if (two_CRLF == std::string::npos) {
                    // std::cout << "size still not over!!!" << std::endl;
                    return;
                }
                else
                    chunk = tmp.substr(two_CRLF + 2);
                // std::cout << "now chunk = [" << chunk << "] size " << chunk.size() << std::endl;
            }
            else {
                // std::cout << "here 2" << std::endl;
                _vecChunk[_vecChunk.size() - 1] = tmp.substr(0, one_CRLF);
                chunk = tmp.substr(one_CRLF + 2);
            }
            // std::cout << "here size [" << (*_vecChunk)[(*_vecChunk).size() - 1] << "]" << std::endl;
            _vecChunk.push_back(std::string(""));

            tmp = _vecChunk[_vecChunk.size() - 1] + chunk;
            one_CRLF = tmp.find("\r\n");
            two_CRLF = std::string::npos;

            if ((uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) == 0) {
                if (_vecChunk[_vecChunk.size() - 1].size() < 2)
                    _vecChunk[_vecChunk.size() - 1] = tmp.substr(0, 2);
                // std::cout << "0 VECTOR 0 content [" << (*_vecChunk)[(*_vecChunk).size() - 1] << "]" << std::endl;
                return;
            }
            // std::cout << "0 Completed incomplete size now [" << (*_vecChunk)[(*_vecChunk).size() - 2] << "]" << std::endl;
            // std::cout << "0 Chunk now [" << chunk << "] tmp [" << tmp << "]" << std::endl;
            
        }

        // HEX SIZE IS SHIT
        if (_vecChunk.size() > 1 && (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) < 0)
            return; //bad hex value for next vector

        // HEX SIZE IS 0
        if (_vecChunk.size() > 1 && (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) == 0) {
                std::cout << "0 IN SAVE CHUNK 0 [" << _vecChunk[_vecChunk.size() - 1] << "]" << std::endl;

                _vecChunk[_vecChunk.size() - 1] += chunk;
                // std::cout << "1 IN SAVE CHUNK 0 starting with  [" << (*_vecChunk)[(*_vecChunk).size() - 1][0] << "] size [" << (*_vecChunk)[(*_vecChunk).size() - 1].size() << "]" << std::endl;
                // std::cout << "1 VECTOR 0 content [" << (*_vecChunk)[(*_vecChunk).size() - 1] << "]" << std::endl;

                return;
        }

        // CONTENT IS INCOMPLETE
        if (_vecChunk.size() > 1 && (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) > _vecChunk[_vecChunk.size() - 1].size()) {

            tmp = _vecChunk[_vecChunk.size() - 1] + chunk;
            one_CRLF = tmp.find("\r\n");

            expected_size = (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]);

            missing_chars = expected_size - _vecChunk[_vecChunk.size() - 1].size();
            std::cout << "1 Gonna complete incomplete content starting with  [" << _vecChunk[_vecChunk.size() - 1][0] << "] size [" << _vecChunk[_vecChunk.size() - 1].size() << "] currently missing " << missing_chars << "/" << (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) << std::endl;
            // std::cout << "1 Gonna complete incomplete content currently missing " << missing_chars << "/" << (uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) << std::endl;

            if (one_CRLF == std::string::npos)
                end_of_tmp_chunk = tmp.size();
            else
                end_of_tmp_chunk = one_CRLF;

        //    std::cout << "chunk currently [" << chunk << "]" << std::endl;
            if (expected_size < end_of_tmp_chunk) {
                // std::cout << "expected size " << expected_size << "< end of chunk " << end_of_tmp_chunk << std::endl;
                index += missing_chars;
                _vecChunk[_vecChunk.size() - 1] = tmp.substr(0, expected_size); // or next_crlf -1 ?
                // std::cout << "vecchunk now [" << (*_vecChunk)[(*_vecChunk).size() - 1] << "] index [" << index << "]" << std::endl;
            }
            else if (expected_size >= end_of_tmp_chunk) {
                // std::cout << "expected size " << expected_size << ">= end of chunk " << end_of_tmp_chunk << std::endl;
                index = end_of_tmp_chunk - _vecChunk[_vecChunk.size() - 1].size();
                if (end_of_tmp_chunk == one_CRLF)
                    index += 2;
                _vecChunk[_vecChunk.size() - 1] = tmp.substr(0, end_of_tmp_chunk); // or next_crlf -1 ?

            }
        //     std::cout << "1 Completed incomplete content, now starting with  [" << (*_vecChunk)[(*_vecChunk).size() - 1][0] << "] size " << (*_vecChunk)[(*_vecChunk).size() - 1].size() << "/" << (uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) << std::endl;
        //    std::cout << "1 Completed incomplete content, now containing size " << (*_vecChunk)[(*_vecChunk).size() - 1].size() << "/" << (uint)ft_stoi_hex((*_vecChunk)[(*_vecChunk).size() - 2]) << std::endl;
            continue;
        }
        else if (one_CRLF == std::string::npos) {
            _vecChunk.push_back(tmp);
            std::cout << "Added incomplete size [" << _vecChunk[_vecChunk.size() - 1] << "]" << std::endl;
            // getchar();
            return;
        }
        else {
            if (one_CRLF != 0) {
                std::cout << "bef crash" << std::endl;
                _vecChunk.push_back(tmp.substr(0, one_CRLF));
                _vecChunk.push_back(std::string(""));
                std::cout << "aft crash" << std::endl;

                // std::cout << "Added complete size [" << (*_vecChunk)[(*_vecChunk).size() - 2] << "] with an empty content" << std::endl;
            }
            //getchar();
            index = one_CRLF + 2;
        }
    }
};

bool Cluster::handle_chunk(std::string &s_tmp, ServerResponse &serv_response) {

    // std::cout << "0 IN HANDLE CHUNK s_tmp [" << s_tmp << "]" << std::endl;
    // std::cout << "0 IN HANDLE CHUNK buf [" << buf << "]" << std::endl;
    //input stopped before chunk
    if ((s_tmp.find("\r\n\r\n") + 4) >= s_tmp.size()) {
        if (_cli_request[this->_cur_socket].get_sread().size() < s_tmp.size()) {
            _cli_request[this->_cur_socket].get_sread() += std::string(buf);
        }
        // std::cout << "a IN HANDLE CHUNK _sread [" << _cli_request[this->_cur_socket].get_sread() << "]" << std::endl;
        // getchar();
        return 0;
    }

    //std::cout << "1 IN HANDLE CHUNK s_tmp [" << s_tmp << "]" << std::endl;
    std::string chunk = s_tmp.substr(s_tmp.find("\r\n\r\n") + 4);
    // std::cout << "IN HANDLE CHUNK chunk [" << chunk << "]" << std::endl;
    // getchar();
    //if the end of the header was just sent with the chunk, we update _sread with the end of the headers
    if (_cli_request[this->_cur_socket].get_sread().find("\r\n\r\n") == std::string::npos) {
        std::string sbuf(buf);
        _cli_request[this->_cur_socket].get_sread() = s_tmp.substr(0, s_tmp.find("\r\n\r\n") + 4);
        // std::cout << "b IN HANDLE CHUNK _sread [" << _cli_request[this->_cur_socket].get_sread() << "]" << std::endl;
    }
    // If the end of _sread already has a bit of body, WE CUT IT LIKE THE MONARCHY'S HEADS
    if (_cli_request[this->_cur_socket].get_sread().find("\r\n\r\n") + 4 >= _cli_request[this->_cur_socket].get_sread().size()) {
        _cli_request[this->_cur_socket].get_sread() = _cli_request[this->_cur_socket].get_sread().substr(0, _cli_request[this->_cur_socket].get_sread().find("\r\n\r\n") + 4);
        // getchar();
        // std::cout << "c IN HANDLE CHUNK _sread [" << _cli_request[this->_cur_socket].get_sread() << "]" << std::endl;
    }

    //save all chunks in a vector alternating size and content
    // std::vector<std::string> *_vecChunked_ptr = &(_cli_request[this->_cur_socket].get_vecChunked());
    save_chunk(_cli_request[this->_cur_socket].get_vecChunked(), chunk);
    int chunk_len = ft_stoi_hex(_cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 2]);

    // std::cout << " AFTER SAVE CHUNK LAST CHUNK LEN = " << chunk_len << std::endl;

    if (chunk_len == 0 && _cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 1].size() >= 2) {
        // std::cout << "ADDING ALL CHUNKS TO BODY" << std::endl;
        // getchar();
        // we got the last chunk, we can now add all the chunks together to _sread
        for (size_t i = 0; i < _cli_request[this->_cur_socket].get_vecChunked().size(); i++) {
            //std::cout << " adding chunk index " << i << std::endl;

            size_t len = ft_stoi_hex(_cli_request[this->_cur_socket].get_vecChunked()[i]);
            if (len == 0) { // SHOULD WE CHECK THAT CHUNK 0 HAS THE PROPER \R\N CONTENT?
                // std::cout << "chunk 0!" << std::endl;
                if (_cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 1] != "\r\n") {
                    std::cout << "error in adding last chunk i " << i << " len " << _cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 1].size() << "content [" << _cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 1] << "]" << std::endl;
                    serv_response.error(400);
                    getchar();
                }
                break;
            }
            //If bad chunk len || no chunk content || incomplete chunk || too big chunk ?!
            if (len < 0 || i == _cli_request[this->_cur_socket].get_vecChunked().size() - 1 || len != _cli_request[this->_cur_socket].get_vecChunked()[i + 1].size()) {
                // std::cout << "error in adding chunk i" << i << " len " << len << "(*_vecChunked_ptr)[i + 1].size() - 2" << (*_vecChunked_ptr)[i + 1].size() - 2 << std::endl;
                serv_response.error(400);
            }

            i++;
            //std::cout << "adding chunk [" << (*_vecChunked_ptr)[i].substr(0, len) << "] to body" << std::endl;
            _cli_request[this->_cur_socket].get_sread() += _cli_request[this->_cur_socket].get_vecChunked()[i].substr(0, len);
        }
    }
    else {//not the last chunk, we saved it to our vector and will add it later
        return 0;
    }

    return 1;
}



void Cluster::send_response(std::string &response) {

	//std::cout << response.c_str() << std::endl;
	// if (response.size() >= 10000)
	// {
	// 	std::cout << "BEFORE WRITE [" << response.substr(0, 10000) << "]" << std::endl;
	// 	std::cout << "AFTER WRITE [" << response.substr(response.size()-10000) << "]" << std::endl;
	// }
	size_t	ret = 0;
	ssize_t	res = 0;
	std::string	tmp = response;
    g_socket = _cur_socket;
    g_active_fd_set = &this->_active_fd_set;
    g_clients_fd_set = &this->_clients_fd_set;
    
    std::cout << "RIGHT BEFORE SEND" << std::endl;
    
	while ((res = send(this->_cur_socket , tmp.c_str() , tmp.size(), 0)) != 0 && ret <= response.size() && !g_sigpipe)
	{
        std::cout << "send loop top res " << res << " ret " << ret << " res + ret " << ret + res << std::endl;
		tmp.clear();
        if (res != -1)
            ret += res;
		tmp = response.substr(ret);
		//usleep(900);
        
        std::cout << "send loop bottom" << std::endl;
	}
    if (g_sigpipe) {
        --_nb_clients;
        _cli_request[_cur_socket] = ClientRequest();
        g_sigpipe = false;
    
    }
    std::cout << " after res " << res << " ret " << ret << " res + ret " << ret + res << std::endl;
    /*size_t i = 0;
    // size_t pseudo_chunk = 100000;
    std::string tmp;
    // size_t total_write = 0;
    while (i < response.size()) {
        // std::cout << "chunk " << i << " to " << i + pseudo_chunk << "!" << std::endl;
		tmp.clear();
		if (i + _MAXLINE > response.size())
			tmp = response.substr(i);
		else
			tmp = response.substr(i, _MAXLINE);
        send(this->_cur_socket , tmp.c_str() , tmp.size(), 0);
        i += _MAXLINE;
        usleep(900);
    }*/
	/*tmp = response.substr(i, _MAXLINE);
    send(this->_cur_socket , tmp.c_str() , tmp.size(), 0);*/
    // std::cout << "TOTAL WRITE [" << total_write << "]" << std::endl;
    // getchar();
    //std::cout << "[--- MSG SENT ---]" << std::endl;
    // std::cout << "[" << response << "]" << std::endl << "SIZE{ " << response.size() << "}" << std::endl;
    //std::cout << "SIZE{ " << response.size() << "}" << std::endl;
};
