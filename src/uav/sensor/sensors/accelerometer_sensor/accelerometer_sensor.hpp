#pragma once

#include "../../sensor.hpp"
#include "../../../uav.hpp"

class AccelerometerSensor : public Sensor {
    private:
        float _x, _y, _z;

    public:
        AccelerometerSensor(UAV *uav, float noise_level);

		void update(uint64_t t);

		std::vector<float> get_data();
};

