#pragma once

#include "../uav/uav.hpp"
#include "stream/stream.hpp"
#include <vector>

class Streamer {
	private:
		std::vector<Stream*> _streams;

	public:
		Streamer();
		~Streamer();

		void stream_data(uint64_t t, UAV *uav);
};

