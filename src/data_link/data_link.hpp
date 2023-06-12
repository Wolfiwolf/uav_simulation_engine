#pragma once
#include "../uav/uav.hpp"

class DataLink {
	private:
		UAV *_uav;
		int _port;

		int _listening_socket;
		int _client_socket;



	public:
		DataLink(UAV *uav, int port);
		~DataLink();

		void wait_for_link();
		void send_to_gs();
		uint32_t listen_for_msg(uint8_t *rx_buffer);
};

