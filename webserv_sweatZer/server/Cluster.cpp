/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 10:16:04 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/08 18:02:24 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

//setting definitive tables for the whole program
void Cluster::init_cluster(std::string &config) {

    _sigint = false;
    
    for( int i = 0; i < __FD_SETSIZE; ++i)
        _cli_request.push_back(ClientRequest());

    _nb_clients = 0;

    set_mime();
    set_error();

    this->parse_config(config);

    FD_ZERO (&this->_active_fd_set);
    FD_ZERO (&this->_clients_fd_set);

    for (_serv_it = server_list.begin(); _serv_it != server_list.end(); _serv_it++) {
        _serv_it->init_server();
        FD_SET (_serv_it->get_server_fd(), &this->_active_fd_set);
    }

    //in case of ddos suspicion
    _maxed_out_fds = 0;

    _error_serv_unavailable = std::string("HTTP/1.1 503 Service Unavailable\r\nContent-Type: text/html\r\nContent-Length: 47\r\n\r\nSorry, we're a bit busy but we'll be back ASAP!");
    _error_req_timeout = std::string("HTTP/1.1 408 Request Timeout\r\nContent-Type: text/html\r\nContent-Length: 7\r\n\r\nByeeee!");
    _error_req_timeout_sl = std::string("HTTP/1.1 408 Request Timeout\r\nContent-Type: text/html\r\nContent-Length: 44\r\n\r\nSLOW LORIS DETECTED! DANGER AVERTED! Byeeee!");
};

void Cluster::set_mime() {

    int mime_fd;
    if ((mime_fd = open(MIME_TYPES, O_RDONLY)) < 0)
        throw Exception("Couldn't open mime types file " + std::string(MIME_TYPES));

    _line = NULL;
    _sline.clear();

    while (get_next_line(mime_fd, &_line) > 0) {
        _sline = std::string(_line);
        _mime_types.add_entry(_sline);
        ft_memdel((void**)&_line);
    }
    ft_memdel((void**)&_line);
    close (mime_fd);
};

void Cluster::set_error() {

    int err_fd;

    if ((err_fd = open(ERROR_CODES, O_RDONLY)) < 0)
        throw Exception("Couldn't open error codes file " + std::string(ERROR_CODES));

    _line = NULL;
    _sline.clear();

    while (get_next_line(err_fd, &_line) > 0) {
        _sline = std::string(_line);
        _error_codes.add_entry_w_spaces(_sline);
        ft_memdel((void**)&_line);
    }
    ft_memdel((void**)&_line);
    close (err_fd);
};

void Cluster::parse_config(std::string &config) {

    bool        err = false;

    struct stat buf;
    buf.st_mode = 0;
	stat(config.c_str(), &buf);
    if (!S_ISREG(buf.st_mode))
        throw Exception("Bad configuration file: " + config);

    if ((_config_fd = open(config.c_str(), O_RDONLY)) < 0)
        throw Exception("Couldn't open configuration file: " + config);
    
    _in_location = false;
    _in_server = false;
    _line_nb = 0;
    _line = NULL;

    while (get_next_line(_config_fd, &_line) > 0) {

        ++_line_nb;
        _index = 0;

        if (!err && (parse_field((field = get_conf_token(_line, _index)), config) < 0 ||
            parse_values(field, config) < 0))
                err = true;

        ft_memdel((void**)&_line);
    }
    ft_memdel((void**)&_line);
    close (_config_fd);
    if (err)
        throw Exception(_tmp);
    print_config(); //prints all the contents of _conf
    check_conf(config);
};

//prints all the contents of _conf
void Cluster::print_config() {

    std::cerr << "----CONFIG----"<< std::endl;
    _serv_it = server_list.begin();
    int index = 0;

    while (_serv_it != server_list.end()) {
        std::cerr << "------------------------SERVER " << index++ << "------------------------" << std::endl;
        //printing server content
        _serv_it->print_server_info();

        //printing locations content
        _serv_it->print_server_locations();
        ++_serv_it;
    }
    std::cerr << "-----------------------" << std::endl;
}

