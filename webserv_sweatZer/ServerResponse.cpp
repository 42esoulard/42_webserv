/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 16:23:08 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/09 16:04:37 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"

// simply parses spaces and returns the next non space character sequence
std::string ServerResponse::get_next_token(char *line, int &index) {

    pass_spaces(line, index);
    if (line && !line[index])
        return "";

    int start = index;
    while (line && line[index] && !is_space(line[index]) && line[index] != ';')
        ++index;
    
    return std::string(&line[start], index - start);
}



std::string ServerResponse::get_mime_type(std::string &extension) {

    std::string *value = g_mime_types.get_value(extension);
    if (!value)
        std::cerr << "Heck man I can't read dat" << std::endl;
    else
        std::cerr << "The latin name for the " << extension << " species is " << *value << std::endl;
    
    //WHAT SHOULD WE DO HERE IN CASE OF UNKNOWN EXTENSION ? THROW ERROR ? IGNORE ?
};