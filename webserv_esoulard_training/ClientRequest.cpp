/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 15:46:45 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/08 17:04:54 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest() { _read[0] = '\0'; };
ClientRequest::~ClientRequest() {};

char *ClientRequest::get_read() {
    return _read;
};

void ClientRequest::parse_request(ServerResponse &serv_response) {
    //check if request format is good
    //if not >>> should we stop parsing here and start writing error response ? or keep parsing and recording the info we need.
    //first line will init _method, _file and _protocol
    //then each header field will init each of our ClientRequest attributes. 
    std::string protocol("HTTP/1.1");
    serv_response.set_protocol(protocol);
    
};