int Cluster::parse_field(std::string &field, std::string &config) {
    if (field == "server") {

            if (get_conf_token(_line, _index) != "{" || get_conf_token(_line, _index) != "") {
                ft_memdel((void**)&_line);
                _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb);
                return -1;
            }
            if (_in_server) {
                ft_memdel((void**)&_line);
                _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb) + ": already in a server context!";
                return -1;
            }
            Server new_server;
            server_list.push_back(new_server);
            _in_server = true;
    }
    else if (field == "location") {

        if (_in_location) {
            ft_memdel((void**)&_line);
            _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb) + ": already in a location context!";
            return -1;
        }

        Server::t_content_map new_loc;
        server_list.back().get_locations().push_back(new_loc);
        _in_location = true;

        // std::string tmp;
        if ((_tmp = get_conf_token(_line, _index)) == "" || _tmp == "{") {
            ft_memdel((void**)&_line);
            _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb) + "location needs a path";
            return -1;
        }
        if (_tmp.size() > 1 && (_tmp[_tmp.size() - 1] == '*' || _tmp[_tmp.size() - 1] == '/'))
            _tmp = _tmp.substr(0, _tmp.find_last_of('/'));
        server_list.back().get_locations().back()["path"].push_back(_tmp);
        while ((_tmp = get_conf_token(_line, _index)) != "{" && _tmp != "")
            server_list.back().get_locations().back()["extensions"].push_back(_tmp);
        if (_tmp != "{" || get_conf_token(_line, _index) != "") {
            ft_memdel((void**)&_line);
            _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb);
            return -1;
        }
    }
    else if (field == "}") {

        if (_in_location)
            _in_location = false;
        else if (_in_server)
            _in_server = false;
        if (get_conf_token(_line, _index) != "") {
            ft_memdel((void**)&_line);
            _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb);
            return -1;
        }
    }
    return 0;
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

int Cluster::parse_values(std::string &field, std::string &config) {

    while (_line && _line[_index] && _line[_index] != ';') {

        if (_in_server && _in_location)
            server_list.back().get_locations().back()[field].push_back(get_conf_token(_line, _index));
        else if (_in_server)
            server_list.back().get_serv_info()[field].push_back(get_conf_token(_line, _index));
        else {
            ft_memdel((void**)&_line);
            _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb) + ": fields must be in a context";
            return -1;
        }
        if (_line[_index] == ';' && get_conf_token(_line, _index) != "") {
            ft_memdel((void**)&_line);
            _tmp = "Config file " + config + " parsing error near line " + ft_itos(_line_nb);
            return -1;
        }
    }
    return 0;
}

void    Cluster::check_conf(std::string &config) {

    _serv_it = server_list.begin();
    std::list<Server::t_content_map>::iterator  locations_it;

    Server::t_content_map::iterator             serv_info_it;
    Server::t_content_map::iterator             loc_info_it;

    while (_serv_it != server_list.end()) { //iterate on each server block

        serv_info_it = (*_serv_it).get_serv_info().begin();
        while (serv_info_it != (*_serv_it).get_serv_info().end()) {
            // check that except in the case of server_name, we don't accept more than 1 param
            // MAYBE SEVERAL PORTS ARE OK, NOT SURE. IN THIS CASE, ADD ANOTHER EXCEPTION FOR server_port
            if (serv_info_it->second.size() != 1 && serv_info_it->first != "server_name")
                throw Exception("Config file " + config + ": " + serv_info_it->first + "wrong number of parameters");
            if (serv_info_it->first == "client_max_body_size" && serv_info_it->second.size() == 1 && 
                ((*serv_info_it->second.begin()).size() > 7 || ft_stosize_t((*serv_info_it->second.begin())) > 3000000))
                throw Exception("Config file " + config + ": " + serv_info_it->first + " unsupported");
            ++serv_info_it;
        }

        locations_it = (*_serv_it).get_locations().begin();
        while (locations_it != (*_serv_it).get_locations().end()) {
            loc_info_it = (*locations_it).begin();
            while (loc_info_it != (*locations_it).end()) {
                if (loc_info_it->second.size() != 1 && loc_info_it->first != "accept_methods")
                    throw Exception("Config file " + config + ": " + loc_info_it->first + ": wrong number of parameters");
                if (loc_info_it->first == "client_max_body_size" && loc_info_it->second.size() == 1 && 
                    ((*loc_info_it->second.begin()).size() > 7 || ft_stosize_t((*loc_info_it->second.begin())) > 3000000))
                    throw Exception("Config file " + config + ": " + loc_info_it->first + " unsupported");
                ++loc_info_it;
            }
            ++locations_it;
        }
        ++_serv_it;
    }
};

