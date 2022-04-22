#include "request.hpp"

Request::Request() : _error(0), _requestMethod(""), _requestTarget(""), _requestEnd(false),
	_requestQuery(""), _bodyName(""), _str(""), _hasBody(false), _keepAlive(false), _headersEnd(false),
	_bodySize(0), _contentLength(0) {
		_allowedMethods.push_back("GET");
		_allowedMethods.push_back("POST");
		_allowedMethods.push_back("DELETE");
}

Request::Request(Request const &src) {
	_error = src._error;
	_requestMethod = src._requestMethod;
	_requestTarget = src._requestTarget;
	_requestQuery = src._requestQuery;
	_bodyName = src._bodyName;
	
}

Request& Request::operator=(Request const &src){
	return *this;
}

Request::~Request() {

}

void Request::Parse(std::string &req)
{
	if (_requestEnd == true)
		return ;
	try {
		req = _str.append(req);
		size_t BOH = _str.find("\r\n"), EOH = _str.find("\r\n\r\n");
		if (_headersEnd == false && EOH == std::string::npos)
			return ;
		else if (_headersEnd == false && EOH != std::string::npos)
		{
			parseFirstLine(splinter(_str.substr(0, BOH), ' '));
			parseHeaders(_str.substr(BOH + 2, EOH - BOH));
			req = _str.erase(0, EOH + 4);
			_str = "";
			_headersEnd = true;
		}
		if (_requestMethod == "GET" && _headersEnd == true)
		{
			_requestEnd = true;
			/** WARNING
			 * get a data from server block
			*/
			return;
		}
		/** WARNING
		 * must check for serverBlock
		 * Check with khalid 
		*/
		parseBody(req);
		if (_requestEnd  /* is request Completed ?*/ )
		{
			_bodyFile.close();
			/** WARNING
			 * get data from server block
			 * check for body size and max size
			*/
			_requestEnd = true;
		}
	}
	catch (const char * message) {
		errorHandler();
		/** WARNING 
		 * unlink _bodyFile
		*/
	}
};


void Request::parseHeaders(std::string headers)
{
	std::vector<std::string> vec = splinter(headers, '\n'), vec2;
	for (size_t i = 0; i < vec.size(); i++) {
		vec[i].erase(vec[i].find('\r'));
		vec2 = splinter(vec[i], ':');
		if (isSpace(vec2[1][0])) {
			vec2[1].erase(0, 1);
			if (isSpace(vec2[1][0])){
				_error = BAD_REQUEST;
				throw "Error while request parsing";
			}
		}
		vec2[0].erase(std::remove_if(vec2[0].begin(), vec2[0].end(), isSpace), vec2[0].end());
		_headers[vec2[0]] = vec2[1];
		if (vec2[0].find(" ") != std::string::npos) {
			_error = BAD_REQUEST;
			throw "Error while request parsing";
		}
		if (vec2[0] == "Connection" && vec2[1] == "keep-alive")
			_keepAlive = true;
	}
};

void Request::parseFirstLine(std::vector<std::string> vec)
{
	if (vec.size() != 3){
		_error = BAD_REQUEST;
		throw "Error while request parsing";
	}

	if (vec[2] != HTTP_VERSION) {
		_error = HTTP_VERSION_NOT_SUPPORTED;
		throw "Error while request parsing";
	}

	_requestMethod = vec[0];
	if (_requestMethod != "POST" && _requestMethod != "DELETE" && _requestMethod != "GET"){
		_error = METHOD_NOT_ALLOWED;
		throw "Error while request parsing";
	}

	size_t found = 0;
	if ((found = vec[1].find("?")) != std::string::npos) {
		_requestTarget = vec[1].substr(0, found);
		_requestQuery = vec[1].substr(found + 1);
		if (*(_requestTarget.begin()) != '/') {
			_error = BAD_REQUEST;
			throw "Error while request parsing";
		}
		return ; 
	}
	_requestTarget = vec[1];

}

void	Request::preBody( void ) {
	if (_headers.find("Host") == _headers.end() || _headers["Host"].empty() ) {
		_error = BAD_REQUEST;
		throw "Error while request parsing";
	}
	
}

