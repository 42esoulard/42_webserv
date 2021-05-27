/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 16:23:08 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/27 15:30:40 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"

// simply parses spaces and returns the next non space character sequence
std::string ServerResponse::get_next_token(std::string &line, size_t &index) {

    pass_spaces(line, index);
    if (index >= line.size())
        return "";

    size_t start = index;
    while (index < line.size() && !is_space(line[index]))
        ++index;

    return std::string(&line[start], index - start);
}

int			ServerResponse::error(int code)
{
    //find the html file corresponding to the error
    //save its path in resource_path
    //file_to_body()
    //build_response_headers();
    _error = code;

	// std::string     path("html/");
    // char            buf[4096];
    // int             fd;

    // path += ft_itos(code);
    // path += ".html";
    // if ((fd = open(path.c_str(), O_RDONLY)) == -1) //if not found, try to see if theres a default page!! (see conf, "default_error")
    // {
    //     std::cout << "<!DOCTYPE html>\n<title>Error 500 (Internal Server Error)</title>\n<p><b>Error 500.</b>\n<p>Internal Server Error." << std::endl;
    //     return (500);
    // }
    // else
    // {
    //     if (read(fd, buf, 4096) == -1)
    //     {
    //         std::cout << "<!DOCTYPE html>\n<title>Error 500 (Internal Server Error)</title>\n<p><b>Error 500.</b>\n<p>Internal Server Error." << std::endl;
    //         close(fd);
    //         return (500);
    //     }
    //     std::cout << buf << std::endl;
    // }
	return (code);
}

int ServerResponse::build_error_response(int code) {
    std::string     path(ERROR_FOLDER);
    char            buf[4096];
    int             fd;

    // try to open error matching path
    // if it doesnt work, try to open default path
    // if it doesn't work, give default string to body
    // 

    _error = code; //in case the call is for an error in the latest stages
    path += ft_itos(code);
    path += ".html";
     //if not found, try to see if theres a default page!! (see conf, "default_error")
        //check if theres a default err for location or server
    if ((fd = open(path.c_str(), O_RDONLY)) == -1 && 
        ((*_location).find("default_error") == get_serv_info().end() ||
        (fd = open((*(*_location)["default_error"].begin()).c_str(), O_RDONLY)) == -1) 
        && (get_serv_info().find("default_error") == get_serv_info().end() || 
        (fd = open((*get_serv_info()["default_error"].begin()).c_str(), O_RDONLY)) == -1 )) {
          
            _body = "<!DOCTYPE html>\n<title>Error 500 (Internal Server Error)</title>\n<p><b>Error 500.</b>\n<p>Internal Server Error.";
    }
    else {
        if (read(fd, buf, 4096) == -1)
            _body = "<!DOCTYPE html>\n<title>Error 500 (Internal Server Error)</title>\n<p><b>Error 500.</b>\n<p>Internal Server Error.";
        else
            _body = std::string(buf);
        close(fd);
    }

    std::string s_error = ft_itos(_error);
    std::string *p_error_msg = _error_codes.get_value(s_error);
    std::string s_error_msg = "";
    std::string sp = " ";
    if (p_error_msg)
        s_error_msg = *p_error_msg;
    _payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
    
    _payload += "Content-Type: " + get_mime_type(_extension) + "\r\n";

    _payload += "Content-Length: " + ft_itos(_body.size()) + "\r\n";

    if (_error == 401)
        _payload += "WWW-Authenticate: Basic realm=\"login\"\r\n";
    //  blank line, then content BODY        
    _payload += "\r\n" + _body + "\r\n";
    return (_error);
}

std::string ServerResponse::get_mime_type(std::string &extension) {

	//SimpleHashTable g_mime_types(65);
    if (extension.size() > 1 && extension[0] == '.')
        extension = extension.substr(1);
	std::string *value =_mime_types.get_value(extension);
    std::string ret = "application/octet-stream";

	if (value)
		ret = *value;

	//unknown extension defaults to application/octet-stream type
	
	return (ret);
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
            std::cout << "[IN SERVRESP ID_SERV] SERVER HOST NAME [" << *it << "]" << std::endl;
			if ((_server_conf = get_server_conf_by_name(*it, port)) != NULL ||
				(_server_conf = get_server_conf_by_address(*it, port)) != NULL)
				return (200);
			it++;
		}
	}
	return error(404); //server not found
}

