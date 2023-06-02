#include "file_stream.hpp"

FileStream::FileStream(const std::string &out_dir) {
	_out_file.open(out_dir);
}

FileStream::~FileStream() {
	_out_file.close();
}

void FileStream::stream_data(UAV *uav) {
	// TODO: Write UAV data to file
}
