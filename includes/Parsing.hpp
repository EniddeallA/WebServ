#ifndef PARSING_HPP
#define PARSING_HPP
#define BUFFER 1000
#define	MAX_REQUEST 1024
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
#include <set>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <sys/time.h>

#define CGI_TIME_OUT 2000000
#define REQUEST_TIME_OUT 1000000

typedef std::map<std::string, std::string>::iterator _itr;
struct Location_block
{
	std::map<std::string, std::string> location_data;
	std::string 						path;
	bool 								allowed;
	std::vector<std::string> 			allowed_funct;
	std::string							auto_index;
	std::string 						upload_store;
	std::string 						cgi_path;
	std::vector<std::string> 			cgi_ext;
	std::string 						root;
	std::string 						return_code;
	std::string 						return_path;
    std::string							max_body_size;
	long								max_body_size_in_kb;
	std::string							index_file;
	// Location_block operator=(Location_block block){

	// }


	
};

struct Server_block{
		std::map<std::string, std::string> server_data;
		std::string 						ip;
		std::string 						port;
		std::string 						upload_store;
		bool 								allowed; // to check if thee is allowed key or not.
		std::vector<std::string> 			allowed_funct;
		std::string 						error_page_path;
		std::string 						error_code;
		std::map<int, std::string>			error_page;
		std::vector<std::string> 			name;
		std::string							max_body_size;
        // std::string 						root;
		std::vector<Location_block> 		all_locations;
		int 								server_fd;
		struct sockaddr_in 					s_address;
		long								max_body_size_in_kb;
};

long int 					get_current_time();
std::vector<Server_block>	parsse_the_config_file(std::string);
std::vector<Server_block> 	get_all_server_blocks(std::string);
Location_block 				get_location_block(std::string);
std::vector<std::string> 	split (const std::string, std::string);
std::string					remove_all_comment(std::string);
void						check_error_in_file(std::string);
void 						check_brackets(std::string);
std::string					get_key_from_line(std::string);
std::string					get_value_from_line(std::string);
void 						fill_data_in_struct(std::vector<Server_block> &);
void						start_servers(std::vector<Server_block>&);
void 						run_cgi(std::string, std::vector<std::string>, std::string&);
void 						start_server(Server_block &);
void 						initial_fd_set(fd_set &, fd_set &, fd_set &, fd_set &);
char 						*get_response();
bool						port_already_binded(std::vector<Server_block> &, int, int);
bool						syntax_of_port_is_correct(std::string);
bool 						is_number(std::string str);
int							criet_and_open_file(std::string file_name);

void   				 		file_to_return(Server_block server, std::string path);
#endif

// #include "request.hpp"