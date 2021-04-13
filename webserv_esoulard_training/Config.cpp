/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/13 10:22:02 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/13 10:32:07 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"


std::string Config::get_conf_token(char *line, int &index) {

    pass_spaces(line, index);
    
    if (line && !line[index])
        return "";
    int start = index;
    //std::cout << line << "|" << index << std::endl;
    while (line && line[index] && !is_space(line[index]))
        ++index;
    if (index > 0 && line[index - 1] == ';')
        --index;
    return std::string(&line[start], index - start);

}

//prints all the contents of _conf
void Config::print_config() {

    std::cout << "----CHECKS----"<< std::endl;

    std::list<Server::s_conf>::iterator server_it = _conf.begin();
    std::list<std::string>::iterator list_it;
    std::map <std::string, std::list <std::string > >::iterator serv_info_it;
    std::list <std::map <std::string, std::list <std::string > > >::iterator loc_it;
    std::map <std::string, std::list <std::string > >::iterator loc_info_it;

    while (server_it != _conf.end()) {
        //printing server content
        serv_info_it = (*server_it).serv_info.begin();
        while (serv_info_it != (*server_it).serv_info.end()) {
            list_it = serv_info_it->second.begin();
            std::cout << "KEY [" << serv_info_it->first << "]" << std::endl;
            while (list_it != serv_info_it->second.end()) {
                std::cout << "VALUE [" << *list_it << "]" << std::endl;
                ++list_it;
            }
            ++serv_info_it;
        }
        //printing locations content
        loc_it = (*server_it).locations.begin();
        while (loc_it != (*server_it).locations.end()) {
            loc_info_it = (*loc_it).begin();
            while (loc_info_it != (*loc_it).end()) {
                list_it = loc_info_it->second.begin();
                std::cout << "KEY [" << loc_info_it->first << "]" << std::endl;
                while (list_it != loc_info_it->second.end()) {
                    std::cout << "VALUE [" << *list_it << "]" << std::endl;
                    ++list_it;
                }
                ++loc_info_it;
            }
            ++loc_it;
        }
        ++server_it;
    }
    std::cout << "-----------------------" << std::endl;
}

void Config::parse_config(std::string &config) {

    int config_fd;
    FD_ZERO (&config_fd);
    if ((config_fd = open(config.c_str(), O_RDONLY)) < 0)
        throw Exception("Couldn't open configuration file " + config);
    
    FD_SET (config_fd, &this->_active_fd_set);
    if (select(FD_SETSIZE, &this->_active_fd_set, NULL, NULL, NULL) < 0)
        throw Exception("select error");

    char *line;
    int index;
    bool in_location = false;
    bool in_server = false;
    int line_nb = 0;
    std::string field;
    while (get_next_line(config_fd, &line) > 0) {

        ++line_nb;
        index = 0;
        
        field = get_conf_token(line, index);

        if (field == "server") {
            
            if (get_conf_token(line, index) != "{" || get_conf_token(line, index) != "")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb));
            if (in_server)
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb) + ": already in a server context!");
            s_conf new_conf;
            _conf.push_back(new_conf);
            in_server = true;
        }
        else if (field == "location") {
            
            if (in_location)
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb) + ": already in a location context!");
            
            std::map <std::string, std::list <std::string > > new_loc;
            _conf.back().locations.push_back(new_loc);
            in_location = true;
            
            std::string tmp;
            if ((tmp = get_conf_token(line, index)) == "" || tmp == "{")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb) + "location needs a path");
            _conf.back().locations.back()["path"].push_back(tmp);
            while ((tmp = get_conf_token(line, index)) != "{" && tmp != "")
                _conf.back().locations.back()["extensions"].push_back(tmp);
            if (tmp != "{" || get_conf_token(line, index) != "")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb));
        }
        else if (field == "}") {
            if (in_location)
                in_location = false;
            else if (in_server)
                in_server = false;
            if (get_conf_token(line, index) != "")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb));
        }
        while (line && line[index] && line[index] != ';') {
            
            if (in_server && in_location)
                _conf.back().locations.back()[field].push_back(get_conf_token(line, index));
            else if (in_server)
                _conf.back().serv_info[field].push_back(get_conf_token(line, index));
            else 
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb) + ": fields must be in a context");
            if (line[index] == ';' && get_conf_token(line, index) != "")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(line_nb));
        }
        free (line);
    }
    close (config_fd);
    //print_config(); //prints all the contents of _conf
};