/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 14:10:49 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/27 14:52:16 by esoulard         ###   ########.fr       */
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
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

// utils
#include <errno.h>
#include <string.h> // REMOVE THIS?
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <cstdlib>

// webserv accessories
#include "Exception.hpp"
#include "utils.hpp"
#include "HashTable.hpp"

// server
//#include "Server.hpp"
//#include "Cluster.hpp"
//#include "ServerResponse.hpp"
//#include "ClientRequest.hpp"


#define DEFAULT_CONFIG_FILE "conf.d/webserv.conf"
#define MIME_TYPES          "conf.d/mime.types"
#define ERROR_CODES         "conf.d/error_codes"
#define DEFAULT_MAX_BODY    "1000000"
#define DEFAULT_UPLOAD_NAME "new.txt"
#define ERROR_FOLDER        "html/"

#endif
