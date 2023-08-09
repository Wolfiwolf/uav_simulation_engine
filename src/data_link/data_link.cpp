#include "data_link.hpp"
#include <exception>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>


DataLink::DataLink(int port) {
	_port = port;
    _is_socket_connected = false;
}

DataLink::~DataLink() {
	close(_client_socket);
	shutdown(_listening_socket, SHUT_RDWR);
}

void DataLink::wait_for_link() {
    _is_socket_connected = false;

	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	if ((_listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cout << "Listening socket failed!\n";
	}

	if (setsockopt(_listening_socket, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		std::cout << "Listening failed!\n";
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(_port);

	if (bind(_listening_socket, (struct sockaddr*)&address,
				sizeof(address))
			< 0) {
		std::cout << "Listening socket binding failed!\n";
	}
	if (listen(_listening_socket, 3) < 0) {
		std::cout << "Listening failed!\n";
	}
	if ((_client_socket
				= accept(_listening_socket, (struct sockaddr*)&address,
					(socklen_t*)&addrlen))
			< 0) {
		std::cout << "Accepting connection failed!\n";
	}

	close(_listening_socket);

    _is_socket_connected = true;
}

void DataLink::send_to_gs(uint8_t *data, uint8_t size) {
    if (_is_socket_connected) {
	    send(_client_socket, data, size, 0);
    }
}

uint32_t DataLink::listen_for_msg(uint8_t *rx_buffer) {
	return read(_client_socket, rx_buffer, 64);
}
