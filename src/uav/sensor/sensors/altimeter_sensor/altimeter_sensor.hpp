#pragma once

#include "../../sensor.hpp"
#include "../../../uav.hpp"


class AltimeterSensor : public Sensor {
    private:
        float _altitude_window[10000];
        float _altitude;

	public:
		AltimeterSensor(UAV *uav, float noise_level);

		void update(uint64_t t);

		std::vector<float> get_data();
};



