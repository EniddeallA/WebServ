#include "../includes/Response.hpp"

Response::Response(Request	request):
	// _request(request),
	_fd(-1),
	is_autoindex(0),
	_size_sended(0)
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

void Response::reset()
{
	_request.clear();
	_response.clear();
	_path.clear();
	_fd = -1;
	is_autoindex = 0;
	_is_request_handled = false;
	_statuscode = HttpStatus::statusCode(0);
	_size_of_file = 0;
	_size_sended = 0;
}

std::string Response::get_respone( void ) const
{
	return _response;
}

std::fstream& Response::get_body( void )
{
	return _body;
}

int Response::get_fd( void )
{
	struct stat s;
	stat(_filepath.c_str(), &s);
	_size_of_file = (long)(s.st_size);
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

void Response::set_error_header(int statuscode, std::string msg, std::string path)
{
	time_t rawtime;
	struct stat s;

	time(&rawtime);
	_response = "HTTP/1.1 " + std::to_string(statuscode) + msg + "\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	stat(path.c_str(), &s);
	int fd = open(path.c_str(), O_RDONLY);
	char buff[s.st_size];
	int r_d = read(fd, buff, s.st_size);
	close(fd);
	_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n";
	_response += "Connection: close\r\n\r\n";
	_response += std::string(buff, r_d);
	_response += "\r\n\r\n";
}

void Response::set_redirection(int statuscode, std::string path)
{
	time_t rawtime;
	struct stat s;

	time(&rawtime);
	std::string msg = HttpStatus::reasonPhrase(HttpStatus::statusCode(statuscode));
	_response = "HTTP/1.1 " + std::to_string(statuscode) + msg + "\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Location: "+ path +"\r\n";
	stat(path.c_str(), &s);
	int fd = open(path.c_str(), O_RDONLY);
	char buff[s.st_size];
	int reading = read(fd, buff, s.st_size);
	close(fd);
	_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n";
	_response += "Connection: close\r\n\r\n";
	_response +=  std::string(buff, reading);
	_response += "\r\n\r\n";
}

std::string errorPage(std::string const &message)
{
	std::string error_body;

	error_body += std::string("<html>\r\n<head>\r\n");
	error_body += std::string("<title>") + message;
	error_body += std::string("</title>\r\n</head>\r\n<body>\r\n<center>\r\n<h1>") + message;
	error_body += std::string("</h1>\r\n</center>\r\n<hr>\r\n<center>webserver</center>\r\n</body>\r\n</html>\r\n");
	return error_body;
}

void Response::unallowedMethod()
{
	set_error_header(405, "Method Not Allowed", "./error_pages/405.html");
}

void Response::forbidden()
{
	set_error_header(403, "Forbidden", "./error_pages/403.html");
}

void Response::badRequest()
{
	set_error_header(400, "Bad Request", "./error_pages/400.html");
}

void Response::notFound()
{
	set_error_header(404, "Not Found", "./error_pages/404.html");
}

void Response::httpVersionNotSupported(std::string const &version)
{
	set_error_header(505, "HTTP Version Not Supported", "./error_pages/505.html");
}


void Response::internalError()
{
	set_error_header(500, "Internal Server Error", "./error_pages/500.html");
}

void Response::payloadTooLarge()
{
	set_error_header(413, "Payload Too Large", "./error_pages/413.html");
}

void Response::time_out()
{
	set_error_header(504, "Gateway Time-out", "./error_pages/504.html");
}

void Response::ok(size_t bodysize)
{
		time_t rawtime;

		time(&rawtime);
		_response += "HTTP/1.1 200 ok\r\n";
		_response += "Server: webserver\r\n"; 
		struct stat s;
		stat(_path.c_str(), &s);
		const char *check_type = MimeTypes::getType(_path.c_str());
		if (((s.st_mode & S_IFREG)) && check_type == NULL){
			std::string path_name = _path.substr(_path.find_last_of('/') + 1, _path.size());
			_response += "Content-Disposition: attachement; filename=" + path_name + "\r\n";
		}
		_response += "Date: " + std::string(ctime(&rawtime));
		_response.erase(--_response.end());
		_response += "\r\n";
		if (check_type)
			_response += "Content-Type: " + std::string(check_type) + "\r\n";	
		_response += "Content-Length: " + std::to_string(bodysize) + "\r\n\n";
}

std::string Response::get_file_path(){
	return _filepath;
}

int Response::check_max_body_size()
{
	long long locationsize = stod(_location.max_body_size) * 1000000;
	std::cout << locationsize << std::endl;
	if (_response.size() <= locationsize)
	{
		std::cout << _response.size() << " " << locationsize << std::endl;
		return (1);
	}
	return (0);
}

void Response::create_file()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int us = tp.tv_sec * 1000000 + tp.tv_usec;
	std::string file_name =   std::to_string(us);
	std::string filepath = "/tmp/autoindex_" + file_name;
	std::ofstream out(filepath);
	_filepath = filepath;
	// if (!check_max_body_size())
	// {
	// 	this->payloadTooLarge();
	// 	std::cout << _response << std::endl;
	// }
	out << _response;
	out.close();
	_fd = open(filepath.c_str(), O_RDONLY);
	fcntl(_fd, F_SETFL, O_NONBLOCK); // LEARN MORE ABOUT fcntl
	_size_sended = 0;
}

