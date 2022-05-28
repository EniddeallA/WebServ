#include "../../includes/Parsing.hpp"
#include "../../includes/Response.hpp"

//? criet and lunch servers 
//! cgi time-out
void start_servers(std::vector<Server_block> &all_servers){
	fd_set 	_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp;

	initial_fd_set(_fd_set_read, _fd_set_read_temp, _fd_set_write, _fd_set_write_temp);
	std::vector<int> fds;
	int new_socket, valread;
	int number_of_servers = all_servers.size();
	std::vector<Request>v_of_request_object(MAX_REQUEST, Request());
	int fd_max = 0;
	int fd_min = INT32_MAX;
	char buffer[BUFFER] = {0};
	for (int i = 0; i < number_of_servers; i++){ // start all servers
		int port = atoi(all_servers[i].port.c_str());
		if (port_already_binded(all_servers, i, port))
			continue;
		start_server(all_servers[i]);
		fcntl(all_servers[i].server_fd, F_SETFL, O_NONBLOCK);
		fd_max = std::max(all_servers[i].server_fd, fd_max);
		fd_min = std::min(all_servers[i].server_fd, fd_min);
		fds.push_back(all_servers[i].server_fd); // THIS vector is for get max fd
		FD_SET(all_servers[i].server_fd, &_fd_set_read);
	}

	std::map<int, char*> fd_with_response;
	std::map<int, Response> fd_with_response_object;

	std::map<int, long int> fd_with_time;
	int max_server_fd = fd_max;
	struct timeval select_time;
	select_time.tv_sec = 1;
	select_time.tv_usec = 0;
	while (1){
		bzero(buffer, BUFFER);
		_fd_set_read_temp = _fd_set_read;
		_fd_set_write_temp = _fd_set_write;
		int selected;
		selected = select(fd_max + 1, &_fd_set_read_temp, &_fd_set_write_temp, NULL, &select_time); //? check timeout
		//? check for time out ----------------------------*
		for (int i = 0; i < fd_max + 1; i++){
			long int now = get_current_time();
			if (fd_with_time.count(i)){
				if (now - fd_with_time[i] >= REQUEST_TIME_OUT){
					v_of_request_object[i].clear();
					fd_with_response_object[i].get_request().clear();
					if (FD_ISSET(i, &_fd_set_read_temp)){
						FD_CLR(i, &_fd_set_read);
					}
					else if (FD_ISSET(i, &_fd_set_write_temp)){
						FD_CLR(i, &_fd_set_write);
					}
					fd_with_time.erase(i);
				}
			}
		}
		//?----------------------------------------------*
		for (int i = 0; i < fd_max + 1; i++){
			if (FD_ISSET(i, &_fd_set_read_temp)){
				socklen_t addrlen = sizeof(all_servers[i -fd_min].s_address); //? -fd_min because fd start with 3 after 0 and 1 and 2 
				if (i <= max_server_fd){ //! check if it's new connection or not
					if ((new_socket = accept(i, (struct sockaddr *) &(all_servers[i -fd_min].s_address), &addrlen)) == -1)
						continue;
					if (std::find(fds.begin(), fds.end(), new_socket) == fds.end()){
						FD_SET(new_socket, &_fd_set_read);
						fds.push_back(new_socket);
						fd_max = new_socket;
					}
				}
				else{
					new_socket = i;
				}
				if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1){
					continue;
				}
				valread = read(new_socket, buffer, BUFFER);
				std::string s;
				if (valread > 0){
					s = std::string(buffer, valread);
					fd_with_time[new_socket] = get_current_time();
				}
				else if (valread == 0){
					s = "";
				}
				else{ //? remove client in error
					v_of_request_object[new_socket].clear();
					fd_with_response_object[new_socket].get_request().clear();
					if (FD_ISSET(new_socket, &_fd_set_read_temp)){
						FD_CLR(new_socket, &_fd_set_read);
					}
					else if (FD_ISSET(new_socket, &_fd_set_write_temp)){
						FD_CLR(new_socket, &_fd_set_write);
					}
					fd_with_time.erase(i);
					continue;
				}
				//? start parssing the file
				v_of_request_object[new_socket].Parse(s);
				if (v_of_request_object[new_socket].isRequestCompleted() && valread != -1){
					fd_with_response_object[new_socket] = Response(v_of_request_object[new_socket]);
					fd_with_response_object[new_socket].handleRequest(v_of_request_object[new_socket].setServer(all_servers));
					FD_CLR(new_socket, &_fd_set_read);
					FD_SET(new_socket, &_fd_set_write);
				}
			}

			if (FD_ISSET(i, &_fd_set_write_temp)){
			//? for first time save length and size of the file;
				new_socket = i;
				bzero(buffer, BUFFER);
				int fd = fd_with_response_object[new_socket].get_fd(); //! implement fcntl to all fds!!!!!
				valread =  read(fd, buffer, BUFFER);
				int sended = 0;
				if (valread != -1)
					sended = send(new_socket, buffer, valread, 0); //! should remove client in errore
				if (sended > 0){
					fd_with_time[new_socket] = get_current_time();
					fd_with_response_object[new_socket].update_size_sended(sended);
				}
				//? remove client in error in sendig
				if (sended  == -1 ){
					v_of_request_object[new_socket].clear();
					fd_with_response_object[new_socket].get_request().clear();
					if (FD_ISSET(new_socket, &_fd_set_read_temp)){
						FD_CLR(new_socket, &_fd_set_read);
					}
					else if (FD_ISSET(new_socket, &_fd_set_write_temp)){
						FD_CLR(new_socket, &_fd_set_write);
					}
					fd_with_time.erase(i);
					continue;
				}
				//? move the pointer of the file to send all the file 
				if (valread != sended && fd != -1 && valread > 0){
					int defferent = valread - sended;
					if (defferent > 0)
						lseek(fd, -defferent ,SEEK_CUR);
				}



				if (valread <= 0 && sended == 0){ //? after finish sending all responce
					close(fd_with_response_object[new_socket].get_fd());
					unlink(fd_with_response_object[new_socket].get_file_path().c_str());

					fd_with_response_object[new_socket].reset();
					if (v_of_request_object[new_socket]._isKeepAlive() == false){
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
					fd_with_response_object[new_socket].get_request().clear();
				}
			}

		}
	}
}


//! if unchunked and missing of content lentgh  ----> 400