// find which location in the server is requested by the client and save its conf in *_location
int ServerResponse::identify_location(std::string &file, std::string &extension) {
    
    unsigned int i = file.find_first_of("/");

    std::string path;
    std::list < t_content_map >::iterator it;
    std::list < std::string >::iterator ext_it;

    while (i < file.size()) {
        if (file.size() > 1)
            path = file.substr(0, i + 1);
        else
            path = file;

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
            if (*(*it)["path"].begin() == path) {
                _location = &(*it);
                return i;
            }
            
           ++it;
        }
        i = file.find_first_of("/", i + 1);
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
                    ++port_it;
                }			
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
                    ++port_it;
                }
            }
            ++host_it;
        }
        ++server_it;
    }
    return NULL;
};
//if (tmp decoded from base64 as login=password is not found in *(*_location)["root"].begin()
// + "/" + ".auth" file ) (each entry in auth being in format login=base64password or 
//something like that)

int ServerResponse::check_auth(std::string &tmp) {

    int ir;
    struct stat buf;
    if ((ir = stat(_resource_path.c_str(), &buf)) < 0) 
        return error(500);

    std::string chest;
    if (!S_ISDIR(buf.st_mode))
        chest = _resource_path.substr(0, _resource_path.find_last_of("/"));
    else
        chest += _resource_path + "/";
    chest += ".auth";

    std::cout << "AUTH path ["<< chest <<"]" << std::endl;
    int fd;
    if ((fd = open(chest.c_str(), O_RDONLY)) < 0)
        return error(500);
    std::string sub = decode(tmp);

    // gnl on .auth
    // decode .auth line by line && compare w cred
    // if match -> return 0
    // end of function -> return -1
    char *line;
    std::string cred;
    std::string pass;
    unsigned int i;
    while (get_next_line(fd, &line) > 0) {
        cred = std::string(line);
        if ((i = cred.find_first_of('=')) != cred.npos && i < cred.size()) {// '=' can be in pwd, but not in login. Also format has to be login=password, if there's no '=' we disregard the entry
            pass = decode(cred.substr(i + 1, cred.size()));
            cred = cred.substr(0, i);
            cred += pass;
            std::cout << "AUTH cred ["<< cred <<"] sub ["<< sub <<"]" << std::endl;
            if (sub == cred) {
                free(line);
                close (fd);
                return 0;
            }
        }
        free (line);
    }
    free(line);
    close (fd);
 
    return -1;
}

