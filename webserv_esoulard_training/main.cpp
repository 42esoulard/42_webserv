/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 11:28:30 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/08 17:33:06 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"

int main(int ac, char **av) {

    try {
        
        Server ws;
        if (ac > 2)
            throw Exception("too many arguments");
        (void)av;
        // else if (ac == 2)
        //     CONFIG_FILE = std::string(av[1]);
        // note: CONFIG_FILE is defined by default in webserv.h
        
        while (1)
            ws.handle_connection();  
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}