int g_socket = -1;
fd_set *g_active_fd_set = NULL;
fd_set *g_clients_fd_set = NULL;
bool g_sigpipe = false;
bool g_sigint = false;


void Cluster::handle_connection(){

    //std::cerr << std::endl << "*--------------- [--- WAITING FOR NEW CONNECTION ---] ---------------*" << std::endl;
    if (g_sigint) {
        _sigint = true;
        return;
    }
    this->_read_fd_set = this->_active_fd_set;
    this->_write_fd_set = this->_active_fd_set;
    int ret = 0;

    _timeout.tv_sec  = _SELECT_TIMEOUT;
	_timeout.tv_usec = 0;

    ret = select(FD_SETSIZE, &this->_read_fd_set, &this->_write_fd_set, NULL, &_timeout);
    if (ret == -1)
        return;
    for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
        if (FD_ISSET(this->_cur_socket, &this->_clients_fd_set) && !FD_ISSET(this->_cur_socket, &this->_read_fd_set)
            && this->_cli_request[_cur_socket].check_timeout()) {
            std::cerr << "-----------------TIMEOUT SOCK ["<< _cur_socket <<"]---------------------------" << std::endl;

            _cli_request[_cur_socket].reinit_cli();
            send_response(_error_req_timeout);
            close(this->_cur_socket);
            FD_CLR (this->_cur_socket, &this->_clients_fd_set);
            FD_CLR (this->_cur_socket, &this->_active_fd_set);
            --_nb_clients;

        }
    }

    for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
        if (_nb_clients >= _MAXCLIENTS) {

            std::cerr << "----------------- CLIENTS " << _nb_clients << " > MAXCLIENTS(" << _MAXCLIENTS << ")---------------------------" << std::endl;
            ++_maxed_out_fds;
            if (_maxed_out_fds > 20) { //detecting ddos attempt
                for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
                    if (FD_ISSET(this->_cur_socket, &this->_clients_fd_set)) {
                        std::cerr << "----------------- SUSPECTING A DDOS ATTACK, SHILEDING SERVER (error 503)---------------------------" << std::endl;

                        _cli_request[_cur_socket].reinit_cli();
                        send_response(_error_serv_unavailable);
                        close(this->_cur_socket);
                        FD_CLR (this->_cur_socket, &this->_clients_fd_set);
                        FD_CLR (this->_cur_socket, &this->_active_fd_set);
                        --_nb_clients;
                    }
                }
                _maxed_out_fds = 0;
            }
            return;
        }
        _maxed_out_fds = 0;
        if (FD_ISSET (this->_cur_socket, &this->_read_fd_set)) {

            _serv_it = server_list.begin();
            while (_serv_it != server_list.end()) {
                if (this->_cur_socket == _serv_it->get_server_fd()) {
                    /* Connection request on original socket. */

                    if ((this->_new_socket = accept(_serv_it->get_server_fd(), (struct sockaddr *)&(_serv_it->get_address()), &_serv_it->get_address_sz())) < 0) {
                        std::cerr << "----------------- ACCEPT FAILED on socket " << this->_new_socket << std::endl;
                        return;
                    }

                    ++_nb_clients;
                    this->_cli_request[_new_socket].set_timeout();

                    std::cerr << "----------------- Server: connect from host " << inet_ntoa (_serv_it->get_address().sin_addr) << ", port " <<  ntohs (_serv_it->get_address().sin_port) << " to socket [" << this->_cur_socket << "]" << std::endl;

                    fcntl(this->_new_socket, F_SETFL, O_NONBLOCK);
                    FD_SET (this->_new_socket, &this->_active_fd_set);
                    FD_SET (this->_new_socket, &this->_clients_fd_set);

                    g_socket = _new_socket;
                    g_active_fd_set = &this->_active_fd_set;
                    g_clients_fd_set = &this->_clients_fd_set;
                    return ;
                }
                ++_serv_it;
            }
            /* Data arriving on an already-connected socket. */
            g_socket = _cur_socket;
            g_active_fd_set = &this->_active_fd_set;
            g_clients_fd_set = &this->_clients_fd_set;
            this->parse_request();
        }
    }
};

