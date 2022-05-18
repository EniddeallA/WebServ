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
		std::iostream* _body;
		std::string _path;

	public:
		Response(Request	request);
		Response(){

		}
		Response(const Response& other);
		Response& operator=(const Response& other);
		~Response();
	
		std::string get_respone( void ) const;
		
		void setHeader(size_t status_code, std::string const &message);
		void unallowedMethod();
		void badRequest();
		void internalError();
		void httpVersionNotSupported(std::string const &version);
		void time_out();
		void ok(std::string const &tmp_path);

		Location_block getLocation(Server_block server);
		void handleRequest(Server_block server);
		void handleGetRequest(Location_block file_path);
		void handlePostRequest(Location_block file_path);
		void handleDeleteRequest();

		void setErrorPage(const StatusCodeException & e, const Location_block * location, Server_block *server);
};

#endif
