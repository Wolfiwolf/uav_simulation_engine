#include "streamer.hpp"
#include "stream/streams/file_stream/file_stream.hpp"
#include "../config_manager/config_manager.hpp"

Streamer::Streamer() {
	std::string out_folder; 

	bool res = ConfigManager::get_param_val("out_folder", out_folder);
	if (!res) out_folder = ".";
	_streams.push_back(new FileStream(out_folder + "/uav_state_log.txt"));
}

Streamer::~Streamer() {

}

void Streamer::stream_data(UAV *uav) {
	for (Stream *data_stream : _streams) {
		data_stream->stream_data(uav);
	}
}
