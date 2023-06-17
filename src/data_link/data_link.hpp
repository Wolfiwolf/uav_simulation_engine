#pragma once
#include <stdint.h>

class DataLink {
	private:
		int _port;

		int _listening_socket;
		int _client_socket;

	public:
		DataLink(int port);
		~DataLink();

		void wait_for_link();
		void send_to_gs();
		uint32_t listen_for_msg(uint8_t *rx_buffer);
};

