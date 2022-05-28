#include "../../includes/Response.hpp"

Response::Response(Request	request):
	_fd(-1),
	is_autoindex(0),
	_size_sended(0){
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

void Response::set_error_header(int statuscode, std::string msg, std::string path)
{
	time_t rawtime;
	struct stat s;
	stat(path.c_str(), &s);
	time(&rawtime);
	_response = "HTTP/1.1 " + std::to_string(statuscode) + " " + msg + "\r\n";
	_response += "Date: " + std::string(ctime(&rawtime));
	_response.erase(--_response.end());
	_response += "\r\n";
	_response += "Server: webserver\r\n";
	_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n";
	_response += "Connection: close\r\n\r\n";
	int fd = open(path.c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	char buff[s.st_size];
	int r_d = read(fd, buff, s.st_size);	
	if (r_d >= 0)
		_response += std::string(buff, r_d);
	close(fd);
	_response += "\r\n\r\n";
	if (r_d == -1)
		return;
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
	_response += "Content-Length: " + std::to_string(s.st_size) + "\r\n";
	_response += "Connection: close\r\n\r\n";
	path = _location.root + path;
	stat(path.c_str(), &s);
	int fd = open(path.c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (fd == -1)
	{
		close(fd);
		this->notFound();
		return;
	}
	char buff[s.st_size];
	int reading = read(fd, buff, s.st_size);
	close(fd);
	if (reading >= 0)
		_response +=  std::string(buff, reading);
	_response += "\r\n\r\n";
	if (reading == -1)
		this->notFound();
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
	int fd = open(_server.error_page[405].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(405) && fd != -1)
		set_error_header(405, "Method Not Allowed", _server.error_page[405]);
	else
		set_error_header(405, "Method Not Allowed", "./error_pages/405.html");
}

void Response::forbidden()
{
	int fd = open(_server.error_page[403].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(403) && fd != -1)
		set_error_header(403, "Forbidden", _server.error_page[403]);
	else
		set_error_header(403, "Forbidden", "./error_pages/403.html");
}

void Response::badRequest()
{
	int fd = open(_server.error_page[400].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(400) && fd != -1)
		set_error_header(400, "Bad Request", _server.error_page[405]);
	else
		set_error_header(400, "Bad Request", "./error_pages/400.html");
}

void Response::notFound()
{
	int fd = open(_server.error_page[404].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(404) && fd != -1)
		set_error_header(404, "Not Found", _server.error_page[404]);
	else
		set_error_header(404, "Not Found", "./error_pages/404.html");
}

void Response::httpVersionNotSupported()
{
	int fd = open(_server.error_page[505].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(505) && fd != -1)
		set_error_header(505, "HTTP Version Not Supported", _server.error_page[505]);
	else
		set_error_header(505, "HTTP Version Not Supported", "./error_pages/505.html");
}


void Response::internalError()
{
	int fd = open(_server.error_page[500].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(500) && fd != -1)
		set_error_header(500, "Internal Server Error", _server.error_page[500]);
	else
		set_error_header(500, "Internal Server Error", "./error_pages/500.html");
}

void Response::payloadTooLarge()
{
	int fd = open(_server.error_page[413].c_str(), O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(413) && fd != -1)
		set_error_header(413, "Payload Too Large", _server.error_page[413]);
	else
		set_error_header(413, "Payload Too Large", "./error_pages/413.html");
}

void Response::time_out()
{
	int fd = -1;
	if (_server.error_page.count(504))
		fd = open(_server.error_page[504].c_str(), O_RDONLY);
	if (fd != -1)
		fcntl(fd, F_SETFL, O_NONBLOCK);
	close(fd);
	if (_server.error_page.count(504) && fd != -1){

		set_error_header(504, "Gateway Time-out", _server.error_page[504]);
	}
	else{

		set_error_header(504, "Gateway Time-out", "./error_pages/504.html");
	}
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
	struct stat s;
	stat(_request.getBody().c_str(), &s);
	if (s.st_size <= locationsize)
		return (1);
	return (0);
}

//? we use current time to criet file
void Response::create_file()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int us = tp.tv_sec * 1000000 + tp.tv_usec;
	std::string file_name =   std::to_string(us);
	std::string filepath = "/tmp/autoindex_" + file_name;
	std::ofstream out(filepath);
	_filepath = filepath;
	out << _response;
	out.close();
	_fd = open(filepath.c_str(), O_RDONLY);
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	_size_sended = 0;
}

//? get what location user is targitting from server
Location_block Response::getLocation(Server_block &server)
{
	_file_not_found = 0;
	std::string path = _request.getRequestTarget();
	std::string save = "";
	std::string concate  = "";
    unsigned long i = 0;
    int count = 0;
    while (i < path.size()){
        if (path[i] == '/')
            count++;
        i++;
    }
    for (int r = 0; r < count + 1; r++){
        for (unsigned long i = 0; i < server.all_locations.size(); i++){
            if (server.all_locations[i].path == path){
				_path = save;
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

void Response::auto_index()
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
		for(unsigned long i=0; i < files.size(); i++){
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
		int reading = read(fd, buff, s.st_size);
		if (reading >= 0)
			_response += buff;
		close(fd);
		_response += "\r\n\r\n";
		if (reading == -1)
			notFound();
		create_file();
    }
	else
		notFound();
}

//? check if the file that the user whant is supported by the cgi
bool file_is_suported_from_cgi(Location_block location, std::string path){
    std::string ext = "";
    int i =  path.size();
    i--;
    while (i >= 0){
        if (path[i] == '.')     
            break;
        ext = path[i] + ext;
        i--;
    }
	for (unsigned long i = 0; i < location.cgi_ext.size() ; i++){
		if (ext == location.cgi_ext[i])
			return true;
	}
    return false;
    
}

//? set all variables neded by cgi 
std::vector<std::string> make_meta_variables_for_cgi(Request request, Server_block server, std::string script){
	std::vector<std::string> vec_of_string;
	std::string meta_variables = "";
	std::string str;

	str = "CONTENT_LENGTH=" + std::to_string(request.getContentLength());
	vec_of_string.push_back(str);
	std::string path = "localhost:" + server.port + request.getRequestTarget() ;
	str = "CONTENT_TYPE=" + request.getContentType();
	vec_of_string.push_back(str);
	str = "GATEWAY_INTERFACE=CGI/1.1";
	vec_of_string.push_back(str);
	str = "PATH_INFO=/";
	vec_of_string.push_back(str);
	str = "PATH_TRANSLATED=/";
	vec_of_string.push_back(str);
	str = "QUERY_STRING=" + request.getRequestQuery();
	vec_of_string.push_back(str);
	str = "REQUEST_METHOD=" + request.getRequestMethod();
	vec_of_string.push_back(str);
	str = "SCRIPT_NAME=localhost:" + server.port;
	vec_of_string.push_back(str);
	vec_of_string.push_back("SCRIPT_FILENAME=" + script);
	str = "SERVER_NAME=" + request.getHost();
	vec_of_string.push_back(str);
	str = "SERVER_PROTOCOL=HTTP/1.1";
	vec_of_string.push_back(str);
	str = "SERVER_SOFTWARE=FT_Web_SERVE";
	vec_of_string.push_back(str);
	str = "REDIRECT_STATUS=200";
	vec_of_string.push_back(str);
	str = "FCGI_ROLE=RESPONDER";
	vec_of_string.push_back(str);
	str = "REQUEST_SCHEME=http";
	vec_of_string.push_back(str);
	str = "PATH=" + std::string(std::getenv("PATH"));
	vec_of_string.push_back(str);
	vec_of_string.push_back("SCRIPT_FILENAME=" +  script);
	vec_of_string.push_back("SERVER_PORT="+ server.port);
	vec_of_string.push_back("REMOTE_ADDR=");
	vec_of_string.push_back("HTTP_HOST=");
	vec_of_string.push_back("REQUEST_URI=REQ");
	vec_of_string.push_back("DOCUMENT_URI=" + script);
	return vec_of_string;
}


//? convert vector to char**
char **vector_to_char(std::vector<std::string> params){
	char **out = new char*[params.size() + 1];
	for (size_t i = 0; i < params.size(); i++){
		out[i] = strdup(params[i].c_str());
	}
	out[params.size()] = NULL;
	return out;
}

//? fork to run the cgi
std::string  Response::cgi(Server_block server, std::string cgi_runner, std::string script, std::string body_file){
	std::vector<std::string> vec_of_meta_vars = make_meta_variables_for_cgi(_request, server, script);
	char **meta_vars = vector_to_char(vec_of_meta_vars);
	long int now = get_current_time();
	std::stringstream ss;
	ss << now;
	std::string file_name =  ss.str();
	char **vars = new char*[3];
	vars[0] = (char*)cgi_runner.c_str();
	vars[1] = (char*)script.c_str();
	vars[2] = NULL;
	std::string output;
	int pid = fork();	
	if (pid == 0){
		int fd_out = criet_and_open_file(file_name);
		int fd_in = open(body_file.c_str(), O_RDONLY);
		fcntl(fd_in, F_SETFL, O_NONBLOCK);
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		int exec;
		exec  = execve(vars[0], vars, meta_vars);
		this->internalError();
		return output;	
 	}
	else{
		long start = get_current_time();
		while(waitpid(pid, NULL, WNOHANG) == 0){
			long end = get_current_time();
			if (end - start > CGI_TIME_OUT){
				this->time_out();
				create_file();
				int i = 0;
				while(meta_vars[i]){
					delete[] meta_vars[i++];
				}
				delete[] meta_vars;
				delete[] vars;
				return "";
			}
		}
		int fd = open(file_name.c_str(), O_RDONLY);
		fcntl(fd, F_SETFL, O_NONBLOCK);
		char *s = new char[1000];
		int readed = 0;
		size_t size = 0;
		while((readed = read(fd, s, 1000)) > 0){
			output += std::string(s, readed);
			size += readed;
		}
		int i = 0;
		while(meta_vars[i]){
			delete[] meta_vars[i++];
		}
		delete[] meta_vars;
		delete[] vars;
		delete[] s;
		close(fd);
		if (readed == -1)
			return output;
		unlink(file_name.c_str());
		return output;
	}
	return output;
} 

void Response::handleRequest(Server_block server) {
	 //* WORK ON CGI 
	Location_block location = getLocation(server);
	_location = location;
	_server = server;
	_path = location.root + _path;
	if (_request.getHost().size() ==0 ){
		this->badRequest();
		create_file();
		return;
	}
	if (_request.is_chunked() == false && _request.get_has_body() && _request.getContentLength() == 0){
		this->badRequest();
		create_file();
		return;
	}
	if (_request.getRequestMethod() != "GET" && _request.getRequestMethod() != "POST" && _request.getRequestMethod() != "DELETE")
    {
        this->unallowedMethod();
        create_file();
        return;
    }
	if (_request.getBody().size() && location.max_body_size.size() && !check_max_body_size())
	{
		this->payloadTooLarge();
		create_file();
		return;
	}
	if (location.return_path.size())
	{
		int statuscode = std::stoi(location.return_code);
		set_redirection(statuscode, location.return_path);
		create_file();
		return;
	}

	struct stat s, s2;
	stat(_path.c_str(), &s);
	if(S_ISDIR(s.st_mode))
	{
		std::fstream * file = new std::fstream();

		//? check if index file exist return index_file else if auto index exit  return it else 404
		//? this is for index_file or auto_index
		std::string index_file = _path + "/" + location.index_file;;
		stat(index_file.c_str(), &s2);
		if (location.auto_index == "on" && (location.index_file == ""  || (s2.st_mode & S_IFREG) == 0) && _request.getRequestMethod() == "GET" &&
					std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "GET") != location.allowed_funct.end()){
			is_autoindex = 1;
			auto_index();
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
					this->handleGetRequest(server, location);		
			}
			else if (_request.getRequestMethod() == "DELETE" &&
					std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "DELETE") != location.allowed_funct.end()){
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
		else
		{
			this->unallowedMethod();
			create_file();
		}
	}
	else if ( _request.getRequestMethod() == "DELETE"){
		if (std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "DELETE") != location.allowed_funct.end())
			this->handleDeleteRequest();
		else
		{
			this->unallowedMethod();
			create_file();
		}
	}
	else if ( _request.getRequestMethod() == "GET"){
		if (std::find(location.allowed_funct.begin(), location.allowed_funct.end(), "GET") != location.allowed_funct.end())
			this->handleGetRequest(server, location);
		else
		{
			this->unallowedMethod();
			create_file();
		}
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
		if (rd >= 0)
			_response += std::string(buff, rd);
		
		_body.close();
		create_file();
		if (fd == -1)
			return;
		close(fd);
		
		return;
	}

}

void Response::handleGetRequest(Server_block server, Location_block location)
{
	//?------------------------------------------------------------------------------------	CGI
	if (location.cgi_ext.size() && location.cgi_path.size()){ 
			if (file_is_suported_from_cgi(location, _path)){ //! run cgi
				std::string cgi_runner  = location.root + '/' + location.cgi_path;
				int fd = open(_path.c_str(), O_RDONLY);
				fcntl(fd, F_SETFL, O_NONBLOCK);
				close(fd);
				if (fd > 0){//! check for file to execute if true run cgi	
					std::string cgi_output = cgi(server, cgi_runner, _path, _request.getBody());
					if (cgi_output.size() == 0)
						return;
					if ((int)cgi_output.find("Status:", 0) == -1)
						_response += "HTTP/1.1 200 Ok\r\n";
					else{
							int start = cgi_output.find("Status", 0);
							int end = cgi_output.find("\r\n", start);
							std::string status_code = cgi_output.substr(start + 7, end);
						_response += "HTTP/1.1 " + status_code + "\r\n";

					}
					int index = cgi_output.find("\r\n\r\n", 0);
					int size = cgi_output.size() - index;
					_response += "Content-Length: " + std::to_string(size - 5) + "\r\n";
					_response += cgi_output;
					create_file();
					return;
				}
				else{			//!  404
					notFound();
					create_file();
					return;
				}
			}
	}
	//?------------------------------------------------------------------------------------

	struct stat fileStat;
	stat (_path.c_str(), &fileStat);
	int fd  = open(_path.c_str(), O_RDONLY);

	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (fd == -1)
	{
		this->notFound();
		create_file();
		return;
	}
	char buff[1001] = {0};
	this->ok(fileStat.st_size);
	int reading = 0;
	while((reading = read(fd, buff, 1000)) > 0){
		_response +=  std::string(buff, reading);
		bzero(buff, 1000);
	}
	close(fd);
	if (reading == -1)
		this->notFound();
	close(fd);
	create_file();				
}


void Response::handlePostRequest(Server_block &server, Location_block &location){
	if (location.upload_store == ""){ //? return forbiden function
		if (location.cgi_ext.size() && location.cgi_path.size()){ 
				if (file_is_suported_from_cgi(location, _path)){ 
					std::string cgi_runner  = location.root + '/' + location.cgi_path;
					int fd = open(_path.c_str(), O_RDONLY);
					fcntl(fd, F_SETFL, O_NONBLOCK);
					close(fd);
					if (fd > 0){	
						std::string cgi_output = cgi(server, cgi_runner, _path, _request.getBody());
						if (cgi_output.size() == 0)
							return;
						if ((int)cgi_output.find("Status:", 0) == -1)
							_response += "HTTP/1.1 200 Ok\r\n";
						else{
							int start = cgi_output.find("Status", 0);
							int end = cgi_output.find("\r\n", start);
							std::string status_code = cgi_output.substr(start + 7, end);
							_response += "HTTP/1.1 " + status_code + "\r\n";
						}
						int index = cgi_output.find("\r\n\r\n", 0);
						int size = cgi_output.size() - index;
						_response += "Content-Length: " + std::to_string(size - 5) + "\r\n";
						_response += cgi_output;
						_response += "\r\n\r\n";
						create_file();
						return;
					}
					else{ //? file to run not found 404
						notFound();
						create_file();
						return;
					}
				}

		}
		else{ 
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
		std::string mv = "mv " + _request.getBody() + " " + file_path; //? move the file from tmp and save it in the upload directory in the server
		int ret = system(mv.c_str());
		time_t rawtime;
		time(&rawtime);
		if (ret != -1)
			_response += "HTTP/1.1 201 created\r\n"; 
		else
			_response += "HTTP/1.1 409 Conflict\r\n";
		_response += "Date: " + std::string(ctime(&rawtime));
		_response += "Server: webserver";
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
	if (lstat(_path.c_str(), &st) == -1)
	{
		if (errno == ENOTDIR || errno == ENOENT)
			this->notFound();
		else if (errno == EACCES)
			this->forbidden();
		else if (errno == EEXIST)
			this->unallowedMethod();
	}
    else
	{
		if (st.st_mode & S_IFDIR) {
			_path += (_path.back() != '/') ? "/" : "";
			if ((dirp = opendir(_path.c_str())))
				deleteDirectoryFiles(dirp, _path);
		}
		else if (st.st_mode & S_IFREG)
			unlink(_path.c_str());
		this->ok(0);
	}
	create_file();
}