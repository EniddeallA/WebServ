#include "../../includes/Parsing.hpp"
#include "../../includes/Response.hpp"
void start_servers(std::vector<Server_block> &all_servers){
	fd_set 	_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp;

	initial_fd_set(_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp);
	std::vector<int> fds;
	int new_socket, valread;
	int number_of_servers = all_servers.size();
	std::vector<Request>v_of_request_object(MAX_REQUEST, Request());
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

					}
				}
				else
					new_socket = i;

				if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1){
					throw "Error in fcntl() function";
				}
				valread = read(new_socket, buffer, BUFFER);
				std::string s = buffer;
				//
				
				// std::cout << s;
				v_of_request_object[new_socket].Parse(s);
				
				if (v_of_request_object[new_socket].isRequestCompleted() && valread != -1){ // tst valread !!!
					// std::cout << "====================================================" << valread << std::endl;
					// v_of_request_object[new_socket].printData();
					// std::cout << v_of_request_object[new_socket].getBody();
					// std::cout << "====================================================END OF PRINTING IN SERVER" << all_servers.size()  << " new socket is " << new_socket << " size is " << v_of_request_object.size()<< std::endl;
					fd_with_response_object[new_socket] = Response(v_of_request_object[new_socket]);
					// std::cout << "start handiling " << std::endl;
					fd_with_response_object[new_socket].handleRequest(all_servers[all_servers.size() - 1]); // just for test use the last server bloc
					fd_with_response[new_socket] = (char*)fd_with_response_object[new_socket].get_respone().c_str(); //? that just return the head but we still need the body
					std::cout << "finish geting responce " << std::endl;
					// std::cout << "====================================================RESPONCE\n";
					// std::cout << fd_with_response[new_socket] << std::endl;
					// std::cout << "====================================================RESPONCE\n";
					// fd_with_response[new_socket] = get_response();
					// file_to_return(all_servers[3], v_of_request_object[new_socket].getRequestTarget()); // fkdsjvnvjjdshgjlghjgdlshsdlghghds
					// std::cout << fd_with_response[new_socket] << std::endl;
					// std::cout << "====================================================RESPONCE\n";
					fd_with_send_size[new_socket] = 0;
					FD_CLR(new_socket, &_fd_set_read);
					FD_SET(new_socket, &_fd_set_write);
					std::cout << "start sendiing responce" << std::endl;
				}
			}
			//? i need to send the head then send the body
			if (FD_ISSET(i, &_fd_set_write_temp)){ //CHECK FOR WRITTING FD_SET
				new_socket = i;
				int index = fd_with_send_size[new_socket];
				int l = 0;
				int buffer_size = BUFFER;
				while (fd_with_response[new_socket][l])
					l++;
				if (index + buffer_size > l) 
					buffer_size = l - index;
				int sended = write(new_socket, fd_with_response[new_socket] + index, buffer_size);
									std::cout << "write head" << std::endl;
									write(0, fd_with_response[new_socket] + index, buffer_size);
				fd_with_send_size [new_socket] += sended;
				index += sended;

				if (index >= l){ //? after sending the head i need to start sending the body
					std::cout << "finish sendiing head and start sending body" << std::endl;

					bzero(buffer, BUFFER);
					fd_with_response_object[new_socket].get_body()->read(buffer, BUFFER - 1);
					write(new_socket, buffer, buffer_size);
					std::cout << "body buffer is " << buffer ;

				}
				if (index >= l && fd_with_response_object[new_socket].get_body()->eof()){ //? after finish sending all responce
					std::cout << "finish sendiing body" << std::endl;

					if (v_of_request_object[new_socket]._isKeepAlive() == false){ //correct this function the default is keep-alive not close
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
						FD_CLR(new_socket, &_fd_set_write);
						FD_SET(new_socket, &_fd_set_read);
					}
					v_of_request_object[new_socket].clear();
				}
			}
		}
	}
}