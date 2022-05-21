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
#include "StatusCodeException.hpp"
#include "Utils.hpp"
#include "Parsing.hpp"
#include "MimeTypes.hpp"
#include <algorithm>

class Response
{
	private:
		HttpStatus::statusCode _statuscode;
		Request	_request;
		std::string _response;
		int _fd;
		fd_set _set;
		bool _is_request_handled;
		std::fstream _body;
		std::string _path;
		std::string _filepath;

	public:
		Response(Request	request);
		Response(){};
		Response(const Response& other);
		Response& operator=(const Response& other);
		~Response();
	
		std::string get_respone( void ) const;
		int get_fd( void ) const;
		void close_fd( void);

		std::fstream& get_body( void );
		void errorTemplate(const StatusCodeException & e);
		void create_file();
		
		void setHeader(size_t status_code, std::string const &message, size_t bodysize);
		void unallowedMethod();
		void badRequest();
		void internalError();
		void httpVersionNotSupported(std::string const &version);
		void time_out();
		void notFound();
		void ok(size_t bodysize);

		void auto_index();
		Location_block getLocation(Server_block server);
		void handleRequest(Server_block server);
		void handleGetRequest();
		void handlePostRequest();
		void handleDeleteRequest();

		void setErrorPage(const StatusCodeException & e, const Location_block * location, Server_block *server);
};

#endif
