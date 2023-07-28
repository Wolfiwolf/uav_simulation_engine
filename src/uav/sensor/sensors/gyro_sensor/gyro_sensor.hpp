#pragma once

#include "../../sensor.hpp"
#include "../../../uav.hpp"


class GyroSensor : public Sensor {
    private:
        float _p, _q, _r;

	public:
		GyroSensor(UAV *uav, float noise_level);

		void update();

		std::vector<float> get_data();
};


