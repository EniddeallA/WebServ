#ifndef PARSING_HPP
#define PARSING_HPP
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
struct Location_block{
	std::string path;
	std::vector<std::string> allowed_funct;
	std::string	auto_index;
	std::string upload_store;
	std::string cgi_path;
	std::vector<std::string> cgi_ext;
	std::string root;
	std::pair<int, std::string> return_pair;
};

struct Server_block{
		int port;
		std::string ip;
		std::string error_page_path;
		std::string name;
		int	max_body_size;
		std::vector<Location_block> all_locations;
};

std::vector<Server_block>	parsse_the_config_file(std::string);
std::vector<Server_block> 	get_all_server_blocks(std::string);
Location_block 				get_location_block(std::string);
std::vector<std::string> 	split (const std::string, std::string);
Server_block 				get_server_block(std::string);
std::string					get_ip_address(std::string);
int							get_port(std::string);
std::string					get_error_page_path(std::string);
int							get_client_max_body_size(std::string);
std::string					get_server_name(std::string);
Location_block 				get_location_block(std::string data);
std::vector<Location_block> get_all_location_blocks(std::string);
std::string 				get_url_path(std::string);
std::string 				get_auto_index(std::string);
std::vector<std::string> 	get_allowed_funct(std::string);
std::string 				get_upload_store(std::string);
std::string 				get_cgi_pass(std::string);
std::vector<std::string> 	get_cgi_ext(std::string data);
std::string 				get_root(std::string);
std::pair<int, std::string> get_return_pair(std::string data);

#endif