/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 16:23:08 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/15 22:04:25 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerResponse.hpp"

ServerResponse::ServerResponse(SimpleHashTable &mime_table, SimpleHashTable &error_codes, std::list<Server> &server_list): _mime_types(mime_table), _error_codes(error_codes), _server_list(server_list), _error(200), _body(""), _payload(""), _cgi_on(false) {
	_cgi = new Cgi();
	std::cout << "IN SERVRESP 0 NEW CGI" << std::endl;

	init_methods_list();
};

// ServerResponse::ServerResponse() {
// 	// _cgi = new Cgi();
// 	// std::cout << "IN SERVRESP 1 NEW CGI" << std::endl;
// 	std::cout << "IN SERVRESP 1 NO NEW CGI" << std::endl;
// };

void ServerResponse::reinit_serv_response() {
	
	_server_conf = NULL;
    _location = NULL;
	_resource_path.clear();
    memset(_abs_resource_path, 0, PATH_MAX);
    _extension.clear();
    _query.clear();
    _error = 200;
    _method.clear();
    _cli_body.clear();
    _max_body = -1;
    _body.clear();
    _payload.clear();
	i = -1;
	std::cout << "bef reinit cgi in reinit serv resp" << std::endl;
    (*_cgi).reinit_cgi();
	std::cout << "aft reinit cgi in reinit serv resp" << std::endl;
    _cgi_on = false;
}

ServerResponse::~ServerResponse() { std::cout << "IN ~SERVRESP DELETING CGI" << std::endl;
delete _cgi; };

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

// This just sets up the error code. Error return handling is done
// via build_error_response upon entering serverResponse
int			ServerResponse::error(int code)
{
	_error = code;
	return (code);
}

