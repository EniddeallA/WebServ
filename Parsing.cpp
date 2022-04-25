#include "./Parsing.hpp"

std::pair<int, std::string> get_return_pair(std::string data){
	std::pair<int, std::string> return_pair;
	std::vector<std::string> store_data;
	std::string to_find = "return";
	size_t index = data.find(to_find);
	if (index == std::string::npos)
		return (return_pair);
	std::string funct;
	index += to_find.length();
	while(index < data.length() && data[index] == ' ')
		index++;
	while(index < data.length() && data[index] != ';'){
		if (data[index] == ' '){
			store_data.push_back(funct);
			funct.clear();
			while (index < data.length() && data[index] == ' ')
				index++;
		}
		else{
			funct += data[index++];
		}
	}
	if (funct.size())
		store_data.push_back(funct);
	if (store_data.size() < 2)
		return (return_pair);
	std::string return_code = store_data[0];
	std::istringstream ss(return_code.c_str());
	int code;
	ss >> code;
	return_pair.first = code;
	return_pair.second = store_data[1];
	return (return_pair);

}

std::string get_cgi_pass(std::string data){
	std::string cgi_pass;
    std::string to_find("cgi_pass");
    size_t index = data.find(to_find);
    if (index == std::string::npos)
        return (cgi_pass);
    index += to_find.length();
    char c = data[index];
    while (index < data.length() && c == ' ')
        c = data[++index];
    while (index < data.length() && c != ' ' && c != ';' && c != '\n'){
        cgi_pass += c;
        c = data[++index];
    }
    return (cgi_pass);
}

std::vector<std::string> get_cgi_ext(std::string data){
	std::vector<std::string> all_ext;
	std::string to_find = "cgi_ext";
	size_t index = data.find(to_find);
	if (index == std::string::npos)
		return (all_ext);
	std::string funct;
	index += to_find.length();
	while(index < data.length() && data[index] == ' ')
		index++;
	while(index < data.length() && data[index] != ';'){
		if (data[index] == ' '){

			all_ext.push_back(funct);
			funct.clear();
			while (index < data.length() && data[index] == ' ')
				index++;
		}
		else
			funct += data[index++];
	}
	if (funct.size())
		all_ext.push_back(funct);
	return (all_ext);
}

std::string get_root(std::string data){
	std::string upload_store;
    std::string to_find("root");
    size_t index = data.find(to_find);
    if (index == std::string::npos)
        return (upload_store);
    index += to_find.length();
    char c = data[index];
    while (index < data.length() && c == ' ')
        c = data[++index];
    while (index < data.length() && c != ' ' && c != ';' && c != '\n'){
        upload_store += c;
        c = data[++index];
    }
    return (upload_store);
}


std::string get_upload_store(std::string data){
	std::string upload_store;
    std::string to_find("upload_store");
    size_t index = data.find(to_find);
    if (index == std::string::npos)
        return (upload_store);
    index += to_find.length();
    char c = data[index];
    while (index < data.length() && c == ' ')
        c = data[++index];
    while (index < data.length() && c != ' ' && c != ';' && c != '\n'){
        upload_store += c;
        c = data[++index];
    }
    return (upload_store);
}

std::string get_url_path(std::string data){
    std::string path;
    size_t index = 0;
    while (index < data.length()){
        if (data[index] == '\n')
            break;
        if (data[index] != ' ')
            path += data[index];
        index++;
    }
    return (path);
}

std::string get_auto_index(std::string data){
	std::string auto_index = "off";
	std::string to_find = "autoindex";
	size_t index = data.find(to_find);
	if (index == std::string::npos)
		return (auto_index);
	index += to_find.length();
	while (index < data.length() && data[index] == ' ')
		index++;
	auto_index.clear();
	while (index < data.length() && isalpha(data[index])){
		auto_index += data[index++];
	}
	return(auto_index);
	
}
std::vector<std::string> get_allowed_funct(std::string data){
	std::vector<std::string> all_funct;
	std::string to_find = "allow";
	size_t index = data.find(to_find);
	if (index == std::string::npos)
		return (all_funct);
	std::string funct;
	index += to_find.length();
	while(index < data.length() && data[index] == ' ')
		index++;
	while(index < data.length() && data[index] != ';'){
		if (data[index] == ' '){

			all_funct.push_back(funct);
			funct.clear();
			while (index < data.length() && data[index] == ' ')
				index++;
		}
		else
			funct += data[index++];
	}
	if (funct.size())
		all_funct.push_back(funct);
	return (all_funct);
}

