/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 16:23:08 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/22 16:22:02 by esoulard         ###   ########.fr       */
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

int			ServerResponse::error(int code)
{
	std::cout << "ERROR " << code << std::endl;
	return (code);
}

std::string ServerResponse::get_mime_type(std::string &extension) {

	//SimpleHashTable g_mime_types(65);
	std::string *value =_mime_types.get_value(extension);

	if (!value) {
		std::cerr << "Heck man I can't read dat" << std::endl; 
		*value = std::string("application/octet-stream");
	}
	//unknown extension defaults to application/octet-stream type
	
	return (*value);
};

// find which server is requested by the client and save its conf
int ServerResponse::identify_server(t_content_map &cli_conf) {
	
	std::list<std::string>::iterator it;
	std::string port;
	if (cli_conf.find("port") == cli_conf.end())
		port = std::string(ft_itoa(PORT));
	else
		port = *(cli_conf["port"].begin());
	
	if (cli_conf.find("host") != cli_conf.end()) {
		it = cli_conf["host"].begin();
		while (it != cli_conf["host"].end()) {
			if ((_server_conf = get_server_conf_by_name(*it, port)) != NULL ||
				(_server_conf = get_server_conf_by_address(*it, port)) != NULL)
				return (200);
			it++;
		}
	}
	return error(502); //unknown server error
}

// find which location in the server is requested by the client and save its conf in *_location
int ServerResponse::identify_location(std::string &file, std::string &extension) {
    
    unsigned int i = file.find_first_of("/");
    std::string path;
    std::list < t_content_map >::iterator it;
    std::list < std::string >::iterator ext_it;

    while (i != file.npos) {
        path = file.substr(0, i + 1);

        it = get_locations().begin();
        while(it != get_locations().end()) {

            //check if extension fits extension listed in location conf
            if (extension != "" && (*it)["extensions"].begin() != (*it)["extensions"].end()) { //first check extension
                ext_it = (*it)["extensions"].begin();
                while (ext_it != (*it)["extensions"].end()) {
                    if (extension == *ext_it)
                        break;
                    ext_it++;
                }
                if (ext_it == (*it)["extensions"].end()) {
                    ++it;
                    continue;
                }
            }
           
           //check if path matches path
            if (*(*it)["root"].begin() == path) {
                *_location = *it;
                return i;
            }
           ++it;
        }

        i = file.find_first_of("/", i);
    }
    return -1;
}

// find a server with one of its names, NOT TESTED YET
Server::t_conf *ServerResponse::get_server_conf_by_name(std::string &searched_name, std::string &searched_port) {

    std::list<Server>::iterator server_it = _server_list.begin();
    std::list<std::string>::iterator    content_it;
	std::list<std::string>::iterator    port_it;

    while (server_it != _server_list.end()) {

        content_it = (*server_it).get_serv_info()["server_name"].begin();
        while (content_it != (*server_it).get_serv_info()["server_name"].end()) {

            if (*content_it == searched_name) {
				port_it = (*server_it).get_serv_info()["server_port"].begin();
                while (port_it != (*server_it).get_serv_info()["server_port"].end()) {

                    if (*port_it == searched_port)
                        return &((*server_it).get_conf());
                }
			    return &((*server_it).get_conf());
			
			}
            ++content_it;
        }
        ++server_it;
    }
    return NULL;
};

// find a server with its address, NOT TESTED YET
Server::t_conf  *ServerResponse::get_server_conf_by_address(std::string &searched_host, std::string &searched_port) {

    std::list<Server>::iterator server_it = _server_list.begin();
    std::list<std::string>::iterator    host_it;
    std::list<std::string>::iterator    port_it;

    while (server_it != _server_list.end()) {

        host_it = (*server_it).get_serv_info()["server_host"].begin();
        while (host_it != (*server_it).get_serv_info()["server_host"].end()) {

            if (*host_it == searched_host) {
                port_it = (*server_it).get_serv_info()["server_port"].begin();
                while (port_it != (*server_it).get_serv_info()["server_port"].end()) {

                    if (*port_it == searched_port)
                        return &((*server_it).get_conf());
                }
            }
            ++host_it;
        }
        ++server_it;
    }
    return NULL;
};

//t_content_map &cli_conf as param ?
int ServerResponse::check_file_access(t_content_map &cli_conf) {
	struct stat *buf = NULL;

    // 0) save extension in a string
    std::string full_path = *cli_conf["file"].begin();
    std::string extension = full_path.substr(full_path.find_last_of("."));

    // first, rebuild path thanks to conf
    // TO GET LOCATION, LOOP ON:
    // 1) split path from file name until you get something in the format / + dir (ex: /bla/blou) (ex: /) (ex: /bli):
    //  first compare up to first /, then go to the next (ex: for /bla/bli/blou check /bla then /bla/bli then /bla/bli/blou)
    //  each time, first compare location["extensions"] if theres one with the extension. If it doesnt match, next.
    //  if theres no location["extensions"] -> just check path
    
    int i;
    if ((i = identify_location(full_path, extension)) < 0)
        return error(404); // location not found

    // 2) get location from a identify_location function 
    //  when a match is found, replace this with the location["root"], end of loop

    std::string file = *(*_location)["root"].begin() + "/" + full_path.substr(i + 1);

    // 3) check that one of location["accept_method"] is compatible with cli_conf["method"]
    //  if not, error 403 (forbidden)

    std::list<std::string>::iterator it = (*_location)["accept_method"].begin();
    while (it != (*_location)["accept_method"].end()) {
        if (*it == *cli_conf["method"].begin())
            break;
        ++it;
    }
    if (it != (*_location)["accept_method"].end())
        return error(405); // method not allowed

	// 4) check file exists	with the newly built path
    if (stat(full_path.c_str(), buf) < 0)
        return error(404); // file not found
        
    // 5) if protected, check authorization (first Basic, else Unknown auth method error. Second, decode base64 and check against against server /admin/.htaccess) .)
    if (*(*_location)["accept_method"].begin() == "on") {
        
    }


    // 6) if IS_DIR && autoindex = "on"
    //  return an autoindexing of website tree. SHOULD WE BUILD AN INDEX OURSELVES OR IS IT AN AUTOMATIC THINGY?
    // 7) else if IS_DIR && autoindex = "off"
    //  return if_dir file
    
	
    return (200);
}

