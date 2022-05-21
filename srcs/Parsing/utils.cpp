#include "../../includes/Parsing.hpp"

std::vector<std::string> split (const std::string s, std::string delim){
	std::vector<std::string> result;
	size_t index = s.find(delim);
	size_t position = 0;
	std::string str;
	while (index != (size_t)-1){
		str = s.substr(position, index - position);
		if (str.length())
			result.push_back(str);
		position = index + delim.length();
		index = s.find(delim, index + 1);
	}
	str = s.substr(position, s.length() - position);
	if (str != delim  && str != "")
		result.push_back(str);
	return (result);
}

bool is_the_end_of_data(std::string data, int i){
	if (data[i] == '#' || data[i] == ';')
		return true;
	if (data[i] == ' '){
		while (i < (int)data.length() && data[i] == ' '){
			i++;
		}
		if (i == (int)data.length() || data[i] == '\n' || data[i] == ';' || data[i] == '#')
			return true;	
	}
	return false;
}

std::string remove_all_comment(std::string data){
	int i = 0;
	int len = data.length();
	std::string new_data = "";
	while (i < len){
		if (is_the_end_of_data(data, i))
			while (i < len && data[i] != '\n'){
				i++;
			}
		new_data += data[i];
		i++;
	}
	i = new_data.length() - 1;
	while (i >= 0 && (new_data[i] == ' ' || new_data[i] == '\t')){
		new_data.pop_back();
	}
	return (new_data);
}

std::string  get_value_from_line(std::string element){
	std:: string value = "";
	int i = 0;
	while (element[i] && (element[i] == ' ' || element[i] == '\t'))
		i++;
	while (element[i] && (element[i] != ' ' && element[i] != '\t'))
		i++;
	while (element[i] && (element[i] == ' ' || element[i] == '\t'))
		i++;
    while (element[i] && element[i] != '\n')
		value += element[i++];
    i = value.length();
    while( i &&( value[i - 1] == ' ' || value[i - 1] == '\t')){
        value.pop_back();
        i--;
    }
	return (value);
}

std::string	get_key_from_line(std::string element){
	std:: string key = "";
	int i = 0;
	while (element[i] && (element[i] == ' ' || element[i] == '\t'))
		i++;
	while (element[i] && (element[i] != ' ' && element[i] != '\t')){
		key += element[i];
		i++;
	}
	return (key);
}

void check_brackets(std::string data){
	char open_brackets = '{';
	char close_brackets = '}';
	int check = 0;
	for(int i = 0; i < (int)data.length(); i++){
		char c = data[i];
		if (c == open_brackets)
			check--;
		else if (c == close_brackets)
			check++;
	}
	if (check != 0)
		throw "There is error in Brackets";
	return ;
}

void check_error_in_file(std::string data){
	check_brackets(data);
	std::string valide_keyword[] = {"listen", "server_name", "error_page", "client_max_body_size", "location", "allow", "autoindex", "upload_store", "cgi_pass", "cgi_ext", "index", "return", "root", "server", "}", "{", "" };
	int count_of_keys = 17;
	std::vector<std::string> check = split(data, "\n");
	std::vector<std::string>::iterator it_begin(check.begin());
	std::vector<std::string>::iterator it_end(check.end());
	std::string element;
	--it_begin;
	while(++it_begin != it_end){
		element = *it_begin;
		std::string key = get_key_from_line(element);
		for (int i = 0; i < count_of_keys + 1; i++)
		{
			if (i == count_of_keys){
				throw "Found keyword not allowed";
			}
			if (valide_keyword[i] == key)
				break ;
		}
	}
}

bool syntax_of_port_is_correct(std::string port){
	for (size_t i = 0; i < port.length(); i++){
		if (isdigit(port[i]) == false)
			return false;
	}
	return true;
}

long convert_to_kb(std::string value){
	long size = atol(value.c_str()) * 1000;
	return size;
}

int	criet_and_open_file(std::string file_name){
	int fd = open(file_name.c_str(), std::ios::out | std::ios::trunc| O_WRONLY);
	if (fd == -1){
		std::fstream outfile;
		outfile.open(file_name.c_str(), std::ios_base::app);
		outfile.close();
		fd = open(file_name.c_str(), std::ios::out | std::ios::trunc| O_RDWR);
	}
	return fd;
}

bool is_number(std::string str){
	for (size_t i = 0; i < str.length(); i++){
		if (isdigit(str[i]) == false)
			return false;
	}
	return true;
}