Location_block Response::getLocation(Server_block &server)
{
    // if they are return function there
    // std::vector<std::string> splited_path = split(path, "/");
	//_request.printData();
	_file_not_found = 0;
	std::string path = _request.getRequestTarget();
	std::string save = "";
	std::string concate  = "";
    // Location_block l_block;
    int i = 0;
    int count = 0;
    while (i < path.size()){
        if (path[i] == '/')
            count++;
        i++;
    }
    for (int r = 0; r < count + 1; r++){
        for (int i = 0; i < server.all_locations.size(); i++){
            if (server.all_locations[i].path == path){ // gennerate file to uplade
				_path = save;
                if (server.all_locations[i].return_path.size()){ //send responce
                    //std::cout << "--------------------------------Return function---------------\n";
                    //std::cout << "Return code is " << l_block.return_code << "  to path" << l_block.return_path << std::endl;
                    //std::cout << "--------------------------------Return function---------------\n";
                }
				return server.all_locations[i];
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
	_file_not_found = 1;
	Location_block l_block;
	return l_block;
}

void Response::auto_index(Location_block location)
{

	is_autoindex = 1;
	DIR *dir; struct dirent *diread;
    std::vector<std::string> files;
	struct stat s;
	stat(_path.c_str(), &s);
	if ((dir = opendir(_path.c_str())) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            files.push_back(diread->d_name);
        }
        closedir (dir);
		std::string body;
		std::string path_name = _path.substr(_path.find_last_of('/'), _path.size());
		body += std::string("<html>\r\n<head>\r\n");
		body += std::string("<title>Index of ") + path_name;
		body +=std::string("</title>\r\n</head>\r\n<body>\r\n<h1>Index of ") + path_name;
		body += std::string("</h1>\r\n<hr>\r\n<ul>\r\n");
		for(int i=0; i < files.size(); i++){
			std::string to_go  = _request.getRequestTarget();
			if (to_go.size() && to_go[to_go.size() - 1] != '/')
				to_go += '/';
			body += std::string("<a href='" + to_go + files[i] + "'>") + files[i] + std::string("</a></br>\r\n");
		}
		body += std::string("</ul>\r\n</body>\r\n</html>\r\n");

		this->ok(body.size());
		_response += body;
		create_file();
		is_autoindex = 0;
    } else if ((s.st_mode & S_IFREG)) {
		time_t rawtime;
		time(&rawtime);
		_response += "HTTP/1.1 200 ok\r\n";
		_response += "Server: webserver\r\n";
		_response += "Date: " + std::string(ctime(&rawtime));
		_response.erase(--_response.end());
		_response += "\r\n";
		_response += "Content-Type: text/html\r\n";	
		_response += "Content-Disposition: attachement; filename='file'\r\n";
		_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n\n";
		int fd = open(_path.c_str(), O_RDONLY | O_NONBLOCK);
		fcntl(fd, F_SETFL, O_NONBLOCK);
		char buff[s.st_size];
		read(fd, buff, s.st_size);
		_response += buff;
		_response += "\r\n\r\n";
		close(fd);
		create_file();
    }
	else
		notFound();
}

void Response::handleRequest(Server_block server) {
	Location_block location = getLocation(server);
	_location = location;
	_path = location.root + _path;
	if (location.return_path != "" && location.return_path.size())
	{
		int statuscode = std::stoi(location.return_code);
		set_redirection(statuscode, location.return_path);
		create_file();
		return;
	}
	if (_file_not_found){
		
		notFound();
		create_file();
		return;
	}
	struct stat s, s2;
	stat(_path.c_str(), &s);
	if(s.st_mode & S_IFDIR)
	{

		std::fstream * file = new std::fstream();

		//? check if index file exist return index_file else if auto index exit  return it else 404
		std::string index_file = _path + "/" + location.index_file;;

		stat(index_file.c_str(), &s2);
		//! check if method is get and is allowed the run the auto_index
		if (location.auto_index == "on" && (location.index_file == ""  || (s2.st_mode & S_IFREG) == 0) && _request.getRequestMethod() == "GET" &&
					std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "GET") != location.allowed_funct.end()){
			is_autoindex = 1;
			auto_index(location);
			delete file;
			return ;
		}
		else if (_request.getRequestMethod() != "DELETE"){
			if (_path.size() && _path[_path.size() - 1] != '/')
				_path += "/" + location.index_file;
			else
				_path +=  location.index_file;
		}
		delete file;
	}
	stat(_path.c_str(), &s);
	if((s.st_mode & S_IFREG) &&  _request.getRequestMethod() != "POST")
	{
			_is_request_handled = true;
			if (_request.getRequestMethod() == "GET" &&
					std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "GET") != location.allowed_funct.end()){

					this->handleGetRequest();
				
					}

			// else if (_request.getRequestMethod() == "POST" &&
			// 		std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "POST") != location.allowed_funct.end())
			// 	this->handlePostRequest();
			else if (_request.getRequestMethod() == "DELETE" &&
					std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "DELETE") != location.allowed_funct.end()){
				std::cout << "I'm here to delete\n\n";
				this->handleDeleteRequest();
			}
			else 
			{
				unallowedMethod();
				_body.close();
				create_file();
				_is_request_handled = false;
				return;
			}
	}
	else if ( _request.getRequestMethod() == "POST"){
		if (std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "POST") != location.allowed_funct.end())
			this->handlePostRequest(server, location);
	}
	else if ( _request.getRequestMethod() == "DELETE"){
		if (std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "DELETE") != location.allowed_funct.end())
			this->handleDeleteRequest();
	}
	else{		
		struct stat fileStat;
		_body.open("./error_pages/404.html");
		stat ("./error_pages/404.html", &fileStat);

		int fd = open("./error_pages/404.html", O_RDONLY);
		fcntl(fd, F_SETFL, O_NONBLOCK);
		char buff[fileStat.st_size];
		int rd = read(fd, buff, fileStat.st_size);
		notFound();
		_response += std::string(buff, rd);
		close(fd);
		_body.close();
		create_file();
		return;
	}

}

