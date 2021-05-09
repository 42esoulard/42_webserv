/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 11:28:30 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/09 10:10:53 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "Cluster.hpp"

int main(int ac, char **av) {

    try {

        if (ac > 2)
            throw Exception("too many arguments");

        std::string config(DEFAULT_CONFIG_FILE); // default file defined in webserv.h
        if (ac == 2)
            config = std::string(av[1]);

        Cluster ws(config);

        while (1)
            ws.handle_connection();
		/*
		// to simply test parsing
		ClientRequest cli_request;
		int	fd = open("./request.txt", O_RDONLY);
		read(fd, cli_request.get_read(), _MAXLINE);
	    ServerResponse serv_response;
	    cli_request.parse_request(serv_response);
		*/
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
