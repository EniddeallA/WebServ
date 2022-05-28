#include "../../includes/Parsing.hpp"

void fill_server_data(Server_block &server, _itr begin, _itr end){
	std::string key = begin->first;
	std::string value = begin->second;		
	while (begin != end){
		key = begin->first;
		value = begin->second;
		if (key == "listen"){
			std::vector<std::string> data = split(value, ":");
			if (data.size() == 2){
				server.ip = split(value, ":")[0];
				server.port = split(value, ":")[1];
				//check if port is digits
				if (syntax_of_port_is_correct(server.port) == false)
					throw "Port argument should be just digit charachters";
			}
			else
				throw "Listen argument should have ip and port spilted by :";
		}
		else if (key == "upload_store")
			server.upload_store = value;
		else if (key == "allow"){
			server.allowed = true;
			server.allowed_funct = split(value, " ");
			std::string allows[] = {"GET", "DELETE", "POST"};
			for (size_t i = 0; i < server.allowed_funct.size(); i++){
				std::string allow = server.allowed_funct[i];
				if (allow != allows[0] && allow != allows[1] && allow != allows[2])
					throw "Function not valide in allowed_functions";
			}
		}
		else if (key == "error_page"){
			if (split(value, " ").size() != 2)
				throw "Error page should have 2 arguments";
			server.error_code = split(value, " ")[0];
			if (is_number(server.error_code) == false)
				throw "Error code argument should be just digit charachters";
			server.error_page_path = split(value, " ")[1];
			server.error_page[atoi(server.error_code.c_str())] = server.error_page_path;
		}
		else if (key == "server_name"){
			server.name = split(value, " ");
		}
		else if(key == "client_max_body_size"){
			server.max_body_size = value;
			// server.max_body_size_in_kb = convert_to_kb( )
		}
		// else if(key == "root")
			// server.root = value;
		begin++;
	}
	return;
}

void fill_location_data(Server_block server, Location_block &location, _itr begin, _itr end){
	std::string key = begin->first;
	std::string value = begin->second;
	// fill the data that can be in location and server bloc in location
	location.allowed_funct = server.allowed_funct;
	location.allowed = server.allowed;
	location.upload_store = server.upload_store;
	// location.root = server.root;
	location.max_body_size = server.max_body_size;
	while (begin != end){
		key = begin->first;
		value = begin->second;
		begin++;
		if (key == "location")
			location.path = value;
		if (key == "allow"){
			location.allowed_funct = split(value, " ");
			location.allowed = true;
			std::string allows[] = {"GET", "DELETE", "POST"};
			for (size_t i = 0; i < location.allowed_funct.size(); i++){
				std::string allow = location.allowed_funct[i];
				if (allow != allows[0] && allow != allows[1] && allow != allows[2])
					throw "Function not valide in allowed_functions";
			}
		}
		if (key == "autoindex")
			location.auto_index = value;
		if (key == "upload_store")
			location.upload_store = value;
		if (key == "cgi_pass")
			location.cgi_path = value;
		if (key == "cgi_ext")
			location.cgi_ext = split(value, " ");
		if (key == "root")
			location.root = value;
		if (key == "return"){
			if (split(value, " ").size() != 2)
				throw "Return should have 2 variable";
			location.return_code = split(value, " ")[0];
			location.return_path = split(value, " ")[1];
		}
		if (key == "client_max_body_size")
			location.max_body_size = value;	
		if (key == "index")
			location.index_file = value;
	}

}

bool check_server_name_duplicate(std::vector<std::string> server_1, std::vector<std::string> server_2){
	for (size_t i = 0; i < server_1.size(); i++){
		for (size_t j = 0; j < server_2.size(); j++){
			if (server_1[i] == server_2[j])
				return false;
		}
	}
	return true;
}

void check_duplicate_server(std::vector<Server_block> &all_servers){
	for (size_t i = 0; i < all_servers.size(); i++){
		for (size_t j = i + 1; j < all_servers.size(); j++){
			if (all_servers[i].port == all_servers[j].port && !check_server_name_duplicate(all_servers[i].name, all_servers[j].name)){
				throw "There is duplicate server name and port";
			}
		}
	}
	
}

void check_duplicate_locations(std::vector<Location_block> &all_locations){
	for (size_t i = 0; i < all_locations.size(); i++){
		for (size_t j = i + 1; j < all_locations.size(); j++){
			if (all_locations[i].path == all_locations[j].path){
				throw "There is duplicate in location";
			}
		}
	}
	
}

void fill_data_in_struct(std::vector<Server_block> &all_servers){
	int size = all_servers.size();
	for (int i = 0; i < size; i++){
		_itr begin(all_servers[i].server_data.begin());
		_itr end(all_servers[i].server_data.end());
		fill_server_data(all_servers[i], begin, end);
		for (size_t j = 0; j < all_servers[i].all_locations.size(); ++j) {
			_itr _begin(all_servers[i].all_locations[j].location_data.begin());
			_itr _end(all_servers[i].all_locations[j].location_data.end());
			fill_location_data(all_servers[i], all_servers[i].all_locations[j], _begin, _end);
		}
		check_duplicate_locations(all_servers[i].all_locations);
	}
	check_duplicate_server(all_servers);
	//! check for missing data
}