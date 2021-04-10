/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 11:28:30 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/10 17:15:33 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

int main(int ac, char **av) {

    try {
        
        if (ac > 2)
            throw Exception("too many arguments");

        std::string config(DEFAULT_CONFIG_FILE); // default file defined in webserv.h
        if (ac == 2)
            config = std::string(av[1]);

        Server ws(config);
        
        while (1)
            ws.handle_connection();  
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}