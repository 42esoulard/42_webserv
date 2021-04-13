/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/13 10:22:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/13 15:35:21 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>

#include "Exception.hpp"
#include "utils.hpp"

class Config {
    
    private:
        typedef std::map <std::string, std::list <std::string > >  t_content_map;

        typedef struct          s_conf {
            t_content_map               serv_info;
            // std::list<std::string>      server_host;
            // std::list<std::string>      server_port;
            // std::list<std::string>      server_name;
            // std::list<std::string>      client_max_body_size;
            //------------------------------------------------------------------------

            std::list < t_content_map > locations;
            //     std::list<std::string>      path;
            //     std::list<std::string>      extensions;
            //     std::list<std::string>      accept_methods;
            //     std::list<std::string>      root;
            //     std::list<std::string>      autoindex;
            //     std::list<std::string>      default_error;
            //------------------------------------------------------------------------
        }                       t_conf;

        std::list<Config::t_conf> _server_list;

        // parsing utils
        char    *_line;
        int     _index;
        bool    _in_location;
        bool    _in_server;
        int     _line_nb;
        int     _config_fd;
        
    public:

        Config() {};
        ~Config() {};

        void        parse_config(std::string &config_file);
        std::string get_conf_token(char *line, int &index);
        void        parse_field(std::string &field, std::string &config);
        void        parse_values(std::string &field, std::string &config);
        void        print_config(); //DEV UTIL
        void        print_server_info(std::list<Config::t_conf>::iterator &server_it); //DEV UTIL
        void        print_server_locations(std::list<Config::t_conf>::iterator &server_it); // DEV UTIL
        
        // t_conf                                                  &get_server_config() { return _serv; };
        // std::map <std::string, std::list <std::string > >       &get_server_info(t_conf server) { return server.serv_info; };
        // std::list < Config::t_loc >                             &get_locations(t_conf server) { return server.locations; };
        
        Config::t_conf *get_server_conf_by_name(std::string &searched_name);
        Config::t_conf  *get_server_conf_by_address(std::string &searched_host, std::string &searched_port);

};

//TO ACCESS ELEMENTS:
//  
        //  to get all servers, iterate on _server_list (each server block is a list element):
            // std::list<t_conf>::iterator         server_it = _server_list.begin(); 

        // iterate on locations within servers
            // std::list<t_content_map>::iterator  locations_it;
        // iterate on content (last level)    
            // std::list<std::string>::iterator    content_it;

            // while (server_it != _server_list.end()) { //iterate on each server block
            //     content_it = (*server_it).serv_info["server_host"].begin(); // iterate on the whole list value corresponding to a key
            //     while (content_it != (*server_it).serv_info["server_host"].end()) {
            //         std::cout << "server_host is " << *content_it << std::endl;
            //         ++content_it;
            //     }

            //     locations_it = (*server_it).locations.begin();
            //     while (locations_it != (*server_it).locations.end()) {
            //         content_it = (*locations_it)["path"].begin();
            //         while (content_it != (*locations_it)["path"].end()) {
            //             std::cout << "current location path is " << *content_it << std::endl;
            //             ++content_it;
            //         }
            //         ++locations_it;
            //     }

            //     ++server_it;
            // } 

// -- TRYING TO REPRESENT HOW THIS MONSTRUOSITY WORKS --   

//lvl1                      lvl2                        lvl3                lvl4
//
// server1_conf             server_info                 key - val - val - val
//                                                      key - val - val
//                          locations                   location1           key - val - val
//                                                                          key - val - val - val
//                                                      location2           key - val - val
//                                                                          key - val - val - val
//                                                                          key - val - val
//
//
//
// server2_conf             server_info                 key - val - val
//                                                      key - val
//                          locations                   location1           key - val - val
//                                                                          key - val
//                                                      location2           key - val - val - val
//                                                                          key - val - val - val
//                                                                          key - val
// etc etc

#endif