int ServerResponse::build_response(t_content_map &cli_conf) {

   

    // -1) find which server is the request addressed to
    std::cout << "BEFORE BUILD RESPONSE" << std::endl;
    int i;
    if ((i = identify_server(cli_conf)) != 200)
        return error(i); //404, server not found

    // 0) save extension in a string
    std::string requested_path = *cli_conf["file"].begin();

    _extension = "";
    if (requested_path.find_last_of(".") < requested_path.size())
       _extension = requested_path.substr(requested_path.find_last_of("."));
    // first, rebuild path thanks to conf
    // TO GET LOCATION, LOOP ON:
    // 1) split path from file name until you get something in the format / + dir (ex: /bla/blou) (ex: /) (ex: /bli):
    //  first compare up to first /, then go to the next (ex: for /bla/bli/blou check /bla then /bla/bli then /bla/bli/blou)
    //  each time, first compare location["extensions"] if theres one with the extension. If it doesnt match, next.
    //  if theres no location["extensions"] -> just check pathrequested_path.substr(requested_path.find_last_of("."));

    if ((i = identify_location(requested_path, _extension)) < 0)
        return error(404); // location not found
    // 2) get location from a identify_location function 
    //  when a match is found, replace this with the location["root"], end of loop

     //3) FIRST THING SHOULD BE TO INTERCEPT ERROR
    // AFTER WE FOUND THE SERVER AND LOCATION
    // IF ERROR != 200 || 201, RETURN BUILD_ERROR()
    if (_error != 200 && _error != 201)
        return build_error_response(_error);

    //std::string actual_path;
    int ir;
    struct stat buf;
    std::string method = *(cli_conf["method"].begin());
    if (method == "PUT" || method == "POST") {
        if ((*_location).find("up_dir") == (*_location).end())
            return build_error_response(500); //uploads path not configured

        if ((*_location).find("root") != (*_location).end() && requested_path != "/")
            _resource_path = *(*_location)["root"].begin() + *(*_location)["up_dir"].begin() + "/" + requested_path.substr(i + 1);
        else if ((*_location).find("root") != (*_location).end() && requested_path == "/")
            _resource_path = *(*_location)["root"].begin() + *(*_location)["up_dir"].begin();
        else
            _resource_path = requested_path;

        if ((ir = stat(_resource_path.c_str(), &buf)) < 0) 
            _error = 201; //not an error, means file doesn't exist and is created
        if (S_ISDIR(buf.st_mode)) {
            _error = 201;
            _resource_path += "/" + std::string(DEFAULT_UPLOAD_NAME);
        }
        std::cout << "_resource_path " << _resource_path << " requested_path " << requested_path << " up_dir " << *(*_location)["up_dir"].begin() << std::endl;
        _cli_body = (*cli_conf["body"].begin());
    }
    else {
        if ((*_location).find("root") != (*_location).end() && requested_path != "/")
            _resource_path = *(*_location)["root"].begin() + "/" + requested_path.substr(i + 1);
        else if ((*_location).find("root") != (*_location).end() && requested_path == "/")
            _resource_path = *(*_location)["root"].begin();
        else
            _resource_path = requested_path;

        // 4) check file exists	with the newly built path
        
        if ((ir = stat(_resource_path.c_str(), &buf)) < 0) 
            return build_error_response(404); // file not found        
    }
  
    // 3) check that one of location["accept_method"] is compatible with cli_conf["method"]
    //  if not, error 405 method not allowed
    std::list<std::string>::iterator it = (*_location)["accept_methods"].begin();
    while (it != (*_location)["accept_methods"].end()) {
        if (*it == *cli_conf["method"].begin())
            break;
        ++it;
    }
    if (it == (*_location)["accept_methods"].end())
        return build_error_response(405); // method not allowed
    
	

    // 5) if protected, check authorization (first Basic, else Unknown auth method error. Second, decode base64 and check against against server /admin/.htaccess) .)
    if (*(*_location)["auth"].begin() == "on") {
        size_t j;
        if (cli_conf.find("authorization") == cli_conf.end())
            return build_error_response(401); // unauthorized (missing credentials)
        std::string tmp = get_next_token(*cli_conf["authorization"].begin(), j);
        if (tmp != "Basic")
            return build_error_response(401); // unauthorized (authorization method unknown)
        if ((tmp = get_next_token(*cli_conf["authorization"].begin(), j)) == "")
            return build_error_response(401); // unauthorized (missing credentials)
        std::cout << "+++++++++++++HEREEEE" << std::endl;
        if (check_auth(tmp) < 0)
            return build_error_response(401); //unauthorized(bad credentials)
    }

    // 6) if IS_DIR && autoindex = "on"
    //  return an autoindexing of website tree. SHOULD WE BUILD AN INDEX OURSELVES OR IS IT AN AUTOMATIC THINGY?
    // 7) else if IS_DIR && autoindex = "off"
    //  return if_dir _resource_path
    if (method == "GET" || method == "HEAD" || (_error != 200 && _error != 201)) {
        if (S_ISDIR(buf.st_mode) && (*_location).find("if_dir") == (*_location).end() && *(*_location)["autoindex"].begin() == "on") { 
            if ((i = make_index()) != 0)
                return build_error_response(500);
        }
        else {
            if (S_ISDIR(buf.st_mode)) {
                _resource_path = *(*_location)["if_dir"].begin();
                if (_resource_path.find_last_of(".") < _resource_path.size())
                    _extension = _resource_path.substr(_resource_path.find_last_of("."));
            }
            // if (*_location).find("cgi_bin") != (*_location).end()
            //      go CGI
            //      CGI result to body ?
            // else
            if (file_to_body() != 0)
                return build_error_response(500);
        }
    }
    
    // go to the proper header function
    (this->*_methods[method])();

    std::cout << std::endl << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
    std::cout << "*-*-*-*-*-*-*-*-*-RESPONSE-*-*-*-*-*-*-*-*-*-*-*--*" << std::endl;
    std::cout << "PAYLOAD: [" << _payload << "]" << std::endl;
    std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
    std::cout << "AFTER BUILD RESPONSE" << std::endl;
    return (0);
}

void ServerResponse::method_get() {
 
    //  do first line - we should do a std::string table of [error codes x error message] maybe (ex: table[500] = "Internal Server Error")
    std::string s_error = ft_itos(_error);
    std::string *p_error_msg = _error_codes.get_value(s_error);
    std::string s_error_msg = "";
    std::string sp = " ";
    if (p_error_msg)
        s_error_msg = *p_error_msg;
    _payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
    
    _payload += "Content-Type: " + get_mime_type(_extension) + "\r\n";

    _payload += "Content-Length: " + ft_itos(_body.size()) + "\r\n";

    //  blank line, then content BODY        
    _payload += "\r\n" + _body + "\r\n";
};

