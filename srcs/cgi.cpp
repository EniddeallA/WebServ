#include "../includes/Parsing.hpp"

// THE URL FORMAT
/**
 * 
      script-URI = <scheme> "://" <server-name> ":" <server-port> <script-path> <extra-path> "?" <query-string>

 * 
 */
//  Meta-variables contain data about the request passed from the server to the script
/*
	  	meta-variable-name = "AUTH_TYPE" | "CONTENT_LENGTH" |
						   "CONTENT_TYPE" | "GATEWAY_INTERFACE" |
						   "PATH_INFO" | "PATH_TRANSLATED" |
						   "QUERY_STRING" | "REMOTE_ADDR" |
						   "REMOTE_HOST" | "REMOTE_IDENT" |
						   "REMOTE_USER" | "REQUEST_METHOD" |
						   "SCRIPT_NAME" | "SERVER_NAME" |
						   "SERVER_PORT" | "SERVER_PROTOCOL" |
						   **********************************************
		meta-variable-name = "AUTH_TYPE" | "CONTENT_LENGTH" |
					   "CONTENT_TYPE" | "GATEWAY_INTERFACE" |
					   "PATH_INFO" | "PATH_TRANSLATED" |
					   "QUERY_STRING" | "REMOTE_ADDR" |
					   "REMOTE_HOST" | "REMOTE_IDENT" |
					   "REMOTE_USER" | "REQUEST_METHOD" |
					   "SCRIPT_NAME" | "SERVER_NAME" |
					   "SERVER_PORT" | "SERVER_PROTOCOL" |
					   "SERVER_SOFTWARE" | scheme |
					   protocol-var-name | extension-var-name
*/

std::vector<std::string> make_meta_variables_for_cgi(Request request){
	//prefer to return vector insted of tring
	std::vector<std::string> vec_of_string;
	std::string meta_variables = "";
	std::string str;

	// CONTENT_LENGTH size of the body
	str = "CONTENT_LENGTH=" + std::to_string(request.getContentLength()) + "\n";
	vec_of_string.push_back(str);


	//CONTENT_TYPE indicate the media type of the resource 
	//should achraf add this function
	str = "CONTENT_TYPE=" + request.getContentType() + "\n";
	vec_of_string.push_back(str);

	// GATEWAY_INTERFACE  it's CGI/revision.
	str = "GATEWAY_INTERFACE=CGI/1.1\n";
	vec_of_string.push_back(str);
	
	//PATH_INFO is the path after the_cgi  The extra path information as given by the client. 
	str = "PATH_INFO=/\n";
	vec_of_string.push_back(str);

	//PATH_TRANSLATED  translate the path info by convert space and point and , to there value
	str = "PATH_TRANSLATED=/\n";
	vec_of_string.push_back(str);

	//QUERY_STRING   is the last data in the url after the question mark
	// str = "QUERY_STRING=" + getQueryStringFromUrl(pathInfo) + "\n";
	vec_of_string.push_back(str);

	//REQUEST_METHOD   is the type of request
	str = "REQUEST_METHOD=" + request.getRequestMethod() + "\n";
	vec_of_string.push_back(str);

	//SCRIPT_NAME  is the name of the script to run
	// str = "SCRIPT_NAME=" + request.getscript_name() + "\n";
	vec_of_string.push_back(str);

	
	//SERVER_NAME the name of server that user target , i should get it from dollar
	// str = "SERVER_NAME=" +  request.getserver_name()+ "\n";
	vec_of_string.push_back(str);
	
	//SERVER_PORT the port of server that user target , i should get it from dollar
	// str = "SCRIPT_PORT=" +  request.getserver_port()+ "\n";
	vec_of_string.push_back(str);
	
	//SERVER_PROTOCOL the version of http that the server support
	str = "SERVER_PROTOCOL=HTTP/1.1\n";
	vec_of_string.push_back(str);

	//SERVER_SOFTWARE the version of http that the server support
	str = "SERVER_SOFTWARE=FT_Web_SERVE\n";
	vec_of_string.push_back(str);
	return vec_of_string;
}


char **vector_to_char(std::vector<std::string> params){
	char **out = new char*[params.size() + 1];
	for (size_t i = 0; i < params.size(); i++){
		out[i] = strdup(params[i].c_str());
		out[i] = strdup(params[i].c_str());
	}
	out[params.size()] = NULL;

	return out;
}

void run_cgi(std::string path, std::vector<std::string> params, std::string &output, char  **envp){
	std::string file_name = "./.cgi_file_rederect.";
	char **vars = vector_to_char(params);
	int pid = fork();
	if (pid == 0){
		int fd = open(file_name.c_str(), std::ios::out | std::ios::trunc| O_WRONLY);
		if (fd == -1){
			std::fstream outfile;
			outfile.open(file_name.c_str(), std::ios_base::app);
			outfile.close();
			fd = open(file_name.c_str(), std::ios::out | std::ios::trunc| O_RDWR);

		}
		dup2(fd, 2);
		dup2(fd, 1);
		int exec = execve(path.c_str(), vars, envp);
		perror("Execve Fail");
 	}
	else{
		int status;
		while(waitpid(pid, NULL, 0) > 0){
		}
		int fd = open(file_name.c_str(), O_RDONLY);
		char *s = new char[1000];
		while(read(fd, s, 1000)  == 1000){
			output += s;
		}
		output += s;
		std::cout << output;
		close(fd);
		unlink(file_name.c_str());
	}
}

/* 
	the input of the cgi is the request body
	chose one cgi to yun it  (php, perl, python)
*/



void cgi(Request request){
	std::vector<std::string> vec_of_meta_vars = make_meta_variables_for_cgi(request);
	char **meta_vars = vector_to_char(vec_of_meta_vars);
	std::string file_name = "cgi_file_rederect2";
	char **vars = new char*[4];
	vars[0] = strdup("/usr/bin/perl");
	vars[1] = strdup("-wT");
	vars[2] = strdup("/Users/kbenlyaz/desktop/env.cgi");
	vars[3] = NULL;
	std::string output;
	int pid = fork();
	//////////////////////////////////////////////////
	std::vector<std::string> meta;
	meta.push_back("Content-type=");
	//////////////////////////////////////////////////
	if (pid == 0){
		int fd = open(file_name.c_str(), std::ios::out | std::ios::trunc| O_WRONLY);
		if (fd == -1){
			std::fstream outfile;
			outfile.open(file_name.c_str(), std::ios_base::app);
			outfile.close();
			fd = open(file_name.c_str(), std::ios::out | std::ios::trunc| O_RDWR);

			std::cout << "fd is " << fd << std::endl;
		}
		dup2(fd, 2);
		dup2(fd, 1);
		int exec = execve(vars[0], vars, meta_vars);
		perror("Execve Fail");
 	}
	else{
		int status;
		while(waitpid(pid, NULL, 0) > 0){
		}
		int fd = open(file_name.c_str(), O_RDONLY);
		char *s = new char[1000];
		while(read(fd, s, 1000)  == 1000){
			output += s;
		}
		output += s;
		std::cout << output;
		close(fd);
		// unlink(file_name.c_str());
	}
}


int main(){
	Request request;
	cgi(request);
	return 0;
}