void Cluster::parse_request() {

	unsigned char		buf2[_MAXLINE];
    memset(buf2, 0, _MAXLINE);
	buf.clear();

    _cli_request[_cur_socket].set_timeout();

    int ret = recv(this->_cur_socket, buf2, _MAXLINE - 1, 0);

	if (ret > 0)
	{
		for (int ct = 0 ; ct < ret ; ++ct)
			buf.push_back(buf2[ct]);
	}

    if (ret <= 0) {

        _cli_request[_cur_socket].reinit_cli();

        if (ret < 0) {

            close(this->_cur_socket);
            FD_CLR (this->_cur_socket, &this->_clients_fd_set);
            FD_CLR (this->_cur_socket, &this->_active_fd_set);
            --_nb_clients;
            std::cerr << "----------------- READ ERROR: closing connection.\n" << std::endl;
        }
        else {

            close(this->_cur_socket);
            FD_CLR (this->_cur_socket, &this->_clients_fd_set);
            FD_CLR (this->_cur_socket, &this->_active_fd_set);
            --_nb_clients;
            std::cerr << "----------------- Client closed connection.\n" << std::endl;
        }
		return ;
	}

    (*_serv_response).reinit_serv_response();

    _tmp = (_cli_request[this->_cur_socket].get_sread()) + buf;
    if (_tmp.find("\r\n\r\n") == std::string::npos) { //check if headers are over

        _cli_request[this->_cur_socket].get_sread() += buf;
        return; //headers not over
    }
	else {

        if (!check_body_end(_tmp, *_serv_response)) //body not over
			return ;
	}

    if ((*_serv_response).get_error() == 803) {

        _cli_request[_cur_socket].reinit_cli();
        send_response(_error_req_timeout_sl);
        close(this->_cur_socket);
        FD_CLR (this->_cur_socket, &this->_clients_fd_set);
        FD_CLR (this->_cur_socket, &this->_active_fd_set);
        --_nb_clients;
        std::cerr << "----------------- SLOW LORIS DETECTED: closing connection.\n" << std::endl;
        return;
    }

    _cli_request[_cur_socket].parse_request(*_serv_response);

    if (_cli_request[_cur_socket].get_sread().size() < 10000)
        std::cerr << std::endl << "+++++++++++++++++++++ REQUEST ++++++++++++++++++++" << std::endl << _cli_request[_cur_socket].get_sread() << std::endl;
    else
        std::cerr << std::endl << "+++++++++++++++++++++ REQUEST (too long, truncated) +++++++++++++++++++++" << std::endl << _cli_request[_cur_socket].get_sread().substr(0, 10000) << std::endl;
    std::cerr << "+++++++++++++++++ END OF REQUEST +++++++++++++++++" << std::endl << std::endl;

    (*_serv_response).build_response(_cli_request[_cur_socket].get_conf());
    if (FD_ISSET (this->_cur_socket, &this->_write_fd_set))
	    this->send_response((*_serv_response).get_payload());

    _cli_request[_cur_socket].reinit_cli();
};

