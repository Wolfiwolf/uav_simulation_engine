#pragma once

#include <vector>

class UAV;

class Sensor {
	protected:
		UAV *_uav;

	public:
		void set_uav(UAV *uav) {
			_uav = uav;
		}

		virtual void init() = 0;

		virtual void update() = 0;

		virtual std::vector<float> get_data() = 0;
};

