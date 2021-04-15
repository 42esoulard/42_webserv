/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/13 10:22:02 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/15 16:39:18 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

//prints general info on a specific server block
void Config::print_server_info(std::list<Config::t_conf>::iterator &server_it) {

    std::cout << "----SERVER GENERAL INFO----"<< std::endl;

    t_content_map::iterator serv_info_it;
    std::list<std::string>::iterator content_it;

    //printing server content
    serv_info_it = (*server_it).serv_info.begin();
    while (serv_info_it != (*server_it).serv_info.end()) {
        content_it = serv_info_it->second.begin();
        std::cout << "KEY [" << serv_info_it->first << "]" << std::endl;
        while (content_it != serv_info_it->second.end()) {
            std::cout << "VALUE [" << *content_it << "]" << std::endl;
            ++content_it;
        }
        ++serv_info_it;
    }
}

//prints all the contents of _conf
void Config::print_server_locations(std::list<Config::t_conf>::iterator &server_it) {

    std::cout << "----SERVER LOCATIONS INFO----"<< std::endl;

    std::list <t_content_map>::iterator loc_it;
    t_content_map::iterator             loc_info_it;
    std::list<std::string>::iterator    content_it;

    int index = 0;
    //printing locations content
    loc_it = (*server_it).locations.begin();
    while (loc_it != (*server_it).locations.end()) {
        loc_info_it = (*loc_it).begin();

        std::cout << "----LOCATION " << index++ << "----" << std::endl;

        while (loc_info_it != (*loc_it).end()) {
            content_it = loc_info_it->second.begin();
            std::cout << "KEY [" << loc_info_it->first << "]" << std::endl;
            
            while (content_it != loc_info_it->second.end()) {
                std::cout << "VALUE [" << *content_it << "]" << std::endl;
                ++content_it;
            }
            ++loc_info_it;
        }
        ++loc_it;
    }
}

//prints all the contents of _conf
void Config::print_config() {

    std::cout << "----CONFIG----"<< std::endl;
    
    std::list<Config::t_conf>::iterator server_it = _server_list.begin();
    int index = 0;
    while (server_it != _server_list.end()) {
        std::cout << "------------------------SERVER " << index++ << "------------------------" << std::endl;
        //printing server content
        print_server_info(server_it);

        //printing locations content
        print_server_locations(server_it);
        ++server_it;
    }
    std::cout << "-----------------------" << std::endl;
}

void Config::parse_field(std::string &field, std::string &config) {
    if (field == "server") {
            
            if (get_conf_token(_line, _index) != "{" || get_conf_token(_line, _index) != "")
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
            if (_in_server)
                throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + ": already in a server context!");
            s_conf new_conf;
            _server_list.push_back(new_conf);
            _in_server = true;
    }
    else if (field == "location") {
            
        if (_in_location)
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + ": already in a location context!");
        
        std::map <std::string, std::list <std::string > > new_loc;
        _server_list.back().locations.push_back(new_loc);
        _in_location = true;
            
        std::string tmp;
        if ((tmp = get_conf_token(_line, _index)) == "" || tmp == "{")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + "location needs a path");
        _server_list.back().locations.back()["path"].push_back(tmp);
        while ((tmp = get_conf_token(_line, _index)) != "{" && tmp != "")
            _server_list.back().locations.back()["extensions"].push_back(tmp);
        if (tmp != "{" || get_conf_token(_line, _index) != "")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
    }
    else if (field == "}") {
        
        if (_in_location)
            _in_location = false;
        else if (_in_server)
            _in_server = false;
        if (get_conf_token(_line, _index) != "")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
    }
}

// simply parses spaces and returns the next non space character sequence
std::string Config::get_conf_token(char *line, int &index) {

    pass_spaces(line, index);
    if (line && !line[index])
        return "";

    int start = index;
    while (line && line[index] && !is_space(line[index]) && line[index] != ';')
        ++index;
    
    return std::string(&line[start], index - start);

}

void Config::parse_values(std::string &field, std::string &config) {
    
    while (_line && _line[_index] && _line[_index] != ';') {
    
        if (_in_server && _in_location)
            _server_list.back().locations.back()[field].push_back(get_conf_token(_line, _index));
        else if (_in_server)
            _server_list.back().serv_info[field].push_back(get_conf_token(_line, _index));
        else 
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb) + ": fields must be in a context");
        if (_line[_index] == ';' && get_conf_token(_line, _index) != "")
            throw Exception("Config file " + config + " parsing error near line " + ft_itoa(_line_nb));
    }
}

void    Config::check_conf(std::string &config) {

    std::list<t_conf>::iterator         server_it = _server_list.begin(); 
    std::list<t_content_map>::iterator  locations_it;  

    t_content_map::iterator             serv_info_it;
    t_content_map::iterator             loc_info_it;

    while (server_it != _server_list.end()) { //iterate on each server block

        serv_info_it = (*server_it).serv_info.begin();
        while (serv_info_it != (*server_it).serv_info.end()) {
            // check that except in the case of server_name, we don't accept more than 1 param
            // MAYBE SEVERAL PORTS ARE OK, NOT SURE. IN THIS CASE, ADD ANOTHER EXCEPTION FOR server_port
            if (serv_info_it->second.size() != 1 && serv_info_it->first != "server_name")
                throw Exception("Config file " + config + ": " + serv_info_it->first + "wrong number of parameters");
            ++serv_info_it;
        }

        locations_it = (*server_it).locations.begin();
        while (locations_it != (*server_it).locations.end()) {
            loc_info_it = (*locations_it).begin();
            while (loc_info_it != (*locations_it).end()) {
                if (loc_info_it->second.size() != 1 && loc_info_it->first != "accept_methods")
                    throw Exception("Config file " + config + ": " + loc_info_it->first + ": wrong number of parameters");
                ++loc_info_it;
            }
            ++locations_it;
        }
        if ((*server_it).serv_info.find("server_host") == (*server_it).serv_info.end())
            (*server_it).serv_info["server_host"] = "127.0.0.1";
        if ((*server_it).serv_info.find("server_port") == (*server_it).serv_info.end())
            (*server_it).serv_info["server_port"] = "80";
        ++server_it;
    } 
    // WE WILL STILL NEED TO CHECK EACH PARAMETER'S VALIDITY UPON USING IT.
};

