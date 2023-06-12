#include "streamer.hpp"
#include "stream/streams/file_stream/file_stream.hpp"
#include "../config_manager/config_manager.hpp"
#include "stream/streams/udp_stream/udp_stream.hpp"
#include <string>

Streamer::Streamer() {
	std::string out_folder; 

	bool res = ConfigManager::get_param_val("out_folder", out_folder);
	if (!res) out_folder = ".";
	_streams.push_back(new FileStream(out_folder + "/uav_state_log.txt"));

	std::string udp_url; 
	res = ConfigManager::get_param_val("udp_ip_address", udp_url);
	if (!res) udp_url = "127.0.0.1";

	std::string udp_port; 
	res = ConfigManager::get_param_val("udp_port", udp_port);
	if (!res) udp_port = "5050";

	int port = std::stoi(udp_port);

	_streams.push_back(new UDPStream(udp_url, port));
}

Streamer::~Streamer() {

}

void Streamer::stream_data(uint64_t t, UAV *uav) {
	for (Stream *data_stream : _streams) {
		data_stream->stream_data(t, uav);
	}
}
