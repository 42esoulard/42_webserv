/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 16:23:08 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/25 16:35:19 by esoulard         ###   ########.fr       */
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

    int mime_fd;
    FD_ZERO (&mime_fd);
    if ((mime_fd = open(MIME_TYPES, O_RDONLY)) < 0)
        throw Exception("Couldn't open mime types file " + std::string(MIME_TYPES));
    
    fd_set active_fd_set;
    FD_SET (mime_fd, &active_fd_set);
    if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0)
        throw Exception("select error");
    
    std::string field;
    char *line;
    int index;
    std::string mime;
    std::string ext;
    while (get_next_line(mime_fd, &line) > 0) {

        index = 0;
        mime = get_next_token(line, index);
        while ((ext = get_next_token(line, index)) != "") {
            if (ext == extension) {
                close (mime_fd);
                return mime;
            }
        }
        free (line);
    }
    close (mime_fd);
    //WHAT SHOULD WE DO HERE IN CASE OF UNKNOWN EXTENSION ? THROW ERROR ? IGNORE ?
};