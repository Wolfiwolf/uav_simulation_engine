#include "file_stream.hpp"
#include <iostream>

FileStream::FileStream(const std::string &out_dir) {
	_out_file.open(out_dir);
}

FileStream::~FileStream() {
	_out_file.close();
}

void FileStream::stream_data(uint64_t t, UAV *uav) {
	_out_file << "------------\n";
	_out_file << t << "\n";

	_out_file << uav->get_position().rows[0][0] << "\n";
	_out_file << uav->get_position().rows[1][0] << "\n";
	_out_file << uav->get_position().rows[2][0] << "\n";

	_out_file << uav->get_position_geodetic().rows[0][0] << "\n";
	_out_file << uav->get_position_geodetic().rows[1][0] << "\n";
	_out_file << uav->get_position_geodetic().rows[2][0] << "\n";

	_out_file << uav->get_orientation_q().rows[0][0] << "\n";
	_out_file << uav->get_orientation_q().rows[1][0] << "\n";
	_out_file << uav->get_orientation_q().rows[2][0] << "\n";
	_out_file << uav->get_orientation_q().rows[3][0] << "\n";
}
