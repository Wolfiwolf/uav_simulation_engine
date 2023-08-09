#pragma once

#include <stdint.h>
#include <iostream>

class DataLink {
	private:
		int _port;
		int _listening_socket;
		int _client_socket;

		bool _is_socket_connected;

	public:
		DataLink(int port);
		~DataLink();

		void wait_for_link();
		void send_to_gs(uint8_t *data, uint8_t size);
		uint32_t listen_for_msg(uint8_t *rx_buffer);
};

