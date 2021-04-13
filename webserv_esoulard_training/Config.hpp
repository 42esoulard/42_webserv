/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/13 10:22:15 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/13 12:33:39 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.h"



class Config {
    
    public:
        typedef struct          s_loc {

            std::map <std::string, std::list <std::string > >  location;
            //     std::list<std::string>      path;
            //     std::list<std::string>      extensions;
            //     std::list<std::string>      accept_methods;
            //     std::list<std::string>      root;
            //     std::list<std::string>      autoindex;
            //     std::list<std::string>      default_error;
            //------------------------------------------------------------------------
        }                       t_loc;

        typedef struct          s_conf {
            std::map <std::string, std::list <std::string > >   serv_info;
            // std::list<std::string>      server_host;
            // std::list<std::string>      server_port;
            // std::list<std::string>      server_name;
            // std::list<std::string>      client_max_body_size;
            //------------------------------------------------------------------------

            std::list < Config::t_loc >                         locations;

        }                       t_conf;

        Config() {};
        ~Config() {};

        void        parse_config(std::string &config_file);
        std::string get_conf_token(char *line, int &index);
        void        print_config(); //DEV UTIL

        //t_conf                                                  &get_server_config() { return _serv; };
        std::map <std::string, std::list <std::string > >       &get_server_info() { return _serv.serv_info; };

        std::list < Config::t_loc >                             &get_locations() { return _serv.locations; };

    private:

        Config::t_conf _serv;
            
 
};

//TO ACCESS ELEMENTS:
//  in Server functions:
//  to get all servers, iterate on _conf (each server block is a list element):
//  std::list<Config>::iterator server_it = _conf.begin(); 
//
//  std::list<std::string> content_it;
//  std::list<std::string> locations_it;
//
//  while (server_it != conf.end()) {
//      content_it = (*server_it).get_serv_info()["server_host"].begin();
//      while (content_it != (*server_it).get_server_info()["server_host"].end()) {
//          std::cout << "server_host is " << *content_it << std::endl;
//          ++content_it;
//      }
//
//      locations_it =  (*it).get_locations().begin();
//      while (locations_it != (*it).get_locations().end()) {
//          content_it = (*server_it).(*locations_it)["path"].begin();
//          while (content_it != (*server_it).(*locations_it)["server_host"].end()) {
//              std::cout << "current location path is " << *content_it << std::endl;
//              ++content_it;
//          }
//      }
//
//      ++server_it;
//      
//  } 
//      (*it).get_server_config().serv_info["path"];
//    

//lvl1                      lvl2                        lvl3                lvl4
//
// server1_conf             server_info                 key - value
//                                                      key - value
//                          locations                   location1           key - value
//                                                                          key - value
//                                                      location2           key - value
//                                                                          key - value
//                                                                          key - value
//
//
//
// server2_conf             server_info                 key - value
//                                                      key - value
//                          locations                   location1           key - value
//                                                                          key - value
//                                                      location2           key - value
//                                                                          key - value
//                                                                          key - value


#endif