int ServerResponse::build_error_response(int code) {
	std::string	 path(ERROR_FOLDER);
	char			buf[4096];
	int			 fd;

	// try to open error matching path
	// if it doesnt work, try to open default path
	// if it doesn't work, give default string to body

	_error = code; //in case the call is for an error in the latest stages
	path += ft_itos(code);
	path += ".html";
	 //if not found, try to see if theres a default page!! (see conf, "default_error")
		//check if theres a default err for location or server
		//if we fail at opening anything, we format a default error

	if ((fd = open(path.c_str(), O_RDONLY)) == -1 &&
		((*_location).find("default_error") == (*_location).end() ||
		(fd = open((ERROR_FOLDER + (*(*_location)["default_error"].begin())).c_str(), O_RDONLY)) == -1)
		&& (get_serv_info().find("default_error") == get_serv_info().end() ||
		(fd = open((ERROR_FOLDER + (*get_serv_info()["default_error"].begin())).c_str(), O_RDONLY)) == -1 )) {

			_body = "<!DOCTYPE html>\n<title>Error0</title>\n";
	}
	else {
		//should we change 4096 ?
		memset(buf, 0, 4096);
		if (read(fd, buf, 4096) == -1)
			_body = "<!DOCTYPE html>\n<title>Error1</title>\n";
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
	std::cout << "IN ERROR METHOD = " << _method << std::endl;
	_payload += "\r\n";
	if (_method != "HEAD")
	{
		std::cout << "HERE" << std::endl;
		_payload += _body;
	}

	std::cout << "[ERROR PAYLOAD:][" << _payload << "]" << std::endl;
	return (_error);
}

std::string ServerResponse::get_mime_type(std::string &extension) {

	std::string ret = "application/octet-stream";

	if (extension.find('/') != std::string::npos)
		return (ret);
	if (extension.size() > 1 && extension[0] == '.')
		extension = extension.substr(1);
	std::string *value =_mime_types.get_value(extension);
	//unknown extension defaults to application/octet-stream type

	if (value)
		ret = *value;

	return (ret);
};

// find which server is requested by the client and save its conf
int ServerResponse::identify_server(t_content_map &cli_conf) {

	std::list<std::string>::iterator it;
	std::string port;
	if (cli_conf.find("port") == cli_conf.end())
		port = ft_itos(PORT);
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
unsigned long ServerResponse::identify_location(std::string &file, std::string &extension) {

	if (file == "")
		return i;
	unsigned long i = file.find_last_of("/");
	if (i == std::string::npos || i == 0)
		i = file.size();

	std::string path;
	std::list < t_content_map >::iterator it;
	std::list < std::string >::iterator ext_it;
	t_content_map 						*ext_location = NULL;

	while (i != std::string::npos) {

		// if (i > 0 && path[i] == '/')
		// 	path = file.substr(0, i - 1);
		if (i == 0 && path[i] == '/')
			path = file.substr(0, 1);
		else
			path = file.substr(0, i);
		// std::cout << "i = " << i << " path " << path << std::endl;
		// getchar();
		// else
		// 	path = file;

		it = get_locations().begin();
		while(it != get_locations().end()) {

			//check if extension fits extension listed in location conf
			if ((*it).find("extensions") != (*it).end()) {
				if (extension == "" && (*it)["extensions"].begin() != (*it)["extensions"].end()) {
					++it;
					continue;
				}
				if (extension != "" && (*it)["extensions"].begin() != (*it)["extensions"].end()) { //first check extension
					ext_it = (*it)["extensions"].begin();
					while (ext_it != (*it)["extensions"].end()) {
						if (extension == *ext_it) {
							break;
						}
						ext_it++;
					}
					if (ext_it == (*it)["extensions"].end()) {
						++it;
						continue;
					}
				}
			}
		   //check if path matches path
		   //ext_location = NULL;
		   	if (extension != "" && *(*it)["path"].begin() == "*")
				ext_location = &(*it);
			_location = &(*it);
			if (*(*it)["path"].begin() == path) {
				if (ext_location) {
					if ((*ext_location).find("accept_methods") != (*ext_location).end()) {
						std::list<std::string>::iterator st = (*ext_location)["accept_methods"].begin();
						while (st != (*ext_location)["accept_methods"].end()) {

							(*_location)["accept_methods"].push_back(*st);
							++st;
						}
					}
					if ((*ext_location).find("cgi_bin") != (*ext_location).end()) {
						if ((*_location).find("cgi_bin") == (*_location).end())
							(*_location)["cgi_bin"].push_back(*(*ext_location)["cgi_bin"].begin());
						else
							*(*_location)["cgi_bin"].begin() = *(*ext_location)["cgi_bin"].begin();
					}
				}
				return i;
			}
		   ++it;
		}
		i = file.find_last_of("/", i - 1);
	}
	return -1;
}

// find a server with one of its names
Server::t_conf *ServerResponse::get_server_conf_by_name(std::string &searched_name, std::string &searched_port) {

	std::list<Server>::iterator server_it = _server_list.begin();
	std::list<std::string>::iterator	content_it;
	std::list<std::string>::iterator	port_it;

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

// find a server with its address
Server::t_conf  *ServerResponse::get_server_conf_by_address(std::string &searched_host, std::string &searched_port) {

	std::list<Server>::iterator server_it = _server_list.begin();
	std::list<std::string>::iterator	host_it;
	std::list<std::string>::iterator	port_it;

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

// tmp decoded from base64 as login:password and searched in *(*_location)["root"].begin()
// + "/" + ".auth" file ) (each entry in .auth being in format login:base64password
// If a folder is protected, the location root MUST contain a .auth file with credentials
// This is NOT secure, but it's better than nothing
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

	int fd;
	if ((fd = open(chest.c_str(), O_RDONLY)) < 0)
		return error(500);
	std::string sub = decode(tmp);

	// decode .auth line by line && compare w submitted cred
	char *line;
	std::string cred;
	std::string pass;
	unsigned int i;
	while (get_next_line(fd, &line) > 0) {
		cred = std::string(line);
		if ((i = cred.find_first_of(':')) != cred.npos && i < cred.size()) {// ':' can be in pwd, but not in login. Also format has to be login=password, if there's no '=' we disregard the entry
			pass = decode(cred.substr(i + 1, cred.size()));
			cred = cred.substr(0, i + 1);
			cred += pass;
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

int ServerResponse::no_host_response()
{

	int 	fd;
	char	buf[4096];

	_error = 200;
	if (_method != "GET")
		return build_error_response(400);
	if ((fd = open(WELCOME2WEBSERV, O_RDONLY)) == -1) {
			return build_error_response(400);
	}
	else {
		//should we change 4096 ?
		memset(buf, 0, 4096);
		if (read(fd, buf, 4096) == -1)
			return build_error_response(400);
		else
			_body = std::string(buf);
		close(fd);
	}
	method_get();
	return 0;
}

bool	ServerResponse::check_path_lvl(std::string &path)
{
	int		lvl = 1;
	size_t	i = 0;

	if (path.empty())
		return (false);
	if (path[0] == '/')
		i++;
	if (path.size() >= (2 + i) && path.substr(i, 2) == "..")
		return (true);
	if (path.size() >= 1 + i && path[i] == '.')
		lvl--;
	while ((i = path.find('/', i)) != std::string::npos)
	{
		if (path.size() == i)
			break ;
		if (path[i + 1] != '.')
			lvl++;
		else if (path.size() >= i + 2 && path.substr(i + 1, 2) == "..")
			lvl--;
		if (lvl < 0)
			return (true);
		i++;
	}
	return (false);
}

int ServerResponse::build_response(t_content_map &cli_conf) {

	// 0) find which server is the request addressed to
	_method = *(cli_conf["method"].begin());
	if (_error == 666) //no host
		return no_host_response();
	if ((i = identify_server(cli_conf)) != 200)
		return error(i); //404, server not found
	// 1) save file extension in a string + extract potential query from url
	std::string requested_path = *cli_conf["file"].begin();


	i = requested_path.find_first_of("?");
	if ((size_t)i < requested_path.size()) {
		_query = requested_path.substr(i + 1);
		requested_path = requested_path.substr(0, i);
	}
	_extension = "";
	if (requested_path.find_last_of(".") < requested_path.size())
	   _extension = requested_path.substr(requested_path.find_last_of("."));

	// 2) rebuild path thanks to conf
	//  TO GET LOCATION, LOOP ON:
	//  split path from file name until you get something in the format / + dir (ex: /bla/blou) (ex: /) (ex: /bli):
	//  first compare up to first /, then go to the next (ex: for /bla/bli/blou check /bla then /bla/bli then /bla/bli/blou)
	//  each time, first compare location["extensions"] if theres one with the extension. If it doesnt match, next.
	//  if theres no location["extensions"] -> just check pathrequested_path.substr(requested_path.find_last_of("."));
	//  when a match is found, replace this with the location["root"], end of loop
	if ((i = identify_location(requested_path, _extension)) < 0)
		return error(404); // location not found

	std::cout << "----------------- SERVER + LOCATION FOUND!" << std::endl;


	// 3) after we got our Server and Location info, we can check if we have a previous error
	// IF ERROR != 200 || 201, RETURN BUILD_ERROR()
	if (_error != 200 && _error != 201)
		return build_error_response(_error);

	std::cout << "----------------- NO PREVIOUS ERROR FOUND!" << std::endl;

	//check body size
	if ((*_location).find("client_max_body_size") != (*_location).end())
		_max_body = ft_stoi(*(*_location)["client_max_body_size"].begin());
	else if ((get_serv_info().find("client_max_body_size")) != get_serv_info().end())
		_max_body = ft_stoi(*get_serv_info()["client_max_body_size"].begin());
	else
		_max_body = DEFAULT_MAX_BODY;
	if (cli_conf.find("body") != cli_conf.end() && cli_conf["body"].begin() != cli_conf["body"].end()) {
		if ((*cli_conf["body"].begin()).size() > _max_body && (_method == "POST" || _method == "PUT") && _max_body != DEFAULT_MAX_BODY)
			return (build_error_response(413));
		else if ((*cli_conf["body"].begin()).size() > _max_body && _method != "POST")
			(*cli_conf["body"].begin()) = (*cli_conf["body"].begin()).substr(0, _max_body);
	}


	// 4) substitute requested path location alias with root path
	//   check file existence and status (or uploads dir existence for PUT/POST)


	if ((*_location).find("root") != (*_location).end()){
		_resource_path = *(*_location)["root"].begin();
	}
	if ((_method == "PUT" || _method == "POST") && (*_location).find("up_dir") != (*_location).end() && *(*_location)["up_dir"].begin() != *(*_location)["up_dir"].end())
		_resource_path += *(*_location)["up_dir"].begin();
	if (i < requested_path.size())
		_resource_path +=  requested_path.substr(i);
	std::cout << "METHOD = " << _method << std::endl;

	if (check_path_lvl(requested_path))
		return build_error_response(403); // forbidden, security
	struct stat buf;
	int ir;

	if ((ir = stat(_resource_path.c_str(), &buf)) < 0)
		_error = 201; //not an error, means file doesn't exist and will be created
	if (_method == "PUT" || _method == "POST" || _method == "DELETE") {
		if (i < requested_path.size())
			std::cout << "0RESOURCE PATH [" << _resource_path << "] substr [" << requested_path.substr(i + 1) << "]" << std::endl;
		else
			std::cout << "0RESOURCE PATH [" << _resource_path << "] substr [" << i << "]" << std::endl;

		if (_method == "DELETE" && ir < 0)
			return build_error_response(404); // file not found
		else if (_method == "DELETE" && S_ISDIR(buf.st_mode))
			return build_error_response(405);//trying to delete a folder is forbidden
		else if (ir != -1 && S_ISDIR(buf.st_mode)) {
			_error = 201; //not an error, means no file name was provided, will be created
			_resource_path += "/" + std::string(DEFAULT_UPLOAD_NAME);
		}
		if (cli_conf.find("body") != cli_conf.end())
			_cli_body = (*cli_conf["body"].begin());
	}
	else {


		std::cout << "1RESOURCE PATH [" << _resource_path << "] substr [" << requested_path << "] loc [" << *(*_location)["path"].begin() << "]" << std::endl;

		if ((ir = stat(_resource_path.c_str(), &buf)) < 0)
			return build_error_response(404); // file not found
	}
	if (!(_method == "PUT" || _method == "POST") && (buf.st_mode & S_IROTH) == 0){
		return build_error_response(403);
	}


	std::cout << "----------------- FILE OK!" << std::endl;

	// 5) check that method is allowed (in conf location)
	//  if not, error 405 method not allowed
	std::list<std::string>::iterator it = (*_location)["accept_methods"].begin();
	while (it != (*_location)["accept_methods"].end()) {
		if (*it == *cli_conf["method"].begin())
			break;
		++it;
	}
	if (it == (*_location)["accept_methods"].end())
		return build_error_response(405); // method not allowed
	std::cout << "----------------- METHOD OK!" << std::endl;


	// 6) if protected  (in conf location auth: on), check authorization (first Basic,
	//  else Unknown auth method error. Second, decode base64 and check against against
	//  location root's .auth file)

	if ((*_location).find("auth") != (*_location).end() && *(*_location)["auth"].begin() == "on") {
		size_t j = 0;
		if (cli_conf.find("authorization") == cli_conf.end())
			return build_error_response(401); // unauthorized (missing credentials)
		std::string tmp = get_next_token(*cli_conf["authorization"].begin(), j);
		if (tmp != "Basic")
			return build_error_response(401); // unauthorized (authorization method unknown)
		if ((tmp = get_next_token(*cli_conf["authorization"].begin(), j)) == "")
			return build_error_response(401); // unauthorized (missing credentials)
		if (check_auth(tmp) < 0)
			return build_error_response(401); //unauthorized(bad credentials)
	}

	std::cout << "----------------- AUTH OK!" << std::endl;
	// 7) if IS_DIR and we didn't define an index in conf and autoindex = "on"
	//  return an autoindexing of website tree.
	//  if IS_DIR && we have an index, use index as requested file
	if (_method == "GET" || _method == "HEAD" || (_error != 200 && _error != 201)) {
		if (!S_ISREG(buf.st_mode) && (*_location).find("if_dir") == (*_location).end() && (*_location).find("autoindex") != (*_location).end() && *(*_location)["autoindex"].begin() == "on") {
			if ((i = make_index()) != 0)
				return build_error_response(500);
		}
		else {
			if (!S_ISREG(buf.st_mode)) {
				_resource_path += "/" + *(*_location)["if_dir"].begin();
				if ((ir = stat(_resource_path.c_str(), &buf)) < 0)
					return build_error_response(404); // file not found
				if (_resource_path.find_last_of(".") < _resource_path.size())
					_extension = _resource_path.substr(_resource_path.find_last_of("."));
				std::cout << "RESOURCE PATH [" << _resource_path << "]" << std::endl;
			}

			if ((*_location).find("cgi_bin") != (*_location).end()) {
				if ((i = requested_path.find_last_of("/")) == std::string::npos)
					i = -1;
				if (requested_path.substr(i + 1) == "webserv")
					return build_error_response(418);
				if (_cgi->launch_cgi(*this, cli_conf) != 0)
					return build_error_response(_error);
				_cgi_on = true;
			}
			else if (file_to_body() != 0) {
				std::cout << "----------------- FILE TO BODY FAIL: [" << _resource_path << "]!" << std::endl;
				return build_error_response(500);
			}
			std::cout << "AFTER CGI OR FILE TO BODY: ---BODY[" << _body << "]" << std::endl;
			std::cout << "AFTER CGI OR FILE TO BODY: ---PAYLOAD[" << _payload << "]" << std::endl;
		}
	}
	else if ((*_location).find("cgi_bin") != (*_location).end() && (_method == "PUT" || _method == "POST" || _method == "DELETE")) {
		if (_cgi->launch_cgi(*this, cli_conf) != 0)
			return build_error_response(_error);
		_cgi_on = true;
	}

	std::cout << "----------------- CGI OK!" << std::endl;
	// 8) go to the proper header function
	(this->*_methods[_method])();

	/*std::cout << std::endl << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
	std::cout << "*-*-*-*-*-*-*-*-*-RESPONSE-*-*-*-*-*-*-*-*-*-*-*--*" << std::endl;
	std::cout << "PAYLOAD: [" << _payload << "]" << std::endl;
	std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;*/
	if (_error != 200 && _error != 201)
		return (build_error_response(_error));
	std::cout << "BEFORE RET SR" << std::endl;
	return (0);
}

void ServerResponse::method_get() {

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
	_payload += "\r\n" + _body;
};

void ServerResponse::method_head() {

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

	if (!_cgi_on) {
		int fd;
		if ((fd = open(_resource_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
			build_error_response(500);
			return ;
		}
		//std::cout << "GONNA WRITE [" << _cli_body.c_str() << "] size [" << _cli_body.size() << "]" << std::endl;
		if (write(fd, _cli_body.c_str(), _cli_body.size()) < 0)
		{
			close(fd);
			build_error_response(500);
			return ;
		}
		close(fd);
	}
	std::string s_error = ft_itos(_error);
	std::string *p_error_msg = _error_codes.get_value(s_error);
	std::string s_error_msg = "";
	std::string sp = " ";
	if (p_error_msg)
		s_error_msg = *p_error_msg;
	_payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
	_payload += "Content-Location: " + _resource_path + "\r\n";
	_payload += "Content-Length: 0\r\n\r\n";

};

void ServerResponse::method_post() {

	std::cout << "in method post resource_path [" << _resource_path << "]" << std::endl;
	if (!_cgi_on) {
		int fd;
		if ((fd = open(_resource_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
			build_error_response(500);
			return;
		}
		//std::cout << "GONNA WRITE [" << _cli_body.c_str() << "] size [" << _cli_body.size() << "]" << std::endl;
		if (write(fd, _cli_body.c_str(), _cli_body.size()) < 0) {
			build_error_response(500);
			return;
		}
		close(fd);
	}
	std::string s_error = ft_itos(_error);
	std::string *p_error_msg = _error_codes.get_value(s_error);
	std::string s_error_msg = "";
	std::string sp = " ";
	if (p_error_msg)
		s_error_msg = *p_error_msg;
	// std::cout << "s_error_msg [" << s_error_msg << "]" << std::endl;
	_payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";

	_payload += "Content-Location: " + _resource_path + "\r\n";

	/*std::cout << "in method post cgi on = " << _cgi_on << std::endl;
	getchar();
	std::cout << "in method post payload = [" << _payload << "]" << std::endl;
	getchar();
	std::cout << "in method post body = [" << _body.substr(0, 10) << "]" << std::endl;
	getchar();*/
	// if (_cgi_on) {
		_payload += "Content-Type: " + get_mime_type(_extension) + "\r\n";
		_payload += "Content-Length: " + ft_itos(_body.size()) + "\r\n";
		_payload += "\r\n" + _body;
	// }

	std::cout << "end of method post" << std::endl;
};

void ServerResponse::method_delete() {

	if (!_cgi_on)
		remove(_resource_path.c_str());
	_error = 204;

	std::string s_error = ft_itos(_error);
	std::string *p_error_msg = _error_codes.get_value(s_error);
	std::string s_error_msg = "";
	std::string sp = " ";
	if (p_error_msg)
		s_error_msg = *p_error_msg;
	_payload += "HTTP/1.1" + sp + s_error + sp + s_error_msg + "\r\n";
	if (_cgi_on) {
		_payload += "Content-Length: " + ft_itos(_body.size()) + "\r\n";
		_payload += "\r\n" + _body;
	}
	_payload += "\r\n";
};

int ServerResponse::file_to_body(void) {

	int fd;
	int size;
	if ((fd = open(_resource_path.c_str(), O_NONBLOCK)) < 0)
		return build_error_response(500);

	// std::string smax_body;
	// if (get_serv_info().find("client_max_body_size") == get_serv_info().end())
	// 	smax_body = DEFAULT_MAX_BODY;
	// else
	// 	smax_body = *get_serv_info()["client_max_body_size"].begin();

	// int max_body = ft_stoi(smax_body);
	char buf[_max_body + 1];
	memset(buf, 0, _max_body);
	if ((size = read(fd, buf, _max_body)) < 0)
		return build_error_response(500);
	for (int i = 0 ; i < size ; ++i)
	{
		_body.push_back(buf[i]);
	}
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
