#include "../includes/Response.hpp"

Response::Response(Request	request):
	// _request(request),
	_fd(-1)
	// _path(0)
	// _response(0)
	// _body(0)
{
	_request = request;
}

Response::Response(const Response& other)
{
	_request = other._request;
	_fd = other._fd;
}

Response& Response::operator=(const Response& other)
{
	_request = other._request;
	_fd = other._fd;
	return *this;
}

Response::~Response(){}

std::string Response::get_respone( void ) const
{
	return _response;
}

std::iostream* Response::get_body( void ) const
{
	return _body;
}

/*
	*To do:
		- Accurate response status codes {90%}
		- GET POST DELETE {90%}
*/

void Response::unallowedMethod()
{
	time_t rawtime;

	time(&rawtime);
	_response = "HTTP/1.1 405 Method Not Allowed\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Content-Length: 0\r\n";
	_response += "Connection: close\r\n\r\n";
}

void Response::badRequest()
{
	time_t rawtime;

	time(&rawtime);
	_response = "HTTP/1.1 400 Bad Request\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Content-Length: 0\r\n";
	_response += "Connection: close\r\n\r\n";
}

void Response::notFound()
{
	time_t rawtime;

	time(&rawtime);
	_response = "HTTP/1.1 404 Not Found\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Content-Length: 0\r\n";
	_response += "Connection: close\r\n\r\n";
}

void Response::httpVersionNotSupported(std::string const &version)
{
	time_t rawtime;

	time(&rawtime);
	_response = version + " 505 HTTP Version Not Supported\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Content-Length: 0\r\n";
	_response += "Connection: close\r\n\r\n";
}

void Response::setHeader(size_t status_code, std::string const &message)
{
	time_t rawtime;
	std::stringstream ss, ss_content;	

	ss << status_code;
	time(&rawtime);
	_response += "HTTP/1.1 " + ss.str() + " " + message + "\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	ss_content << _request.getContentLength();
	_response += "Content-Length: " + ss_content.str() + "\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Connection: close\r\n\r\n";
}

std::string *errorPage(std::string const &message)
{
	std::string *error_body = new std::string();

	*error_body += std::string("<html>\r\n<head>\r\n");
	*error_body += std::string("<title>") + message;
	*error_body += std::string("</title>\r\n</head>\r\n<body>\r\n<center>\r\n<h1>") + message;
	*error_body += std::string("</h1>\r\n</center>\r\n<hr>\r\n<center>webserver</center>\r\n</body>\r\n</html>\r\n");
	return error_body;
}

void Response::internalError()
{
	time_t rawtime;
	std::string *tmp_res;
	std::stringstream ss;

	time(&rawtime);
	tmp_res = errorPage("500 Internal Server Error");
	ss << tmp_res->length();
	_response += "HTTP/1.1 500 Internal Server Error\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\nServer: webserver\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Content-Length: " + ss.str() + "\r\n";
	_response += "Connection: close\r\n\r\n";
	_response += *tmp_res;
	delete tmp_res;
}

void Response::time_out()
{
	time_t rawtime;
	std::string *tmp_res;
	std::stringstream ss;

	time(&rawtime);
	tmp_res = errorPage("504 Gateway Time-out");
	ss << tmp_res->length();
	_response += "HTTP/1.1 504 Gateway Time-out\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\nServer: webserver\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Content-Length: " + ss.str() + "\r\n";
	_response += "Connection: close\r\n\r\n";
	_response += *tmp_res;
	delete tmp_res;
}

void Response::ok(std::string const &path)
{
	std::string line;
	std::string *tmp_resp = new std::string();
	fd_set tmp_set;

	if (_fd == -1)
		_fd = open(path.c_str(), O_RDONLY);
	FD_SET(_fd, &_set);
	tmp_set = _set;
	if (select(_fd + 1, &tmp_set, NULL, NULL, NULL) < 0)
	{
		_response.clear();
		internalError();
		return;
	}
	if (FD_ISSET(_fd, &tmp_set))
	{
		char buff[1024];
		int ret = read(_fd, buff, 1024);
		*tmp_resp += std::string(buff, ret);
	}
	else
	{
		_response.clear();
		internalError();
		return;
	}
	if (_response.empty())
		setHeader(200, "ok");
	_response += *tmp_resp;
	delete tmp_resp;
}

// Location_block Response::getLocation(Server_block server)
// {
// 	Location_block location;

// 	for(Location_block it : server.all_locations)
// 	{
// 		std::string target = _request.getRequestTarget();
// 		_path.clear();
// 		while (target.find_last_of("/") != std::string::npos)
// 		{
// 			std::cout << "compare " << it.path << " with " << target << std::endl;
// 			if (it.path == target)
// 				return it;
// 			_path = target.substr(target.find_last_of("/"), target.size()) + _path;
// 			target = target.substr(0, target.find_last_of("/"));
// 		}
// 	}
// 	std::cout <<  "no matchind founded" << std::endl;
// }



