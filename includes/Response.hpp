#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <cstring>
#include "request.hpp"
#include "Utils.hpp"
#include "Parsing.hpp"
#include "MimeTypes.hpp"
#include <algorithm>
#include "StatusCode.hpp"
#include <stdio.h>


class Response
{
	private:
		HttpStatus::statusCode _statuscode;
		Request	_request;
		Location_block _location;
		std::string _response;
		int _fd;
		fd_set _set;
		bool _is_request_handled;
		std::fstream _body;
		std::string _path;
		std::string _filepath;
		int is_autoindex;
		int	_file_not_found;
		long _size_of_file;
		long _size_sended;
		Server_block _server;

	public:
		Response(Request	request);
		Response(){};
		std::string get_file_path();
		Response(const Response& other);
		Response& operator=(const Response& other);
		~Response();
	
		void reset();
		std::string get_respone( void ) const;
		int get_fd( void );
		void close_fd( void);
		long get_size_of_file(){return _size_of_file;}
		long get_size_sended(){return _size_sended;}
		Request get_request(){return _request;}
		std::fstream& get_body( void );
		void update_size_sended(long send){_size_sended += send;}

		
		void set_error_header(int statuscode, std::string msg, std::string path);
		void set_redirection(int statuscode, std::string path);

		void unallowedMethod();
		void badRequest();
		void internalError();
		void httpVersionNotSupported(std::string const &version);
		void time_out();
		void notFound();
		void forbidden();
		void payloadTooLarge();
		void ok(size_t bodysize);

		void create_file();
		int check_max_body_size();
		void auto_index(Location_block location);
		Location_block getLocation(Server_block &server);
		void handleRequest(Server_block server);
		void handleGetRequest(Server_block server, Location_block location);
		void handlePostRequest(Server_block &server, Location_block &location);
		void handleDeleteRequest();
};

#endif
