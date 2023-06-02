#pragma once

#include "../../uav/uav.hpp"

class Stream {
	public:
		virtual void stream_data(UAV *uav) = 0;
};

