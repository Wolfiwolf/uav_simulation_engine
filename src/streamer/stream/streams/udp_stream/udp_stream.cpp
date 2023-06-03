#include "udp_stream.hpp"

#include <arpa/inet.h>
#include <iostream>

UDPStream::UDPStream(const std::string &url, uint32_t port) {
    if ( (_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&_servaddr, 0, sizeof(_servaddr));
       
    _servaddr.sin_family = AF_INET;
    _servaddr.sin_port = htons(port);
    _servaddr.sin_addr.s_addr = inet_addr(url.c_str());
}

UDPStream::~UDPStream() {
    close(_sockfd);
}

void UDPStream::stream_data(UAV *uav) {
    int n;
    socklen_t len;
       
	uint8_t data[3] = { 0, 1, 2};
    sendto(
			_sockfd, 
			(const char *)data, 3,
        	MSG_CONFIRM, 
			(const struct sockaddr *) &_servaddr, 
            sizeof(_servaddr)
	);
}
