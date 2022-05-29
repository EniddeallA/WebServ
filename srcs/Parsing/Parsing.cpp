#include "../../includes/Parsing.hpp"

int find_the_end_of_location_block(std::string data, int start){
	int end = start + 1;
	char open = '{';
	char close = '}';
	int size = data.length();
	char c;
	int count = 0;
	while (end < size){
		c = data[end];
		if (c == open)
			count++;
		if (c == close)
			count--;
		if (c ==  close && count == 0)
			return  end - 1;
		end++;
	}
	return -1;
}

int find_location_keyword(std::string data, int index){
	int i = 0;
	int length = data.length();
	int start = -2;
	std::string to_find = "location ";
	while (to_find[i] == data[index + i]){
		i++;
	}
	if (i == 9){
		i += index;
		start = index;
		while (i < length && data[i] != '\n'){
			i++;
		}
		if (i + 1 < length && data[i] == '\n'){
			i++;
			while (i < index && data[index] == ' ' && data[index] == '\t'){
				i++;
			}
			i++;
			if (data[i] == '{')
				return start;
		}
	}
	return -1;
}

std::vector<std::string> parse_the_server_block_by_location_keyword(std::string data){
	std::vector<std::string>  locations;
	int i = 0;
	int size = data.length();
	int end = -1;
	std::string location_key = "location";
	while (i < size){
		std::string location = "";
		int start = find_location_keyword(data, i);
		if (start != - 1){
			end = find_the_end_of_location_block(data, start);
			if (end == -1)
				throw "There is error in the location block";
			while (start <= end){
				location += data[start++];
			}
			locations.push_back(location);
		}
		i++;
	}
	return locations;
}
//? find server key_word
int find_serever_key_word(std::string data, int index){
	int i = 0;
	int length = data.length();
	std::string to_find = "server";
	while (to_find[i] == data[index + i]){
		i++;
	}
	if (i == 6){
		i += index;
		if (i  + 2 < length &&( data[i] == ' ' || data[i] == '\n')){
			if (data[i + 1] == '{')
				return i + 2;
		}
	}
	return -1;
}

int find_the_end_of_server_block(std::string data, int start){
	int end = start + 1;
	char open = '{';
	char close = '}';
	int size = data.length();
	char c;
	int count = 0;
	while (end < size){
		c = data[end];
		if (c == open)
			count++;
		if (c == close)
			count--;
		if (c ==  close && count == -1)
			return  end - 1;
		end++;
	}
	return -1;
}

std::map<std::string, std::string> get_location_data(std::string data){
	std::map<std::string, std::string> my_map;
	std::string key, value;
	std::pair<std::string, std::string> p;
	int size = data.length();
	int i = 0;
	while (i < size){
		key = "";
		value = "";
		while (i < size && (data[i] == ' ' || data[i] == '\t')){
			i++;
		}
		while (i < size && data[i] != ' ' && data[i] != '\t' && data[i] != '\n'){
			key += data[i++];
		}
		while (i < size && (data[i] == ' ' || data[i] == '\t') && data[i] != '\n'){
			i++;
		}
		while (i < size && data[i] != '\n'){
			value += data[i++];
		}
		i++;
		p = std::make_pair(key, value);
		if (key == "{" || key == "")
			continue;
		if (value == "" || my_map.insert(p).second == false)
				throw "There is error in the location block";
	}
	return my_map;
}

std::map<std::string, std::string> get_server_data(std::string data){
	int i = 0;
	int count = 0;
	std::map<std::string, std::string> my_map;
	int size = data.size();
	while (i < size){
		std::string line;
		int j = i;
		while (j < size && data[j] != '\n'){
			line += data[j];
			j++;
		}
		i = ++j;
		std::string key = get_key_from_line(line);
		std::string value = get_value_from_line(line);
		std::pair<std::string, std::string> p = std::make_pair(key, value);		
		if (key == "{")
			count++;
		else if (key == "}")
			count--;
		else if (count == 0 && key != "" && key != "location" ){
			if (value == "" || my_map.insert(p).second == false){
				throw "There is error in the server block";
			}
		}
	}
	return my_map;
}

std::vector<std::string> parse_the_file_by_server_keyword(std::string data){
	std::vector<std::string>  servers;
	int i = 0;
	int size = data.length();
	int end = -1;
	std::string server_key = "server";
	while (i < size){
		std::string server;
		int start = find_serever_key_word(data, i);
		if (start != - 1){
			end = find_the_end_of_server_block(data, start);
			if (end == -1)
				throw "There is error in the struct of server block";
			while (start <= end){
				server += data[start++];
			}
			servers.push_back(server);
		}
		i++;
	}
	return servers;
}

std::vector<Server_block> get_all_server_blocks(std::string data){
	std::vector<Server_block> all_servers;
	std::vector<std::string> servers_data = parse_the_file_by_server_keyword(data);
	for(size_t i  = 0; i < servers_data.size(); i++){
		Server_block server;
		server.server_data = get_server_data(servers_data[i]);
		std::vector<std::string> locations_data = parse_the_server_block_by_location_keyword(servers_data[i]);
		std::vector<Location_block> all_locations;
		for (size_t i = 0; i < locations_data.size(); i++){
			Location_block location;
			location.location_data = get_location_data(locations_data[i]);
			all_locations.push_back(location);
		}
		server.all_locations = all_locations;
		all_servers.push_back(server);
	}
	return (all_servers);
}

std::vector<Server_block> parsse_the_config_file(std::string path){
	std::string data, line;
	Server_block block;
	std::ifstream path_stream(path.c_str());
	while (getline(path_stream, line)){
		data += line;
		data += "\n";
	}
	data = remove_all_comment(data);
	check_error_in_file(data);
	std::vector<Server_block> all_server_block = get_all_server_blocks(data);
	fill_data_in_struct(all_server_block);
	return (all_server_block);
}
