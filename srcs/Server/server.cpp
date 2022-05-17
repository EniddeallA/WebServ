#include "../../includes/Parsing.hpp"
#include "../../includes/Response.hpp"
void start_servers(std::vector<Server_block> &all_servers){
	fd_set 	_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp;

	initial_fd_set(_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp);
	std::vector<int> fds;
	int new_socket, valread;
	int number_of_servers = all_servers.size();
	// std::vector<std::string> v_of_request(MAX_REQUEST, "");
	////////
	std::vector<Request>v_of_request_object(MAX_REQUEST, Request());
	/////////
	int fd_max = 0;
	char buffer[BUFFER] = {0};
	for (int i = 0; i < number_of_servers; i++){ // start all servers
		int port = atoi(all_servers[i].port.c_str());
		if (port_already_binded(all_servers, i, port))
			continue;
		start_server(all_servers[i]);
		fcntl(all_servers[i].server_fd, F_SETFL, O_NONBLOCK); // LEARN MORE ABOUT fcntl
		fd_max = std::max(all_servers[i].server_fd, fd_max);
		fds.push_back(all_servers[i].server_fd); // THIS vector is for get max fd
		FD_SET(all_servers[i].server_fd, &_fd_set_read);
	}

	// std::map<int, Server_block> fd_with_server;
	std::map<int, char*> fd_with_response;
	std::map<int, Response> fd_with_response_object;

	std::map<int, int> fd_with_send_size;
	int max_server_fd = fd_max;
	while (1){
		bzero(buffer, BUFFER);
		_fd_set_read_temp = _fd_set_read;
		_fd_set_write_temp = _fd_set_write;
		int selected = select(fd_max + 1, &_fd_set_read_temp, &_fd_set_write_temp, NULL,NULL); //check timeout
		if (selected < 0){
			throw ("Error in select function");
		}
		for (int i = 0; i < fd_max + 1; i++){
			if (FD_ISSET(i, &_fd_set_read_temp)){
				socklen_t addrlen = sizeof(all_servers[i - 3].s_address); // - 3 because fd start with 3 after 0 and 1 and 2
				if (i <= max_server_fd){ // check if it's new connection or not
					if ((new_socket = accept(i, (struct sockaddr *) &(all_servers[i - 3].s_address), &addrlen)) == -1)
						throw "Error in accepting socket";
					if (new_socket > fd_max){
						FD_SET(new_socket, &_fd_set_read);
						fds.push_back(new_socket);
						fd_max = new_socket;
						// fd_with_server[new_socket] = all_servers[i - 3];
					}
				}
				else
					new_socket = i;

			//////////////////////////////
				if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1){
					throw "Error in fcntl() function";
				}
			//////////////////////////////
				
				valread = read(new_socket, buffer, BUFFER);
				// v_of_request[new_socket] += buffer;

				///////////////////////////////
				std::string s = buffer;
				// std::cout << s;
				v_of_request_object[new_socket].Parse(s);
				///////////////////////////////
				

				if (v_of_request_object[new_socket].isRequestCompleted() && valread != -1){
					// std::cout << "====================================================\n";
					// std::cout << v_of_request_object[new_socket].getBody();
					// std::cout << "====================================================\n";
					fd_with_response_object[new_socket] = Response(v_of_request_object[new_socket]);
					fd_with_response[new_socket] = fd_with_response_object[new_socket].get_respone().c_str();
					fd_with_send_size[new_socket] = 0;
					FD_CLR(new_socket, &_fd_set_read);
					FD_SET(new_socket, &_fd_set_write);

					// v_of_request[new_socket].clear();
					// v_of_request_object[new_socket].clear();
					//////////////////////////////////
				}
			}
			if (FD_ISSET(i, &_fd_set_write_temp)){ //CHECK FOR WRITTING FD_SET
				new_socket = i;
				int index = fd_with_send_size[new_socket];
				int l = 0;
				int buffer = BUFFER;
				while (fd_with_response[new_socket][l])
					l++;
				if (index + buffer > l) 
					buffer = l - index;
				int sended = write(new_socket, fd_with_response[new_socket] + index, buffer);
				fd_with_send_size [new_socket] += sended;
				index += sended;
				if (index >= l){
					if (v_of_request_object[new_socket]._isKeepAlive() == false){ //correct this function the default is keep-alive not close
						// std::cout << "CLOSE CONNECTION" << std::endl;
						FD_CLR(new_socket, &_fd_set_write);
						close(new_socket);
						fd_max = fds[0];
						int index = 0;
						for (size_t i = 0; i < fds.size(); i++){
							if (fds[i] == new_socket){
								index = i;
								continue;
							}
							fd_max = std::max(fds[i], fd_max);
						}
						if (index != 0)
							fds.erase(fds.begin() + index);
					}
					else{
						std::cout << "KEEP ALIVE CONNECTION" << std::endl;
						FD_CLR(new_socket, &_fd_set_write);
						FD_SET(new_socket, &_fd_set_read);
					}
					v_of_request_object[new_socket].clear();
				}
			}
		}
	}
}