void Config::parse_config(std::string &config) {

    FD_ZERO (&_config_fd);
    if ((_config_fd = open(config.c_str(), O_RDONLY)) < 0)
        throw Exception("Couldn't open configuration file " + config);

    _in_location = false;
    _in_server = false;
    _line_nb = 0;
    std::string field;
    while (get_next_line(_config_fd, &_line) > 0) {

        ++_line_nb;
        _index = 0;

        parse_field((field = get_conf_token(_line, _index)), config);
        parse_values(field, config);

        free (_line);
    }
    close (_config_fd);
    print_config(); //prints all the contents of _conf
    check_conf(config);
};

// find a server with one of its names, NOT TESTED YET

// (1) by address
Config::t_conf  *Config::get_server_conf(std::string &searched_host, std::string &searched_port) {

    std::list<Config::t_conf>::iterator         server_it = _server_list.begin(); 
    std::list<std::string>::iterator    host_it;
    std::list<std::string>::iterator    port_it;

    while (server_it != _server_list.end()) {

        host_it = (*server_it).serv_info["server_host"].begin();
        while (host_it != (*server_it).serv_info["server_host"].end()) {
                    
            if (*host_it == searched_host) {   
                port_it = (*server_it).serv_info["server_port"].begin();
                while (port_it != (*server_it).serv_info["server_port"].end()) {
                            
                    if (*port_it == searched_port)
                        return &(*server_it);
                    ++port_it;
                }
            }
            ++host_it;
        }
        ++server_it;
    } 
    return NULL;
};

// (2) by name
Config::t_conf *Config::get_server_conf(std::string &searched_name) {

    std::list<Config::t_conf>::iterator         server_it = _server_list.begin(); 
    std::list<std::string>::iterator    content_it;

    while (server_it != _server_list.end()) {

        content_it = (*server_it).serv_info["server_name"].begin();
        while (content_it != (*server_it).serv_info["server_name"].end()) {
                    
            if (*content_it == searched_name)
                return &(*server_it);
            ++content_it;
        }
        ++server_it;
    } 
    return NULL;
};

// (1) by address
Config::t_content_map *Config::get_serv_location(std::string &server_host, std::string &server_port, std::string &location) {

    Config::t_conf *server_conf = get_server_conf(server_host, server_port);
    if (!server_conf)
        return NULL;
    std::list <t_content_map>::iterator loc_it;
    std::list<std::string>::iterator    content_it;
    
    loc_it = server_conf->locations.begin();
    while (loc_it != server_conf->locations.end()) {
        content_it = (*loc_it)["path"].begin();

        while (content_it != (*loc_it)["path"].end()) {
            if (*content_it == location)
                return &(*loc_it);
            ++content_it;
        }
        ++loc_it;
    }
    return NULL;
};

// (2) by name
Config::t_content_map *Config::get_serv_location(std::string &server_name, std::string &location) {

    Config::t_conf *server_conf = get_server_conf(server_name);
    if (!server_conf)
        return NULL;

    std::list <t_content_map>::iterator loc_it;
    std::list<std::string>::iterator    content_it;
    
    loc_it = server_conf->locations.begin();
    while (loc_it != server_conf->locations.end()) {
        content_it = (*loc_it)["path"].begin();

        while (content_it != (*loc_it)["path"].end()) {
            if (*content_it == location)
                return &(*loc_it);
            ++content_it;
        }
        ++loc_it;
    }
    return NULL;
};

// (1) by address
std::list<std::string> *Config::get_location_field_content(std::string &server_host, std::string &server_port, std::string &location_path, std::string &field) {

    Config::t_content_map *location = get_serv_location(server_host, server_port, location_path);
    if (!location)
        return NULL;

    Config::t_content_map::iterator it = location->find(field);
    if (it == location->end())
        return NULL;
        
    return (&((*it).second));
};

// (2) by name
std::list<std::string> *Config::get_location_field_content(std::string &server_name, std::string &location_path, std::string &field) {

    Config::t_content_map *location = get_serv_location(server_name, location_path);
    if (!location)
        return NULL;

    Config::t_content_map::iterator it = location->find(field);
    if (it == location->end())
        return NULL;

    return (&((*it).second));
};

// (1) by address
std::list<std::string> *Config::get_serv_field_content(std::string &server_host, std::string &server_port, std::string &field) {
    
    Config::t_conf *server_conf = get_server_conf(server_host, server_port);

    Config::t_content_map::iterator it = server_conf->serv_info.find(field);
    if (it == server_conf->serv_info.end())
        return NULL;

    return (&(*it).second);
};

// (2) by name
std::list<std::string> *Config::get_serv_field_content(std::string &server_name, std::string &field) {

    Config::t_conf *server_conf = get_server_conf(server_name);

    Config::t_content_map::iterator it = server_conf->serv_info.find(field);
    if (it == server_conf->serv_info.end())
        return NULL;

    return (&(*it).second);
};