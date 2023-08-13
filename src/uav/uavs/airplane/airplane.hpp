#pragma once

#include "../../uav.hpp"
#include "../../sensor/sensors/gyro_sensor/gyro_sensor.hpp"
#include "../../sensor/sensors/accelerometer_sensor/accelerometer_sensor.hpp"

class Airplane : public UAV {
	private:
		GyroSensor *_gyro_sensor;
        AccelerometerSensor *_accelerometer_sensor;

		struct Matrix _target_orientation;

	public:
		Airplane();
		~Airplane();

		void communication_thread();

		void communication_non_blocking_thread();

	protected:
		void state_estimation_update(float delta_t);

		void control_update(float delta_t);

		void forces_update(float delta_t);

		void moments_update(float delta_t);
};