void	Request::parseBody(std::string &req) {
	/** WARNING PRE BODY
	 * Handling Errors before moving to body
	*/
	if (req.empty() == true){
		_requestEnd = true;
		return ;
	}
	if (_bodyName.empty()) {
		_bodyName = _bodyToFile();
		_bodyFile.open(_bodyName.c_str());
		if (_bodyFile.good() == false)
			throw "Error while opening file stream [body]";
	}
	if (_headers.find("Content-Length") != _headers.end()) {
		_contentLength = convertsizeT(_headers.find("Content-Length")->second);
		_bodySize += req.size();
		_bodyFile << req;
		if (_bodySize == _contentLength) {
			_requestEnd = true;
			return ;
		}
		else if (_contentLength < _bodySize) {
			_error = BAD_REQUEST;
			throw "Error on request body";
		}
	} 
	else if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked") {
		toChuncked(req);
	}
	else
		_requestEnd = true;
	_str = "";
}

void 		Request::toChuncked(std::string &req) {
	if (req.empty() || _contentLength == 0)
		return ;
	size_t end = 0;
	/** WARNING
	 *  TRAIT CHUNKED AND RECHECK
	*/
	# define CHUNK_SIZE 0
	# define CHUNK_BODY 1
	int status = CHUNK_SIZE;
	size_t size = 0;
	while ((end = req.find("\r\n")) != std::string::npos) {
		/** DECODE CHUNKED
		 * https://en.wikipedia.org/wiki/Chunked_transfer_encoding
		*/
		if (status == CHUNK_SIZE) {
			std::string hex = req.substr(0, end);
			size = to_hex(hex);
			req.erase(0, end + 2);
			status = CHUNK_BODY;
		} 
		else if (status == CHUNK_BODY) {
			if (size == 0) {
				_requestEnd =true;
				return ;
				/** WARNING 
				 * HANDLER
				*/
			}
			std::string line =  req.substr(0, end);
			if (line.length() != size) {
				_error = BAD_REQUEST;
				throw "Error on body parsing";
			}
			req.erase(0, end + 2);
			size = 0;
			status = CHUNK_SIZE;
		}
	}
}


void		Request::printData( void ) {
	std::cout << "Method :: [" << _requestMethod << "]\n"
			<< "Target :: [" << _requestTarget << "]\n"
			<< "Query :: [" << _requestQuery << "]\n"
			<< "Host :: [" << _headers["Host"] << "]\n"
			<< "Has Body :: [" << _hasBody << "]\n"
			<< "keep-alve :: [" << _keepAlive << "]\n"
			<< "Content-Length :: [" << _contentLength << "]\n" 
			<< "Body file :: [" << _bodyName << "]\n"
			<< "Error :: [" << _error << "]\n";
	std::cout << "=================================\n";
	for (t_headers::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std:: cout << "[" << (it)->first << "] :: [" << (it)->second << "]\n";
	}
}

std::string	Request::_bodyToFile() {
	srand (time(NULL));
	int i = (rand() % 5000);
	std::stringstream iss;

	iss << "/tmp/websev_body_" << i ;
	return iss.str();
}


void		Request::clear( void ) {
	_requestMethod.clear();
	_requestQuery.clear();
	_requestTarget.clear();
	_str.clear();
	_headers.clear();
	if (_bodyName.empty() == false) {
		std::remove(_bodyName.c_str());
	}
}

int Request::getError( void ) const
{
	return _error;
}

std::string Request::getRequestMethod() const
{
	return _requestMethod;
}

std::string Request::getRequestTarget() const
{
	return _requestTarget;
}

std::string Request::getRequestQuery() const
{
	return _requestQuery;
}

std::string Request::getHost( void ) const
{
	return _host;
}

t_headers Request::getHeaders( void ) const
{
	return _headers;
}

std::string Request::getBodyName( void ) const
{
	return _bodyName;
}

bool    Request::getKeepAlive( void ) const
{
	return _keepAlive;
}

size_t Request::getContentLength( void ) const
{
	return _contentLength;
}