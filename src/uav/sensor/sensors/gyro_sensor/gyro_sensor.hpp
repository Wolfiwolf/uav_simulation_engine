#pragma once

#include "../../sensor.hpp"
#include "../../../uav.hpp"

class GyroData {
	public: 
		float p;
		float q;
		float r;
};

class GyroSensor : public Sensor{
	public:
		GyroSensor(UAV *uav);

		void init();

		void update();

		std::vector<float> get_data();
};


