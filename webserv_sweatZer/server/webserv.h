/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 14:10:49 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/04 17:36:59 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
#define WEBSERV_H

#define _TIMEOUT        60 //in seconds
#define _SELECT_TIMEOUT 10
#define _MAXLINE        65536
#define _MAXFD          4096
#define _MAXCLIENTS     250
#define _MAXHEADERSIZE  65536
#define _MAXHEADERFIELD 8000
#define	_MAXURI         2000
#define _LORIS_CHUNK    10
#define PORT            8080 // default port

// socket/IP stuff
#include <sys/socket.h>
#include <arpa/inet.h>

// read/write/fd stuff
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

// CGI and process
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

// utils
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <cstdlib>
#include <cstdio>

// webserv accessories
#include "Exception.hpp"
#include "utils.hpp"
#include "HashTable.hpp"

#define DEFAULT_CONFIG_FILE "conf.d/webserv.conf"
#define MIME_TYPES          "conf.d/mime.types"
#define ERROR_CODES         "conf.d/error_codes"
#define DEFAULT_MAX_BODY    1000000
#define DEFAULT_UPLOAD_NAME "new.txt"
#define ERROR_FOLDER        "html/"
#define WELCOME2WEBSERV     "html/webserv_TM.html"

void		sighandler(int num);

#endif