// 0 to return and take another recv to complete the request
// 1 to go to client parse req
bool Cluster::check_body_end(std::string &s_tmp, ServerResponse &serv_response) {

    if (s_tmp.find("Content-Length: ") != std::string::npos) {

        _cli_request[this->_cur_socket].get_sread() += buf;

        size_t cl_start = s_tmp.find("Content-Length: ") + 16;
        size_t cl_end = s_tmp.find("\r\n", cl_start);
        size_t len = ft_stoi(s_tmp.substr(cl_start, cl_end - cl_start));

        if (s_tmp.size() - (s_tmp.find("\r\n\r\n") + 4) < len) {
            return 0; //body isn't over
        }
    }
    else if (s_tmp.find("Transfer-Encoding: chunked") != std::string::npos) {
        return handle_chunk(s_tmp, serv_response);
    }
    else {
        _cli_request[this->_cur_socket].get_sread() += buf;
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
    size_t len;

    while (index < chunk.size() && index != std::string::npos) {
        // std::cerr << "-----------------save chunk loop, cur vector " << _vecChunk.size() - 1 << " index " << index << "---------------------" << std::endl;
        chunk = chunk.substr(index);
        index = 0;

        _tmp = chunk;
        one_CRLF = _tmp.find("\r\n");
        two_CRLF = std::string::npos;

        if (_vecChunk.size() % 2 != 0) {
            _tmp = _vecChunk[_vecChunk.size() - 1] + chunk;
            one_CRLF = _tmp.find("\r\n");

            if (one_CRLF == std::string::npos || one_CRLF + 2 >= _tmp.size()) {
                _vecChunk[_vecChunk.size() - 1] += chunk;
                return;
            }

            two_CRLF = _tmp.substr(one_CRLF + 2).find("\r\n");
            if (two_CRLF != std::string::npos)
                two_CRLF += one_CRLF + 2;

            if (one_CRLF == 0) {
                len = two_CRLF;
                if (two_CRLF != std::string::npos) {
                    len = two_CRLF - (one_CRLF + 2);
                }
                _vecChunk[_vecChunk.size() - 1] = _tmp.substr(one_CRLF + 2, len);
                if (two_CRLF == std::string::npos) {
                    return;
                }
                else
                    chunk = _tmp.substr(two_CRLF + 2);
            }
            else {
                _vecChunk[_vecChunk.size() - 1] = _tmp.substr(0, one_CRLF);
                chunk = _tmp.substr(one_CRLF + 2);
            }
            _vecChunk.push_back(std::string(""));

            _tmp = _vecChunk[_vecChunk.size() - 1] + chunk;
            one_CRLF = _tmp.find("\r\n");
            two_CRLF = std::string::npos;

            if ((uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) == 0) {
                if (_vecChunk[_vecChunk.size() - 1].size() < 2)
                    _vecChunk[_vecChunk.size() - 1] = _tmp.substr(0, 2);
                return;
            }
        }

        if (_vecChunk.size() > 1 && (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) < 0)
            return;

        if (_vecChunk.size() > 1 && (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) == 0) {

                _vecChunk[_vecChunk.size() - 1] += chunk;
                return;
        }

        if (_vecChunk.size() > 1 && (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]) > _vecChunk[_vecChunk.size() - 1].size()) {

            _tmp = _vecChunk[_vecChunk.size() - 1] + chunk;
            one_CRLF = _tmp.find("\r\n");
            expected_size = (uint)ft_stoi_hex(_vecChunk[_vecChunk.size() - 2]);
            missing_chars = expected_size - _vecChunk[_vecChunk.size() - 1].size();

            if (one_CRLF == std::string::npos)
                end_of_tmp_chunk = _tmp.size();
            else
                end_of_tmp_chunk = one_CRLF;

            if (expected_size < end_of_tmp_chunk) {
                index += missing_chars;
                _vecChunk[_vecChunk.size() - 1] = _tmp.substr(0, expected_size); // or next_crlf -1 ?
            }
            else if (expected_size >= end_of_tmp_chunk) {
                index = end_of_tmp_chunk - _vecChunk[_vecChunk.size() - 1].size();
                if (end_of_tmp_chunk == one_CRLF)
                    index += 2;
                _vecChunk[_vecChunk.size() - 1] = _tmp.substr(0, end_of_tmp_chunk); // or next_crlf -1 ?

            }
            continue;
        }
        else if (one_CRLF == std::string::npos) {
            _vecChunk.push_back(_tmp);
            return;
        }
        else {
            if (one_CRLF != 0) {
                _vecChunk.push_back(_tmp.substr(0, one_CRLF));
                _vecChunk.push_back(std::string(""));
            }
            index = one_CRLF + 2;
        }
    }
};

