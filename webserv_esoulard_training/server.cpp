/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 14:51:46 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/11 12:23:12 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string &config) { init_server(config); };
Server::~Server() {};

std::string get_conf_token(char *line, int &index) {
    pass_spaces(line, index);

}

void Server::parse_config(std::string &config) {
    /*
        if (open(CONFIG_FILE) > 0)
        read stuff from config file
        set the relevant Server function members
        (see subject AND COMPARE WITH NGINX BEHAVIOUR FOR PARSING GUIDELINES)
    */
    int config_fd;
    FD_ZERO (&config_fd);
    if ((config_fd = open(config.c_str(), O_RDONLY)) < 0)
        throw Exception("Couldn't open configuration file " + config);
    
    FD_SET (config_fd, &this->_active_fd_set);
    if (select(FD_SETSIZE, &this->_active_fd_set, NULL, NULL, NULL) < 0)
        throw Exception("select error");

    char *line;
    int index;
    while (get_next_line(config_fd, &line) > 0) {
        // std::cout << line << std::endl;
        //TREAT EACH LINE HERE
        index = -1;
        std::string field = get_conf_token(line, index);
        while (line && line[++index]) {
            this->_conf[field].push_back(get_conf_token(line, index));
            //pass_spaces(line, index);
        }


        
        free (line);
    }
    close (config_fd);
};

void Server::init_server(std::string &config) {

    /*
    ** 1) CREATE A SOCKET
    **
    ** int socket(int domain, int type, int protocol);
    ** domain = communication domain. For IPv4, AF_INET
    ** type = type of service. SOCK_STREAM (virtual circuit service), SOCK_DGRAM (datagram service), SOCK_RAW (direct IP service). Depends on our address family
    ** protocol = in case the type chosen offers several protocols to chose from. But virtual circuit service offers only one form
    */

   this->parse_config(config);
   
    if ((this->_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw Exception("socket error");

    // to allow for fast restart, otherwise binding fails because port is still in use from previous attempt
    this->_reuse = 1;
    if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&this->_reuse, sizeof(this->_reuse)) < 0)
        throw Exception("setsockopt error");
  
    /*
    ** 2) NAME A SOCKET
    ** = assigning a transport address (= port) to the socket, the address with which we will access it.
    **
    ** This address is defined in a structure which is different depending on the communication type used. It contains info.
    ** For IP, we use a sockaddr_in structure (netinet/in.h), which contains:
    **  - sin_family: the address family (here, IPv4)
    **  - sin_port: the transport address/port. Servers communicate the port to which clients can connect, clients just put 0.
    **  - sin_addr: system address, leave it to default INADDR_ANY
    ** int bind(int socket, const struct sockaddr *address, socklen_t address_len);
    */

    memset((char *)&this->_address, 0, sizeof(this->_address));
    this->_address.sin_family = AF_INET; // the IPv4 domain we used to open the socket
    this->_address.sin_addr.s_addr = INADDR_ANY; 
    this->_address.sin_port = htons(PORT); // htons() = short int to network
    this->_address.sin_len = sizeof(this->_address); // just the size of the struct
    memset(this->_address.sin_zero, '\0', sizeof(this->_address.sin_zero));

    // We bind our open socket with its new address

    if (bind(this->_server_fd, (struct sockaddr *)&this->_address, this->_address.sin_len) < 0)
        throw Exception("bind error");

    /*
    ** 3) WAIT FOR A CONNECTION
    ** int listen(int socket, int backlog)
    ** Tells a socket that it should be capable of accepting incoming connections
    ** backlog: maximum number of pending connections that can be queued up before connections are refused
    */

    if (listen(this->_server_fd, 3) < 0) 
        throw Exception("listen error");
    /*
    ** Initialize the set of active sockets. 
    */

    FD_ZERO (&this->_active_fd_set);
    FD_SET (this->_server_fd, &this->_active_fd_set);
};

void Server::handle_connection(){

    std::cout << std::endl << "[--- WAITING FOR NEW CONNECTION ---]" << std::endl;
    this->_read_fd_set = this->_active_fd_set;

    if (select(FD_SETSIZE, &this->_read_fd_set, NULL, NULL, NULL) < 0)
        throw Exception("select error");

    for (this->_cur_socket = 0; this->_cur_socket < FD_SETSIZE; ++this->_cur_socket) {
        if (FD_ISSET (this->_cur_socket, &this->_read_fd_set)) {
            if (this->_cur_socket == this->_server_fd) {
                /* Connection request on original socket. */
                if ((this->_new_socket = accept(this->_server_fd, (struct sockaddr *)&this->_address, (socklen_t*)&this->_address.sin_len)) < 0)
                    throw Exception("accept error");
                std::cerr << "Server: connect from host " << inet_ntoa (this->_address.sin_addr) << ", port " <<  ntohs (this->_address.sin_port) << std::endl;
                FD_SET (this->_new_socket, &this->_active_fd_set);    
            }
            else {
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
    }
};

void Server::parse_request() {

    /*
    ** 5) SEND AND RECEIVE MESSAGES
    ** The same read and write system calls that work on files also work on sockets.
    */

    ClientRequest cli_request;
    read(this->_cur_socket, cli_request.get_read(), _MAXLINE);
    ServerResponse serv_response;
    cli_request.parse_request(serv_response);
    // std::cout << "[CLIENT MSG] " << cli_request.get_read() << std::endl; 
    
    //I NEED TO DO TESTS WITH NGINX TO SEE WHAT MATTERS: ARE ERRORS BEYOND FIRST LINE IMPORTANT? ARE THEY TREATED BEFORE 1ST LINE PARSING?
};

void Server::format_response() {
    
    // we are gonna send a message with a proper HTTP header format (or the browser wouldn't accept it)
    char hello[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    write(this->_cur_socket , hello , strlen(hello));
    std::cout << "[--- HELLO MSG SENT ---]" << std::endl;
};
