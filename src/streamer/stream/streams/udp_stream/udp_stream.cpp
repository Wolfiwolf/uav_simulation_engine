#include "udp_stream.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>

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

void UDPStream::stream_data(uint64_t t, UAV *uav) {
    int n;
    socklen_t len;
       
	uint8_t data[28];

	const struct Matrix pos = uav->get_position();
	const struct Matrix q = uav->get_orientation_q();

	memcpy(data, &(pos.rows[0][0]), 4);
	memcpy(data + 4, &(pos.rows[1][0]), 4);
	memcpy(data + 8, &(pos.rows[2][0]), 4);

	memcpy(data + 12, &(q.rows[0][0]), 4);
	memcpy(data + 16, &(q.rows[1][0]), 4);
	memcpy(data + 20, &(q.rows[2][0]), 4);
	memcpy(data + 24, &(q.rows[3][0]), 4);

    sendto(
			_sockfd, 
			(const char *)data, 28,
        	MSG_CONFIRM, 
			(const struct sockaddr *) &_servaddr, 
            sizeof(_servaddr)
	);
}