bool Cluster::handle_chunk(std::string &s_tmp, ServerResponse &serv_response) {

    //input stopped before chunk
    if ((s_tmp.find("\r\n\r\n") + 4) >= s_tmp.size()) {
        if (_cli_request[this->_cur_socket].get_sread().size() < s_tmp.size()) {
            _cli_request[this->_cur_socket].get_sread() += buf;
        }
        return 0;
    }

    std::string chunk = s_tmp.substr(s_tmp.find("\r\n\r\n") + 4);

    //if the end of the header was just sent with the chunk, we update _sread with the end of the headers
    if (_cli_request[this->_cur_socket].get_sread().find("\r\n\r\n") == std::string::npos) {
        std::string sbuf(buf);
        _cli_request[this->_cur_socket].get_sread() = s_tmp.substr(0, s_tmp.find("\r\n\r\n") + 4);
    }
    // If the end of _sread already has a bit of body, WE CUT IT LIKE THE MONARCHY'S HEADS
    if (_cli_request[this->_cur_socket].get_sread().find("\r\n\r\n") + 4 >= _cli_request[this->_cur_socket].get_sread().size()) {
        _cli_request[this->_cur_socket].get_sread() = _cli_request[this->_cur_socket].get_sread().substr(0, _cli_request[this->_cur_socket].get_sread().find("\r\n\r\n") + 4);
    }

    //save all chunks in a vector alternating size and content
    save_chunk(_cli_request[this->_cur_socket].get_vecChunked(), chunk);

    if (_cli_request[this->_cur_socket].get_vecChunked().size() < 2 || _cli_request[this->_cur_socket].get_vecChunked().size() % 2 != 0)
        return 0;
    int chunk_len = ft_stoi_hex(_cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 2]);
    size_t len;
    if (_cli_request[this->_cur_socket].get_vecChunked().size() >= 5 && _cli_request[this->_cur_socket].get_vecChunked().size() <= 10) {
        size_t i = 0;
        while (i < _cli_request[this->_cur_socket].get_vecChunked().size() && (len = ft_stoi_hex(_cli_request[this->_cur_socket].get_vecChunked()[i])) < _LORIS_CHUNK) {
            i += 2;
            if (i >= _cli_request[this->_cur_socket].get_vecChunked().size()) {
                serv_response.error(803);
                return 1;
            }
        }
    }
    if (chunk_len == 0 && _cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 1].size() >= 2) {
        // we got the last chunk, we can now add all the chunks together to _sread
        for (size_t i = 0; i < _cli_request[this->_cur_socket].get_vecChunked().size(); i++) {

            len = ft_stoi_hex(_cli_request[this->_cur_socket].get_vecChunked()[i]);
            if (len == 0) {
                if (_cli_request[this->_cur_socket].get_vecChunked()[_cli_request[this->_cur_socket].get_vecChunked().size() - 1] != "\r\n") {
                    serv_response.error(400);
                }
                break;
            }
            //If bad chunk len || no chunk content || incomplete chunk || too big chunk ?!
            if (len < 0 || i == _cli_request[this->_cur_socket].get_vecChunked().size() - 1 || len != _cli_request[this->_cur_socket].get_vecChunked()[i + 1].size()) {
                serv_response.error(400);
            }

            i++;
            _cli_request[this->_cur_socket].get_sread() += _cli_request[this->_cur_socket].get_vecChunked()[i].substr(0, len);
        }
    }
    else {//not the last chunk, we saved it to our vector and will add it later
        return 0;
    }

    return 1;
}


void		sighandler(int num)
{
    if (num == SIGPIPE) {
        std::cerr << "----------------- SIGHANDLER: SIGPIPE\n" << std::endl;
        g_sigpipe = true;
    }
    else if (num == SIGINT) {
        std::cerr << "----------------- SIGHANDLER: SIGINT\n" << std::endl;
        g_sigint = true;
    }
}

void Cluster::send_response(std::string &response) {

	size_t	ret = 0;
	ssize_t	res = 0;

	_tmp = response;
    g_socket = _cur_socket;
    g_active_fd_set = &this->_active_fd_set;
    g_clients_fd_set = &this->_clients_fd_set;

	while ((res = send(this->_cur_socket , _tmp.c_str() , _tmp.size(), 0)) != 0 && ret <= response.size() && !g_sigpipe)
	{
		_tmp.clear();
        if (res != -1)
            ret += res;
		_tmp = response.substr(ret);
	}
    if (g_sigpipe) {
        close(g_socket);
        FD_CLR (g_socket, g_active_fd_set);
        FD_CLR (g_socket, g_clients_fd_set);
        --_nb_clients;
        _cli_request[_cur_socket].reinit_cli();
        g_sigpipe = false;
    }
    if (response.size() < 10000)
        std::cerr << std::endl << "+++++++++++++++++++++ RESPONSE ++++++++++++++++++++" << std::endl << response << std::endl;
    else
        std::cerr << std::endl << "++++++++++++ RESPONSE (too long, truncated) ++++++++++++" << std::endl << response.substr(0, 10000) << std::endl;
    std::cerr << "+++++++++++++++++ END OF RESPONSE +++++++++++++++++" << std::endl;

    std::cerr << "----------------- SENT RESPONSE SIZE (" << ret << "/" << response.size() << ")" << std::endl << std::endl;
};