void Response::handleGetRequest()
{
	struct stat fileStat;
	time_t rawtime;
	stat (_path.c_str(), &fileStat);
	int fd = open(_path.c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	// char buff[fileStat.st_size];
	char buff[1001] = {0};
	this->ok(fileStat.st_size);
	int reading = 0;
	while((reading = read(fd, buff, 1000))){
		_response.append(buff, reading);
		bzero(buff, 1000);
	}
	close(fd);
	create_file();				
}


//? check if there is uplode_store if not 
	
void Response::handlePostRequest(Server_block &server, Location_block &location){
	if (location.upload_store == ""){ //? return forbiden function
		if (1 == 11){ //? check for cgi  if there is and support extention		{

		}
		else{ //? if th
			 //! check for error page in locaction , check with creeper
			time_t rawtime;
			time(&rawtime);
			_response += "HTTP/1.1 409 Conflict\r\n";
			_response += "Date: " + std::string(ctime(&rawtime));
			_response += "\r\nServer: webserver";
			_response += "\r\nContent-Length: 0";
			_response +=  "\r\nConnection: close";
			_response +=  "\r\nAccept-Ranges: bytes\r\n\r\n";
			create_file();
			return;
		}
	}

	int index =_request.getRequestTarget().find(location.path);
	std::string name_to_save ;
	if (!location.upload_store.empty()){
		std::string ext;
		std::string file_name;
		std::string file_path;

		ext = _request.getContentType();
		if (index == -1){
			time_t t;
			t = time(NULL);
			file_path = location.root + location.upload_store + "/" + std::to_string(t);
		}
		else{
			name_to_save = _request.getRequestTarget().substr(index + location.path.size(), _request.getRequestTarget().size());
			file_path = location.root + location.upload_store  + name_to_save;
		}
		// int ret = std::rename(_request.getBody().c_str(), file_path.c_str());
		std::string mv = "mv " + _request.getBody() + " " + file_path;
		int ret = system(mv.c_str());
		time_t rawtime;
		time(&rawtime);
		if (ret != -1)
			_response += "HTTP/1.1 201 created\r\n"; 
		else //! check for error page in locaction , check with creeper
			_response += "HTTP/1.1 409 Conflict\r\n";
		_response += "Date: " + std::string(ctime(&rawtime));
		_response += "\r\nServer: webserver";
		_response += "\r\nContent-Length: 0";
		_response +=  "\r\nConnection: close";
		_response +=  "\r\nAccept-Ranges: bytes\r\n\r\n";
		create_file();
	}
}

static void deleteDirectoryFiles(DIR * dir, const std::string & path){
	struct dirent * entry = NULL;
	struct stat st;
	std::string filepath;
	DIR * dirp;
	while ((entry = readdir(dir))) {
		filepath = path + entry->d_name;
		if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
			continue;
		if (stat(filepath.c_str(), &st) == -1)
			std::cerr << "stat(): " << filepath << ": " << strerror(errno) << std::endl;
		if (S_ISDIR(st.st_mode)) {
			filepath += "/";
			if ((dirp = opendir(filepath.c_str())))
				deleteDirectoryFiles(dirp, filepath.c_str());
			else
				std::cerr << "opendir(): " << filepath.c_str() << ": " << strerror(errno) << std::endl;
		} 
		else {
			if (remove(filepath.c_str()) == -1)
				std::cerr << "remove() file: " << filepath.c_str() << ": " << strerror(errno) << std::endl;
		}
	}
	if (remove(path.c_str()) == -1)
		std::cerr << "remove() dir: " << path.c_str() << ": " << strerror(errno) << std::endl;
}

void Response::handleDeleteRequest()
{
	struct stat st;
	DIR * dirp = NULL;

	errno = 0;
	std::cout << "4\n";
	if (lstat(_path.c_str(), &st) == -1)
		this->notFound();
	if (st.st_mode & S_IFDIR) {
		_path += (_path.back() != '/') ? "/" : "";
		if ((dirp = opendir(_path.c_str())))
			deleteDirectoryFiles(dirp, _path);
	}
	else if (st.st_mode & S_IFREG)
		unlink(_path.c_str());
	if (errno == ENOTDIR)
	{
		std::cout << "3\n";
		this->notFound();
	}
    else if (errno == EACCES)
		{
			std::cout << "2\n";
			this->forbidden();}
    else if (errno == EEXIST){
		std::cout << "1\n";
		this->unallowedMethod();}
    else
		this->ok(0);
	create_file();
}