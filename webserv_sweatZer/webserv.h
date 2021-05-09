/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 14:10:49 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/09 15:51:25 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
#define WEBSERV_H

#define _MAXLINE 655360
#define PORT 8080 // default port

// socket/IP stuff
#include <sys/socket.h>
#include <arpa/inet.h>

// read/write/fd stuff
#include <sys/stat.h>
#include <fcntl.h>

// utils
#include <errno.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <cstdlib>

#include "Exception.hpp"
#include "utils.hpp"
#include "HashTable.hpp"

// server
// #include "Cluster.hpp"
// #include "Server.hpp"

// #include "ServerResponse.hpp"
// #include "ClientRequest.hpp"


#define DEFAULT_CONFIG_FILE "conf.d/webserv.conf"
#define MIME_TYPES          "conf.d/mime.types"

extern SimpleHashTable g_mime_types;
#endif
