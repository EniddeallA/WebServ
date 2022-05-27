#include "../../includes/Parsing.hpp"

// check if port is already open if it's dont open it again
bool port_already_binded(std::vector<Server_block> &all_server, int index, int port){
	for (int i = 0; i < index; i++){
		if (atoi(all_server[i].port.c_str()) == port)
			return true;
	}
	return false;
}

void start_server(Server_block &server){
	if ((server.server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw "Fail in socket fd";
	int one = 1;
	std::cout << "server fd is " << server.server_fd << std::endl;
	setsockopt(server.server_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

	bzero(&(server.s_address), sizeof(server.s_address));
	server.s_address.sin_family = AF_INET;
	// if (inet_addr(server.ip.c_str()) == -1)
	// 	throw "Error Adress not valide";
	server.s_address.sin_addr.s_addr = inet_addr(server.ip.c_str());
	int port = atoi(server.port.c_str());
	server.s_address.sin_port = htons(port);
	if (bind(server.server_fd, (struct sockaddr*)&(server.s_address), sizeof(server.s_address)) < 0)
		throw "Error in binding";
	if (listen(server.server_fd, MAX_REQUEST) < 0) // Check the max value of backlog
		throw "Error in listening";
}


//DEFAULT RESPONSE FUNCTION
char *get_response(){
	char * resp = (char *)"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	return resp;
}

void initial_fd_set(fd_set &_fd_set, fd_set &_fd_set_temp, fd_set &_fd_set_write, fd_set &_fd_set_write_temp){
	FD_ZERO(&_fd_set);
	(void)7;
	FD_ZERO(&_fd_set_temp);
	FD_ZERO(&_fd_set_write);
	FD_ZERO(&_fd_set_write_temp);
}