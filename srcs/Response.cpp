#include "../includes/Response.hpp"

Response::Response(Request	request):
	// _request(request),
	_fd(-1),
	is_autoindex(0)
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

std::fstream& Response::get_body( void )
{
	return _body;
}

int Response::get_fd( void ) const
{
	return _fd;
}

void Response::close_fd( void ){
	close(_fd);
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

void Response::setHeader(size_t status_code, std::string const &message, size_t bodysize)
{
	time_t rawtime;
	std::stringstream ss, ss_content;	

	ss << status_code;
	time(&rawtime);
	_response += "HTTP/1.1 " + ss.str() + " " + message + "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Content-Type: text/html\r\n";	
	std::cout << "++++++++++++++++++++++FILE : " << _path << std::endl;
	_response += "Content-Length: " + std::to_string(bodysize) + "\r\n\n";
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

void Response::ok(size_t bodysize)
{
	setHeader(200, "ok", bodysize);
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
// 			//std::cout << "compare " << it.path << " with " << target << std::endl;
// 			if (it.path == target)
// 				return it;
// 			_path = target.substr(target.find_last_of("/"), target.size()) + _path;
// 			target = target.substr(0, target.find_last_of("/"));
// 		}
// 	}
// 	//std::cout <<  "no matchind founded" << std::endl;
// }


void Response::create_file()
{
	std::string filepath = "/tmp/autoindex_" + std::to_string(time(NULL));
	std::ofstream out(filepath);
	_filepath = filepath;
	out << _response;
	out.close();
	_fd = open(filepath.c_str(), O_RDONLY);
}

Location_block Response::getLocation(Server_block server)
{
    // if they are return function there
    // std::vector<std::string> splited_path = split(path, "/");
	//_request.printData();
	std::string path = _request.getRequestTarget();
	std::string save = "";
	std::string concate  = "";
    Location_block l_block;
    int i = 0;
    int count = 0;
    while (i < path.size()){
        if (path[i] == '/')
            count++;
        i++;
    }
    std::cout << "number of locations is " <<  server.all_locations.size() << std::endl;
    for (int r = 0; r < count + 1; r++){
        for (int i = 0; i < server.all_locations.size(); i++){
			std::cout << "compare is " << path << " and " << l_block.path << std::endl;
            l_block = server.all_locations[i];
            if (l_block.path == path){ // gennerate file to uplade
				_path = save;
				std::cout << "*****************_path is " << _path << std::endl;
                if (l_block.return_path.size()){ //send responce
                    //std::cout << "--------------------------------Return function---------------\n";
                    //std::cout << "Return code is " << l_block.return_code << "  to path" << l_block.return_path << std::endl;
                    //std::cout << "--------------------------------Return function---------------\n";
                }
				return l_block;
            }
         }
		 concate = "";
		 char c;
        while(path.size() && path[path.size() - 1] != '/') {
			c = path[path.size() - 1];
			concate = c + concate;
            path.pop_back();
        }
        if (path.size() > 1 && path[path.size() - 1] == '/'){
            path.pop_back();
			concate = '/' + concate;
		}
		else if (path.size() && path[path.size() - 1] == '/'){
			concate = '/' + concate;
		}
		save = concate + save;
    }
	return l_block;
}

void Response::auto_index(Location_block location)
{
	is_autoindex = 1;
	DIR *dir; struct dirent *diread;
    std::vector<std::string> files;
	struct stat s;
	stat(_path.c_str(), &s);
	//std::cout << "_path is for aut index" << _path << std::endl;
	if ((dir = opendir(_path.c_str())) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            files.push_back(diread->d_name);
        }
        closedir (dir);
		std::string body;
		body += std::string("<html>\r\n<head>\r\n");
		body += std::string("<title>Index of ") + _path;
		body +=std::string("</title>\r\n</head>\r\n<body>\r\n<h1>Index of ") + _path;
		body += std::string("</h1>\r\n<hr>\r\n<ul>\r\n");
		for(int i=0; i < files.size(); i++){
			std::string to_go  = _request.getRequestTarget();
			if (to_go.size() && to_go[to_go.size() - 1] != '/')
				to_go += '/';
			body += std::string("<a href='" + to_go + files[i] + "'>") + files[i] + std::string("</a>\r\n");
		}
		body += std::string("</ul>\r\n</body>\r\n</html>\r\n");
		this->ok(body.size());
		_response += body;
		//std::cout << _response << std::endl;
		create_file();
		is_autoindex = 0;
		return ;
    } else if ((s.st_mode & S_IFREG)) {
		time_t rawtime;
		time(&rawtime);
		_response += "HTTP/1.1 200 ok\r\n";
		_response += "Server: webserver\r\n";
		_response += "Date: " + std::string(ctime(&rawtime));
		_response.erase(--_response.end());
		_response += "\r\n";
		_response += "Content-Type: text/html\r\n";	
		std::cout << "++++++++++++++++++++++FILE2 : " << _path << std::endl;
		_response += "Content-Disposition: attachement; filename='file'\r\n";
		_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n\n";
		int fd = open(_path.c_str(), O_RDONLY);
		char buff[s.st_size];
		read(fd, buff, s.st_size);
		_response += buff;
		_response += "\r\n\r\n";
		create_file();
    }
	notFound();
}

void Response::handleRequest(Server_block server) {
	// //std::cout << "start handling req" << std::endl;
	Location_block location = getLocation(server);
	_path = server.root + _path;
	std::cout << "PATH 2 IS " << _path << std::endl;
	if (location.return_path.size())
	{
		_body << location.return_path;
	}

	struct stat s;
	stat(_path.c_str(), &s);
	if(s.st_mode & S_IFDIR)
	{
		std::fstream * file = new std::fstream();
		if (location.auto_index == "on")
		{
			auto_index(location);
		}
		else{
			if (_path.size() && _path[_path.size() - 1] != '/')
				_path += "/" + location.index_file;
			else
				_path +=  location.index_file;
			std::cout << "PATH 3 IS " << _path << std::endl;
		}
	}
	stat(_path.c_str(), &s);
	if((s.st_mode & S_IFREG))
	{
		if (location.auto_index == "on")
		{
			auto_index(location);
		}
		else
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
}

void Response::handleGetRequest()
{
	struct stat fileStat;
	time_t rawtime;
	std::cout << "start get request" << std::endl;
	_body.open(_path.c_str());
	// time(&rawtime);
	stat (_path.c_str(), &fileStat);
	// _response += "200 OK\r\n";
	// // _response += "Date: " + std::string(ctime(&rawtime));
	// _response += "Server: webserver";
	// _response += "\r\nLast-Modified: " + time_last_modification(fileStat);
	// // _response += "\r\nTransfer-Encoding: chunked";
	// const char *type = MimeTypes::getType(_path.c_str());
	// if (type)
	// 	_response += "\r\nContent-Type: " + std::string(type); 
	// _response += "\r\nConnection: Closed";
	// // _response += "\r\nAccept-Ranges: bytes";

	// _response += "\r\nContent-Length: " + std::to_string(fileStat.st_size) + "\r\n";
	// _response += buff;
 	// _response += "\r\n\r\n";
	

	int fd = open(_path.c_str(), O_RDONLY);
	char buff[fileStat.st_size];
	read(fd, buff, fileStat.st_size);
	this->ok(fileStat.st_size);
	_response += buff;
	create_file();
}
/*
		_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n\n";
		_response += buff;
		_response += "\r\n\r\n";
		create_file();
*/
void Response::handlePostRequest()
{
	struct stat fileStat;
	time_t rawtime;
	
	_body.open(_path.c_str());
	time(&rawtime);
	stat (_path.c_str(), &fileStat);
	_response += "HTTP/1.1 200 ok\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response += "\r\nServer: webserver";
	_response += "\r\nLast-Modified: " + time_last_modification(fileStat);
	_response += "\r\nTransfer-Encoding: chunked";
	const char *type = MimeTypes::getType(_path.c_str());
	if (type)
		_response += "\r\nContent-Type: " + std::string(type); 
	_response +=  "\r\nConnection: keep-alive";
	_response +=  "\r\nAccept-Ranges: bytes";
	create_file();
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

void Response::errorTemplate(const StatusCodeException & e) {

	if (e.getStatusCode() >= 400) {
		_body << "<!DOCTYPE html>\n" ;
		_body << "<html lang=\"en\">\n";
		_body << "<head>\n";
		_body << "<title>" << e.getStatusCode() << "</title>\n";
		_body << "</head>\n";
		_body << "<body>\n";
		_body << "<h1 style=\"text-align:center\">" << e.getStatusCode() << " - " << HttpStatus::reasonPhrase(e.getStatusCode()) << "</h1>\n";
		_body << "<hr>\n";
		_body << "<h4 style=\"text-align:center\">WebServer</h4>\n";
		_body << "</body>\n";
	}
}

// void Response::setErrorPage(const StatusCodeException & e, const Location_block *location, Server_block *server) {
// 	_statuscode = e.getStatusCode();
// 	time_t rawtime;

// 	time(&rawtime);
// 	_response += "Connection: keep-alive";
// 	_response += "\r\nContent-Type: text/html";
// 	_response += "\r\nDate: " + std::string(ctime(&rawtime));
// 	_response += "\r\nServer: webserver";
// 	if (location->path != "")
// 		_response += "\r\nLocation: " + location->path;
// 	const std::map<int, std::string> & error_page = server->error_page;

// 	std::fstream errPage;

// 	if (error_page.find(_statuscode) != error_page.end()) {
// 		std::string errPath = error_page.find(_statuscode)->second;
// 		if (errPath[0] == '/' || (errPath[0] == '.' && errPath[1] == '/')) {
// 			errPage.open(errPath.c_str());
// 		} else {
// 			std::string filename = location->root;

// 			filename += errPath;
// 			errPage.open(filename.c_str());
// 		}
// 	}

// 	if (!errPage || !errPage.is_open()) {
// 		_body = errorTemplate(e);
// 	_response += "\r\nTransfer-Encoding: chunked";
// 	}
// }