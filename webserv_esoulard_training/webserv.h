/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 14:10:49 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/15 13:38:09 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
#define WEBSERV_H

// socket/IP stuff
#include <sys/socket.h>
#include <arpa/inet.h>

// read/write/fd stuff
#include <sys/stat.h>
#include <fcntl.h>

// utils
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>

#include "Exception.hpp"
#include "utils.hpp"

// server
#include "Config.hpp"
#include "Server.hpp"
#include "ServerResponse.hpp"
#include "ClientRequest.hpp"


#define DEFAULT_CONFIG_FILE "conf.d/webserv.conf"

#endif