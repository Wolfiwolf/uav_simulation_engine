#include "gyro_sensor.hpp"


GyroSensor::GyroSensor(UAV *uav, float noise_level): Sensor(uav, noise_level) {}

void GyroSensor::update() {
	struct Matrix mat = _uav->get_angular_velocity();

    _p = mat.rows[0][0];
    _q = mat.rows[1][0];
    _r = mat.rows[2][0];
}

std::vector<float> GyroSensor::get_data() {

	return { _p, _q, _r };
}
