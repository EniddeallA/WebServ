#include "../../includes/Parsing.hpp"
#include "../../includes/Response.hpp"
void start_servers(std::vector<Server_block> &all_servers){
	fd_set 	_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp;

	initial_fd_set(_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp);
	std::map<int, std::string> not_sended_yet;
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

		int selected = select(fd_max +1, &_fd_set_read_temp, &_fd_set_write_temp, NULL,NULL); //check timeout
		if (selected < 0){
			throw ("Error in select function");
		}
		for (int i = 0; i < fd_max + 1; i++){
			if (FD_ISSET(i, &_fd_set_read_temp)){

				socklen_t addrlen = sizeof(all_servers[i - 3].s_address); // - 3 because fd start with 3 after 0 and 1 and 2
				if (i <= max_server_fd){ //! check if it's new connection or not
					if ((new_socket = accept(i, (struct sockaddr *) &(all_servers[i - 3].s_address), &addrlen)) == -1)
						throw "Error in accepting socket";
					if (std::find(fds.begin(), fds.end(), new_socket) == fds.end()){
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
				// std::cout << s;
				
				v_of_request_object[new_socket].Parse(s);
				
				if (v_of_request_object[new_socket].isRequestCompleted() && valread != -1){ // tst valread !!!
					std::cout << "--------------------------------------------------------------------" << std::endl;
					v_of_request_object[new_socket].printData();
					fd_with_response_object[new_socket] = Response(v_of_request_object[new_socket]);
					fd_with_response_object[new_socket].handleRequest(all_servers[all_servers.size() - 1]); // just for test use the last server bloc
					std::cout << "--------------------------------------------------------------------" << std::endl;
					fd_with_response[new_socket] = strdup(fd_with_response_object[new_socket].get_respone().c_str()); //? that just return the head but we still need the body
			
					fd_with_send_size[new_socket] = 0;
					FD_CLR(new_socket, &_fd_set_read);
					FD_SET(new_socket, &_fd_set_write);
				}
			}
		

			if (FD_ISSET(i, &_fd_set_write_temp)){ //CHECK FOR WRITTING FD_SET
			//? for first time save leth and size of the file;
				new_socket = i;
				bzero(buffer, BUFFER);
				int fd = fd_with_response_object[new_socket].get_fd(); //! implement fcntl to all fds!!!!!

				valread =  read(fd, buffer, BUFFER);
				int sended = write(new_socket, buffer, valread);
				std::cout << "ON_sendiing " << sended << " of " << valread << std::endl;

				if (sended > 0)
					fd_with_response_object[new_socket].update_size_sended(sended);

				if (valread != sended && fd != -1 && valread > 0){
					int defferent = valread - sended;
					if (defferent > 0)
						lseek(fd, -defferent ,SEEK_CUR);
					else{
						std::cout <<  "check for  " << valread << " " << sended << std::endl;
					}
				}



				if (valread <= 0){ //? after finish sending all responce
					std::cout << "****************************************************" << std::endl;
					std::cout << "finish sendiing data for" << v_of_request_object[new_socket].getRequestTarget() << std::endl;
					std::cout << "finish sendiing " << fd_with_response_object[new_socket].get_size_sended() << " of " << fd_with_response_object[new_socket].get_size_of_file() << std::endl;

					close(fd_with_response_object[new_socket].get_fd());
					unlink(fd_with_response_object[new_socket].get_file_path().c_str());
					fd_with_response_object[new_socket].reset();
					if (v_of_request_object[new_socket]._isKeepAlive() == false){ //correct this function the default is keep-alive not close
					// if (1 == 1){ //correct this function the default is keep-alive not close
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