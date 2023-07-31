#pragma once

#include <vector>
#include <stdint.h>

class UAV;

class Sensor {
	protected:
		UAV *_uav;
        float _noise_level;

	public:
        Sensor(UAV *uav, float noise_level);

		virtual void update(uint64_t t) = 0;

		virtual std::vector<float> get_data() = 0;
};