Location_block get_location_block(std::string data){
    Location_block block;
    block.path = get_url_path(data);
	block.allowed_funct = get_allowed_funct(data);
	block.auto_index = get_auto_index(data);
	block.upload_store = get_upload_store(data);
	block.cgi_path = get_cgi_pass(data);
	block.cgi_ext = get_cgi_ext(data);
	block.root = get_root(data);
	block.return_pair = get_return_pair(data);

    return (block);
}

std::vector<Location_block> get_all_location_blocks(std::string data){
    std::vector<Location_block> all_locations;
    std::vector<std::string> servers_data = split(data, "location ");
    std::vector<std::string>::iterator it(servers_data.begin());
    servers_data.erase(it);
    for(int i = 0; i < servers_data.size(); i++){
        all_locations.push_back(get_location_block(servers_data[i]));
    }
    return (all_locations);
}

std::string	get_server_name(std::string data){
    std::string server_name;
    std::string to_find("server_name");
    size_t index = data.find(to_find);
    if (index == std::string::npos)
        return(NULL);
    index += to_find.length();
    char c = data[index];
    while (index < data.length() && c == ' ')
        c = data[++index];
    while (index < data.length() && c != ' ' && c != ';' && c != '\n'){
        server_name += c;
        c = data[++index];
    }
    return (server_name);
}

int get_client_max_body_size(std::string data){
    std::string size_str;
    std::string to_find("client_max_body_size");
    size_t index = data.find(to_find);
    if (index == std::string::npos)
        return(-1);
    index += to_find.length();
    char c = data[index];
    while (index < data.length() && c == ' ')
        c = data[++index];
    while (index < data.length() && isdigit(c)){
        size_str += c;
        c = data[++index];
    }
    std::istringstream ss(size_str);
    int size_to_return;
    ss >> size_to_return;
    return(size_to_return);   
}

std::string	get_error_page_path(std::string data){
    std::string path;
    std::string to_find("error_page");
    size_t index = data.find(to_find);
    if (index == std::string::npos)
        return(NULL);
    index +=  to_find.length();
    char c = data[index];
    while (index < data.length() && (c == ' ' || isdigit(c)))
        c = data[++index];
    while (index < data.length() && (c != ' ' && c != ';'))
    {
        path += c;
        c = data[++index];
    }
    return (path);
}

std::string get_ip_address(std::string data){
    std::string ip;
    size_t index = data.find("listen");
    if (index == std::string::npos)
        return (NULL);
    while(index < data.length() && data[index] != '\n'){
        if(isdigit(data[index])){
            ip+= data[index];
        }
        else if((data[index]) == '.'){
            ip+= data[index];
        }
        else if (data[index] == ':')
            return (ip);
        index++;
    }
    ip.clear();
    return (ip);
}

int	get_port(std::string data){
    std::string port;
    size_t index = data.find("listen");
    if (index == std::string::npos)
        return (-1);
    while(index < data.length() && data[index] != '\n'){
        if(isdigit(data[index])){
            port+= data[index];
        }
        if(data[index] == ':'){
            port.clear();
            while(data[index] && data[index] != '\n'){
                if(isdigit(data[index])){
                    port+= data[index];
                }
                index++;
            }
        }
        index++;
    }
    std::istringstream ss(port);
    int port_to_return;
    ss >> port_to_return;
    return(port_to_return);
}

Server_block get_server_block(std::string data){
    Server_block block;
    block.port = get_port(data);
    block.ip = get_ip_address(data);
    block.error_page_path = get_error_page_path(data);
    block.max_body_size = get_client_max_body_size(data);
    block.name = get_server_name(data);
    get_all_location_blocks(data);
    return(block);

}

std::vector<Server_block> get_all_server_blocks(std::string data){
    std::vector<Server_block> all_servers;
    std::vector<std::string> servers_data = split(data, "server\n{\n");
    for(size_t i  = 0; i < servers_data.size(); i++){
        all_servers.push_back(get_server_block(servers_data[i]));
        if (all_servers[i].ip.length() == 0 && i != 0)
            all_servers[i].ip = all_servers[i - 1].ip;
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
    get_all_server_blocks(data);
    std::vector<Server_block>all_server_block;
    return (all_server_block);
}
