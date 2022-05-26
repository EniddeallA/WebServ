#ifndef REQUEST
# define REQUEST
# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <vector>
# include <algorithm>
# include <map>
# include "Utils.hpp"

# include "Parsing.hpp"

# define HTTP_VERSION "HTTP/1.1"
# define BAD_REQUEST 400
# define HTTP_VERSION_NOT_SUPPORTED 505
# define TOO_LARGE 413
# define METHOD_NOT_ALLOWED 405
# define CL     "content-length"
# define TE     "transfer-encoding"

typedef std::map<std::string, std::string> t_headers;

class Request{
	public:
		Request();
		Request(Request const &src);
		Request &operator=(Request const &src);
		~Request();
	


		// size_t getError(void) { return _error; }
		void    errorHandler(){};
		void    printData( void );

		void    Parse(std::string &req);
		bool    isRequestCompleted() { return _requestEnd; }
		void    clear( void );
		/** WARNING
		 * getValidServer
		*/
	   /*
		GETTERS
	   */
		// std::string getRequestMethod( void ) const;
		// std::string getRequestTarget( void ) const;
		// std::string getRequestQuery( void ) const;
		// std::string getHost( void ) const;
		// std::string getBodyName( void ) const;
		// bool    getKeepAlive( void ) const;
		// int getError( void ) const;
		// size_t getContentLength( void ) const;



        unsigned int    getError(void) { return _error; }
        std::string     getRequestMethod() const { return _requestMethod; }
        std::string     getRequestTarget() const { return _requestTarget; }
        std::string     getRequestQuery() const { return _requestQuery; }
        std::string     getHost() const { return _host; }
        std::string     getBody() const { return _bodyName; }
        bool            _isKeepAlive() const { return _keepAlive == true; }
        size_t          getContentLength() const { return _contentLength; }
        std::string     getContentType() { return ((_headers.find("Content-Type") != _headers.end()) ? _headers["Content-Type"] : ""); }
		t_headers 		getHeaders( void ) const;
		Server_block 	setServer( std::vector<Server_block > const &serv_confs );
		



	private:
		void parseHeaders(std::string headers);
		void parseFirstLine(std::vector<std::string> vec);
		void parseBody(std::string &req);
		void toChuncked(std::string &req);
		void preBody( void );
		void setLocation( void );
		std::string all_string_req;
		int first_head;
		
		
	private:
		int                         _error;
		std::string                 _requestMethod, _requestTarget, _requestQuery;
		std::string                 _host, _bodyName, _str, _port;
		bool                        _hasBody, _keepAlive;
		bool                        _headersEnd, _requestEnd;
		bool						_isCL, _isTE, _serverFound;
		std::vector<std::string>    _allowedMethods;
		t_headers _headers;
		std::ofstream               _bodyFile;
		size_t                      _bodySize, _contentLength;
		// Location_block				_location;
		Server_block				_server;


	private:
		std::string    _bodyToFile();
};

/*		

HTTP-message   =    start-line
					*( header-field CRLF )
					CRLF
					[ message-body ]

*/
#endif