#pragma once
#include "../../uav.hpp"
#include "../../sensor/sensors/gyro_sensor/gyro_sensor.hpp"
#include "../../sensor/sensors/accelerometer_sensor/accelerometer_sensor.hpp"

class QuadCopter : public UAV {
	private:
		float _power;

		GyroSensor *_gyro_sensor;
        AccelerometerSensor *_accelerometer_sensor;

		float _target_altitude;
		float _altitude_control;
		struct Matrix _target_orientation;
	public:
		QuadCopter();
		~QuadCopter();

		void communication_thread();
		void communication_non_blocking_thread();

		void control_pan(float x, float y);
		void control_elevation(float val);

	protected:
		void state_estimation_update(float delta_t);

		void control_update(float delta_t);

		void forces_update(float delta_t);

		void moments_update(float delta_t);
};
