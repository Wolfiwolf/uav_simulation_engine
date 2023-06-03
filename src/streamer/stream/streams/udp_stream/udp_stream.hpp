#pragma once

#include "../../stream.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class UDPStream : public Stream {
	private:
		int _sockfd;
    	struct sockaddr_in _servaddr;
	public:
		UDPStream(const std::string &url, uint32_t port);
		~UDPStream();

		void stream_data(UAV *uav);
};