Location_block Response::getLocation(Server_block server)
{
    // if they are return function there
    // std::vector<std::string> splited_path = split(path, "/");
	std::string path = _request.getRequestTarget();
	std::cout << "path is " << path << std::endl;
    Location_block l_block;
    int i = 0;
    int count = 0;
    while (i < path.size()){
        if (path[i] == '/')
            count++;
        i++;
    }
    
    std::cout << "ENTER TO FUNCTION size is " << count << std::endl;
    for (int r = 0; r < count; r++){
        for (int i = 0; i < server.all_locations.size(); i++){
            l_block = server.all_locations[i];
            std::cout << "************************************************************************\n";
            std::cout << l_block.path << std::endl;
            std::cout << path << std::endl;
            if (l_block.path == path){ // gennerate file to uplade
                std::cout << "-----------------------------------------------------------\n";
                std::cout << "path is " << path << " compare with " << l_block.path << std::endl;
                std::cout << "-----------------------------------------------------------\n";
                if (l_block.return_path.size()){ //send responce
                    std::cout << "--------------------------------Return function---------------\n";
                    std::cout << "Return code is " << l_block.return_code << "  to path" << l_block.return_path << std::endl;
                    std::cout << "--------------------------------Return function---------------\n";
                }
				return l_block;
            }
         }
        while(path.size() && path[path.size() - 1] != '/') {
            path.pop_back();
        }
        if (path.size() && path[path.size() - 1] == '/')
            path.pop_back();
    }
	return l_block;

}



std::string Response::auto_index()
{
	DIR *dir; struct dirent *diread;
    std::vector<std::string> files;

	if ((dir = opendir(_path.c_str())) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            files.push_back(diread->d_name);
        }
        closedir (dir);
    } else {
		notFound();
    }
	std::string body;

	body = std::string("<html>\r\n<head>\r\n");
	body += std::string("<title>Index of ") + _path;
	body += std::string("</title>\r\n</head>\r\n<body>\r\n<h1>Index of ") + _path;
	body += std::string("</h1>\r\n<hr>");
	for (auto file : files) 
		body += std::string("<a>") + file + std::string("</a>r\n");
	body += std::string("\r\n</body>\r\n</html>\r\n");
	return body;
}

void Response::handleRequest(Server_block server) {
	std::cout << "start handling req" << std::endl;
	Location_block location = getLocation(server);
	std::cout << "check for location " << location.path << std::endl;
	_path = server.root + _path;
	struct stat s;

	stat(_path.c_str(), &s);
	if(s.st_mode & S_IFDIR)
	{
		std::fstream * file = new std::fstream();
		if (location.auto_index == "on")
			_response = auto_index();
		else
			_path += "/" + location.index_file;
		file->open(_path.c_str());
		delete _body;
		_body = file;
	}
	else if(s.st_mode & S_IFREG)
	{
		std::cout << "check  for method " << _request.getRequestMethod() << std::endl;
		_is_request_handled = true;
		if (_request.getRequestMethod() == "GET" &&
				std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "GET") != location.allowed_funct.end())
			this->handleGetRequest();
		else if (_request.getRequestMethod() == "POST" &&
				std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "POST") != location.allowed_funct.end())
			this->handlePostRequest();
		else if (_request.getRequestMethod() == "DELETE" &&
				std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "DELETE") != location.allowed_funct.end())
			this->handleDeleteRequest();
		else 
		{
			this->unallowedMethod();
			_is_request_handled = false;
		}
	}
}

void Response::handleGetRequest()
{
	struct stat fileStat;
	time_t rawtime;
	std::fstream * file = new std::fstream();
	
	file->open(_path.c_str());
	delete _body;
	_body = file;
	time(&rawtime);
	stat (_path.c_str(), &fileStat);
	_response += "Date: " + std::string(ctime(&rawtime));
	_response += "\r\nServer: webserver";
	_response += "\r\nLast-Modified: " + time_last_modification(fileStat);
	_response += "\r\nTransfer-Encoding: chunked";
	const char *type = MimeTypes::getType(_path.c_str());
	if (type)
		_response += "\r\nContent-Type: " + std::string(type); 
	_response += "\r\nConnection: keep-alive";
	_response += "\r\nAccept-Ranges: bytes";
}

void Response::handlePostRequest()
{
	struct stat fileStat;
	time_t rawtime;
	std::fstream * file = new std::fstream();
	
	file->open(_path.c_str());
	delete _body;
	_body = file;
	time(&rawtime);
	stat (_path.c_str(), &fileStat);
	_response += "Date: " + std::string(ctime(&rawtime));
	_response += "\r\nServer: webserver";
	_response += "\r\nLast-Modified: " + time_last_modification(fileStat);
	_response += "\r\nTransfer-Encoding: chunked";
	const char *type = MimeTypes::getType(_path.c_str());
	if (type)
		_response += "\r\nContent-Type: " + std::string(type); 
	_response +=  "\r\nConnection: keep-alive";
	_response +=  "\r\nAccept-Ranges: bytes";
}

