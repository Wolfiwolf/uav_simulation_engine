#pragma once

#include "../../stream.hpp"
#include <string>
#include <fstream>

class FileStream : public Stream {
	private:
		std::ofstream _out_file;

	public:
		FileStream(const std::string &out_dir);
		~FileStream();

		void stream_data(UAV *uav);
};

