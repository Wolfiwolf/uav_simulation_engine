#pragma once

#include "../../uav/uav.hpp"

class Stream {
	public:
		virtual void stream_data(uint64_t t, UAV *uav) = 0;
};