void ServerResponse::method_head() {

    //  do first line
    std::string s_error = ft_itos(_error);
    std::string *p_error_msg = _error_codes.get_value(s_error);
    std::string s_error_msg = "";
    std::string sp = " ";
    if (p_error_msg)
        s_error_msg = *p_error_msg;
    _payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
    
    _payload += "Content-Type: " + get_mime_type(_extension) + "\r\n";

    _payload += "Content-Length: " + ft_itos(_body.size()) + "\r\n";

    //  blank line     
    _payload += "\r\n";
};

/*La différence entre PUT et POST tient au fait que PUT est une méthode idempotente. 
Une requête PUT, envoyée une ou plusieurs fois avec succès, aura toujours le même 
effet (il n'y a pas d'effet de bord). À l'inverse, des requêtes POST successives et 
identiques peuvent avoir des effets additionnels, ce qui peut revenir par exemple à 
passer plusieurs fois une commande.*/

/* PUT puts a file or resource at a specific URI, and exactly at that URI. If there's 
already a file or resource at that URI, PUT replaces that file or resource. If there is 
no file or resource there, PUT creates one. Replaces target resource with the request 
payload. Can be used to update or create a new resources.
POST: Performs resource-specific processing on the payload. Can be used for different actions 
including creating a new resource, uploading a file or submitting a web form.*/
void ServerResponse::method_put() {
    
    //1) if CGI
    //      go CGI
    //      CGI result to body ?
    //2) do POST stuff
    // else {
    int fd;
    if ((fd = open(_resource_path.c_str(), O_RDWR | O_CREAT)) < 0) {
        build_error_response(500);
        return;
    }
    std::cout << "GONNA WRITE [" << _cli_body.c_str() << "] size [" << _cli_body.size() << "]" << std::endl;
    if (write(fd, _cli_body.c_str(), _cli_body.size()) < 0) {
        build_error_response(500);
        return;
    }
    close(fd);
    //}

    //3) build response

    std::string s_error = ft_itos(_error);
    std::string *p_error_msg = _error_codes.get_value(s_error);
    std::string s_error_msg = "";
    std::string sp = " ";
    if (p_error_msg)
        s_error_msg = *p_error_msg;
    _payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
    
    _payload += "Content-Location: " + _resource_path + "\r\n";

    //  blank line      
    _payload += "\r\n";
};

void ServerResponse::method_post() {
 
    //1) if CGI
    //      go CGI
    //      CGI result to body ?
    
    //2) do POST stuff
    // else {
    int fd;
    if ((fd = open(_resource_path.c_str(), O_RDWR | O_CREAT | O_APPEND)) < 0) {
        build_error_response(500);
        return;
    }
    std::cout << "GONNA WRITE [" << _cli_body.c_str() << "] size [" << _cli_body.size() << "]" << std::endl;
    if (write(fd, _cli_body.c_str(), _cli_body.size()) < 0) {
        build_error_response(500);
        return;
    }
    close(fd);
    //}

    //3) build response

    std::string s_error = ft_itos(_error);
    std::string *p_error_msg = _error_codes.get_value(s_error);
    std::string s_error_msg = "";
    std::string sp = " ";
    if (p_error_msg)
        s_error_msg = *p_error_msg;
    _payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
    
    _payload += "Content-Location: " + _resource_path + "\r\n";

    //  blank line      
    _payload += "\r\n";
};


int ServerResponse::file_to_body(void) {

    int fd;
    if ((fd = open(_resource_path.c_str(), O_NONBLOCK)) < 0)
        return build_error_response(500);

    std::string smax_body;
    if (get_serv_info().find("client_max_body_size") == get_serv_info().end())
        smax_body = DEFAULT_MAX_BODY;
    else
        smax_body = *get_serv_info()["client_max_body_size"].begin();

    int max_body = ft_stoi(smax_body);
    char buf[max_body + 1];
    if (read(fd, buf, max_body) < 0)
        return build_error_response(500);
    _body += buf;

    close(fd);
    return 0;
}

int ServerResponse::make_index(void) {
    
    DIR *cur_dir;
    if (!(cur_dir = opendir(_resource_path.c_str())))
        return build_error_response(500);

    _body += "<!DOCTYPE html>\r\n<title><b> Index:\r\n" + _resource_path + "</b></title>\r\n<ul>";
    
    struct dirent *content;
    while ((content = readdir(cur_dir)) != NULL) {
        if (content->d_type == DT_DIR)
            _body += "<li><b>" + std::string(content->d_name) + "</b></li>\r\n";
        else
            _body += "<li>" + std::string(content->d_name) + "</li>\r\n";
    }
    _body += "</ul>\r\n";
    closedir(cur_dir);
    return 0;
}
