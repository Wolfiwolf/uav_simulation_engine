#include "streamer.hpp"
#include "stream/streams/file_stream/file_stream.hpp"

Streamer::Streamer() {
	_streams.push_back(new FileStream("./logs"));
}

Streamer::~Streamer() {

}

void Streamer::stream_data(UAV *uav) {
	for (Stream *data_stream : _streams) {
		data_stream->stream_data(uav);
	}
}
