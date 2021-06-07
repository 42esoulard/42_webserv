/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerResponse.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 16:27:00 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/07 21:58:09 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP

#include "webserv.h"
#include "Server.hpp"
#include "Cgi.hpp"
class Cgi;

/*
    ◦ Allow: valid methods for a specified resource. To be used for a 405 Method not allowed
                [Allow: GET, HEAD]
    ◦ Content-Language: the natural language or languages of the intended audience for the enclosed content
                [Content-Language: da]
    ◦ Content-Length: the length of the response body in octets (8-bit bytes)
                [Content-Length: 348]
    ◦ Content-Location: an alternate location for the returned data
                [Content-Location: /index.htm]
    ◦ Content-Type: the MIME type of this content - its media type two part identifier
                [Content-Type: text/html; charset=utf-8]
    ◦ Date: the date and time that the message was sent (in "HTTP-date" format as defined by RFC 7231)
                [Date: Tue, 15 Nov 1994 08:12:31 GMT]
    ◦ Last-Modified: last modified date for the requested object (in "HTTP-date" format as defined by RFC 7231)
                [Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT]
    ◦ Location: Used in redirection, or when a new resource has been created.
                [Location: http://www.w3.org/pub/WWW/People.html]
                [Location: /pub/WWW/People.html]
    ◦ Retry-After: If an entity is temporarily unavailable, this instructs the client to try again later. Value could be a specified period of time (in seconds) or a HTTP-date
                [Retry-After: 120]
                [Retry-After: Fri, 07 Nov 2014 23:59:59 GMT]
    ◦ Server: A name for the server
                [Server: Apache/2.4.1 (Unix)]
    ◦ Transfer-Encoding: The form of encoding used to safely transfer the entity to the user. Currently defined methods are: chunked, compress, deflate, gzip, identity.
                [Transfer-Encoding: chunked]
    ◦ WWW-Authenticate: Indicates the authentication scheme that should be used to access the requested entity.
                [WWW-Authenticate: Basic]
*/

class ServerResponse {

    public:

        ServerResponse(SimpleHashTable &mime_table, SimpleHashTable &error_codes, std::list<Server> &server_list);
        
        ~ServerResponse();

        std::string &get_payload() { return _payload; }

        void set_conf(std::string &key, std::string &val) { _conf[key] = val; };

    //put all our method functions in a map, this way we can just parse
    // the request and call e.g. (ServerResponse.get_method("GET"))()
        void init_methods_list() {
            _methods["GET"] = &ServerResponse::method_get;
            _methods["HEAD"] = &ServerResponse::method_head;
            _methods["POST"] = &ServerResponse::method_post;
            _methods["PUT"] = &ServerResponse::method_put;
            _methods["DELETE"] = &ServerResponse::method_delete;
            // _methods["CONNECT"] = &ServerResponse::method_connect;
            // _methods["OPTIONS"] = &ServerResponse::method_options;
            // _methods["TRACE"] = &ServerResponse::method_trace;
        };

        typedef std::map<std::string, std::list<std::string> >  t_content_map;
        
        typedef void (ServerResponse::*method_func)();
        method_func get_method(std::string &method) {
            return _methods[method];
        };

        std::string get_mime_type(std::string &extension);
        std::string get_next_token(std::string &line, size_t &index);
        std::list < t_content_map > &get_locations() {return _server_conf->locations;}
        Server::t_content_map       &get_serv_info() { return _server_conf->serv_info; }

        Server::t_conf *get_server_conf_by_name(std::string &searched_name, std::string &searched_port);
        Server::t_conf *get_server_conf_by_address(std::string &searched_host, std::string &searched_port);
        int identify_server(t_content_map &cli_conf); 
        int identify_location(std::string &file, std::string &extension);

        int build_response(t_content_map &cli_conf); // CALL IN CLIREQ AFTER PARSING FIELDS
        int file_to_body(void);
       // int build_response_headers(t_content_map &cli_conf);
        int make_index(void);
        int check_auth(std::string &tmp);


		//this is temporary
		int		error(int code);//this one should be called if error in Clireq along with a return
                        // ie: if (blabla != 0)
                        //          return error(500);
                        //this way we cut parsing short and head straight to response building
        int     build_error_response(int code);// <- once in build_response, we first check if _error
                        // indicates a previous error, and all subsequent errors will go straight
                        // there and return
                        // ie: if (blabla is ugly)
                        //          return build_error_response(error code);

    private:
    //CHECK IF FIELDS MUST BE SENT BACK IN A SPECIFIC ORDER

        SimpleHashTable     _mime_types;
        SimpleHashTable     _error_codes;
        std::list<Server>   _server_list;
        Server::t_conf      *_server_conf;
        t_content_map       *_location;
        std::string         _resource_path;
        char                _abs_resource_path[PATH_MAX];
        std::string         _extension;
        std::string         _query;
        int                 _error;
        std::string         _method;
        std::string         _cli_body;
        std::string         _body;
        std::string         _payload;

        int                 i;

        friend class        Cgi;
        Cgi                 *_cgi;
        bool                _cgi_on;


        //*************************************************************************
        // PUT ALL THIS IN A MAP<std::string, std::string>, make it shine
        std::map<std::string, std::string> _conf;

        // ^^^ this will contain this vvv

        // //first line
        // std::string _protocol;// HTTP/1.1
        // std::string _status_code;
        // std::string _status_msg;

        // //general info stuff
        // std::string _server;// as declared in the config file
        // std::string _date;// need a nice time/date formatting function

        // //content related stuff
        // std::string _body;
        // std::string _content_lang;
        // std::string _content_len;
        // std::string _content_loc; // does this appear when there's no alternate location?
        // std::string _content_type;//need to calculate size of data returned
        // std::string _last_modified;
        // std::string _location;// in case of redir or newly created content

        // //if theres an error stuff
        // std::string _allow;// if status code 405, indicate which methods were valid
        // std::string _retry_after;

        // //security stuff
        // std::string _encoding;
        // std::string _www_auth;

        // function pointer on methods, all in a nice map (yes i love maps)
        //*************************************************************************

        //*************************************************************************

        std::map< std::string, method_func >  _methods;

        void method_get();
        void method_head();
        void method_post();
        void method_put();
        void method_delete();
        // void method_connect(){ /* do CONNECT related stuff here */ };
        // void method_options(){ /* do OPTIONS related stuff here */ };
        // void method_trace(){ /* do TRACE related stuff here */ };
        // see this great simple guide https://www.tutorialspoint.com/http/http_methods.htm
        //*************************************************************************
};

#endif
