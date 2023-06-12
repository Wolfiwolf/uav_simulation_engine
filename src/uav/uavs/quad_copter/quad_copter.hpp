#pragma once
#include "../../uav.hpp"
#include "../../sensor/sensors/gyro_sensor/gyro_sensor.hpp"

class QuadCopter : public UAV {
	private:
		float _power;
		GyroSensor *_gyro_sensor;

	public:
		QuadCopter();
		~QuadCopter();

		void handle_gs_bytes(uint8_t *bytes, uint32_t len);

	protected:
		void control_update(float delta_t);

		void forces_update(float delta_t);

		void moments_update(float delta_t);
};
