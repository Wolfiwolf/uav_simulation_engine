#include "file_stream.hpp"
#include <iostream>

FileStream::FileStream(const std::string &out_dir) {
	_out_file.open(out_dir);
}

FileStream::~FileStream() {
	_out_file.close();
}

void FileStream::stream_data(uint64_t t, UAV *uav) {
	_out_file << t << "\n";
	_out_file << uav->get_angular_velocity().rows[0][0] << "\n";
	_out_file << uav->get_angular_velocity().rows[1][0] << "\n";
	_out_file << uav->get_angular_velocity().rows[2][0] << "\n";

	_out_file << uav->get_orientation_euler_angles_ZYX().rows[0][0] << "\n";
	_out_file << uav->get_orientation_euler_angles_ZYX().rows[1][0] << "\n";
	_out_file << uav->get_orientation_euler_angles_ZYX().rows[2][0] << "\n";
}