static void deleteDirectoryFiles(DIR * dir, const std::string & path) {
	struct dirent * entry = NULL;
	struct stat st;
	std::string filepath;
	DIR * dirp;
	
	while ((entry = readdir(dir))) {

		filepath = path + entry->d_name;

		if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		if (stat(filepath.c_str(), &st) == -1) {
			std::cerr << "stat(): " << filepath << ": " << strerror(errno) << std::endl;
		}

		if (S_ISDIR(st.st_mode)) {
			filepath += "/";
			if ((dirp = opendir(filepath.c_str()))) {
				deleteDirectoryFiles(dirp, filepath.c_str());
			} else {
				std::cerr << "opendir(): " << filepath.c_str() << ": " << strerror(errno) << std::endl;
			}
		} else {
			if (remove(filepath.c_str()) == -1) {
				std::cerr << "remove() file: " << filepath.c_str() << ": " << strerror(errno) << std::endl;
			}
		}
	}
	if (remove(path.c_str()) == -1) {
		std::cerr << "remove() dir: " << path.c_str() << ": " << strerror(errno) << std::endl;
	}
}

void Response::handleDeleteRequest()
{
	struct stat st;
	DIR * dirp = NULL;

	errno = 0;
	if (lstat(_request.getBody().c_str(), &st) == -1) {
		if (errno == ENOTDIR) {
			throw StatusCodeException(HttpStatus::conflict);
		} else {
			throw StatusCodeException(HttpStatus::notFound);
		}
	}

	if (S_ISDIR(st.st_mode)) {
		if (_request.getRequestTarget().at(_request.getRequestTarget().length() - 1) != '/') {
			throw StatusCodeException(HttpStatus::conflict);
		} else {
			if ((dirp = opendir(_request.getBody().c_str()))) {
				deleteDirectoryFiles(dirp, _request.getBody());
			}
		}
	} else {
		remove(_request.getBody().c_str());
	}

	if (errno) {
		perror("");
	}
	if (errno == ENOENT || errno == ENOTDIR || errno == ENAMETOOLONG) {
		throw StatusCodeException(HttpStatus::notFound);
    } else if (errno == EACCES || errno == EPERM) {
		throw StatusCodeException(HttpStatus::forbidden);
    } else if (errno == EEXIST) {
		throw StatusCodeException(HttpStatus::methodNotAllowed);
    } else if (errno == ENOSPC) {
		throw StatusCodeException(HttpStatus::insufficientStorage);
    } else if (errno) {
		throw StatusCodeException(HttpStatus::internalServerError);
    } else {
		throw StatusCodeException(HttpStatus::noContent);
	}
}

std::stringstream * errorTemplate(const StatusCodeException & e) {
	std::stringstream * alloc = new std::stringstream("");
	std::stringstream & body = *alloc;

	if (e.getStatusCode() >= 400) {
		body << "<!DOCTYPE html>\n" ;
		body << "<html lang=\"en\">\n";
		body << "<head>\n";
		body << "<title>" << e.getStatusCode() << "</title>\n";
		body << "</head>\n";
		body << "<body>\n";
		body << "<h1 style=\"text-align:center\">" << e.getStatusCode() << " - " << HttpStatus::reasonPhrase(e.getStatusCode()) << "</h1>\n";
		body << "<hr>\n";
		body << "<h4 style=\"text-align:center\">WebServer</h4>\n";
		body << "</body>\n";
	}
	return &body;
}

void Response::setErrorPage(const StatusCodeException & e, const Location_block *location, Server_block *server) {
	_statuscode = e.getStatusCode();
	time_t rawtime;

	time(&rawtime);
	_response += "Connection: keep-alive";
	_response += "\r\nContent-Type: text/html";
	_response += "\r\nDate: " + std::string(ctime(&rawtime));
	_response += "\r\nServer: webserver";
	if (location->path != "")
		_response += "\r\nLocation: " + location->path;
	const std::map<int, std::string> & error_page = server->error_page;

	std::fstream * errPage = NULL;

	if (error_page.find(_statuscode) != error_page.end()) {
		errPage = new std::fstream();
		std::string errPath = error_page.find(_statuscode)->second;
		if (errPath[0] == '/' || (errPath[0] == '.' && errPath[1] == '/')) {
			errPage->open(errPath.c_str());
		} else {
			std::string filename = location->root;

			filename += errPath;
			errPage->open(filename.c_str());
		}
	}

	delete _body;
	if (!errPage || !errPage->is_open()) {
		_body = errorTemplate(e);
		if (errPage) {
			delete errPage;
	}
	} else {
		_body = errPage;
	}
	_response += "\r\nTransfer-Encoding: chunked";
}