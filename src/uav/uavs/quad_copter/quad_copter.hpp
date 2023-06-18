#pragma once
#include "../../uav.hpp"
#include "../../sensor/sensors/gyro_sensor/gyro_sensor.hpp"

class QuadCopter : public UAV {
	private:
		float _power;
		GyroSensor *_gyro_sensor;

		float _target_altitude;
		struct Matrix _target_orientation;
	public:
		QuadCopter();
		~QuadCopter();

		void communication_thread();

	protected:
		void control_update(float delta_t);

		void forces_update(float delta_t);

		void moments_update(float delta_t);
};
